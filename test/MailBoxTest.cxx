#include "../include/Buffer.hxx"
#include "../include/BufferPool.hxx"
#include "../include/MailBox.hxx"
#include <memory>

#include <iostream>

template <typename T> void doFunc(T & v) {
  v = T(0);
}

int main() {
  // create a mailbox
  SoDa::MailBox<std::shared_ptr<SoDa::Buffer<int>>> mailbox("TestMailBox"); 
  // create a buffer pool
  SoDa::BufferPool<int> pool("TestPool", 5);
  
  // make two subscriptions
  std::vector<int> subs;
  subs.push_back(mailbox.subscribe());
  subs.push_back(mailbox.subscribe());  

  
  // try a bad get
  try {
    mailbox.get(99);
  }
  catch(SoDa::MailBoxMissingSubscriberException & e) {
    std::cerr << e.what() << "\n";
  }

  // now push three messages from each subscriber
  for(auto s : subs) {
    std::cout << "Sending from subscriber " << s << "\n";
    auto bp = pool.getFromPool(5);
    bp->getVec()[2] = s; 
    mailbox.put(bp);
  }
  
  // now get messages until we are out 
  for(auto s : subs) {
    std::cout << "Receiving to subscriber " << s << "\n";
    std::shared_ptr<SoDa::Buffer<int>> p; 
    while((p = mailbox.get(s)) != nullptr) {
      std::cout << "subscriber " << s << " got message from subscriber " << p->getVec()[2] << "\n";
    }
    std::cout << "No more messages for subscriber " << s << "\n";
  }
}
