/*
 * Carcass of the compiler
 */

#include "stdio.h"
#include "stdlib.h"

// Lookahead letter of a program text
char nextChar;

// Read next character from input
void readChar() {
    nextChar = getc(stdin);
}

// Report an error
void error(const char * str) {
    printf("\nError: %s\n", str);
}

// Report an error and exit
void halt(const char * str) {
    error(str);
    exit(1);
}

void init() {
    readChar();
}

int main() {
    init();
}
