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


// Functions for processing of math expressions

// Parse a factor of a product
void factor() {
    char strValue[20];

    char num = getNum();
    strValue[0] = num;
    strValue[1] = '\0';

    encode("movq $");
    encodePlain(strValue);
    encodeLn(", %rdx");
}

// Translate a multiplication
void multiply() {
    match('*');
    factor();
    encodeLn("imulq %rdx");
    encodeLn("movq %rax, %rdx");
}

//Translate a division
void divide() {
    match('/');
    factor();
    encodeLn("movq %rdx, %rsi");
    encodeLn("cqo");
    encodeLn("idivq %rsi");
    encodeLn("movq %rax, %rdx");
}

// Parse and translate a single term
void term() {
    factor();
    while(nextChar == '*' || nextChar == '/') {
        //encodeLn("pushq %rdx");
        encodeLn("movq %rdx, %rax");
        switch (nextChar) {
            case '*':
                multiply();
                break;
            case '/':
                divide();
                break;
            default:
                expected("'/' or '*' operation");
        }
    }
}

// Translate an addition
void add() {
    match('+');
    encodeLn("pushq %rax");
    term();
    encodeLn("popq %rax");
    encodeLn("add %rax, %rdx");
}

// Translate a subtraction
void subtract() {
    match('-');
    encodeLn("pushq %rax");
    term();
    encodeLn("popq %rax");
    encodeLn("sub %rdx, %rax");
    encodeLn("movq %rax, %rdx");
}

// Parse and translate an expression
void expression() {
    term();
    while (nextChar == '+' || nextChar == '-') {
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
}





// Translating functions

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

    // BASH pipe pointer
    FILE *bashPipe;
    char *cmd = "as asm.s -o asm.o; gcc asm.o -o asm";
    char buf[128];

    // Assemble and link with GCC
    if ((bashPipe = popen(cmd, "r")) == NULL) {
        printf("Error running BASH! \n");
        exit(1);
    }
    while (fgets(buf, 128, bashPipe) != NULL) {
        printf("%s", buf);
    }
    if (pclose(bashPipe)) {
        printf("GCC not found or exited with error status\n");
        exit(1);
    }
}

int main() {
    init();
    expression();

    compile();
}

