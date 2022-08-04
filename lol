==59349== Memcheck, a memory error detector
==59349== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==59349== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==59349== Command: ./lush
==59349== 
==59349== Thread 3:
==59349== Invalid read of size 8
==59349==    at 0x40286C8: strncmp (strcmp.S:172)
==59349==    by 0x400668D: is_dst (dl-load.c:216)
==59349==    by 0x400810E: _dl_dst_count (dl-load.c:253)
==59349==    by 0x400810E: expand_dynamic_string_token (dl-load.c:395)
==59349==    by 0x40082B7: fillin_rpath.isra.0 (dl-load.c:483)
==59349==    by 0x4008602: decompose_rpath (dl-load.c:654)
==59349==    by 0x400ABF5: cache_rpath (dl-load.c:696)
==59349==    by 0x400ABF5: cache_rpath (dl-load.c:677)
==59349==    by 0x400ABF5: _dl_map_object (dl-load.c:2165)
==59349==    by 0x4003494: openaux (dl-deps.c:64)
==59349==    by 0x51ABC27: _dl_catch_exception (dl-error-skeleton.c:208)
==59349==    by 0x4003C7B: _dl_map_object_deps (dl-deps.c:248)
==59349==    by 0x400EA0E: dl_open_worker_begin (dl-open.c:592)
==59349==    by 0x51ABC27: _dl_catch_exception (dl-error-skeleton.c:208)
==59349==    by 0x400DF99: dl_open_worker (dl-open.c:782)
==59349==  Address 0x5534e69 is 9 bytes inside a block of size 15 alloc'd
==59349==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==59349==    by 0x40271FF: malloc (rtld-malloc.h:56)
==59349==    by 0x40271FF: strdup (strdup.c:42)
==59349==    by 0x4008594: decompose_rpath (dl-load.c:629)
==59349==    by 0x400ABF5: cache_rpath (dl-load.c:696)
==59349==    by 0x400ABF5: cache_rpath (dl-load.c:677)
==59349==    by 0x400ABF5: _dl_map_object (dl-load.c:2165)
==59349==    by 0x4003494: openaux (dl-deps.c:64)
==59349==    by 0x51ABC27: _dl_catch_exception (dl-error-skeleton.c:208)
==59349==    by 0x4003C7B: _dl_map_object_deps (dl-deps.c:248)
==59349==    by 0x400EA0E: dl_open_worker_begin (dl-open.c:592)
==59349==    by 0x51ABC27: _dl_catch_exception (dl-error-skeleton.c:208)
==59349==    by 0x400DF99: dl_open_worker (dl-open.c:782)
==59349==    by 0x51ABC27: _dl_catch_exception (dl-error-skeleton.c:208)
==59349==    by 0x400E34D: _dl_open (dl-open.c:883)
==59349== 
==59349== Invalid read of size 8
==59349==    at 0x40286C8: strncmp (strcmp.S:172)
==59349==    by 0x400668D: is_dst (dl-load.c:216)
==59349==    by 0x4007F79: _dl_dst_substitute (dl-load.c:295)
==59349==    by 0x40082B7: fillin_rpath.isra.0 (dl-load.c:483)
==59349==    by 0x4008602: decompose_rpath (dl-load.c:654)
==59349==    by 0x400ABF5: cache_rpath (dl-load.c:696)
==59349==    by 0x400ABF5: cache_rpath (dl-load.c:677)
==59349==    by 0x400ABF5: _dl_map_object (dl-load.c:2165)
==59349==    by 0x4003494: openaux (dl-deps.c:64)
==59349==    by 0x51ABC27: _dl_catch_exception (dl-error-skeleton.c:208)
==59349==    by 0x4003C7B: _dl_map_object_deps (dl-deps.c:248)
==59349==    by 0x400EA0E: dl_open_worker_begin (dl-open.c:592)
==59349==    by 0x51ABC27: _dl_catch_exception (dl-error-skeleton.c:208)
==59349==    by 0x400DF99: dl_open_worker (dl-open.c:782)
==59349==  Address 0x5534e69 is 9 bytes inside a block of size 15 alloc'd
==59349==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==59349==    by 0x40271FF: malloc (rtld-malloc.h:56)
==59349==    by 0x40271FF: strdup (strdup.c:42)
==59349==    by 0x4008594: decompose_rpath (dl-load.c:629)
==59349==    by 0x400ABF5: cache_rpath (dl-load.c:696)
==59349==    by 0x400ABF5: cache_rpath (dl-load.c:677)
==59349==    by 0x400ABF5: _dl_map_object (dl-load.c:2165)
==59349==    by 0x4003494: openaux (dl-deps.c:64)
==59349==    by 0x51ABC27: _dl_catch_exception (dl-error-skeleton.c:208)
==59349==    by 0x4003C7B: _dl_map_object_deps (dl-deps.c:248)
==59349==    by 0x400EA0E: dl_open_worker_begin (dl-open.c:592)
==59349==    by 0x51ABC27: _dl_catch_exception (dl-error-skeleton.c:208)
==59349==    by 0x400DF99: dl_open_worker (dl-open.c:782)
==59349==    by 0x51ABC27: _dl_catch_exception (dl-error-skeleton.c:208)
==59349==    by 0x400E34D: _dl_open (dl-open.c:883)
==59349== 
Thread aborted: Shader file loading errorselected queue: 0

==59349== 
==59349== HEAP SUMMARY:
==59349==     in use at exit: 1,600,117 bytes in 6,457 blocks
==59349==   total heap usage: 18,352 allocs, 11,895 frees, 4,962,434 bytes allocated
==59349== 
==59349== LEAK SUMMARY:
==59349==    definitely lost: 56 bytes in 2 blocks
==59349==    indirectly lost: 0 bytes in 0 blocks
==59349==      possibly lost: 250,661 bytes in 1,001 blocks
==59349==    still reachable: 1,349,400 bytes in 5,454 blocks
==59349==         suppressed: 0 bytes in 0 blocks
==59349== Rerun with --leak-check=full to see details of leaked memory
==59349== 
==59349== For lists of detected and suppressed errors, rerun with: -s
==59349== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
