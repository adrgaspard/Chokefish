#ifndef DYNAMIC_COLLECTIONS_H
#define DYNAMIC_COLLECTIONS_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define DECLARE_DYNAMIC_ARRAY_TYPE(type, name)                      \
typedef struct name                                                 \
{                                                                   \
    type *items;                                                    \
    uint32_t count;                                                 \
    uint32_t capacity;                                              \
} name;                                                             \
                                                                    \
static inline struct name create_##name(uint32_t capacity);         \
static inline void clear_##name(name *array_ptr);                   \
static inline void finalize_##name(name *array_ptr);                \
                                                                    \
static inline struct name create_##name(uint32_t capacity)          \
{                                                                   \
    name array;                                                     \
    array.count = 0;                                                \
    array.capacity = capacity;                                      \
    array.items = (type*)calloc((size_t)capacity, sizeof(type));    \
    if (array.items == NULL)                                        \
    {                                                               \
        perror("Runtime allocation error");                         \
        exit(1);                                                    \
    }                                                               \
    return array;                                                   \
}                                                                   \
                                                                    \
static inline void append_on_##name(name *array_ptr, type element)  \
{                                                                   \
    assert(array_ptr->count < array_ptr->capacity);                 \
    array_ptr->items[array_ptr->count++] = element;                 \
}                                                                   \
                                                                    \
static inline void clear_##name(name *array_ptr)                    \
{                                                                   \
    array_ptr->count = 0;                                           \
}                                                                   \
                                                                    \
static inline void finalize_##name(name *array_ptr)                 \
{                                                                   \
    free(array_ptr->items);                                         \
}

#endif // DYNAMIC_COLLECTIONS_H
