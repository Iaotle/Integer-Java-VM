#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdint.h>
/**
 * This stores basically everything, from program counter
 * to local frame and objref index arrays.
 */
struct ijvm {
    int count;
    int currentframe;
    int heapcount;
    struct ijvmbinary *data;
    struct stack *stack;
    struct frame *frame, *heap;
    int refptr[128];
};

/**
 * This stores only the ijvmbinary stats.
 */
struct ijvmbinary {
    uint8_t buffer[1536], poolsize[4], *textdata;
    int32_t *constdata;
    unsigned int numConstants, numText; // number of bytes for constants and text
};

/**
 * This is a simple stack struct.
 */
struct stack {
    int top;
    unsigned int capacity, *array;
};

/**
 * This is a frame - an arary of stacks.
 */
struct frame {
    struct stack **localstack;
};


#endif