#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* ==========================================
   TOKEN CODES (Matches your project table)
   ========================================== */
#define INT_LIT      10
#define IDENT        11
#define ASSIGN_OP    20
#define ADD_OP       21
#define SUB_OP       22
#define MULT_OP      23
#define DIV_OP       24
#define LEFT_PAREN   25
#define RIGHT_PAREN  26
#define INT_TYPE     27   /* tam_sayı */
#define IF           28   /* eğer */
#define ELSE         29   /* değilse */
#define ISE          38   /* ise */
#define WHILE        30   /* iken */
#define FOR          39   /* için */
#define PRINT        31   /* yazdır */
#define EQ_OP        32   /* == */
#define NEQ_OP       40   /* != */
#define LT_OP        33   /* < */
#define LE_OP        41   /* <= */
#define GT_OP        34   /* > */
#define GE_OP        42   /* >= */
#define LEFT_BRACE   35   /* { */
#define RIGHT_BRACE  36   /* } */
#define SEMICOLON    37   /* ; */
#define PLUSPLUS     43   /* ++ (grammar requirement) */
#define MINUSMINUS   44   /* -- (grammar requirement) */
#define EOF_CODE     -1

/* ==========================================
   GLOBAL STATE (Textbook Chapter 4 style)
   ========================================== */
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int nextToken;
FILE *in_fp;

/* ==========================================
   SYMBOL TABLE
   ========================================== */
#define MAX_SYMBOLS 100
char symbolTable[MAX_SYMBOLS][100];
int symbolCount = 0;

void addSymbol(char *name) {
    /* Check if the identifier already exists */
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i], name) == 0) {
            return; /* Already in table */
        }
    }
    
    /* Add new identifier to the table */
    if (symbolCount < MAX_SYMBOLS) {
        strcpy(symbolTable[symbolCount], name);
        printf("    [SYMBOL TABLE] Added identifier: %s\n", name);
        symbolCount++;
    } else {
        fprintf(stderr, "Error: Symbol table full\n");
    }
}

/* ==========================================
   FORWARD DECLARATIONS (For clean compilation)
   ========================================== */
void getChar(void);
void addChar(void);
void getNonBlank(void);
int lex(void);
void error(void);
void match(int expectedToken);

void program(void);
void stmt(void);
void declaration(void);
void assign(void);
void if_stmt(void);
void optional(void);
void loop_stmt(void);
void begin_index(void);
void operation(void);
void condition(void);
void rel_op(void);
void expr(void);
void term(void);
void factor(void);


/* ==========================================
   LEXER FUNCTIONS
   ========================================== */
int isValidBNFChar(char c) {
    /* Standard ASCII letters */
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;
    /* Turkish lowercase: ç, ğ, ı, ö, ş, ü */
    if (c == 'ç' || c == 'ğ' || c == 'ı' || c == 'ö' || c == 'ş' || c == 'ü') return 1;
    /* Turkish uppercase: Ç, Ğ, İ, Ö, Ş, Ü */
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
        fprintf(stderr, "Lexical Error: Lexeme too long\n");
    }
}

void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isValidBNFChar(nextChar))
            charClass = 0; /* LETTER */
        else if (isValidBNFDigit(nextChar))
            charClass = 1; /* DIGIT */
        else
            charClass = 99; /* UNKNOWN */
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
        case '+':
            getChar();
            if (nextChar == '+') { addChar(); nextToken = PLUSPLUS; }
            else { ungetc(nextChar, in_fp); nextToken = ADD_OP; }
            break;
        case '-':
            getChar();
            if (nextChar == '-') { addChar(); nextToken = MINUSMINUS; }
            else { ungetc(nextChar, in_fp); nextToken = SUB_OP; }
            break;
        case '*': nextToken = MULT_OP; break;
        case '/': nextToken = DIV_OP; break;
        case '=':
            getChar();
            if (nextChar == '=') { addChar(); nextToken = EQ_OP; }
            else { ungetc(nextChar, in_fp); nextToken = ASSIGN_OP; }
            break;
        case '!':
            getChar();
            if (nextChar == '=') { addChar(); nextToken = NEQ_OP; }
            else { ungetc(nextChar, in_fp); error(); }
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
        case '{': nextToken = LEFT_BRACE; break;
        case '}': nextToken = RIGHT_BRACE; break;
        case ';': nextToken = SEMICOLON; break;
        default:
            fprintf(stderr, "Lexical Error: Unrecognized character '%c'\n", ch);
            nextToken = EOF_CODE;
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
        case 0: /* LETTER */
            addChar(); getChar();
            while (charClass == 0 || charClass == 1) { addChar(); getChar(); }
            if (!isKeyword(lexeme)) nextToken = IDENT;
            break;
        case 1: /* DIGIT */
            addChar(); getChar();
            while (charClass == 1) { addChar(); getChar(); }
            nextToken = INT_LIT;
            break;
        case 99: /* UNKNOWN */
            lookup(nextChar);
            getChar();
            break;
        case EOF_CODE:
            nextToken = EOF_CODE;
            strcpy(lexeme, "EOF");
            break;
    }
    return nextToken;
}

/* ==========================================
   PARSER HELPER FUNCTIONS
   ========================================== */
void match(int expectedToken) {
    if (nextToken == expectedToken) {
        lex();
    } else {
        error();
    }
}

void error() {
    fprintf(stderr, "\n❌ SYNTAX ERROR at '%s'\n", lexeme);
    fprintf(stderr, "   Parser expected a valid token according to the grammar.\n");
    exit(1);
}

/* ==========================================
   RECURSIVE-DESCENT PARSER FUNCTIONS
   ========================================== */

/* <program> → <stmt_list> EOF */
void program() {
    printf("▶ Enter <program>\n");
    while (nextToken != EOF_CODE) {
        stmt();
    }
    printf("✅ Exit <program> - Parsing successful!\n");
}

/* <stmt> → <declaration> | <assign> | <if_stmt> | <loop_stmt> */
void stmt() {
    printf("  ▶ Enter <stmt>\n");
    switch (nextToken) {
        case INT_TYPE: declaration(); break;
        case IDENT:    assign(); break;
        case IF:       if_stmt(); break;
        case WHILE:
        case FOR:      loop_stmt(); break;
        default:       error(); break;
    }
    printf("  ✅ Exit <stmt>\n");
}

/* <declaration> → tam_sayı <id> ";" */
void declaration() {
    printf("    ▶ Enter <declaration>\n");
    match(INT_TYPE);
    
    if (nextToken == IDENT) {
        addSymbol(lexeme); /* Add the variable to the symbol table */
    }
    match(IDENT);
    match(SEMICOLON);
    
    printf("    ✅ Exit <declaration>\n");
}

/* <assign> → <id> "=" <expr> ";" */
void assign() {
    printf("    ▶ Enter <assign>\n");
    if (nextToken == IDENT) {
        addSymbol(lexeme); /* Add the variable to the symbol table just in case it wasn't declared */
    }
    match(IDENT);
    match(ASSIGN_OP);
    expr();
    match(SEMICOLON);
    printf("    ✅ Exit <assign>\n");
}

/* <if_stmt> → eğer "(" <condition> ")" <statement> <optional> */
void if_stmt() {
    printf("    ▶ Enter <if_stmt>\n");
    match(IF);
    match(LEFT_PAREN);
    condition();
    match(RIGHT_PAREN);
    stmt();
    optional();
    printf("    ✅ Exit <if_stmt>\n");
}

/* <optional> → değilse <statement> | ise "(" <condition> ")" <statement> <optional> | ε */
void optional() {
    printf("    ▶ Enter <optional>\n");
    if (nextToken == ELSE) {
        lex();
        stmt();
    } else if (nextToken == ISE) {
        lex();
        match(LEFT_PAREN);
        condition();
        match(RIGHT_PAREN);
        stmt();
        optional();
    }
    /* else: ε (do nothing) */
    printf("    ✅ Exit <optional>\n");
}

/* <loop_stmt> → iken "(" <condition> ")" <statement> 
              | için "(" <begin_index> ";" <condition> ";" <operation> ")" <statement> */
void loop_stmt() {
    printf("    ▶ Enter <loop_stmt>\n");
    if (nextToken == WHILE) {
        lex();
        match(LEFT_PAREN);
        condition();
        match(RIGHT_PAREN);
        stmt();
    } else if (nextToken == FOR) {
        lex();
        match(LEFT_PAREN);
        begin_index();
        match(SEMICOLON);
        condition();
        match(SEMICOLON);
        operation();
        match(RIGHT_PAREN);
        stmt();
    } else {
        error();
    }
    printf("    ✅ Exit <loop_stmt>\n");
}

/* <begin_index> → <id> "=" <expr> */
void begin_index() {
    printf("      ▶ Enter <begin_index>\n");
    if (nextToken == IDENT) {
        addSymbol(lexeme);
    }
    match(IDENT);
    match(ASSIGN_OP);
    expr();
    printf("      ✅ Exit <begin_index>\n");
}

/* <operation> → <id> "++" | <id> "--" | "++" <id> | "--" <id> */
void operation() {
    printf("      ▶ Enter <operation>\n");
    if (nextToken == PLUSPLUS || nextToken == MINUSMINUS) {
        lex();
        match(IDENT);
    } else if (nextToken == IDENT) {
        lex();
        if (nextToken == PLUSPLUS || nextToken == MINUSMINUS) {
            lex();
        } else {
            error();
        }
    } else {
        error();
    }
    printf("      ✅ Exit <operation>\n");
}

/* <condition> → <expr> <rel_op> <expr> */
void condition() {
    printf("      ▶ Enter <condition>\n");
    expr();
    rel_op();
    expr();
    printf("      ✅ Exit <condition>\n");
}

/* <rel_op> → "<" | ">" | "==" | "!=" | "<=" | ">=" */
void rel_op() {
    printf("        ▶ Enter <rel_op>\n");
    switch (nextToken) {
        case LT_OP: case GT_OP: case EQ_OP: 
        case NEQ_OP: case LE_OP: case GE_OP:
            lex(); break;
        default: error(); break;
    }
    printf("        ✅ Exit <rel_op>\n");
}

/* <expr> → <term> { ("+" | "-") <term> } */
void expr() {
    printf("        ▶ Enter <expr>\n");
    term();
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        term();
    }
    printf("        ✅ Exit <expr>\n");
}

/* <term> → <factor> { ("*" | "/") <factor> } */
void term() {
    printf("          ▶ Enter <term>\n");
    factor();
    while (nextToken == MULT_OP || nextToken == DIV_OP) {
        lex();
        factor();
    }
    printf("          ✅ Exit <term>\n");
}

/* <factor> → "(" <expr> ")" | <id> | <number> */
void factor() {
    printf("            ▶ Enter <factor>\n");
    if (nextToken == LEFT_PAREN) {
        lex();
        expr();
        match(RIGHT_PAREN);
    } else if (nextToken == IDENT) {
        addSymbol(lexeme); /* Track identifier usage */
        lex();
    } else if (nextToken == INT_LIT) {
        lex();
    } else {
        error();
    }
    printf("            ✅ Exit <factor>\n");
}

/* ==========================================
   MAIN DRIVER
   ========================================== */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }
    
    if ((in_fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Error: Cannot open %s\n", argv[1]);
        return 1;
    }
    
    printf("--- Lexical & Syntax Analysis Started ---\n");
    getChar();
    lex(); /* Prime the lexer */
    program();
    
    printf("--- Analysis Complete ---\n");
    
    printf("\n--- Final Symbol Table ---\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("[%d] %s\n", i, symbolTable[i]);
    }
    
    fclose(in_fp);
    return 0;
}