# TODO explore those

## `tsan` error on commit `eda143f6cd35a850178249fa5d6e31400eefaaff`

```bats
suite.bats
   configuring and building with preset 'tsan_d' ... done
 ✓ sanitizers work as expected [147]
 ✗ dummy app reports all markers with no sanitizer issues [230]
   (from function `refute_output' in file tests/integration/external/bats-helper/bats-assert/src/refute_output.bash, line 189,
    in test file tests/integration/suite/suite.bats, line 92)
     `refute_output --partial "Sanitizer"' failed

   -- output should not contain substring --
   substring (1 lines):
     Sanitizer
   output (50 lines):
     ==================
     WARNING: ThreadSanitizer: data race (pid=31513)
       Write of size 8 at 0x7b0400000070 by thread T10:
         #0 pipe ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:1726 (libtsan.so.0+0x3ea28)
         #1 __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long) ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 (libubsan.so.1+0x20102)
         #2 ~_State_impl /usr/include/c++/11/bits/std_thread.h:201 (rsm_dummy_app+0x1569d)
         #3 <null> <null> (libstdc++.so.6+0xdc25b)

       Previous write of size 8 at 0x7b0400000070 by thread T8:
         #0 pipe ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:1726 (libtsan.so.0+0x3ea28)
         #1 __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long) ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 (libubsan.so.1+0x20102)
         #2 ~_State_impl /usr/include/c++/11/bits/std_thread.h:201 (rsm_dummy_app+0x1569d)
         #3 <null> <null> (libstdc++.so.6+0xdc25b)

       Thread T10 (tid=31524, running) created by main thread at:
         #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
         #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xdc328)
         #2 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/ext/new_allocator.h:162 (rsm_dummy_app+0x1439d)
         #3 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/alloc_traits.h:516 (rsm_dummy_app+0x1360c)
         #4 emplace_back<main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/vector.tcc:115 (rsm_dummy_app+0x11af3)
         #5 main /home/lukas/tmp/rsm/tests/dummy_app/main.cpp:97 (rsm_dummy_app+0x10a05)

       Thread T8 (tid=31522, finished) created by main thread at:
         #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
         #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xdc328)
         #2 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/ext/new_allocator.h:162 (rsm_dummy_app+0x1439d)
         #3 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/alloc_traits.h:516 (rsm_dummy_app+0x1360c)
         #4 emplace_back<main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/vector.tcc:115 (rsm_dummy_app+0x11af3)
         #5 main /home/lukas/tmp/rsm/tests/dummy_app/main.cpp:97 (rsm_dummy_app+0x10a05)

     SUMMARY: ThreadSanitizer: data race ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 in __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long)
     ==================
     deduced RSM_DEFAULT_BLOCK_SIZE: 2
     31515: 'scoped 1', color -1, tag -1: 4598110844062 -> 4598112344029 ~ 1499967 [ns]
     31516: 'scoped 2', color -1, tag -1: 4598113291969 -> 4598114504851 ~ 1212882 [ns]
     31517: 'scoped 3', color -1, tag -1: 4598115096163 -> 4598116346416 ~ 1250253 [ns]
     31518: 'scoped 4', color -1, tag -1: 4598117205703 -> 4598118321244 ~ 1115541 [ns]
     31519: 'scoped 5 (macro with both default color and tag)', color -1, tag -1: 4598118946611 -> 4598120053352 ~ 1106741 [ns]
     31520: 'scoped 6 (macro with explicit color and default tag)', color 1, tag -1: 4598120496072 -> 4598121673527 ~ 1177455 [ns]
     31521: 'scoped 7 (macro with both explicit color and tag)', color 1, tag 2: 4598122283364 -> 4598123435281 ~ 1151917 [ns]
     31513: 'loop', color -1, tag -1: 4598076172134 -> 4598109726351 ~ 33554217 [ns]
     31513: 'int store', color -1, tag -1: 4598109744355 -> 4598109744455 ~ 100 [ns]
     31513: 'int load', color -1, tag -1: 4598109745076 -> 4598109745127 ~ 51 [ns]
     31513: 'third local macro marker testing no shadowing occurs', color -1, tag -1: 4598123565498 -> 4598123566039 ~ 541 [ns]
     31513: 'second local macro marker testing no shadowing occurs', color -1, tag -1: 4598123564897 -> 4598123571078 ~ 6181 [ns]
     31513: 'first local macro marker', color -1, tag -1: 4598123563304 -> 4598123603981 ~ 40677 [ns]
     31522: 'scoped 8 (in 3 various parallel threads)', color -1, tag 0: 4598124110723 -> 4598127207180 ~ 3096457 [ns]
     31524: 'scoped 8 (in 3 various parallel threads)', color -1, tag 2: 4598126055374 -> 4598127210566 ~ 1155192 [ns]
     31523: 'scoped 8 (in 3 various parallel threads)', color -1, tag 1: 4598125802914 -> 4598203995876 ~ 78192962 [ns]
     ThreadSanitizer: reported 1 warnings
   --


2 tests, 1 failure in 1 seconds
```
