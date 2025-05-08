# TODO explore those

## `tsan` error

### on commit `eda143f6cd35a850178249fa5d6e31400eefaaff`

```bash
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

### on commit `4ecfd631d95377881ca79a29bef856f0559f2e90`

```bash
$ time ./tests.bash --bats --rounds 2000
...
Executing bats tests round no. 59/2000:
...
suite.bats
   configuring and building with preset 'tsan_d' ... done
 ✓ sanitizers work as expected [153]
 ✗ dummy app reports all markers with no sanitizer issues [236]
   (from function `refute_output' in file tests/integration/external/bats-helper/bats-assert/src/refute_output.bash, line 189,
    in test file tests/integration/suite/suite.bats, line 92)
     `refute_output --partial "Sanitizer"' failed

   -- output should not contain substring --
   substring (1 lines):
     Sanitizer
   output (50 lines):
     ==================
     WARNING: ThreadSanitizer: data race (pid=102122)
       Write of size 8 at 0x7b0400000070 by thread T8:
         #0 pipe ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:1726 (libtsan.so.0+0x3ea28)
         #1 __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long) ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 (libubsan.so.1+0x20102)
         #2 ~_State_impl /usr/include/c++/11/bits/std_thread.h:201 (rsm_dummy_app+0x1569d)
         #3 <null> <null> (libstdc++.so.6+0xdc25b)

       Previous write of size 8 at 0x7b0400000070 by thread T10:
         #0 pipe ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:1726 (libtsan.so.0+0x3ea28)
         #1 __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long) ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 (libubsan.so.1+0x20102)
         #2 ~_State_impl /usr/include/c++/11/bits/std_thread.h:201 (rsm_dummy_app+0x1569d)
         #3 <null> <null> (libstdc++.so.6+0xdc25b)

       Thread T8 (tid=102131, running) created by main thread at:
         #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
         #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xdc328)
         #2 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/ext/new_allocator.h:162 (rsm_dummy_app+0x1439d)
         #3 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/alloc_traits.h:516 (rsm_dummy_app+0x1360c)
         #4 emplace_back<main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/vector.tcc:115 (rsm_dummy_app+0x11af3)
         #5 main /home/lukas/tmp/rsm/tests/dummy_app/main.cpp:97 (rsm_dummy_app+0x10a05)

       Thread T10 (tid=102133, finished) created by main thread at:
         #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
         #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xdc328)
         #2 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/ext/new_allocator.h:162 (rsm_dummy_app+0x1439d)
         #3 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/alloc_traits.h:516 (rsm_dummy_app+0x1360c)
         #4 emplace_back<main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/vector.tcc:115 (rsm_dummy_app+0x11af3)
         #5 main /home/lukas/tmp/rsm/tests/dummy_app/main.cpp:97 (rsm_dummy_app+0x10a05)

     SUMMARY: ThreadSanitizer: data race ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 in __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long)
     ==================
     deduced RSM_DEFAULT_BLOCK_SIZE: 2
     102124: 'scoped 1', color -1, tag -1: 6227955882147 -> 6227957116700 ~ 1234553 [ns]
     102125: 'scoped 2', color -1, tag -1: 6227957938390 -> 6227959096327 ~ 1157937 [ns]
     102126: 'scoped 3', color -1, tag -1: 6227959775677 -> 6227960888158 ~ 1112481 [ns]
     102127: 'scoped 4', color -1, tag -1: 6227961191013 -> 6227962290730 ~ 1099717 [ns]
     102128: 'scoped 5 (macro with both default color and tag)', color -1, tag -1: 6227962584428 -> 6227963626285 ~ 1041857 [ns]
     102129: 'scoped 6 (macro with explicit color and default tag)', color 1, tag -1: 6227963968084 -> 6227965066639 ~ 1098555 [ns]
     102130: 'scoped 7 (macro with both explicit color and tag)', color 1, tag 2: 6227965362150 -> 6227966454263 ~ 1092113 [ns]
     102122: 'loop', color -1, tag -1: 6227915169133 -> 6227954620352 ~ 39451219 [ns]
     102122: 'int store', color -1, tag -1: 6227954629339 -> 6227954629429 ~ 90 [ns]
     102122: 'int load', color -1, tag -1: 6227954629940 -> 6227954629990 ~ 50 [ns]
     102122: 'third local macro marker testing no shadowing occurs', color -1, tag -1: 6227966527863 -> 6227966528043 ~ 180 [ns]
     102122: 'second local macro marker testing no shadowing occurs', color -1, tag -1: 6227966527682 -> 6227966529816 ~ 2134 [ns]
     102122: 'first local macro marker', color -1, tag -1: 6227966527141 -> 6227966543322 ~ 16181 [ns]
     102133: 'scoped 8 (in 3 various parallel threads)', color -1, tag 2: 6227968756803 -> 6227969919991 ~ 1163188 [ns]
     102132: 'scoped 8 (in 3 various parallel threads)', color -1, tag 1: 6227968488344 -> 6228050147644 ~ 81659300 [ns]
     102131: 'scoped 8 (in 3 various parallel threads)', color -1, tag 0: 6227966789890 -> 6227969924560 ~ 3134670 [ns]
     ThreadSanitizer: reported 1 warnings
   --


2 tests, 1 failure in 1 seconds
```

### on commit `6baca2b20a59ef02636d452619c8b58d7fb7ffc2`

```bash
$ time ./tests.bash --bats --rounds 2000
...
Executing bats tests round no. 124/2000:
...
suite.bats
   configuring and building with preset 'tsan_d' ... done
 ✓ sanitizers work as expected [157]
 ✗ dummy app reports all markers with no sanitizer issues [238]
   (from function `refute_output' in file tests/integration/external/bats-helper/bats-assert/src/refute_output.bash, line 189,
    in test file tests/integration/suite/suite.bats, line 92)
     `refute_output --partial "Sanitizer"' failed

   -- output should not contain substring --
   substring (1 lines):
     Sanitizer
   output (50 lines):
     ==================
     WARNING: ThreadSanitizer: data race (pid=220221)
       Write of size 8 at 0x7b0400000070 by thread T8:
         #0 pipe ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:1726 (libtsan.so.0+0x3ea28)
         #1 __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long) ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 (libubsan.so.1+0x20102)
         #2 ~_State_impl /usr/include/c++/11/bits/std_thread.h:201 (rsm_dummy_app+0x156a1)
         #3 <null> <null> (libstdc++.so.6+0xdc25b)

       Previous write of size 8 at 0x7b0400000070 by thread T10:
         #0 pipe ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:1726 (libtsan.so.0+0x3ea28)
         #1 __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long) ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 (libubsan.so.1+0x20102)
         #2 ~_State_impl /usr/include/c++/11/bits/std_thread.h:201 (rsm_dummy_app+0x156a1)
         #3 <null> <null> (libstdc++.so.6+0xdc25b)

       Thread T8 (tid=220230, running) created by main thread at:
         #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
         #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xdc328)
         #2 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/ext/new_allocator.h:162 (rsm_dummy_app+0x143a1)
         #3 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/alloc_traits.h:516 (rsm_dummy_app+0x13610)
         #4 emplace_back<main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/vector.tcc:115 (rsm_dummy_app+0x11af7)
         #5 main /home/lukas/tmp/rsm/tests/dummy_app/main.cpp:97 (rsm_dummy_app+0x10a0a)

       Thread T10 (tid=220232, finished) created by main thread at:
         #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
         #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xdc328)
         #2 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/ext/new_allocator.h:162 (rsm_dummy_app+0x143a1)
         #3 construct<std::thread, main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/alloc_traits.h:516 (rsm_dummy_app+0x13610)
         #4 emplace_back<main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/vector.tcc:115 (rsm_dummy_app+0x11af7)
         #5 main /home/lukas/tmp/rsm/tests/dummy_app/main.cpp:97 (rsm_dummy_app+0x10a0a)

     SUMMARY: ThreadSanitizer: data race ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 in __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long)
     ==================
     deduced RSM_DEFAULT_BLOCK_SIZE: 2
     220223: 'scoped 1', color -1, tag -1: 8699125396194 -> 8699126491092 ~ 1094898 [ns]
     220224: 'scoped 2', color -1, tag -1: 8699126961274 -> 8699128052686 ~ 1091412 [ns]
     220225: 'scoped 3', color -1, tag -1: 8699128716666 -> 8699129824729 ~ 1108063 [ns]
     220226: 'scoped 4', color -1, tag -1: 8699130124318 -> 8699131224165 ~ 1099847 [ns]
     220227: 'scoped 5 (macro with both default color and tag)', color -1, tag -1: 8699131513955 -> 8699132594576 ~ 1080621 [ns]
     220228: 'scoped 6 (macro with explicit color and default tag)', color 1, tag -1: 8699133766421 -> 8699134891696 ~ 1125275 [ns]
     220229: 'scoped 7 (macro with both explicit color and tag)', color 1, tag 2: 8699135199420 -> 8699136293637 ~ 1094217 [ns]
     220221: 'loop', color -1, tag -1: 8699084788139 -> 8699124044699 ~ 39256560 [ns]
     220221: 'int store', color -1, tag -1: 8699124060599 -> 8699124060749 ~ 150 [ns]
     220221: 'int load', color -1, tag -1: 8699124061340 -> 8699124061400 ~ 60 [ns]
     220221: 'third local macro marker testing no shadowing occurs', color -1, tag -1: 8699136367307 -> 8699136367487 ~ 180 [ns]
     220221: 'second local macro marker testing no shadowing occurs', color -1, tag -1: 8699136367107 -> 8699136369010 ~ 1903 [ns]
     220221: 'first local macro marker', color -1, tag -1: 8699136366576 -> 8699136381113 ~ 14537 [ns]
     220232: 'scoped 8 (in 3 various parallel threads)', color -1, tag 2: 8699138564878 -> 8699139716594 ~ 1151716 [ns]
     220230: 'scoped 8 (in 3 various parallel threads)', color -1, tag 0: 8699136614106 -> 8699139736512 ~ 3122406 [ns]
     220231: 'scoped 8 (in 3 various parallel threads)', color -1, tag 1: 8699138310115 -> 8699216880212 ~ 78570097 [ns]
     ThreadSanitizer: reported 1 warnings
   --


2 tests, 1 failure in 1 seconds
```

### on commit `ff45f1d1f69fea0bd0d26b5a78c72f13ae8450ac`

```bash
$ time ./tests.bash --bats --rounds 2000
...
Executing bats tests round no. 46/2000:
...
suite.bats
   configuring and building with preset 'tsan' ... done
 ✓ sanitizers work as expected [147]
 ✗ dummy app reports all markers with no sanitizer issues [238]
   (from function `refute_output' in file tests/integration/external/bats-helper/bats-assert/src/refute_output.bash, line 189,
    in test file tests/integration/suite/suite.bats, line 92)
     `refute_output --partial "Sanitizer"' failed

   -- output should not contain substring --
   substring (1 lines):
     Sanitizer
   output (80 lines):
     ==================
     WARNING: ThreadSanitizer: data race (pid=266469)
       Write of size 8 at 0x7b0400000070 by thread T36:
         #0 pipe ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:1726 (libtsan.so.0+0x3ea28)
         #1 __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long) ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 (libubsan.so.1+0x20102)
         #2 <null> <null> (libstdc++.so.6+0xdc25b)

       Previous write of size 8 at 0x7b0400000070 by thread T37:
         #0 pipe ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:1726 (libtsan.so.0+0x3ea28)
         #1 __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long) ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 (libubsan.so.1+0x20102)
         #2 <null> <null> (libstdc++.so.6+0xdc25b)

       As if synchronized via sleep:
         #0 nanosleep ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:366 (libtsan.so.0+0x6696c)
         #1 void std::this_thread::sleep_for<long, std::ratio<1l, 1000l> >(std::chrono::duration<long, std::ratio<1l, 1000l> > const&) /usr/include/c++/11/bits/this_thread_sleep.h:82 (rsm_dummy_app+0xab2a)
         #2 operator() /home/lukas/tmp/rsm/tests/dummy_app/main.cpp:101 (rsm_dummy_app+0xab2a)
         #3 __invoke_impl<void, main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/invoke.h:61 (rsm_dummy_app+0xab2a)
         #4 __invoke<main(int, char const**)::<lambda()> > /usr/include/c++/11/bits/invoke.h:96 (rsm_dummy_app+0xab2a)
         #5 _M_invoke<0> /usr/include/c++/11/bits/std_thread.h:259 (rsm_dummy_app+0xab2a)
         #6 operator() /usr/include/c++/11/bits/std_thread.h:266 (rsm_dummy_app+0xab2a)
         #7 _M_run /usr/include/c++/11/bits/std_thread.h:211 (rsm_dummy_app+0xab2a)
         #8 <null> <null> (libstdc++.so.6+0xdc252)

       Thread T36 (tid=266506, running) created by main thread at:
         #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
         #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xdc328)
         #2 __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58 (libc.so.6+0x29d8f)

       Thread T37 (tid=266507, finished) created by main thread at:
         #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
         #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xdc328)
         #2 __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58 (libc.so.6+0x29d8f)

     SUMMARY: ThreadSanitizer: data race ../../../../src/libsanitizer/sanitizer_common/sanitizer_posix_libcdep.cpp:276 in __sanitizer::IsAccessibleMemoryRange(unsigned long, unsigned long)
     ==================
     deduced RSM_DEFAULT_BLOCK_SIZE: 2
     266471: 'scoped 1', color -1, tag -1: 9020917937323 -> 9020919035798 ~ 1098475 [ns]
     266472: 'scoped 2', color -1, tag -1: 9020919432701 -> 9020920516418 ~ 1083717 [ns]
     266473: 'scoped 3', color -1, tag -1: 9020920825775 -> 9020922023980 ~ 1198205 [ns]
     266474: 'scoped 4', color -1, tag -1: 9020922458024 -> 9020923742768 ~ 1284744 [ns]
     266475: 'scoped 5 (macro with both default color and tag)', color -1, tag -1: 9020924509658 -> 9020925594416 ~ 1084758 [ns]
     266476: 'scoped 6 (macro with explicit color and default tag)', color 1, tag -1: 9020925893725 -> 9020926975989 ~ 1082264 [ns]
     266477: 'scoped 7 (macro with both explicit color and tag)', color 1, tag 2: 9020927268794 -> 9020928388830 ~ 1120036 [ns]
     266469: 'loop', color -1, tag -1: 9020912411295 -> 9020916937546 ~ 4526251 [ns]
     266469: 'int store', color -1, tag -1: 9020916952354 -> 9020916952664 ~ 310 [ns]
     266469: 'int load', color -1, tag -1: 9020916953255 -> 9020916953336 ~ 81 [ns]
     266469: 'third local macro marker testing no shadowing occurs', color -1, tag -1: 9020928559965 -> 9020928560566 ~ 601 [ns]
     266469: 'second local macro marker testing no shadowing occurs', color -1, tag -1: 9020928559434 -> 9020928563111 ~ 3677 [ns]
     266469: 'first local macro marker', color -1, tag -1: 9020928558202 -> 9020928592757 ~ 34555 [ns]
     266507: 'scoped 8 (in 3 various parallel threads)', color -1, tag 29: 9020939971755 -> 9020941069038 ~ 1097283 [ns]
     266506: 'scoped 8 (in 3 various parallel threads)', color -1, tag 28: 9020939060896 -> 9020941148759 ~ 2087863 [ns]
     266505: 'scoped 8 (in 3 various parallel threads)', color -1, tag 27: 9020938789611 -> 9021022719231 ~ 83929620 [ns]
     266504: 'scoped 8 (in 3 various parallel threads)', color -1, tag 26: 9020938518517 -> 9021022835562 ~ 84317045 [ns]
     266503: 'scoped 8 (in 3 various parallel threads)', color -1, tag 25: 9020938294511 -> 9021022865378 ~ 84570867 [ns]
     266502: 'scoped 8 (in 3 various parallel threads)', color -1, tag 24: 9020937612948 -> 9021022987911 ~ 85374963 [ns]
     266501: 'scoped 8 (in 3 various parallel threads)', color -1, tag 23: 9020937343997 -> 9021023017477 ~ 85673480 [ns]
     266500: 'scoped 8 (in 3 various parallel threads)', color -1, tag 22: 9020937037205 -> 9021023133187 ~ 86095982 [ns]
     266499: 'scoped 8 (in 3 various parallel threads)', color -1, tag 21: 9020936740632 -> 9021023229199 ~ 86488567 [ns]
     266498: 'scoped 8 (in 3 various parallel threads)', color -1, tag 20: 9020936453938 -> 9021023318318 ~ 86864380 [ns]
     266497: 'scoped 8 (in 3 various parallel threads)', color -1, tag 19: 9020936170440 -> 9021023342614 ~ 87172174 [ns]
     266496: 'scoped 8 (in 3 various parallel threads)', color -1, tag 18: 9020935887840 -> 9021023461380 ~ 87573540 [ns]
     266495: 'scoped 8 (in 3 various parallel threads)', color -1, tag 17: 9020935618703 -> 9021023485315 ~ 87866612 [ns]
     266494: 'scoped 8 (in 3 various parallel threads)', color -1, tag 16: 9020935274239 -> 9021023671328 ~ 88397089 [ns]
     266493: 'scoped 8 (in 3 various parallel threads)', color -1, tag 15: 9020934991773 -> 9021023688531 ~ 88696758 [ns]
     266490: 'scoped 8 (in 3 various parallel threads)', color -1, tag 12: 9020933322705 -> 9021023918157 ~ 90595452 [ns]
     266489: 'scoped 8 (in 3 various parallel threads)', color -1, tag 11: 9020932964755 -> 9021023956860 ~ 90992105 [ns]
     266488: 'scoped 8 (in 3 various parallel threads)', color -1, tag 10: 9020932683401 -> 9021023987989 ~ 91304588 [ns]
     266486: 'scoped 8 (in 3 various parallel threads)', color -1, tag 8: 9020932100235 -> 9021024157741 ~ 92057506 [ns]
     266487: 'scoped 8 (in 3 various parallel threads)', color -1, tag 9: 9020932406105 -> 9021024145279 ~ 91739174 [ns]
     266485: 'scoped 8 (in 3 various parallel threads)', color -1, tag 7: 9020931860500 -> 9021024188910 ~ 92328410 [ns]
     266484: 'scoped 8 (in 3 various parallel threads)', color -1, tag 6: 9020931445372 -> 9021024325451 ~ 92880079 [ns]
     266483: 'scoped 8 (in 3 various parallel threads)', color -1, tag 5: 9020931147326 -> 9021024438791 ~ 93291465 [ns]
     266481: 'scoped 8 (in 3 various parallel threads)', color -1, tag 3: 9020930596460 -> 9021024529136 ~ 93932676 [ns]
     266480: 'scoped 8 (in 3 various parallel threads)', color -1, tag 2: 9020930314716 -> 9021024562590 ~ 94247874 [ns]
     266479: 'scoped 8 (in 3 various parallel threads)', color -1, tag 1: 9020929741979 -> 9021024680464 ~ 94938485 [ns]
     266478: 'scoped 8 (in 3 various parallel threads)', color -1, tag 0: 9020929189720 -> 9021024703758 ~ 95514038 [ns]
     266491: 'scoped 8 (in 3 various parallel threads)', color -1, tag 13: 9020934013432 -> 9021024935938 ~ 90922506 [ns]
     266482: 'scoped 8 (in 3 various parallel threads)', color -1, tag 4: 9020930874789 -> 9021024971456 ~ 94096667 [ns]
     266492: 'scoped 8 (in 3 various parallel threads)', color -1, tag 14: 9020934708315 -> 9021024981355 ~ 90273040 [ns]
     ThreadSanitizer: reported 1 warnings
   --


2 tests, 1 failure in 1 seconds
```
