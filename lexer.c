#include <stdio.h>
#include <ctype.h>
#include <string.h>

int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int nextToken;
FILE *in_fp;

void addChar();
void getChar();
void getNonBlank();
int lex();
int lookup(char ch);

#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define INT_TYPE 27   /* tam_sayı */
#define IF 28         /* eğer */
#define ELSE 29       /* değilse */
#define WHILE 30      /* iken */
#define PRINT 31      /* yazdır */
#define EQ_OP 32      /* == */
#define LT_OP 33      /* < */
#define GT_OP 34      /* > */
#define LEFT_BRACE 35 /* { */
#define RIGHT_BRACE 36/* } */
#define SEMICOLON 37  /* ; */
#define EOF_CODE -1

int isTurkishLetter(char c) {
    return (c == 'ç' || c == 'Ç' || c == 'ğ' || c == 'Ğ' || 
            c == 'ı' || c == 'I' || c == 'İ' || c == 'ö' || 
            c == 'Ö' || c == 'ş' || c == 'Ş' || c == 'ü' || c == 'Ü');
}

void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    } else {
        printf("Error: lexeme is too long\n");
    }
}

void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isalpha(nextChar) || isTurkishLetter(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else
            charClass = UNKNOWN;
    } else {
        charClass = EOF;
    }
}

void getNonBlank() {
    while (isspace(nextChar))
        getChar();
}

int lookup(char ch) {
    switch (ch) {
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;
        case '=':
            addChar();
            if (nextChar == '=') {
                addChar();
                nextToken = EQ_OP;
            } else {
                nextToken = ASSIGN_OP;
            }
            break;
        case '<':
            addChar();
            nextToken = LT_OP;
            break;
        case '>':
            addChar();
            nextToken = GT_OP;
            break;
        case '{':
            addChar();
            nextToken = LEFT_BRACE;
            break;
        case '}':
            addChar();
            nextToken = RIGHT_BRACE;
            break;
        case ';':
            addChar();
            nextToken = SEMICOLON;
            break;
        default:
            addChar();
            nextToken = EOF_CODE;
            break;
    }
    return nextToken;
}

int isKeyword(char *lex) {
    if (strcmp(lex, "tam_sayı") == 0) {
        nextToken = INT_TYPE;
        return 1;
    }
    if (strcmp(lex, "eğer") == 0) {
        nextToken = IF;
        return 1;
    }
    if (strcmp(lex, "değilse") == 0) {
        nextToken = ELSE;
        return 1;
    }
    if (strcmp(lex, "iken") == 0) {
        nextToken = WHILE;
        return 1;
    }
    if (strcmp(lex, "yazdır") == 0) {
        nextToken = PRINT;
        return 1;
    }
    return 0;
}

int lex() {
    lexLen = 0;
    getNonBlank();
    
    switch (charClass) {
        case LETTER:
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT || isTurkishLetter(nextChar)) {
                addChar();
                getChar();
            }
            if (!isKeyword(lexeme)) {
                nextToken = IDENT;
            }
            break;
            
        case DIGIT:
            addChar();
            getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }
            nextToken = INT_LIT;
            break;
            
        case UNKNOWN:
            lookup(nextChar);
            getChar();
            break;
            
        case EOF:
            nextToken = EOF_CODE;
            lexeme[0] = 'E'; lexeme[1] = 'O'; lexeme[2] = 'F'; lexeme[3] = 0;
            break;
    }
    printf("Next token is: %d, Next lexeme is %s\n", nextToken, lexeme);
    return nextToken;
}