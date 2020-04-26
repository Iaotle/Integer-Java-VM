#include <ijvm.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "structs.h"
#include "stack.h"
#include "machine.h"


/**
 * VIP functions
 */
int readbinary(char *binary_file) {
    int currentbyte = 0;
    FILE *fp;
    fp = fopen(binary_file, "rb");
    if (fp == NULL) {
        return -1;
    }
    debug("%s opened.\n", binary_file);
    fread(ijvm->data->buffer, sizeof(uint8_t), 1536, fp);
    currentbyte = 8; // actually 9th byte, skipping magic number and pool origin
    for (int i = 0; i < 4; i++) { //parsing constant pool size
        ijvm->data->poolsize[i] = ijvm->data->buffer[currentbyte];
        currentbyte++;
    }
    ijvm->data->numConstants = endConvert(ijvm->data->poolsize) / 4;
    debug("Constants: %i\n", ijvm->data->numConstants);
    ijvm->data->constdata = (int32_t *) malloc(ijvm->data->numConstants * 4); // allocate memory for constants
    for (int i = 0; i < ijvm->data->numConstants; i++) { // read constants
        uint8_t temp[4];
        for (int k = 0; k < 4; k++) {
            temp[k] = ijvm->data->buffer[currentbyte];
            currentbyte++;
        }
        debug("constant: 0x%02x\n", endConvert(temp));
        ijvm->data->constdata[i] = endConvert(temp);
    }
    currentbyte += 4; // skipping text origin
    for (int i = 0; i < 4; i++) { //parsing text pool size
        ijvm->data->poolsize[i] = ijvm->data->buffer[currentbyte];
        currentbyte++;
    }
    ijvm->data->numText = endConvert(ijvm->data->poolsize);
    debug("Instruction bytes: %i\n", ijvm->data->numText);
    ijvm->data->textdata = (uint8_t *) malloc(ijvm->data->numText); // allocate memory for text
    for (int i = 0; i < ijvm->data->numText; i++) { // reading text
        ijvm->data->textdata[i] = ijvm->data->buffer[currentbyte];
        currentbyte++;
    }
    debug("Total: %i bytes\n", currentbyte);
    return 0;
}

int init_ijvm(char *binary_file) {
    ijvm = (struct ijvm *) malloc(sizeof(struct ijvm));
    ijvm->data = (struct ijvmbinary *) malloc(sizeof(struct ijvmbinary));
    ijvm->count = 0;
    ijvm->currentframe = 0;
    ijvm->heapcount = 0;
    ijvm->heap = createframe();
    ijvm->frame = createframe();
    ijvm->frame->localstack[ijvm->currentframe] = createStack(0);
    ijvm->stack = createStack(0);
    if (!in){ // check if we already have an input file set
        debug("!in\n");
        set_input(stdin);
    }
    if (!out){ // check if we already have an output file set
        debug("!out\n");
        set_output(stdout);
    }
    return readbinary(binary_file);
}

void destroy_ijvm() {
    free(ijvm);
}

/**
 * Utility functions
 */
//Converters
int endConvert(uint8_t *data) {
    return data[3] | (uint32_t) data[2] << 8 | (uint32_t) data[1] << 16 | (uint32_t) data[0] << 24;
} 

int shortConvert(uint8_t data, uint8_t data2) {
    return data << 8 | data2;
} 

//IJVM.h functions
int32_t tos() {
    return (signed char)peek(ijvm->stack);
} 

int32_t *get_stack() {
    return (int32_t*)ijvm->stack->array;
}

int stack_size() {
    if (ijvm->stack->top == -1) {
        return 0;
    }
    return ijvm->stack->capacity;
}

uint8_t *get_text() {
    return ijvm->data->textdata;
}

int text_size() {
    return ijvm->data->numText;
}

int get_program_counter() {
    return ijvm->count;
}

int32_t get_local_variable(int i) {
    return ijvm->frame->localstack[ijvm->currentframe]->array[i];
}

bool step() {
    uint8_t *text = get_text();
    debug("\033[1;31m0x%02x\033[0m - ", text[ijvm->count]);
    switch (text[ijvm->count]) {
        case OP_NOP:
            oputil(0);
            break;
        case OP_BIPUSH:
            opbipush(text);
            break;
        case OP_LDC_W:
            opldc_w(text);
            break;
        case OP_ILOAD:
            opiload(text);
            break;
        case OP_ISTORE:
            opistore(text);
            break;
        case OP_POP:
            oppop(text);
            break;
        case OP_DUP:
            opdup(text);
            break;
        case OP_SWAP:
            opswap(text);
            break;
        case OP_IADD:
            opiadd(text);
            break;
        case OP_ISUB:
            opisub(text);
            break;
        case OP_IAND:
            opiand(text);
            break;
        case OP_IINC:
            opiinc(text);
            break;
        case OP_IFEQ:
            opifeq(text);
            break;
        case OP_IFLT:
            opiflt(text);
            break;
        case OP_ICMPEQ:
            opicmpeq(text);
            break;
        case OP_GOTO:
            opgoto(text);
            break;
        case OP_IRETURN:
            opireturn(text);
            break;
        case OP_IOR:
            opior(text);
            break;
        case OP_INVOKEVIRTUAL:
            opinvoke(text);
            break;
        case OP_WIDE:
            opwide(text);
            break;
        case 0xD1:
            opnewarr(text);
            break;
        case 0xD2:
            opiaload(text);
            break;
        case 0xD3:
            opiastore(text);
            break;
        case OP_IN:
            opin(text);
            break;
        case OP_OUT:
            opout(text);
            break;
        case OP_ERR:
            return oputil(1);
        case OP_HALT:
            return oputil(2);
        default:
            return oputil(3);
    }
    ijvm->count++;
    return true;
}

bool finished() { // EOF            HALT/ERR
    return ijvm->count == text_size() || !step();
}

void run() {
    while (step()) {
    }
}

int32_t get_constant(int i) {
    return ijvm->data->constdata[i];
}

uint8_t get_instruction() {
    return ijvm->data->textdata[ijvm->count];
}

void set_input(FILE *fp) {
    in = fp;
} 

void set_output(FILE *fp) {
    out = fp;
}

/**
 *  Opcode functions
 */
void opbipush(uint8_t *text){
    ijvm->count++;
    debug("BIPUSH 0x%02x\n", (char)text[ijvm->count]);
    int i = (char)text[ijvm->count];
    push(ijvm->stack, (short)i);
}
void opldc_w(uint8_t *text){
    debug("LDC_W %i (load 0x%02x)\n", shortConvert(text[ijvm->count + 1], text[ijvm->count + 2]), ijvm->data->constdata[shortConvert(text[ijvm->count + 1], text[ijvm->count + 2])]); //short
    push(ijvm->stack, ijvm->data->constdata[shortConvert(text[ijvm->count + 1], text[ijvm->count + 2])]);
    ijvm->count += 2;
}
void opiload(uint8_t *text){
    ijvm->count++;
    debug("ILOAD 0x%02x -> 0x%08x\n", text[ijvm->count], ijvm->frame->localstack[ijvm->currentframe]->array[text[ijvm->count]]);
    push(ijvm->stack, ijvm->frame->localstack[ijvm->currentframe]->array[text[ijvm->count]]);
}
void opistore(uint8_t *text){
    ijvm->count++;
    debug("ISTORE 0x%02x (", text[ijvm->count]);
    if (text[ijvm->count] < ijvm->frame->localstack[ijvm->currentframe]->capacity) {
        if(text[ijvm->count] > ijvm->frame->localstack[ijvm->currentframe]->top){
            ijvm->frame->localstack[ijvm->currentframe]->top = text[ijvm->count];
        }
        ijvm->frame->localstack[ijvm->currentframe]->array[text[ijvm->count]] = pop(ijvm->stack);
        debug("local var #%i)\n", text[ijvm->count]);
    } else {
        push(ijvm->frame->localstack[ijvm->currentframe], pop(ijvm->stack));
        debug("new local var #%i)\n", ijvm->frame->localstack[ijvm->currentframe]->top);
    }
}
void oppop(uint8_t *text){
    debug("POP\n");
    pop(ijvm->stack);
}
void opdup(uint8_t *text){
    debug("DUP\n");
    push(ijvm->stack, peek(ijvm->stack));
}
void opswap(uint8_t *text){
    debug("SWAP\n");
    int32_t a = pop(ijvm->stack);
    int32_t b = pop(ijvm->stack);
    push(ijvm->stack, a);
    push(ijvm->stack, b);
}
void opiadd(uint8_t *text){
    unsigned int a = pop(ijvm->stack);
    a += pop(ijvm->stack);
    push(ijvm->stack, a);
    debug("IADD result: %08x\n", peek(ijvm->stack));
}
void opisub(uint8_t *text){
    unsigned int a = pop(ijvm->stack);
    unsigned int b = pop(ijvm->stack);
    b = b-a;
    push(ijvm->stack, b);
    debug("ISUB = %08x\n", peek(ijvm->stack));
}
void opiand(uint8_t *text){
    debug("IAND\n");
    push(ijvm->stack, pop(ijvm->stack) & pop(ijvm->stack));
}
void opiinc(uint8_t *text){
    debug("IINC 0x%02x 0x%02x (", text[ijvm->count + 1], text[ijvm->count + 2]);
    int i = (char)text[ijvm->count + 2];
    ijvm->frame->localstack[ijvm->currentframe]->array[text[ijvm->count + 1]] += (short)i;
    debug("%i)\n", ijvm->frame->localstack[ijvm->currentframe]->array[text[ijvm->count + 1]]);
    ijvm->count += 2;
}
void opifeq(uint8_t *text){
    debug("IFEQ 0x%02x ", peek(ijvm->stack));
    if (pop(ijvm->stack) == 0) {
        debug("= true\n");
        ijvm->count += (short) shortConvert(text[ijvm->count + 1], text[ijvm->count + 2]) - 1;
    } else {
        debug("= false\n");
        ijvm->count += 2;
    }
}
void opiflt(uint8_t *text){
    debug("IFLT 0x%02x ", shortConvert(text[ijvm->count + 1], text[ijvm->count + 2]) - 1);
    int iflttest = pop(ijvm->stack);
    debug("hex value: %08x, value: %i", iflttest, iflttest);
    if ((signed int)iflttest < 0) {
        ijvm->count += (short)shortConvert(text[ijvm->count + 1], text[ijvm->count + 2]) - 1;
        debug(" = true, count = %i\n", ijvm->count);
    } else {
        debug(" = false\n");
        ijvm->count += 2;
    }
}
void opicmpeq(uint8_t *text){
    debug("IF_ICMPEQ %i", shortConvert(text[ijvm->count + 1], text[ijvm->count + 2]) - 1);
    if (pop(ijvm->stack) == pop(ijvm->stack)) {
        debug(" = true\n");
        ijvm->count += shortConvert(text[ijvm->count + 1], text[ijvm->count + 2]) - 1;
        debug("Count is now %i\n", ijvm->count);
    } else {
        debug(" = false\n");
        ijvm->count += 2;
    }
}
void opgoto(uint8_t *text){
    debug("GOTO 0x%02x (%i)\n", shortConvert(text[ijvm->count + 1], text[ijvm->count + 2]) - 1, (short) shortConvert(text[ijvm->count + 1], text[ijvm->count + 2]) - 1);
    ijvm->count += (short) shortConvert(text[ijvm->count + 1], text[ijvm->count + 2]) - 1;
}
void opireturn(uint8_t *text){
    debug("IRETURN %i to index %i, setting count to %i\n", peek(ijvm->stack), ijvm->refptr[ijvm->currentframe], ijvm->frame->localstack[ijvm->currentframe]->array[0]);
    ijvm->count = ijvm->frame->localstack[ijvm->currentframe]->array[0]; // retrieve previous count
    unsigned int returnval = peek(ijvm->stack); // store return value
    while(ijvm->stack->top >= ijvm->refptr[ijvm->currentframe]){ // remove remaining garbage from stack
        pop(ijvm->stack);
    }
    ijvm->refptr[ijvm->currentframe] = 0;
    push(ijvm->stack, returnval); // push return to top of stack
    free(ijvm->frame->localstack[ijvm->currentframe]);
    ijvm->currentframe--;
}
void opior(uint8_t *text){
    debug("IOR\n");
    push(ijvm->stack, pop(ijvm->stack) | pop(ijvm->stack));
}
void opinvoke(uint8_t *text){
    debug("INVOKEVIRTUAL %02x%02x 0x%02x", text[ijvm->count + 1], text[ijvm->count + 2], ijvm->data->constdata[(shortConvert(text[ijvm->count + 1], text[ijvm->count + 2]))]); //short (invoke method, pop OBJREF and pop arg from stack
    int oldcount = ijvm->count + 2; // store count in temp var
    ijvm->count = get_constant(shortConvert(text[ijvm->count+1], text[ijvm->count+2])); // move count to method area
    int numArgs = shortConvert(text[ijvm->count], text[ijvm->count+1]);
    int localframesize = shortConvert(text[ijvm->count+2], text[ijvm->count+3]);
    debug(" (%i arguments, %i local variables)\n", numArgs, localframesize);
    ijvm->currentframe++;
    ijvm->frame->localstack[ijvm->currentframe] = createStack(localframesize+numArgs);
    push(ijvm->frame->localstack[ijvm->currentframe], oldcount); // store count as lv[0]
    debug("Pushing count = %i,", oldcount);
    for (int i = numArgs - 1; i > 0; i--){
        debug(" arg #%i = %i,", i, peek(ijvm->stack));
        ijvm->frame->localstack[ijvm->currentframe]->array[i] = pop(ijvm->stack);
    }
    debug(" OBJREF at [%i]", ijvm->stack->top);
    ijvm->refptr[ijvm->currentframe] = ijvm->stack->top;
    debug(" refptr[%i] = %i\n", ijvm->currentframe, ijvm->refptr[ijvm->currentframe]);
    pop(ijvm->stack); // pop objref
    ijvm->count += 3;
}
void opwide(uint8_t *text){
    ijvm->count++;
    switch (text[ijvm->count]) {
        case OP_BIPUSH:
            ijvm->count++;
            debug("BIPUSH 0x%02x\n", (char)shortConvert(text[ijvm->count], text[ijvm->count+1]));
            int i = (char)shortConvert(text[ijvm->count], text[ijvm->count+1]);
            push(ijvm->stack, (short)i);
            break;
        case OP_ILOAD:
            ijvm->count++;
            debug("ILOAD 0x%02x -> 0x%02x\n", shortConvert(text[ijvm->count], text[ijvm->count+1]), ijvm->frame->localstack[ijvm->currentframe]->array[shortConvert(text[ijvm->count], text[ijvm->count+1])]);
            push(ijvm->stack, ijvm->frame->localstack[ijvm->currentframe]->array[shortConvert(text[ijvm->count], text[ijvm->count+1])]);
            break;
        case OP_ISTORE:
            ijvm->count++;
            debug("ISTORE 0x%02x (", shortConvert(text[ijvm->count], text[ijvm->count+1]));
            if (shortConvert(text[ijvm->count], text[ijvm->count+1]) < ijvm->frame->localstack[ijvm->currentframe]->capacity) {
                ijvm->frame->localstack[ijvm->currentframe]->array[shortConvert(text[ijvm->count], text[ijvm->count+1])] = pop(ijvm->stack);
                debug("local var #%i)\n", shortConvert(text[ijvm->count], text[ijvm->count+1]), ijvm->frame->localstack[ijvm->currentframe]->top, shortConvert(text[ijvm->count], text[ijvm->count+1]));
            } else {
                push(ijvm->frame->localstack[ijvm->currentframe], pop(ijvm->stack));
                debug("new local var #%i)\n", ijvm->frame->localstack[ijvm->currentframe]->top);
            }
            break;
    }
    ijvm->count++;
}
void opnewarr(uint8_t *text){ //0xBEEF0000 is the array pointer magic number
    debug("\033[0;33mNEWARRAY\033[0m");
    debug(" making [%i]array\n", ijvm->heapcount);
    ijvm->heap->localstack[ijvm->heapcount] = createStack(pop(ijvm->stack)); //make new heap array
    push(ijvm->stack, ijvm->heapcount+0xBEEF0000); // push reference back to stack
    ijvm->heapcount++; // increase array counter
}
void opiaload(uint8_t *text){
    debug("\033[0;33mIALOAD\033[0m"); // poparrref, popindex
    int arref = pop(ijvm->stack)-0xBEEF0000;
    int index = pop(ijvm->stack);
    debug(" loading [%i]array[%i] = %i\n", arref, index, ijvm->heap->localstack[arref]->array[index]);
    push(ijvm->stack, ijvm->heap->localstack[arref]->array[index]);

}
void opiastore(uint8_t *text){
    debug("\033[0;33mIASTORE\033[0m");  // poparrref, popindex, popvalue
    int arref = pop(ijvm->stack)-0xBEEF0000;
    int index = pop(ijvm->stack);
    int value = pop(ijvm->stack);
    debug(" storing [%i]array[%i] = %i\n", arref, index, value);
    ijvm->heap->localstack[arref]->array[index] = value;

}
void opin(uint8_t *text){
    debug("IN ");
    int word = fgetc(in);
    if (word == EOF) {
        debug("EOF\n", word);
        push(ijvm->stack, 0x0);
    } else {
        debug("0x%02x\n", word);
        push(ijvm->stack, word);
    }
}
void opout(uint8_t *text){
    debug("\033[0;36mOUT\033[0m %i\n", peek(ijvm->stack));
    unsigned char c = pop(ijvm->stack);
    fprintf(out, "%c", c);
}
bool oputil(uint8_t i){
    switch(i){
        case 0:
            debug("NOP\n", peek(ijvm->stack));
            return true;
        case 1:
            debug("ERR\n");
            break;
        case 2:
            debug("HALT\n");
            break;
        case 3:
            debug("?\n");
            break;
    }
    return false;
}