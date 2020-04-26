#ifndef STACK_H
#define STACK_H
#include <structs.h>
#include <stdbool.h>
/**
 *   Stack creation function
 */
struct stack *createStack(unsigned int size);

/**
 *  @return 1 if full, 0 if not
 */
bool isFull(struct stack *stack);

/**
 *  @return 1 if empty, 0 if not
 */
bool isEmpty(struct stack *stack);

/**
 *  pushes word to stack*
 */
void push(struct stack *stack, unsigned int word);

/**
 *  pops word from stack*
 */
unsigned int pop(struct stack *stack);

/**
 *  returns top of stack*
 */
unsigned int peek(struct stack *stack);

/**
 *  Frame creation function
 */
struct frame *createframe(void);

#endif