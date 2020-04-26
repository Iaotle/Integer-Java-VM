#include "structs.h"
#include "stack.h"
#include <stdint.h>
#include <stdlib.h>

struct stack *createStack(unsigned int size) {
    struct stack *stack = (struct stack *) malloc(sizeof(struct stack));
    stack->capacity = size;
    stack->top = -1;
    stack->array = (uint32_t *) malloc(stack->capacity * sizeof(int32_t));
    return stack;
} 

bool isFull(struct stack *stack) {
    return stack->top == stack->capacity - 1;
} 

bool isEmpty(struct stack *stack) {
    return stack->top == -1;
} 

void push(struct stack *stack, unsigned int word) {
    if (isFull(stack)) {
        stack->array = realloc(stack->array, ++stack->capacity * (sizeof(int32_t)));
    }
    stack->array[++stack->top] = word;
} 

unsigned int pop(struct stack *stack) {
    if (isEmpty(stack)) {
        return -1;
    }
    int32_t word = stack->array[stack->top--];
    stack->array = realloc(stack->array, --stack->capacity * (sizeof(int32_t)));
    return word;
} 

unsigned int peek(struct stack *stack) {
    if (isEmpty(stack)) {
        return -1;
    }
    return stack->array[stack->top];
}

struct frame *createframe() {
    struct frame *frame = (struct frame *) malloc(sizeof(struct frame));
    frame->localstack = (struct stack **) malloc(sizeof(struct stack));
    return frame;

} 