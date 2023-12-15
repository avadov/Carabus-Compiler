/*
 * Carcass of the compiler
 */

#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"

// Lookahead letter of a program text
char nextChar;

// Program source
FILE *src;

// Output file for translation of the source code to assembly language
FILE *asm_code;

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
void encode(const char * str) {
    printf("\t%s", str);
    fprintf(asm_code, "\t%s", str);
}

// Print a string with tab and \n
void encodeLn(const char * str) {
    printf("\t%s\n", str);
    fprintf(asm_code, "\t%s\n", str);
}

// Parse and translate a single term
void term() {
    char num = getNum();
    encode("MOVE $");
    printf("%c", num);
    encodeLn(", %DX");
}

// Translate an addition
void add() {
    match('+');
    term();
    encodeLn("ADD %AX, %DX");
}

// Translate a subtraction
void subtract() {
    match('-');
    term();
    encodeLn("SUB %DX, %AX");
    encodeLn("MOV %AX, %DX");
}

// Parse and translate an expression
void expression() {
    term();
    encodeLn("MOVE %DX, %AX");
    switch (nextChar) {
        case '+':
            add();
            break;
        case '-':
            subtract();
            break;
        default:
            expected("Operation");
    }
}

void init() {
    asm_code = fopen("asm.s", "w");
    readChar();
}

// Compile assembly code to executable file
void compile() {
    fclose(asm_code);
}

int main() {
    init();
    expression();

    compile();
}


