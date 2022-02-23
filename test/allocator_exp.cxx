#include <memory>
#include <vector>
#include <iostream>
#include <map>
#include <queue>
#include <chrono>

template <typename T>
class ExpAlloc {
public:
  typedef T value_type;
  ExpAlloc() noexcept {
    std::cerr << "Creating allocator\n";
  }
  
  template <typename U> ExpAlloc(const ExpAlloc<U>&) noexcept 
  {
    std::cerr << " In custom allocator!\n";
  }

  T* allocate(std::size_t n) {
    std::cerr << " allocating n = " << n << "\n";
    if(buffer_pool.count(n) == 0) {
      auto nq = new std::queue<char*>;
      std::cerr << "create buffer pool queue ;) " << nq << "\n";
      buffer_pool[n] = nq; 
    }

    if(buffer_pool[n]->empty()) {
      initBufferPool(n, 5);
    }

    char * buf = buffer_pool[n]->front(); buffer_pool[n]->pop();
    std::cerr << "Allocated " << ((void *) buf) << "\n";
    
    return (T*)(buf);
  }
  void deallocate(T* p, std::size_t n) {
    std::cerr << " deallocating  n = " << n << " buffer_pool[n] = " << buffer_pool[n] << " p = " << p << "\n";    
    char * bp = (char*) p;
    std::cerr << "#";
    buffer_pool[n]->push(bp);
    std::cerr << "_";
  }

  void initBufferPool(size_t s, int num) {
    // first allocate the stuff
    char * abuf = new char(s * num);
    for(int b = 0; b < num; b++) {
      buffer_pool[s]->push(abuf);
      abuf += s; 
    }
  }

  std::map<int, std::queue<char *> *> buffer_pool; 
};

template <typename T, typename U>
constexpr bool operator==(const ExpAlloc<T>, const ExpAlloc<U>&) noexcept {
  return true; }


template <typename T, typename U>
constexpr bool operator!=(const ExpAlloc<T>, const ExpAlloc<U>&) noexcept {
  return false; 
}


int get1(std::vector<int> v) { return v[1]; }

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
int doCustom(int len, int j) {
  int ret; 
  {
    std::vector<int, ExpAlloc<int>> r(len);
  
    for(int i = 0; i < len; i++) {
      r[i] = i * j; 
    }


    ret = doSum<std::vector<int, ExpAlloc<int>>> (r);
    std::cerr << "=";
  }
  std::cerr << "!";
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
  std::chrono::steady_clock::time_point custom_start = std::chrono::steady_clock::now();
  for(int i = 0; i < 1000; i++) {
    std::cerr << "+";
    sumval += doCustom(100000, i);
    std::cerr << "-";    
  }
  std::chrono::steady_clock::time_point custom_end = std::chrono::steady_clock::now();
  std::cout << "Custom alloc took " << std::chrono::duration_cast<std::chrono::microseconds>(custom_end - custom_start).count() << "us\n";

  std::chrono::steady_clock::time_point normal_start = std::chrono::steady_clock::now();
  for(int i = 0; i < 1000; i++) {
    sumval += doNormal(100000, i);
  }
  std::chrono::steady_clock::time_point normal_end = std::chrono::steady_clock::now();
  std::cout << "Normal alloc took " << std::chrono::duration_cast<std::chrono::microseconds>(normal_end - normal_start).count() << "us\n";

  
  return 0; 
}
