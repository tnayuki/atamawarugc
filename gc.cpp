/*
 * Copyright 2018 Toru Nayuki
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

#include "atamawarugc.hpp"

atamawarugc::gc gc;

extern "C" int GC_dummy = 0;

extern "C" size_t atamawarugc_get_heap_size() {
    return gc.get_heap_size();
}

extern "C" void *atamawarugc_malloc(size_t sz, int atomic) {
   return gc.allocate(sz, atomic);
}

extern "C" void *atamawarugc_malloc_uncollectable(size_t sz, int atomic) {
    void *obj = gc.allocate(sz, atomic);

    gc.add_root(obj);
    
    return obj;
}

extern "C" void atamawarugc_free(void *obj) {
    gc.remove_root(obj);
}

extern "C" void atamawarugc_gcollect(void) {
    gc.trigger_gc();
}

extern "C" void atamawarugc_clear_roots(void) {
    gc.clear_roots();
}

extern "C" size_t atamawarugc_get_free_bytes(void) {
    return 0;
}
