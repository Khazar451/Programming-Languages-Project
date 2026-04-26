#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* Set to 0 before integrating with your parser */
#define DEBUG 1

/* Global declarations */
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int nextToken;
FILE *in_fp;

/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();
int lookup(char ch);
int isKeyword(char *lex);

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
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
#define ISE 38        /* ise */
#define WHILE 30      /* iken */
#define FOR 39        /* için */
#define PRINT 31      /* yazdır */
#define EQ_OP 32      /* == */
#define NEQ_OP 40     /* != */
#define LT_OP 33      /* < */
#define LE_OP 41      /* <= */
#define GT_OP 34      /* > */
#define GE_OP 42      /* >= */
#define LEFT_BRACE 35 /* { */
#define RIGHT_BRACE 36/* } */
#define SEMICOLON 37  /* ; */
#define EOF_CODE -1
#define LEX_ERROR 99  /* Handles unrecognized characters safely */

int isValidBNFChar(char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;
    if (c == 'ç' || c == 'ğ' || c == 'ı' || c == 'ö' || c == 'ş' || c == 'ü') return 1;
    if (c == 'Ç' || c == 'Ğ' || c == 'İ' || c == 'Ö' || c == 'Ş' || c == 'Ü') return 1;
    return 0;
}

int isValidBNFDigit(char c) {
    return (c >= '0' && c <= '9');
}

void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = '\0';
    } else {
        printf("Error: lexeme is too long\n");
    }
}

void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isValidBNFChar(nextChar))
            charClass = LETTER;
        else if (isValidBNFDigit(nextChar))
            charClass = DIGIT;
        else
            charClass = UNKNOWN;
    } else {
        charClass = EOF_CODE;
    }
}

void getNonBlank() {
    while (isspace(nextChar))
        getChar();
}

int lookup(char ch) {
    addChar();
    switch (ch) {
        case '(': nextToken = LEFT_PAREN; break;
        case ')': nextToken = RIGHT_PAREN; break;
        case '+': nextToken = ADD_OP; break;
        case '-': nextToken = SUB_OP; break;
        case '*': nextToken = MULT_OP; break;
        case '/': nextToken = DIV_OP; break;
        case '{': nextToken = LEFT_BRACE; break;
        case '}': nextToken = RIGHT_BRACE; break;
        case ';': nextToken = SEMICOLON; break;
        case '=':
            getChar();
            if (nextChar == '=') { addChar(); nextToken = EQ_OP; }
            else { ungetc(nextChar, in_fp); nextToken = ASSIGN_OP; }
            break;
        case '!':
            getChar();
            if (nextChar == '=') { addChar(); nextToken = NEQ_OP; }
            else { 
                ungetc(nextChar, in_fp); 
                printf("Lexical Error: Invalid '!' operator\n");
                nextToken = LEX_ERROR; 
            }
            break;
        case '<':
            getChar();
            if (nextChar == '=') { addChar(); nextToken = LE_OP; }
            else { ungetc(nextChar, in_fp); nextToken = LT_OP; }
            break;
        case '>':
            getChar();
            if (nextChar == '=') { addChar(); nextToken = GE_OP; }
            else { ungetc(nextChar, in_fp); nextToken = GT_OP; }
            break;
            
        default:
            printf("Lexical Error: Unrecognized character '%c'\n", ch);
            nextToken = LEX_ERROR;
            break;
    }
    return nextToken;
}

int isKeyword(char *lex) {
    if (strcmp(lex, "tam_sayı") == 0)   { nextToken = INT_TYPE; return 1; }
    if (strcmp(lex, "eğer") == 0)       { nextToken = IF;       return 1; }
    if (strcmp(lex, "değilse") == 0)    { nextToken = ELSE;     return 1; }
    if (strcmp(lex, "ise") == 0)        { nextToken = ISE;      return 1; }
    if (strcmp(lex, "iken") == 0)       { nextToken = WHILE;    return 1; }
    if (strcmp(lex, "için") == 0)       { nextToken = FOR;      return 1; }
    if (strcmp(lex, "yazdır") == 0)     { nextToken = PRINT;    return 1; }
    return 0;
}

int lex() {
    lexLen = 0;
    getNonBlank();
    
    switch (charClass) {
        case LETTER:
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {
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
            
        case EOF_CODE:
            nextToken = EOF_CODE;
            lexeme[0] = 'E'; lexeme[1] = 'O'; lexeme[2] = 'F'; lexeme[3] = '\0';
            break;
    }
    
    #if DEBUG
    printf("Next token is: %d, Next lexeme is %s\n", nextToken, lexeme);
    #endif
    
    return nextToken;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }
    
    if ((in_fp = fopen(argv[1], "r")) == NULL) {
        printf("Error: Cannot open %s\n", argv[1]);
        return 1;
    }
    
    printf("--- Lexical Analysis Started ---\n");
    getChar();
    do {
        lex();
    } while (nextToken != EOF_CODE);
    
    printf("--- Lexical Analysis Complete ---\n");
    fclose(in_fp);
    return 0;
}
