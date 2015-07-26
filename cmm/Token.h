//
//  token.h
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#ifndef __CMM__token__
#define __CMM__token__

#include <stdio.h>
#include <string.h>
#define MAX_LEXEME_LEN 256

typedef enum {
    // errors
    ERR_BADINPUT = -5,
    ERR_BADSTR = -4,
    ERR_BADFLOAT = -3,
    ERR_BADCOMMENT = -2,
    ERR_BUFFEROVRFLW = -1,
    
    // tokens
    TOK_EOF = 0,
    TOK_IDENT,
    
    // literals
    LIT_STR,
    LIT_INT,
    LIT_FLOAT,
    LIT_CHAR,
    
    // keywords
    KW_EXTERN,
    KW_INT,
    KW_FLOAT,
    KW_VOID,
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_RETURN,
    
    //symbols
    SYM_OPEN,
    SYM_CLOSE,
    SYM_PLUS,
    SYM_MINUS,
    SYM_MUL,
    SYM_DIV,
    SYM_LESS,
    SYM_LESS_EQ,
    SYM_GREATER,
    SYM_GREATER_EQ,
    SYM_EQUAL,
    SYM_NOT_EQ,
    SYM_ASSIGN,
    SYM_SEMICOLON,
    SYM_COMMA,
    SYM_SQ_OPEN,
    SYM_SQ_CLOSE,
    SYM_CURLY_OPEN,
    SYM_CURLY_CLOSE,
    SYM_EXCLAMATION
    
} tokenType;

class Token { // Token Class
    tokenType _tokenType;      // the actual token value
    char _lexeme[MAX_LEXEME_LEN];  // the lexeme attribute
    int _row, _col;         // row and column of the lexeme
public:
    
    Token() { // default constructor
        _row = 0;
        _col = 0;
        strcpy(_lexeme, "");
        strncpy(_lexeme, "", MAX_LEXEME_LEN - 1);
        _tokenType = TOK_EOF;
    }
    
    Token(tokenType tokenType, int row, int col, char* lexeme) {  // create an instance of Token class with provided values
        _tokenType = tokenType;
        _row = row;
        _col = col;
        strncpy(_lexeme, lexeme, MAX_LEXEME_LEN - 1);
    }

    void setRow(int);
    void incrementRow();
    int getRow();
    
    void setCol(int);
    void incrementCol();
    int getCol();
    
    void setTokenType(tokenType);
    tokenType getTokenType();
    
    void setLexeme(char*);
    char* getLexeme();
    char* getFormattedLexeme();
};

#endif /* defined(__CMM__token__) */
