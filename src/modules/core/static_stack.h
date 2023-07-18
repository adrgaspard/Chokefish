#ifndef STATIC_STACK_H
#define STATIC_STACK_H

#include <assert.h>
#include <stdint.h>

#define DECLARE_STATIC_STACK_TYPE(type, size)                                                                                   \
typedef struct static_stack__##type##__##size                                                                                   \
{                                                                                                                               \
    type items[size];                                                                                                           \
    uint32_t count;                                                                                                             \
    uint32_t capacity;                                                                                                          \
} static_stack__##type##__##size;                                                                                                                              \
                                                                                                                                \
static inline struct static_stack__##type##__##size _create_static_stack__##type##__##size();                                   \
static inline void _push_on_static_stack__##type##__##size(struct static_stack__##type##__##size *stack_ptr, type element);     \
static inline type _pop_from_static_stack__##type##__##size(struct static_stack__##type##__##size *stack_ptr);                  \
static inline type _peek_from_static_stack__##type##__##size(struct static_stack__##type##__##size *stack_ptr);                 \
                                                                                                                                \
static inline struct static_stack__##type##__##size _create_static_stack__##type##__##size()                                    \
{                                                                                                                               \
    struct static_stack__##type##__##size stack;                                                                                \
    stack.count = 0;                                                                                                            \
    stack.capacity = size;                                                                                                      \
    return stack;                                                                                                               \
}                                                                                                                               \
                                                                                                                                \
static inline void _push_on_static_stack__##type##__##size(struct static_stack__##type##__##size *stack_ptr, type element)      \
{                                                                                                                               \
    assert(stack_ptr->count < stack_ptr->capacity);                                                                             \
    stack_ptr->items[stack_ptr->count++] = element;                                                                             \
}                                                                                                                               \
                                                                                                                                \
static inline type _pop_from_static_stack__##type##__##size(struct static_stack__##type##__##size *stack_ptr)                   \
{                                                                                                                               \
    assert(stack_ptr->count > 0);                                                                                               \
    stack_ptr->count--;                                                                                                         \
    return stack_ptr->items[stack_ptr->count];                                                                                  \
}                                                                                                                               \
                                                                                                                                \
static inline type _peek_from_static_stack__##type##__##size(struct static_stack__##type##__##size *stack_ptr)                  \
{                                                                                                                               \
    assert(stack_ptr->count > 0);                                                                                               \
    return stack_ptr->items[stack_ptr->count - 1];                                                                              \
}

#define CREATE_EMPTY_STACK(type, size) _create_static_stack__##type##__##size()
#define PUSH_ON_STACK(stack_ptr, element) _push_on_static_stack__##type##__##size(stack_ptr, element)
#define POP_FROM_STACK(stack_ptr) _pop_from_static_stack__##type##__##size(stack_ptr)
#define PEEK_FROM_STACK(stack_ptr) _peek_from_static_stack__##type##__##size(stack_ptr)

#endif // STATIC_STACK_H
