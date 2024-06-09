#ifndef STATIC_COLLECTIONS_H
#define STATIC_COLLECTIONS_H

#include <assert.h>
#include <stdint.h>

#define DECLARE_STATIC_STACK_CONTRACT(type, size, name)             \
typedef struct name                                                 \
{                                                                   \
    type items[size];                                               \
    uint32_t count;                                                 \
    uint32_t capacity;                                              \
} name;                                                             \
                                                                    \
struct name create_##name();                                        \
void push_on_##name(name *stack_ptr, type element);                 \
type pop_from_##name(name *stack_ptr);                              \
type peek_from_##name(name *stack_ptr);                             \
void clear_##name(name *stack_ptr);

#define DECLARE_STATIC_STACK_IMPLEMENTATION(type, size, name)       \
struct name create_##name()                                         \
{                                                                   \
    name stack;                                                     \
    stack.count = 0;                                                \
    stack.capacity = size;                                          \
    return stack;                                                   \
}                                                                   \
                                                                    \
void push_on_##name(name *stack_ptr, type element)                  \
{                                                                   \
    assert(stack_ptr != NULL);                                      \
    assert(stack_ptr->count < stack_ptr->capacity);                 \
    stack_ptr->items[stack_ptr->count++] = element;                 \
}                                                                   \
                                                                    \
type pop_from_##name(name *stack_ptr)                               \
{                                                                   \
    assert(stack_ptr != NULL);                                      \
    assert(stack_ptr->count > 0);                                   \
    stack_ptr->count--;                                             \
    return stack_ptr->items[stack_ptr->count];                      \
}                                                                   \
                                                                    \
type peek_from_##name(name *stack_ptr)                              \
{                                                                   \
    assert(stack_ptr != NULL);                                      \
    assert(stack_ptr->count > 0);                                   \
    return stack_ptr->items[stack_ptr->count - 1];                  \
}                                                                   \
                                                                    \
void clear_##name(name *stack_ptr)                                  \
{                                                                   \
    assert(stack_ptr != NULL);                                      \
    stack_ptr->count = 0;                                           \
}

#define DECLARE_STATIC_ARRAY_CONTRACT(type, size, name)             \
typedef struct name                                                 \
{                                                                   \
    type items[size];                                               \
    uint32_t capacity;                                              \
} name;                                                             \
                                                                    \
struct name create_##name();

#define DECLARE_STATIC_ARRAY_IMPLEMENTATION(type, size, name)       \
struct name create_##name()                                         \
{                                                                   \
    name array;                                                     \
    array.capacity = size;                                          \
    return array;                                                   \
}

#endif // STATIC_COLLECTIONS_H
