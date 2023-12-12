/*
 * Carcass of the compiler
 */

#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"

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

// Abort program: missing expected value
void expected(const char * str) {
    printf("%s expected\n", str);
    exit(1);
}

// Match a predetermined input character
void match(const char x) {
    if (nextChar == x) {
        readChar();
    } else {
        char str[2];
        str[0] = x;
        str[1] = '\0';
        expected(str);
    }
}

// Get an identifier
char getName() {
    if (!isalpha(nextChar)) {
        expected("Name");
    }
    char name = toupper(nextChar);
    readChar();
    return name;
}

// Get a number
char getNum() {
    if (!isdigit(nextChar)) {
        expected("Integer value");
    }
    char num = nextChar;
    readChar();
    return num;
}

// Print a string after tab
void emit(const char * str) {
    printf("\t%s", str);
}

// Print a string with tab and \n
void emitLn(const char * str) {
    printf("\t%s\n", str);
}

void init() {
    readChar();
}

int main() {
    init();
}
