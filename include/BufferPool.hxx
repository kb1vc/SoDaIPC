#pragma once

#include <string>
#include <exception>
#include <stdexcept>
#include <deque>
#include <vector>
#include <map>
#include <mutex>

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
 * "Buffer."  Perhaps you have written one of them.  Naming a class "Buffer"
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
  /**
   * @class BufferPool
   * 
   * @brief A creator of SoDa::Buffer objects (actually shared_pointers to them) that allocates
   * the storage from a pool. 
   *
   * @tparam T type of value to be stored in the allocated buffers (e.g. float, std::complex<float>, FooObj...)
   */
  template <typename T>
  class BufferPool {
  public:

    /**
     * @brief Create the buffer pool. 
     * 
     * @param name All buffer pools have a name. This makes exceptions
     * a little more useful: they can identify the pool that had the
     * problem.
     *
     * @param pool_refill_size The pool for buffers of size N will be
     * "filled" with this number of buffers each time it runs dry. Big
     * numbers reduce the likelihood of having to refill the pool. But
     * they also can consume space.
     */
    BufferPool(std::string name, size_t pool_refill_size) :
      pool_refill_size(pool_refill_size), name(name) {
    }

    /**
     * @brief Catch this if you don't care what the reason for the 
     * exception is. 
     */
    class Exception : public std::runtime_error {
    public:
      Exception(BufferPool * bp, const std::string & problem) : 
	std::runtime_error("BufferPool[" + bp->name + "] " + problem) {
      }
    };
    
    /**
     * @brief The widget couldn't fill the buffer pool.  This is
     * really odd and suggests that things have gotten way out of hand
     * somehow. I can't imagine what would cause this.
     */
    class FillPoolException : public Exception {
    public:
      FillPoolException(BufferPool * bp) : 
	Exception(bp, "Tried to fill the buffer pool, but it is still empty") {	
      }
    };

    /**
     * @brief Somebody resized the vector.  That is really really bad. SoDa::Buffer 
     * users should never resize or otherwise fiddle with a vector. See the explanation 
     * in SoDa::Buffer. 
     */
    class ReturnPointerException : public Exception {
    public:
      ReturnPointerException(BufferPool * bp) :
	Exception(bp, "Could not return a pointer to the pool. This is worse than it sounds.") {
      }
    };

  private:
    /**
     * @brief Inside baseball: This is where the allocation/deallocation magic happens. 
     * This is a subclass of Buffer<T> that knows which BufferPool object created it. 
     * On destruction, the internal vector is returned to the appropriate pool.
     */
    class PoolAllocatedBuffer : public Buffer<T> {
    public:
      PoolAllocatedBuffer(BufferPool<T> * bp, size_t n) :
	Buffer<T>(n) {
	//	std::cerr << "allocating " << this->vec_p << " ab initio\n";		
	from_pool = bp; 
      }

      PoolAllocatedBuffer(BufferPool<T> * bp, 
			  std::shared_ptr<std::vector<T>> & vp) :
	Buffer<T>(vp) {
	//	std::cerr << "allocating " << vp << " from pool\n";	
	from_pool = bp; 
      }

      ~PoolAllocatedBuffer() {
	//	std::cerr << "returning " << this->vec_p << "\n";
	from_pool->returnToPool(this->vec_p, this->vec_p->size());
      }

      /**
       * @brief return a reference to a vector.  Useful in passing
       * shared pointers to buffers as std::vectors... It made sense
       * when I was writing this. 
       *
       * @returns Reference to the vector. 
       */
      operator std::vector<T> & () { return this->getVec(); }
      
      BufferPool<T> * from_pool;
    };

    // mutual exclusion stuff
    std::mutex allocation_mtx; 

  public:
    /**
     * @brief Create a buffer. 
     * 
     * @param n The number of elements in the buffer's vector. 
     *
     * @returns A shared pointer to a buffer. The shared pointer
     * is the trigger for releasing the buffer's storage. 
     */
    //    std::shared_ptr<PoolAllocatedBuffer> getFromPool(size_t n) {
    std::shared_ptr<Buffer<T>> getFromPool(size_t n) {    
      std::lock_guard<std::mutex> lock(allocation_mtx);
      bool second_try = false; 

#if 1 // ENABLE_POOL      
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
	  pool[n] = std::deque<std::shared_ptr<std::vector<T>>>();
	  fillPool(n); 
	}
      }
#else
      return std::make_shared<Buffer<T>>(n);      
#endif
    }
    
  private:
    std::map<int, std::deque<std::shared_ptr<std::vector<T>>>> pool;
    size_t pool_refill_size;
    std::string name; 

    // don't lock this, as it can only be called from getFromPool.
    void fillPool(size_t n) {
      if(pool.count(n) == 0) {
	pool[n] = std::deque<std::shared_ptr<std::vector<T>>>();
      }

      while(pool[n].size() < pool_refill_size) {
	pool[n].push_back(std::make_shared<std::vector<T>>(n));
      }
    }
    
    void returnToPool(std::shared_ptr<std::vector<T>> & p, size_t n) {
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
