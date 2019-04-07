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

#ifdef __cplusplus
extern "C" {
#endif

void atamawarugc_clear_roots(void);
void atamawarugc_free(void *);
void atamawarugc_gcollect(void);
size_t atamawarugc_get_heap_size(void);
size_t atamawarugc_get_free_bytes(void);
void *atamawarugc_malloc(size_t, int);
void *atamawarugc_malloc_uncollectable(size_t, int);

typedef void (*GC_warn_proc)(char *, long);

#define GC_all_interior_pointers GC_dummy
#define GC_java_finalization GC_dummy
#define GC_no_dls GC_dummy
#define GC_gc_no GC_dummy
    
#define GC_clear_roots() atamawarugc_clear_roots()
#define GC_collect_a_little()
#define GC_init()
#define GC_get_free_bytes() atamawarugc_get_free_bytes()
#define GC_gcollect() atamawarugc_gcollect()
#define GC_get_heap_size() atamawarugc_get_heap_size()
#define GC_set_warn_proc(x)
#define GC_pthread_create(w, x, y, z) pthread_create(w, x, y, z)

#define GC_REGISTER_FINALIZER_NO_ORDER(v, w, x, y, z)
#define GC_MALLOC(x) atamawarugc_malloc(x, 0)
#define GC_MALLOC_ATOMIC(x) atamawarugc_malloc(x, 1)
#define GC_MALLOC_IGNORE_OFF_PAGE(x) GC_MALLOC(x)
#define GC_MALLOC_ATOMIC_IGNORE_OFF_PAGE(x) GC_MALLOC_ATOMIC(x)
#define GC_MALLOC_UNCOLLECTABLE(x) atamawarugc_malloc_uncollectable(x, 0)
#define GC_MALLOC_ATOMIC_UNCOLLECTABLE(x) atamawarugc_malloc_uncollectable(x, 1)
#define GC_FREE(x) atamawarugc_free(x)

#define GC_NEW(t) ((t*)GC_MALLOC(sizeof(t)))
#define GC_NEW_ATOMIC(t) ((t*)GC_MALLOC_ATOMIC(sizeof(t)))

extern int GC_dummy;

#ifdef __cplusplus
}
#endif
