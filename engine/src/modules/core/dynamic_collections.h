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
    uint32_t capacity;                                              \
} name;                                                             \
                                                                    \
static inline struct name create_##name(uint32_t capacity);         \
static inline void finalize_##name(name *array_ptr);                \
                                                                    \
static inline struct name create_##name(uint32_t capacity)          \
{                                                                   \
    name array;                                                     \
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
static inline void finalize_##name(name *array_ptr)                 \
{                                                                   \
    free(array_ptr->items);                                         \
    array_ptr->items = NULL;                                        \
}

#endif // DYNAMIC_COLLECTIONS_H
