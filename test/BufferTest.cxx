#include "../include/Buffer.hxx"
#include "../include/BufferPool.hxx"
#include <type_traits>

#include <iostream>

template <typename T> void doFunc(T & v) {
  v = T(0);
}

int main() {
  // create a buffer
  SoDa::Buffer<int> buf(10); 


  buf.getVec()[3] = 5;
  std::cerr << "buf[3] = " << buf.getVec()[3] << "\n";

  // now create a buffer pool
  SoDa::BufferPool<int> pool("TestPool", 5);
  
  // allocate 8 buffers and fiddle with them
  for(int i = 0; i < 8; i++) {
    std::shared_ptr<SoDa::Buffer<int>> el = pool.getFromPool(3);
    el->getVec()[2] = i;
    std::cerr << "el->getVec()[2] = " << el->getVec()[2] << "\n";
  }
  

  std::cerr << "\n\nHere goes!\n";
  auto el = pool.getFromPool(3);  

  doFunc(el);
  std::cerr << "There it went\n";
}
