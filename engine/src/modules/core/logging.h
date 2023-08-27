#ifndef LOGGING_H
#define LOGGING_H

#define STRINGIFY_LITERAL(literal) # literal

#if defined(__MINGW32__) || defined(__MINGW64__)

#define H64 STRINGIFY_LITERAL(%016llX)
#define U64 STRINGIFY_LITERAL(%llu)
#define I64 STRINGIFY_LITERAL(%lld)
#define U32 STRINGIFY_LITERAL(%lu)
#define I32 STRINGIFY_LITERAL(%d)
#define U16 STRINGIFY_LITERAL(%u)
#define U8 STRINGIFY_LITERAL(%hhu)

#else

#define H64 STRINGIFY_LITERAL(%016lX)
#define U64 STRINGIFY_LITERAL(%lu)
#define I64 STRINGIFY_LITERAL(%ld)
#define U32 STRINGIFY_LITERAL(%u)
#define I32 STRINGIFY_LITERAL(%d)
#define U16 STRINGIFY_LITERAL(%u)
#define U8 STRINGIFY_LITERAL(%hhu)

#endif

#define FG_BLACK "\x1b[0;30m"
#define FG_DARK_RED "\x1b[0;31m"
#define FG_DARK_GREEN "\x1b[0;32m"
#define FG_DARK_YELLOW "\x1b[0;33m"
#define FG_DARK_BLUE "\x1b[0;34m"
#define FG_DARK_MAGENTA "\x1b[0;35m"
#define FG_DARK_CYAN "\x1b[0;36m"
#define FG_GRAY "\x1b[0;37m"
#define FG_DARK_GRAY "\x1b[1;30m"
#define FG_RED "\x1b[1;31m"
#define FG_GREEN "\x1b[1;32m"
#define FG_YELLOW "\x1b[1;33m"
#define FG_BLUE "\x1b[1;34m"
#define FG_MAGENTA "\x1b[1;35m"
#define FG_CYAN "\x1b[1;36m"
#define FG_WHITE "\x1b[1;37m"

#define BG_BLACK "\x1b[0;40m"
#define BG_DARK_RED "\x1b[0;41m"
#define BG_DARK_GREEN "\x1b[0;42m"
#define BG_DARK_YELLOW "\x1b[0;43m"
#define BG_DARK_BLUE "\x1b[0;44m"
#define BG_DARK_MAGENTA "\x1b[0;45m"
#define BG_DARK_CYAN "\x1b[0;46m"
#define BG_GRAY "\x1b[0;47m"
#define BG_DARK_GRAY "\x1b[1;40m"
#define BG_RED "\x1b[1;41m"
#define BG_GREEN "\x1b[1;42m"
#define BG_YELLOW "\x1b[1;43m"
#define BG_BLUE "\x1b[1;44m"
#define BG_MAGENTA "\x1b[1;45m"
#define BG_CYAN "\x1b[1;46m"
#define BG_WHITE "\x1b[1;47m"

#define COLOR_RESET "\x1b[0m"

#endif // LOGGING_H
