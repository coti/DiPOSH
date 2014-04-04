/*
 *
 * Copyright (c) 2014 LIPN - Universite Paris 13
 *                    All rights reserved.
 *
 * This file is part of POSH.
 * 
 * POSH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * POSH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with POSH.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <string.h>
#include <stdlib.h>
#include "shmem_internal.h"

#define BLOCK 2097152

#ifndef _SHMEM_UTILS_
#define _SHMEM_UTILS_

/* useful macros */

#ifdef __GNUC__
#define _shmem_likely(x)       __builtin_expect((x),1)
#define _shmem_unlikely(x)     __builtin_expect((x),0)
#else
#define _shmem_likely(x)       (x)
#define _shmem_unlikely(x)     (x)
#endif



/* Copy functions (very very important) */

static inline void* _shmem_memcpy( void*, const void*, size_t );

static inline void* _shmem_memcpy2( void*, const void*, size_t );

static inline void * __memcpy( void*, const void*, size_t );
static inline void * mmx_memcpy( void*, const void*, size_t );
static inline void * mmx2_memcpy( void*, const void*, size_t );
static inline void * sse_memcpy( void*, const void*, size_t );

/********************************************************************/

#define small_memcpy(to,from,n)                 \
    {                                           \
        register unsigned long int dummy;       \
        __asm__ __volatile__(                   \
                             "rep; movsb"       \
                             :"=&D"(to), "=&S"(from), "=&c"(dummy)  \
                             :"0" (to), "1" (from),"2" (n)          \
                             : "memory");                           \
    }

/********************************************************************/

static inline void* _shmem_memcpy( void* dst, const void* src, size_t cnt ) {

#if _USE_BLOCK_MEMCPY
    unsigned int k;
    unsigned int offset;
    const char* src8;
    char* dst8;

    src8 = (const char*)src;
    dst8 = (char*)dst;

    k = 0; offset = 0;

    while( _shmem_likely( offset < cnt ) ) {
        if( _shmem_likely( offset + BLOCK <= cnt ) ) {
            k = BLOCK;
        } else {
            k = cnt - offset;
        } 
        _shmem_memcpy2( dst8 + offset, src8 + offset, k );

        offset += BLOCK;
    }
#else
    _shmem_memcpy2( dst, src, cnt );
#endif
    return dst;

}

static inline void* _shmem_memcpy2( void* dst, const void* src, size_t cnt ) {

#ifdef _USE_MEMCPY_ 
    (void) std::memcpy( dst, src, cnt );
#endif

#ifdef _USE_VECTOR_COPY_ // can be optimized quite decently by the compiler
    const char* src8;
    char* dst8;

    src8 = (const char*)src;
    dst8 = (char*)dst;

    for( unsigned int i = 0 ; i < cnt ; i++ ) {
        dst8[i] = src8[i];
    }
#endif

#ifdef _USE_PTR_VEC_COPY_
    const char* src8;
    char* dst8;
    size_t count;

    src8 = src;
    dst8 = dst;
    count = cnt;

    --src8;
    --dst8;
    while (count--) {
        *++dst8 = *++src8;
    }
#endif

#ifdef _USE_KER_MEMCPY_
    return __memcpy( dst, src, cnt );

#endif

#ifdef _USE_MMX_MEMCPY_
    return mmx_memcpy( dst, src, cnt );
#endif

#ifdef _USE_MMX2_MEMCPY_
    return  mmx2_memcpy( dst, src, cnt );
#endif

#ifdef _USE_SSE_MEMCPY_
    return sse_memcpy( dst, src, cnt );
#endif

    return dst;
}

/********************************************************************/

/* linux kernel __memcpy (from: /include/asm/string.h) */
static inline void * __memcpy( void * to, const void * from, size_t n ){
     int d0, d1, d2;

     if ( n < 4 ) {
          small_memcpy(to,from,n);
     }
     else
          __asm__ __volatile__(
                              "rep ; movsl\n\t"
                              "testb $2,%b4\n\t"
                              "je 1f\n\t"
                              "movsw\n"
                              "1:\ttestb $1,%b4\n\t"
                              "je 2f\n\t"
                              "movsb\n"
                              "2:"
                              : "=&c" (d0), "=&D" (d1), "=&S" (d2)
                              :"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from)
                              : "memory");

     return(to);
}

/********************************************************************/

/* MMX */

#define MMX_MMREG_SIZE 8

#define MMX1_MIN_LEN 0x800  /* 2K blocks */
#define MIN_LEN 0x40  /* 64-byte blocks */

static inline void * mmx_memcpy(void * to, const void * from, size_t len)
{
     void *retval;
     size_t i;
     retval = to;

     const unsigned char* from_c = (const unsigned char *) from;
     unsigned char* to_c = (unsigned char *)to;

     if (len >= MMX1_MIN_LEN) {
          register unsigned long int delta;
          /* Align destinition to MMREG_SIZE -boundary */
          delta = ((unsigned long int)to)&(MMX_MMREG_SIZE-1);
          if (delta) {
               delta=MMX_MMREG_SIZE-delta;
               len -= delta;
               small_memcpy(to, from, delta);
          }
          i = len >> 6; /* len/64 */
          len&=63;
          for (; i>0; i--) {
               __asm__ __volatile__ (
                                    "movq (%0), %%mm0\n"
                                    "movq 8(%0), %%mm1\n"
                                    "movq 16(%0), %%mm2\n"
                                    "movq 24(%0), %%mm3\n"
                                    "movq 32(%0), %%mm4\n"
                                    "movq 40(%0), %%mm5\n"
                                    "movq 48(%0), %%mm6\n"
                                    "movq 56(%0), %%mm7\n"
                                    "movq %%mm0, (%1)\n"
                                    "movq %%mm1, 8(%1)\n"
                                    "movq %%mm2, 16(%1)\n"
                                    "movq %%mm3, 24(%1)\n"
                                    "movq %%mm4, 32(%1)\n"
                                    "movq %%mm5, 40(%1)\n"
                                    "movq %%mm6, 48(%1)\n"
                                    "movq %%mm7, 56(%1)\n"
                                    :: "r" (from_c), "r" (to_c) : "memory");
               from_c += 64;
               to_c += 64;
          }
          __asm__ __volatile__ ("emms":::"memory");
     }
     /*
      * Now do the tail of the block
      */
     to = to_c;
     from = from_c;
     if ( len ) __memcpy( to, from, len );
     return retval;
}

/********************************************************************/

static inline void * mmx2_memcpy( void * to, const void * from, size_t len ) {
     void *retval;
     size_t i;
     retval = to;

     const unsigned char* from_c = (const unsigned char *) from;
     unsigned char* to_c = (unsigned char *)to;

     /* PREFETCH has effect even for MOVSB instruction ;) */
     __asm__ __volatile__ (
                          "   prefetchnta (%0)\n"
                          "   prefetchnta 64(%0)\n"
                          "   prefetchnta 128(%0)\n"
                          "   prefetchnta 192(%0)\n"
                          "   prefetchnta 256(%0)\n"
                          : : "r" (from) );

     if (len >= MIN_LEN) {
          register unsigned long int delta;
          /* Align destinition to MMREG_SIZE -boundary */
          delta = ((unsigned long int)to)&(MMX_MMREG_SIZE-1);
          if (delta) {
               delta=MMX_MMREG_SIZE-delta;
               len -= delta;
               small_memcpy(to, from, delta);
          }
          i = len >> 6; /* len/64 */
          len&=63;
          for (; i>0; i--) {
               __asm__ __volatile__ (
                                    "prefetchnta 320(%0)\n"
                                    "movq (%0), %%mm0\n"
                                    "movq 8(%0), %%mm1\n"
                                    "movq 16(%0), %%mm2\n"
                                    "movq 24(%0), %%mm3\n"
                                    "movq 32(%0), %%mm4\n"
                                    "movq 40(%0), %%mm5\n"
                                    "movq 48(%0), %%mm6\n"
                                    "movq 56(%0), %%mm7\n"
                                    "movntq %%mm0, (%1)\n"
                                    "movntq %%mm1, 8(%1)\n"
                                    "movntq %%mm2, 16(%1)\n"
                                    "movntq %%mm3, 24(%1)\n"
                                    "movntq %%mm4, 32(%1)\n"
                                    "movntq %%mm5, 40(%1)\n"
                                    "movntq %%mm6, 48(%1)\n"
                                    "movntq %%mm7, 56(%1)\n"
                                    :: "r" (from_c), "r" (to_c) : "memory");
               from_c += 64;
               to_c += 64;
          }
          /* since movntq is weakly-ordered, a "sfence"
          * is needed to become ordered again. */
          __asm__ __volatile__ ("sfence":::"memory");
          __asm__ __volatile__ ("emms":::"memory");
     }
     /*
      * Now do the tail of the block
      */
     to = to_c;
     from = from_c;
     if (len) __memcpy(to, from, len);
     return retval;
}

/********************************************************************/

#define SSE_MMREG_SIZE 16

static inline void * sse_memcpy( void * to, const void * from, size_t len ){
     void *retval;
     size_t i;
     retval = to;

     const unsigned char* from_c = (const unsigned char *) from;
     unsigned char* to_c = (unsigned char *)to;

     /* PREFETCH has effect even for MOVSB instruction ;) */
     __asm__ __volatile__ (
                          "   prefetchnta (%0)\n"
                          "   prefetchnta 64(%0)\n"
                          "   prefetchnta 128(%0)\n"
                          "   prefetchnta 192(%0)\n"
                          "   prefetchnta 256(%0)\n"
                          : : "r" (from) );

     if (len >= MIN_LEN) {
          register unsigned long int delta;
          /* Align destinition to MMREG_SIZE -boundary */
          delta = ((unsigned long int)to)&(SSE_MMREG_SIZE-1);
          if (delta) {
               delta=SSE_MMREG_SIZE-delta;
               len -= delta;
               small_memcpy(to, from, delta);
          }
          i = len >> 6; /* len/64 */
          len&=63;
          if (((unsigned long)from) & 15)
               /* if SRC is misaligned */
               for (; i>0; i--) {
                    __asm__ __volatile__ (
                                         "prefetchnta 320(%0)\n"
                                         "movups (%0), %%xmm0\n"
                                         "movups 16(%0), %%xmm1\n"
                                         "movups 32(%0), %%xmm2\n"
                                         "movups 48(%0), %%xmm3\n"
                                         "movntps %%xmm0, (%1)\n"
                                         "movntps %%xmm1, 16(%1)\n"
                                         "movntps %%xmm2, 32(%1)\n"
                                         "movntps %%xmm3, 48(%1)\n"
                                         :: "r" (from_c), "r" (to_c) : "memory");
                    from_c += 64;
                    to_c += 64;
               }
          else
               /*
                  Only if SRC is aligned on 16-byte boundary.
                  It allows to use movaps instead of movups, which required
                  data to be aligned or a general-protection exception (#GP)
                  is generated.
               */
               for (; i>0; i--) {
                    __asm__ __volatile__ (
                                         "prefetchnta 320(%0)\n"
                                         "movaps (%0), %%xmm0\n"
                                         "movaps 16(%0), %%xmm1\n"
                                         "movaps 32(%0), %%xmm2\n"
                                         "movaps 48(%0), %%xmm3\n"
                                         "movntps %%xmm0, (%1)\n"
                                         "movntps %%xmm1, 16(%1)\n"
                                         "movntps %%xmm2, 32(%1)\n"
                                         "movntps %%xmm3, 48(%1)\n"
                                         :: "r" (from_c), "r" (to_c) : "memory");
                    from_c += 64;
                    to_c += 64;
               }
          /* since movntq is weakly-ordered, a "sfence"
           * is needed to become ordered again. */
          __asm__ __volatile__ ("sfence":::"memory");
          /* enables to use FPU */
          __asm__ __volatile__ ("emms":::"memory");
     }
     /*
      * Now do the tail of the block
      */
     to = to_c;
     from = from_c;
     if (len) __memcpy(to, from, len);
     return retval;
}

/********************************************************************/

/* http://www.danielvik.com/2010/02/fast-memcpy-in-c.html */

static inline void* vec_memcpy( void* dst, const void* src, size_t cnt ) {
    char* dst8 = (char*)dst;
    char* src8 = (char*)src;
    
    unsigned int j;
    for( j = 0 ; j < cnt ; j++ ) {
        dst8[j] = src8[j];
    }
    return dst;
}

/********************************************************************/

static inline void* fast_memcpy_ptr( void* dest, const void* src,size_t count ) {
    char* dst8 = (char*)dest;
    char* src8 = (char*)src;
    
    while (count--) {
        *dst8++ = *src8++;
    }
    
    return dest;
}

/********************************************************************/

static inline void* fast_memcpy_fixed_offset( void* dest, const void* src, size_t count ) {
    char* dst8 = (char*)dest;
    char* src8 = (char*)src;
        
    if (count & 1) {
        dst8[0] = src8[0];
        dst8 += 1;
        src8 += 1;
    }
    
    count /= 2;
    while (count--) {
        dst8[0] = src8[0];
        dst8[1] = src8[1];
        
        dst8 += 2;
        src8 += 2;
    }
    return dest;
}


#endif
