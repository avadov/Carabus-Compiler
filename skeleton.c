/*
 * Carcass of the compiler
 */

#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"

char strValue[80];

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

// Print a string without tab. Then add the string to assembly source
void encodePlain(const char * str) {
    printf("%s", str);
    fprintf(asm_code, "%s", str);
}

// Print a string after tab. Then add the string to assembly source
void encode(const char * str) {
    printf("\t%s", str);
    fprintf(asm_code, "\t%s", str);
}

// Print a string with tab and \n. Then add the string to assembly source
void encodeLn(const char * str) {
    printf("\t%s\n", str);
    fprintf(asm_code, "\t%s\n", str);
}

// Parse and translate a single term
void term() {
    char num = getNum();
    strValue[0] = num;
    strValue[1] = '\0';

    encode("movq $");
    encodePlain(strValue);
    encodeLn(", %rdx");
}

// Translate an addition
void add() {
    match('+');
    term();
    encodeLn("add %rax, %rdx");
}

// Translate a subtraction
void subtract() {
    match('-');
    term();
    encodeLn("sub %rdx, %rax");
    encodeLn("movq %rax, %rdx");
}

// Parse and translate an expression
void expression() {
    term();
    encodeLn("movq %rdx, %rax");
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
    fprintf(asm_code, "%s",
            ".globl main \n"
            ".text \n"
            "main: \n");
    readChar();
}

// Compile assembly code to executable file
void compile() {
    fprintf(asm_code, "%s",
            "\t"
            "movq %rdx, %rdi \n\t"
            "call print_number\n"
            "exit: \n\t"
            "ret \n\n"
            "# Print a number passed in RDI register \n"
            "print_number: \n\t"
            "push %rbx \n\t"
            "movq %rdi, %rsi \n\t"
            "xor %eax, %eax \n\t"
            "lea format(%rip), %rdi \n\t"
            "call printf \n\t"
            "pop %rbx \n\t"
            "ret \n\n"
            ".data \n\t"
            "format: .asciz \"%d\\n\" \n");
    fclose(asm_code);
}

int main() {
    init();
    expression();

    compile();
}

