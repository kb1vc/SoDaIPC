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
 * @page SoDa::BufferPoll A buffer allocator that produces vectors
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
    BufferPool(size_t min_pool_size) :
      min_pool_size(min_pool_size) {
    }

    class Exception : public std::runtime_error {
    public:
      Exception(const std::string & problem) : 
	std::runtime_error(problem) {
      }
    };
  
  private:
    class PoolAllocatedBuffer : public Buffer<T> {
    public:
      PoolAllocatedBuffer(BufferPool<T> * bp, size_t n) :
	Buffer<T>(n) {
	std::cerr << "Making a new Pool Allocated Buffer\n";	
	from_pool = bp; 
      }

      PoolAllocatedBuffer(BufferPool<T> * bp, std::vector<T> * vp) :
	Buffer<T>(vp) {
	std::cerr << "Making a new Pool Allocated Buffer\n";	
	from_pool = bp; 
      }

      ~PoolAllocatedBuffer() {
	std::cerr << "Deallocating " << this << "\n";
	from_pool->returnToPool(this->vec_p, this->vec_p->size());
      }
      
      BufferPool<T> * from_pool;
    };

  public:
    std::shared_ptr<PoolAllocatedBuffer> getFromPool(size_t n) {
      if(pool.count(n) > 0) {
	std::cerr << "Found something in the pool for size " << n << "\n";
	if(pool[n].size() <= 0) fillPool(n);
	
	auto r = pool[n].front();
	// take the most recently pushed -- as it may
	// still be in the TLBs and caches.
	pool[n].pop_front();
	std::cerr << "Allocated buffer pointer " << r << " queue now has " << pool[n].size() << " entries\n";
	return std::make_shared<PoolAllocatedBuffer>(this, r);
      }
      else {
	std::cerr << "Need to allocate elements for pool[ " << n << "\n";
	pool[n] = std::deque<std::vector<T> *>();
	fillPool(n); 
	return getFromPool(n);
      }
    }

  private:
    std::map<int, std::deque<std::vector<T> *>> pool;
    size_t min_pool_size;
    
    void fillPool(size_t n) {
      std::cerr << "Filling the pool.\n";
      if(pool.count(n) == 0) {
	pool[n] = std::deque<std::vector<T> *>();
      }

      while(pool[n].size() < min_pool_size) {
	pool[n].push_back(new std::vector<T>(n));
      }
    }
    
    void returnToPool(std::vector<T> * p, size_t n) {
      std::cerr << "Attempting to return buffer pointer " << p << "\n";      
      if(pool.count(n) > 0) {
	pool[n].push_front(p);
      }
      else {
	// whoa!  That's not right.
	throw Exception("Could not return pointer to pool.\n");
      }
    }
  };
}
