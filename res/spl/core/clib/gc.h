/*  SPL - Simple Programming Language compiler
*  Copyright (C) 2022  Valentyn Tymchyshyn
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or any
 *  later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
 *  Valentyn Tymchyshyn (val.t.develop) (val.t.develo@gmail.com)
 *
 *  Realization of gc part of standard library of spl.
 */

#ifndef GC_H
#define GC_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct __spl__gcmap__entry {
    void* ptr;
    int32_t refs;
} __spl__gcmap__entry_t;

typedef struct __spl__gcmap {
    int32_t size;
    int32_t capacity;
    __spl__gcmap__entry_t *entries;
} __spl__gcmap__t;

__spl__gcmap__t __spl__m;

__attribute__((used))
void __spl__init__gcmap() {
    __spl__m.size = 0;
    __spl__m.capacity = 10;
    __spl__m.entries = malloc(__spl__m.capacity*sizeof(__spl__gcmap__entry_t));
}

__attribute__((used))
void __spl__destroy__gcmap() {
    free(__spl__m.entries);
}

__attribute__((used))
void __spl__add__to__gc(void *ptr, int32_t refs) {
    if (__spl__m.size+1 < __spl__m.capacity) {
        __spl__m.entries[__spl__m.size].ptr = ptr;
        __spl__m.entries[__spl__m.size].refs = refs;
        __spl__m.size++;
    } else {
        __spl__m.capacity *= 2;
        __spl__m.entries = realloc(__spl__m.entries, __spl__m.capacity*sizeof(__spl__gcmap__entry_t));
        __spl__m.entries[__spl__m.size].ptr = ptr;
        __spl__m.entries[__spl__m.size].refs = refs;
        __spl__m.size++;
    }
}

__attribute__((used))
int32_t __spl__get__refs(void *ptr) {
    for(int i = 0; i < __spl__m.size; ++i) {
        if (__spl__m.entries[i].ptr == ptr) {
            return __spl__m.entries[i].refs;
        }
    }
    return -1;
}

__attribute__((used))
void __spl__set__refs(void *ptr, int32_t refs) {
    for(int i = 0; i < __spl__m.size; ++i) {
        if (__spl__m.entries[i].ptr == ptr) {
            __spl__m.entries[i].refs = refs;
        }
    }
    __spl__add__to__gc(ptr, refs);
}

__attribute__((used))
void __spl__dec__refs(void *ptr) {
    for(int i = 0; i < __spl__m.size; ++i) {
        if (__spl__m.entries[i].ptr == ptr) {
            __spl__m.entries[i].refs--;
        }
    }
}

__attribute__((used))
void __spl__inc__refs(void *ptr) {
    for(int i = 0; i < __spl__m.size; ++i) {
        if (__spl__m.entries[i].ptr == ptr) {
            __spl__m.entries[i].refs++;
        }
    }
}

__attribute__((used))
void *__spl__alloc(int32_t size) {
    void *ptr = malloc(size);
    __spl__add__to__gc(ptr, 1);
    return ptr;
}

__attribute__((used))
void __spl__write(void *dest, void *data) {

    if (dest != 0)
        __spl__dec__refs(dest);

    if (data != 0)
        __spl__inc__refs(data);

    if (dest != 0) {
        if (__spl__get__refs(dest) == 0) {
            __spl__set__refs(dest, -1);
            free(dest);
        }
    }
}

__attribute__((used))
void __spl__destroyvar(void *ptr, void (*destructor)(void*)) {

    if (ptr != 0) {
        __spl__dec__refs(ptr);
        if (__spl__get__refs(ptr) == 0) {
            __spl__set__refs(ptr, -1);
            if (destructor != 0) {
                destructor(ptr);
            }
            free(ptr);
        }
    }
}

#endif //GC_H