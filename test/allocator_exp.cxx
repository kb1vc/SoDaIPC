#include <memory>
#include <vector>
#include <iostream>
#include <map>
#include <stack>
#include <chrono>
#include <mutex>
template <typename T>
class ExpAlloc {
public:
  typedef T value_type;
  ExpAlloc() noexcept {
    //std::cerr << "Creating allocator\n";
  }
  
  template <typename U> ExpAlloc(const ExpAlloc<U>&) noexcept 
  {
    //std::cerr << " In custom allocator!\n";
  }

  T* allocate(std::size_t n) {
    std::lock_guard<std::mutex> lock(mtx);
    //std::cerr << " allocating n = " << n << "\n";
    
    if(buffer_pool.count(n) == 0) {
      auto nq = new std::stack<void*>;
      //std::cerr << "create buffer pool stack ;) " << nq << "\n";
      buffer_pool[n] = std::stack<void*>(); // nq; 
    }

    if(buffer_pool[n].empty()) {
      initBufferPool(n, 5);
    }

    void * buf = buffer_pool[n].top(); buffer_pool[n].pop();
    //std::cerr << "Allocated " << ((void *) buf) << "\n";
    
    return (T*)(buf);
  }
  void deallocate(T* p, std::size_t n) {
    std::lock_guard<std::mutex> lock(mtx);    
    //std::cerr << " deallocating  n = " << n << " buffer_pool[n] = " << buffer_pool[n] << " p = " << p << "\n";    
    char * bp = (char*) p;
    //std::cerr << "#";
    initBufferPool(n, 2);
    //std::cerr << "Buffer pool[" << n << "].size() " << buffer_pool[n]->size() << " bp = " << bp << "\n";
    buffer_pool[n].push(p);
    //std::cerr << "_";
  }

  void initBufferPool(size_t s, int num) {
    // first allocate the stuff
    char * abuf = new char(s * num);
    for(int b = 0; b < num; b++) {
      //std::cerr << "allocated " << ((void *) abuf) << " into buffer pool[" << s << "]\n";
      buffer_pool[s].push(abuf);
      abuf += s; 
    }
  }

  std::mutex mtx; 
  std::map<int, std::stack<void *>> buffer_pool; 
};

template <typename T, typename U>
constexpr bool operator==(const ExpAlloc<T>, const ExpAlloc<U>&) noexcept {
  return true; }


template <typename T, typename U>
constexpr bool operator!=(const ExpAlloc<T>, const ExpAlloc<U>&) noexcept {
  return false; 
}


template<typename T> int get1(T & v) { return v[1]; }

int showProblem() {
  std::vector<int> int_vec(10);
  std::vector<int, ExpAlloc<int>> int_alloc_vec(10);

  return get1(int_vec) + get1(int_alloc_vec);
}

template<typename T> int doSum(T & iv) {
  int ret = 0; 
  for(auto & v: iv) {
    ret += v; 
  }
  return ret; 
}

template<typename V>
int doTest(int len, int j) {
  int vlen = 9 + (j * j * j) % 88; 
  std::vector<V*> vvp(vlen); 
  
  //  std::cerr << "vlen = " << vlen << "\n";
  int k = 0; 
  for(k = 0; k < vlen; k++) {
    vvp[k] = new V(len);
    //    std::cerr << "vvp[" << k << "] = " << vvp[k] << "\n";    
    for(auto & e : *(vvp[k])) {
      e = k * j; 
    }
  }

  int jsum = 0; 
  for(int i = 0; i < 37; i++) {
    int idx = (i * 5) % 37;
    if(idx >= vvp.size()) continue; 

    // std::cerr << "idx = " << idx << " vvp[idx] = " << vvp[idx] << "\n";
    
    jsum += doSum(*vvp[idx]);
    delete vvp[idx]; 
  }
  
  return jsum; 
}

int doCustom(int len, int j) {
  int ret; 
  {
    std::vector<int, ExpAlloc<int>> r(len);
  
    for(int i = 0; i < len; i++) {
      r[i] = i * j; 
    }


    //ret = doSum<std::vector<int, ExpAlloc<int>>> (r);
    ret = doSum (r);    
    //std::cerr << "=";
  }
  //std::cerr << "!";
  return ret; 
}

int doNormal(int len, int j) {
  std::vector<int> r(len);
  
  for(int i = 0; i < len; i++) {
    r[i] = i * j; 
  }
    
  return doSum<std::vector<int>>(r);
}

int main() {

  int sumval = 0;
#if 1
  std::chrono::steady_clock::time_point custom_start = std::chrono::steady_clock::now();
  for(int i = 0; i < 100; i++) {
    //std::cerr << "+";
    // sumval += doCustom(100000, i);
    sumval += doTest<std::vector<int, ExpAlloc<int>>>(100000, i);
    //std::cerr << "-";    
  }
  std::chrono::steady_clock::time_point custom_end = std::chrono::steady_clock::now();
  std::cout << "Custom alloc took " << std::chrono::duration_cast<std::chrono::microseconds>(custom_end - custom_start).count() << "us\n";
#endif
  std::chrono::steady_clock::time_point normal_start = std::chrono::steady_clock::now();
  for(int i = 0; i < 100; i++) {
    //sumval += doNormal(100000, i);
    sumval += doTest<std::vector<int>>(100000, i);    
  }
  std::chrono::steady_clock::time_point normal_end = std::chrono::steady_clock::now();
  std::cout << "Normal alloc took " << std::chrono::duration_cast<std::chrono::microseconds>(normal_end - normal_start).count() << "us\n";

  
  return 0; 
}
