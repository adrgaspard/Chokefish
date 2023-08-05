#ifndef STATIC_COLLECTIONS_H
#define STATIC_COLLECTIONS_H

#include <assert.h>
#include <stdint.h>

#define DECLARE_STATIC_STACK_TYPE(type, size, name)                 \
typedef struct name                                                 \
{                                                                   \
    type items[size];                                               \
    uint32_t count;                                                 \
    uint32_t capacity;                                              \
} name;                                                             \
                                                                    \
static inline struct name create_##name();                          \
static inline void push_on_##name(name *stack_ptr, type element);   \
static inline type pop_from_##name(name *stack_ptr);                \
static inline type peek_from_##name(name *stack_ptr);               \
static inline void clear_##name(name *stack_ptr);                   \
                                                                    \
static inline struct name create_##name()                           \
{                                                                   \
    name stack;                                                     \
    stack.count = 0;                                                \
    stack.capacity = size;                                          \
    return stack;                                                   \
}                                                                   \
                                                                    \
static inline void push_on_##name(name *stack_ptr, type element)    \
{                                                                   \
    assert(stack_ptr->count < stack_ptr->capacity);                 \
    stack_ptr->items[stack_ptr->count++] = element;                 \
}                                                                   \
                                                                    \
static inline type pop_from_##name(name *stack_ptr)                 \
{                                                                   \
    assert(stack_ptr->count > 0);                                   \
    stack_ptr->count--;                                             \
    return stack_ptr->items[stack_ptr->count];                      \
}                                                                   \
                                                                    \
static inline type peek_from_##name(name *stack_ptr)                \
{                                                                   \
    assert(stack_ptr->count > 0);                                   \
    return stack_ptr->items[stack_ptr->count - 1];                  \
}                                                                   \
                                                                    \
static inline void clear_##name(name *stack_ptr)                    \
{                                                                   \
    stack_ptr->count = 0;                                           \
}

#define DECLARE_STATIC_ARRAY_TYPE(type, size, name)                 \
typedef struct name                                                 \
{                                                                   \
    type items[size];                                               \
    uint32_t capacity;                                              \
} name;                                                             \
                                                                    \
static inline struct name create_##name();                          \
                                                                    \
static inline struct name create_##name()                           \
{                                                                   \
    name array;                                                     \
    array.capacity = size;                                          \
    return array;                                                   \
}

#endif // STATIC_COLLECTIONS_H
