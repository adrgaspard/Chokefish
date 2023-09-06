#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include <pthread.h>

#define DECLARE_CONCURRENT_TYPE(type, name)     \
typedef struct name                             \
{                                               \
    pthread_rwlock_t lock;                      \
    type value;                                 \
} name;                                         \
                                                \
static inline name create_##name(type value);   \
static inline void rdlock_##name(name *ptr);    \
static inline void wrlock_##name(name *ptr);    \
static inline void unlock_##name(name *ptr);    \
                                                \
static inline name create_##name(type value)    \
{                                               \
    name result;                                \
    pthread_rwlock_init(&(result.lock), NULL);  \
    result.value = value;                       \
    return result;                              \
}                                               \
                                                \
static inline void rdlock_##name(name *ptr)     \
{                                               \
    pthread_rwlock_rdlock(&(ptr->lock));        \
}                                               \
                                                \
static inline void wrlock_##name(name *ptr)     \
{                                               \
    pthread_rwlock_wrlock(&(ptr->lock));        \
}                                               \
                                                \
static inline void unlock_##name(name *ptr)     \
{                                               \
    pthread_rwlock_unlock(&(ptr->lock));        \
}

#endif // CONCURRENCY_H