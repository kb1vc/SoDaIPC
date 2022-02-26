#include "../include/Buffer.hxx"
#include "../include/BufferPool.hxx"
#include <type_traits>

#include <iostream>

template <typename T> void doFunc(std::vector<T> & v) {
  v[0] = T(0);
}

int test1() {
  // create a buffer
  SoDa::Buffer<int> buf(10); 
  //std::shared_ptr<SoDa::Buffer<int>> = std::make_shared
  auto buf_p = std::make_shared<SoDa::Buffer<int>>(10); 
  std::vector<int> & vbuf = *buf_p; 

  //buf.getVec()[3] = 5;
  vbuf[3] = 5; 
  std::cerr << "buf[3] = " << buf_p->getVec()[3] << "\n";

  // now create a buffer pool
  SoDa::BufferPool<int> pool("TestPool", 5);
  
  // allocate 8 buffers and fiddle with them
  {
    std::vector<std::shared_ptr<SoDa::Buffer<int>>> bufs;     
    for(int i = 0; i < 8; i++) {
      auto bp = pool.getFromPool(3);    
      bufs.push_back(bp); 
      std::vector<int> & el = *bp;
      el[i % 3] = i * i;
      std::cerr << "el[i] = " << el[i] << "\n";
    }

  
    for(int i = 0; i < 8; i++) {
      std::vector<int> & el = *bufs[i];
      std::cerr << "el[i] = " << el[i % 3] << "\n";
    }
  }
  

  std::cerr << "\n\nHere goes!\n";
  std::vector<int> & el = *pool.getFromPool(3);  

  doFunc(el);
  std::cerr << "There it went\n";

  return 0;
}

int test2() {
  SoDa::BufferPool<int> pool("TestPool", 25);  
  for(int trial = 0; trial < 1000; trial++) {
    // create 100 vectors, and free them
    {
      std::vector<std::shared_ptr<SoDa::Buffer<int>>> bufs;
      for(int i = 0; i < 1000; i++) {
	auto bp = pool.getFromPool(5000);
	bufs.push_back(bp); 
	bp->getVec()[3] = trial;
	bp->getVec()[3000] = trial; 	
      }
    }
  }
  return 0;
}

int main() {
  return test2();
}
