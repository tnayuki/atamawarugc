/*
 * Copyright 2019 Toru Nayuki
 *
 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 *
 * Permission is hereby granted to use or copy this program
 * for any purpose,  provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 */

#include <memory.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/errno.h>
#include <sys/mman.h>

#include <set>
#include <stack>
#include <vector>

namespace atamawarugc {
    class gc {

    private:
        size_t heap_size = 0;
        size_t gc_threshold = 1024 * 1024;

        void *heaps[16];
        char *heap_marks[16];
        int heap_next_free[16];

        std::stack<void *, std::vector<void *> > mark_stack;
        std::set<void *> roots;

        static int nextpow2(int n) {
            if ((n & (n - 1)) == 0) return n;

            int bsr;
            __asm__("bsr %1, %0" : "=r"(bsr) : "r"(n));

            return 1 << (bsr + 1);
        }

        int allocate_heaps() {
            for (int i = 2; i < 48; i++) {
                void *result = mmap((void *)(0x100000000 * i), 0x100000000, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED | MAP_ANON, -1, 0);

                if (result != (void *)EINVAL) {
                    char *start = (char *)result;

                    for (int i = 0; i < 16; i++) {
                        heaps[i] = start + 0x10000000L * i;
                        heap_marks[i] = (char *)calloc(0x10000000 / (64 << (i % 8)), 1);
                        heap_next_free[i] = 0;
                    }

                    return 1;
                }
            }

            return 0;
        }

        void mark_object(void *obj) {
            int heap_no = ((long)obj & 0xf0000000L) >> 28;
            int block_no = ((long)obj & 0x0fffffffL) / (64 << (heap_no % 8));
            int sz = 64 << (heap_no % 8);

            if (heap_marks[heap_no][block_no] == 0) {
                heap_size += sz;
                heap_marks[heap_no][block_no] = 1;

                if (heap_no >= 8) {
                    return;
                }

                obj = (void *)((long)obj & ~(sz - 1L));

                for (void **scan = (void **)obj; scan < (void **)(((uint8_t *)obj) + sz); scan++) {
                    if ((void *)((long)*scan & 0xffffffff00000000L) == heaps[0]) {
                        mark_stack.push(*scan);
                    }
                }
            }
        }

        void mark_from_stack() {
            void *stack_start = pthread_get_stackaddr_np(pthread_self());
            void *stack_end = &stack_start;

            for (void **scan = ((void **)stack_start) - 1; scan > (void **)stack_end; scan--) {
                if ((void *)((long)*scan & 0xffffffff00000000L) == heaps[0]) {
                    mark_stack.push(*scan);
                }
            }
        }

    public:
        size_t get_heap_size() { return heap_size; }

        void *allocate(size_t sz, bool atomic) {
            if (sz > 8192) {
                return NULL;
            }

            if (heaps[0] == NULL) {
                if (!allocate_heaps()) return NULL;
            }

            sz = sz > 64 ? nextpow2((int)sz) : 64;
            heap_size += sz;

            if (heap_size > gc_threshold) {
                trigger_gc();

                if (heap_size > gc_threshold) {
                    gc_threshold *= 2;
                }
            }

            int bsf;
            __asm__("bsf %1, %0" : "=r"(bsf) : "r"((int)sz));

            int heap_no = (bsf - 6) + 8 * atomic;
            for (int i = heap_next_free[heap_no]; i < 0x10000000 / (64 << (heap_no % 8)); i++) {
                if (!heap_marks[heap_no][i]) {
                    heap_marks[heap_no][i] = 1;
                    heap_next_free[heap_no] = i + 1;

                    memset((char *)heaps[heap_no] + i * sz, 0, sz);

                    return ((char *)heaps[heap_no] + i * sz);
                }
            }

            return allocate(sz * 2, atomic);
        }

        void add_root(void *obj) {
            roots.insert(obj);
        }

        void remove_root(void *obj) {
            roots.erase(obj);
        }

        void clear_roots() {
            roots.clear();
        }

        void trigger_gc() {
            for (int i = 0; i < 16; i++) {
                memset(heap_marks[i], 0, 0x10000000 / (64 << (i % 8)));
            }

            heap_size = 0;

            for (std::set<void *>::iterator i = roots.begin(); i != roots.end(); i++) {
                mark_stack.push(*i);
            }

            mark_from_stack();

            while (!mark_stack.empty()) {
                void *obj = mark_stack.top();
                mark_stack.pop();

                mark_object(obj);
            }

            for (int i = 0; i < 16; i++) {
                for (int j = 0; j < 0x10000000 / (64 << (i % 8)); j++) {
                    if (!heap_marks[i][j]) {
                        heap_next_free[i] = j;
                        break;
                    }
                }
            }
        }
    };
}
