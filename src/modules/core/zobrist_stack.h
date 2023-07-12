#ifndef ZOBRIST_STACK_H
#define ZOBRIST_STACK_H

#include "types.h"

static inline zobrist_stack create_zobrist_stack()
{
    zobrist_stack stack;
    stack.count = 0;
    return stack;
}

static inline void push_on_stack(zobrist_stack *stack, zobrist_key element)
{
    stack->items[stack->count++] = element;
}

static inline zobrist_key pop_from_stack(zobrist_stack *stack)
{
    stack->count--;
    return stack->items[stack->count];
}

#endif // ZOBRIST_STACK_H
