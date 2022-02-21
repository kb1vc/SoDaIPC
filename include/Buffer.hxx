#pragma once
#include <string>
#include <exception>
#include <stdexcept>
#include <vector>

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
 * @file Buffer.hxx
 * @author Matt Reilly (kb1vc)
 * @date Feb 21, 2022
 */

/**
 * @page SoDa::Buffer A buffer object that produces vectors
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
  
    /**
     * @class Buffer
     * @brief An object that contains a vector and makes new
     * versions of itself. That is, buffers are passed as 
     * smart pointers -- properly used, they return their 
     * enclosed storage to a pool when the last interested
     * widget has disposed of it. 
     */
  
  template <typename T>
  class Buffer {
  public:
    /**
     * @brief create a buffer object, but this can only be done
     * through the buffer pool manager that is peculiar to this type. 
     * The buffer contains a vector of type T. The vector can be 
     * manipulated by getting a reference to it via getVec(). 
     * This is a little dodgy, as the vector will live on
     * after this buffer is returned to the pool, making it possible
     * to read or write the contents of this buffer after it
     * has been returned to a free list. 
     *
     * It is up to the programmer to ensure that references
     * are no longer active when the buffer is deleted or the
     * shared pointer is re-assigned. 
     *
     * The intent here is to avoid the overhead in allocating
     * std::vector objects. 
     * 
     * @param len length of the allocated vector
     */
    Buffer(size_t len) {
      vec_p = new std::vector<T>(len); 
    }

    std::vector<T> & getVec() { return *vec_p; }
    

  protected:
    std::vector<T> * vec_p;
    
    Buffer(std::vector<T> * vptr) {
      vec_p = vptr; 
    }
  };

}



    
