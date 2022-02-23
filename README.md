# SoDa:: IPC

Simple inter-thread (so I guess not "inter-process") communication
widgets.  These will be used in SoDaRadio and other stuff. Licensed
under the BSD 2-Clause license.

## SoDa::Buffer

This is a wrapper for the std::vector class.  It isn't a vector, but
you can get one out of a buffer by calling getVec(). The idea here is
to provide a widget that BufferPool can allocate that will return the
vector storage to a pool when the shared pointer to its corresponding
buffer is "released."

You might think that this might be done with allocator traits. But an experiment revealed that this is more noxious than it looks. In particular, I couldn't find a solution to the problem with the type incompatibility between ... well, like this:
```
int get1(std::vector<int> v) { return v[1]; }

int showProblem() {
  std::vector<int> int_vec(10);
  std::vector<int, ExpAlloc<int>> int_alloc_vec(10);

  return get1(int_vec) + get1(int_alloc_vec);
}
```
results in this: (from clang)
```
/junk/test/allocator_exp.cxx:75:26: error: no matching function for call to 'get1'
  return get1(int_vec) + get1(int_alloc_vec);
                         ^~~~
/junk/test/allocator_exp.cxx:69:5: note: candidate function not viable: no known
      conversion from 'vector<[...], ExpAlloc<int>>' to 'vector<[...], (default) std::allocator<int>>' for 1st argument
int get1(std::vector<int> v) { return v[1]; }
```

Pretty nasty, huh? So either I change all the other functions that consume vectors, or ....

And so the concept of a SoDa::Buffer -- it contains a vector and it
can re-use the storage from that vector without incurring the overhead
of creating a new vector.  Who knows if this is the "right" thing to do, but vector creation can be expensive, especially for long vectors.  With SoDa::Buffer, we don't need to: we've got the SoDa::BufferPool widget:


## SoDa::BufferPool 

SoDa::BufferPool allocates SoDa::Buffers, returning a shared_ptr to each buffer. When the shared_ptr goes out of scope or otherwise dies, the destructor for the Buffer object (actually a subclass of Buffer) puts the enclosed vector into a storage pool. The Buffer creator can allocate buffers of arbitrary size. A pool is created for each size.  

## SoDa::MailBox


## Testing and Using it all

Take a look at the CMakeLists.txt file and BufferTest.cxx and MailBoxTest.cxx in the test
directory.

To build an install in a particular directory -- do this: 
```
cmake -DCMAKE_PREFIX_PATH=${HOME}/my_tools ../
make install
```
The default prefix path is /usr/local.

The build will deposit doxygen generated docs in
```
<prefix>/share/SoDaIPC/doc/html
```


Did that work?

