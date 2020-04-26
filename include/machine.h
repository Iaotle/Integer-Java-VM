#ifndef MACHINE_H
#define MACHINE_H
#include <stdint.h>
#include <structs.h>


struct ijvm *ijvm;
FILE *in;
FILE *out;

/**
 *  Converts endianess
 */
int endConvert(uint8_t *data);

/**
 *  Converts short endianess
 */
int shortConvert(uint8_t data, uint8_t data2);

/**
 *  Reads the contents of ijvm binary
 */
int readbinary(char *binary_file);

/**
 *  Opcode functions
 */
void opbipush(uint8_t *text);
void opldc_w(uint8_t *text);
void opiload(uint8_t *text);
void opistore(uint8_t *text);
void oppop(uint8_t *text);
void opdup(uint8_t *text);
void opswap(uint8_t *text);
void opiadd(uint8_t *text);
void opisub(uint8_t *text);
void opiand(uint8_t *text);
void opiinc(uint8_t *text);
void opifeq(uint8_t *text);
void opiflt(uint8_t *text);
void opicmpeq(uint8_t *text);
void opgoto(uint8_t *text);
void opireturn(uint8_t *text);
void opior(uint8_t *text);
void opinvoke(uint8_t *text);
void opwide(uint8_t *text);
void opnewarr(uint8_t *text);
void opiaload(uint8_t *text);
void opiastore(uint8_t *text);
void opin(uint8_t *text);
void opout(uint8_t *text);
bool oputil(uint8_t i);

#endif