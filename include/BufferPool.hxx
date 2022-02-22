#pragma once

#include <string>
#include <exception>
#include <stdexcept>
#include <deque>
#include <vector>
#include <map>

#include <iostream>

#include "Buffer.hxx"

/*
BSD 2-Clause License

Copyright (c) 2022, Matt Reilly - kb1vc
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
 * @file BufferPool.hxx
 * @author Matt Reilly (kb1vc)
 * @date Feb 21, 2022
 */

/**
 * @page SoDa::BufferPool A buffer allocator that produces vectors
 * from a pool. 
 * 
 * 
 * ## Namespace
 * 
 * SoDa::Buffer is enclosed in the SoDa namespace because it is
 * inevitiable that there are lots of classes out there called
 * "BUffer."  Perhaps you have written one of them.  Naming a class "Buffer"
 * is like naming a street "Oak:" It might make lots of sense, but
 * you're going to have to reconcile yourself that there's a street 
 * with the same name one town over and sometimes your pizza is going
 * to get mis-routed. 
 * 
 * So Buffer is in the SoDa namespace.  SoDa is from 
 * <a href="https://kb1vc.github.io/SoDaRadio/">SoDaRadio</a> though the SoDa::Buffer 
 * class is a completely independent chunk 'o code. Most of the code I'll release
 * will be in the SoDa namespace, just to avoid the Oak Street problem.
 */

/**
 * @namespace SoDa
 * 
 * Not much else is spelled that way, so we're probably not going to
 * have too many collisions with code written by other people.
 *
 * SoDa is the namespace I use in code like <a
 * href="https://kb1vc.github.io/SoDaRadio/">SoDaRadio</a> (The S D
 * and R stand for Software Defined Radio.  The o,a,d,i,o don't stand
 * for anything in particular.)  But this code has nothing to do with
 * software defined radios or any of that stuff.
 */
namespace SoDa {
  
  template <typename T>
  class BufferPool {
  public:
    BufferPool(std::string name, size_t min_pool_size) :
      min_pool_size(min_pool_size), name(name) {
    }

    class Exception : public std::runtime_error {
    public:
      Exception(BufferPool * bp, const std::string & problem) : 
	std::runtime_error("BufferPool[" + bp->name + "] " + problem) {
      }
    };
    
    class FillPoolException : public Exception {
    public:
      FillPoolException(BufferPool * bp) : 
	Exception(bp, "Tried to fill the buffer pool, but it is still empty") {	
      }
    };

    class ReturnPointerException : public Exception {
    public:
      ReturnPointerException(BufferPool * bp) :
	Exception(bp, "Could not return a pointer to the pool. This is worse than it sounds.") {
      }
    };

  private:
    class PoolAllocatedBuffer : public Buffer<T> {
    public:
      PoolAllocatedBuffer(BufferPool<T> * bp, size_t n) :
	Buffer<T>(n) {
	from_pool = bp; 
      }

      PoolAllocatedBuffer(BufferPool<T> * bp, std::vector<T> * vp) :
	Buffer<T>(vp) {
	from_pool = bp; 
      }

      ~PoolAllocatedBuffer() {
	from_pool->returnToPool(this->vec_p, this->vec_p->size());
      }
      
      BufferPool<T> * from_pool;
    };

    // mutual exclusion stuff
    std::mutex allocation_mtx; 

  public:
    std::shared_ptr<PoolAllocatedBuffer> getFromPool(size_t n) {
      std::lock_guard<std::mutex> lock(allocation_mtx);
      bool second_try = false; 
      
      while(1) {
	if(pool.count(n) > 0) {
	  if(pool[n].size() <= 0) fillPool(n);
	
	  auto r = pool[n].front();
	  // take the most recently pushed -- as it may
	  // still be in the TLBs and caches.
	  pool[n].pop_front();
	  return std::make_shared<PoolAllocatedBuffer>(this, r);
	}
	else {
	  if(second_try) {
	    throw FillPoolException(this);
	  }
	  second_try = true; 
	  pool[n] = std::deque<std::vector<T> *>();
	  fillPool(n); 
	}
      }
    }

    
  private:
    std::map<int, std::deque<std::vector<T> *>> pool;
    size_t min_pool_size;
    std::string name; 

    // don't lock this, as it can only be called from getFromPool.
    void fillPool(size_t n) {
      if(pool.count(n) == 0) {
	pool[n] = std::deque<std::vector<T> *>();
      }

      while(pool[n].size() < min_pool_size) {
	pool[n].push_back(new std::vector<T>(n));
      }
    }
    
    void returnToPool(std::vector<T> * p, size_t n) {
      std::lock_guard<std::mutex> lock(allocation_mtx);            
      if(pool.count(n) > 0) {
	pool[n].push_front(p);
      }
      else {
	// whoa!  That's not right.
	throw ReturnPointerException(this);
      }
    }
  };
}
