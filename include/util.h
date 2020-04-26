#ifndef UTIL_H
#define UTIL_H

/* debug print */
#ifdef DEBUG

#define debug(...) \
    fprintf(stderr,  __VA_ARGS__)
#else
#define debug(...)
#endif

#endif
