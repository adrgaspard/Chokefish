#ifndef THREADING_H
#define THREADING_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef void *(*engine_thread_function)(void *argument);

typedef struct engine_atomic_bool
{
    uint32_t value;
} engine_atomic_bool;

typedef struct engine_atomic_uint64
{
    uint64_t value;
} engine_atomic_uint64;

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef CRITICAL_SECTION engine_mutex;

typedef struct engine_thread
{
    HANDLE handle;
} engine_thread;

typedef struct engine_thread_wrapper_argument
{
    engine_thread_function function;
    void *argument;
} engine_thread_wrapper_argument;

static inline void engine_mutex_init(engine_mutex *mutex)
{
    InitializeCriticalSection(mutex);
}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4702)
#endif

static inline void engine_mutex_lock(engine_mutex *mutex)
{
    EnterCriticalSection(mutex);
}

static inline void engine_mutex_unlock(engine_mutex *mutex)
{
    LeaveCriticalSection(mutex);
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

static inline void engine_atomic_bool_init(engine_atomic_bool *flag)
{
    flag->value = 0;
}

static inline void engine_atomic_bool_store(engine_atomic_bool *flag, bool value)
{
    InterlockedExchange((volatile LONG *)&(flag->value), value ? 1 : 0);
}

static inline bool engine_atomic_bool_load(engine_atomic_bool *flag)
{
    return InterlockedCompareExchange((volatile LONG *)&(flag->value), 0, 0) != 0;
}

static inline void engine_atomic_uint64_init(engine_atomic_uint64 *counter)
{
    counter->value = 0;
}

static inline void engine_atomic_uint64_store(engine_atomic_uint64 *counter, uint64_t value)
{
    InterlockedExchange64((volatile LONG64 *)&(counter->value), (LONG64)value);
}

static inline uint64_t engine_atomic_uint64_load(engine_atomic_uint64 *counter)
{
    return (uint64_t)InterlockedCompareExchange64((volatile LONG64 *)&(counter->value), 0, 0);
}

static DWORD WINAPI engine_thread_wrapper(LPVOID raw_argument)
{
    engine_thread_wrapper_argument *wrapper_argument = (engine_thread_wrapper_argument *)raw_argument;
    wrapper_argument->function(wrapper_argument->argument);
    free(wrapper_argument);
    return 0;
}

static inline void engine_thread_create_detached(engine_thread *thread, engine_thread_function function, void *argument)
{
    engine_thread_wrapper_argument *wrapper_argument;
    assert(thread != NULL);
    assert(function != NULL);
    wrapper_argument = (engine_thread_wrapper_argument *)malloc(sizeof(engine_thread_wrapper_argument));
    assert(wrapper_argument != NULL);
    wrapper_argument->function = function;
    wrapper_argument->argument = argument;
    thread->handle = CreateThread(NULL, 0, engine_thread_wrapper, wrapper_argument, 0, NULL);
    assert(thread->handle != NULL);
    CloseHandle(thread->handle);
}

static inline void engine_sleep_ms(uint64_t duration_ms)
{
    while (duration_ms > 0)
    {
        DWORD chunk = duration_ms > 60000 ? 60000 : (DWORD)duration_ms;
        Sleep(chunk);
        duration_ms -= chunk;
    }
}

#else

#include <pthread.h>
#include <time.h>

typedef pthread_mutex_t engine_mutex;

typedef struct engine_thread
{
    pthread_t id;
} engine_thread;

static inline void engine_mutex_init(engine_mutex *mutex)
{
    int result = pthread_mutex_init(mutex, NULL);
    assert(result == 0);
    (void)result;
}

static inline void engine_mutex_lock(engine_mutex *mutex)
{
    int result = pthread_mutex_lock(mutex);
    assert(result == 0);
    (void)result;
}

static inline void engine_mutex_unlock(engine_mutex *mutex)
{
    int result = pthread_mutex_unlock(mutex);
    assert(result == 0);
    (void)result;
}

static inline void engine_atomic_bool_init(engine_atomic_bool *flag)
{
    flag->value = 0;
}

static inline void engine_atomic_bool_store(engine_atomic_bool *flag, bool value)
{
    __atomic_store_n(&(flag->value), value ? 1U : 0U, __ATOMIC_RELEASE);
}

static inline bool engine_atomic_bool_load(engine_atomic_bool *flag)
{
    return __atomic_load_n(&(flag->value), __ATOMIC_ACQUIRE) != 0;
}

static inline void engine_atomic_uint64_init(engine_atomic_uint64 *counter)
{
    counter->value = 0;
}

static inline void engine_atomic_uint64_store(engine_atomic_uint64 *counter, uint64_t value)
{
    __atomic_store_n(&(counter->value), value, __ATOMIC_RELAXED);
}

static inline uint64_t engine_atomic_uint64_load(engine_atomic_uint64 *counter)
{
    return __atomic_load_n(&(counter->value), __ATOMIC_RELAXED);
}

static inline void engine_thread_create_detached(engine_thread *thread, engine_thread_function function, void *argument)
{
    int result;
    pthread_attr_t attribute;
    assert(thread != NULL);
    assert(function != NULL);
    result = pthread_attr_init(&attribute);
    assert(result == 0);
    result = pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_DETACHED);
    assert(result == 0);
    result = pthread_create(&(thread->id), &attribute, function, argument);
    assert(result == 0);
    result = pthread_attr_destroy(&attribute);
    assert(result == 0);
    (void)result;
}

static inline void engine_sleep_ms(uint64_t duration_ms)
{
    struct timespec duration;
    duration.tv_sec = (time_t)(duration_ms / 1000);
    duration.tv_nsec = (long)((duration_ms % 1000) * 1000000);
    while (nanosleep(&duration, &duration) != 0)
    {
        // Retry until the whole duration elapsed
    }
}

#endif

#endif // THREADING_H
