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
 * @file MailBox.hxx
 * @author Matt Reilly (kb1vc)
 * @date Feb 21, 2022
 */

/**
 * @page SoDa::MailBox A buffer object that produces vectors
 * from a pool. 
 * 
 * 
 * ## Namespace
 * 
 * SoDa::MailBox is enclosed in the SoDa namespace because it is
 * inevitiable that there are lots of classes out there called
 * "BUffer."  Perhaps you have written one of them.  Naming a class "MailBox"
 * is like naming a street "Oak:" It might make lots of sense, but
 * you're going to have to reconcile yourself that there's a street 
 * with the same name one town over and sometimes your pizza is going
 * to get mis-routed. 
 * 
 * So MailBox is in the SoDa namespace.  SoDa is from 
 * <a href="https://kb1vc.github.io/SoDaRadio/">SoDaRadio</a> though the SoDa::MailBox 
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
     * @class MailBox<T>
     * @brief Accept messages and distribute them to multiple subscribers
     *
     * @yparam T Type of message that will be found in this mailbox
     */
  
  template<typename T>
  class MailBox {
  public:
    /**
     * @brief Create a mailbox. All subscribers can put and get 
     * messages from the mailbox. 
     *
     * Messages are assumed to be std::shared_ptr as a message pointer
     * will be released from the message queue when a subscriber takes posesion of it. 
     * 
     * Each subscriber gets  message queue.  It is up to the subscriber
     * to "read the mail"
     */
    MailBox(std::string & name) : name(name) {
      vec_p = new std::vector<T>(len); 
    }

    /**
     * @brief Subscribe the caller to a mailbox.  There may be 
     * multiple subscribers to the same mailbox.  A copy of each
     * message will be reserved for each caller. 
     * 
     * @returns subscriber ID. 
     */
    int subscribe() {
      // what will the subscriber number be? 
      int ret = subscribers.size();

      // make a subscriber queue
      subscribers.push_back(std::queue<T>)
    }

    std::string & const getName() { return name; }

    T get(int subscriber_id) {
      if(nsubscribers.size() <= subscriber_id) {
	throw MailBox::Exception(this, "::get() No such subscriber", subscriber_id);
      }
      if(subscribers[subscriber_id].empty()) {
	return T(0);
      }
      else {
	T ret = subscribers[subscriber_id].front();
	subscribers[subscriber_id].pop_front();
      }
    }
    
    void put(int subscriber_id, T msg) {
      if(subscribers.size() <= subscriber_id) {
	throw MailBox::Exception(this, "::put() No such subscriber", subscriber_id);
      }
      else {
	subscribers[subscriber_id].push_back(msg);
      }
    }

    void clear(int subscriber_id) {
      if(nsubscribers.size() <= subscriber_id) {
	throw MailBox::Exception(this, "::get() No such subscriber", subscriber_id);
      }
      while(!subscribers[subscriber_id].empty()) {
	subscribers[subscriber_id].pop_front();
      }
    }


  protected:
    std::string name; 
    
  };

}



    
