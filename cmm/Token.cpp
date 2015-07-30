//
//  token.cpp
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#include "token.h"


void Token::setRow(int row) {
    _row = row;
}

void Token::incrementRow() {
    ++_row;
}

int Token::getRow() {
    return _row;
}

void Token::setCol(int col) {
    _col = col;
}

void Token::incrementCol() {
    ++_col;
}

int Token::getCol() {
    return _col;
}

void Token::setTokenType(tokenType tokenType) {
    _tokenType = tokenType;
}

tokenType Token::getTokenType() {
    return _tokenType;
}

void Token::setLexeme(char *s) {
    strncpy(_lexeme, s, MAX_LEXEME_LEN - 1);
    _lexeme[MAX_LEXEME_LEN - 1] = '\0';
}

char* Token::getLexeme() {
    return _lexeme;
}

char* Token::getFormattedLexeme() {
    
    static char result[MAX_LEXEME_LEN];
    
    switch (_tokenType) {
            // ERRORS
        case ERR_BADINPUT:
            sprintf(result, "ERROR<%d,%d>: Unrecognized input found: \"0x%02x\"", _row, _col, _lexeme[0]);
            break;
        case ERR_BADSTR:
            sprintf(result, "ERROR<%d,%d>: Unterminated String found: \"%s...", _row, _col, _lexeme);
            break;
        case ERR_BADFLOAT:
            sprintf(result, "<ERROR<%d,%d>: Bad Floating point value found: \"%s\"", _row, _col, _lexeme);
            break;
        case ERR_BADCOMMENT:
            sprintf(result, "ERROR<%d,%d>: Unterminated Comment found: EOF", _row, _col);
            break;
        case ERR_BUFFEROVRFLW:
            sprintf(result, "ERROR<%d,%d>:Buffer Overflow", _row, _col);
            break;
            
            // TOKENS
        case TOK_EOF:
            sprintf(result, "EOF<%d,%d>", _row, _col);
            break;
        case TOK_IDENT:
            sprintf(result, "IDENTIFIER<%d,%d>: \"%s\" ",_row, _col, _lexeme);
            break;
            
            // LITERALS
        case LIT_FLOAT:
            sprintf(result, "LITERAL<%d,%d>: float(%s)", _row, _col, _lexeme);
            break;
        case LIT_INT:
            sprintf(result, "LITERAL<%d,%d>: int(%s)", _row, _col, _lexeme);
            break;
        case LIT_STR:
            sprintf(result, "LITERAL<%d,%d>: string(\"%s\")", _row, _col, _lexeme);
            break;
        case LIT_CHAR:
            sprintf(result, "LITERAL<%d,%d>: char(\'%s\')", _row, _col, _lexeme);
            break;
            
            // KEYWORDS
        case KW_IF:
            sprintf(result, "KEYWORD<%d,%d>: if", _row, _col);
            break;
        case KW_WHILE:
            sprintf(result, "KEYWORD<%d,%d>: while", _row, _col);
            break;
        case KW_EXTERN:
            sprintf(result, "KEYWORD<%d,%d>: extern", _row, _col);
            break;
        case KW_ELSE:
            sprintf(result, "KEYWORD<%d,%d>: else", _row, _col);
            break;
        case KW_FLOAT:
            sprintf(result, "KEYWORD<%d,%d>: float", _row, _col);
            break;
        case KW_INT:
            sprintf(result, "KEYWORD<%d,%d>: int", _row, _col);
            break;
        case KW_RETURN:
            sprintf(result, "KEYWORD<%d,%d>: return", _row, _col);
            break;
        case KW_VOID:
            sprintf(result, "KEYWORD<%d,%d>: void", _row, _col);
            break;
            
            // SYMBOLS
        case SYM_OPEN:
            sprintf(result, "SYMBOL<%d,%d>: \"(\"", _row, _col);
            break;
        case SYM_CLOSE:
            sprintf(result, "SYMBOL<%d,%d>: \")\"", _row, _col);
            break;
        case SYM_ASSIGN:
            sprintf(result, "SYMBOL<%d,%d>: \"=\"", _row, _col);
            break;
        case SYM_COMMA:
            sprintf(result, "SYMBOL<%d,%d>: \",\"", _row, _col);
            break;
        case SYM_CURLY_CLOSE:
            sprintf(result, "SYMBOL<%d,%d>: \"}\"", _row, _col);
            break;
        case SYM_CURLY_OPEN:
            sprintf(result, "SYMBOL<%d,%d>: \"{\"", _row, _col);
            break;
        case SYM_DIV:
            sprintf(result, "SYMBOL<%d,%d>: \"/\"", _row, _col);
            break;
        case SYM_EQUAL:
            sprintf(result, "SYMBOL<%d,%d>: \"==\"", _row, _col);
            break;
        case SYM_GREATER:
            sprintf(result, "SYMBOL<%d,%d>: \">\"", _row, _col);
            break;
        case SYM_GREATER_EQ:
            sprintf(result, "SYMBOL<%d,%d>: \">=\"", _row, _col);
            break;
        case SYM_LESS:
            sprintf(result, "SYMBOL<%d,%d>: \"<\"", _row, _col);
            break;
        case SYM_LESS_EQ:
            sprintf(result, "SYMBOL<%d,%d>: \"<=\"", _row, _col);
            break;
        case SYM_MINUS:
            sprintf(result, "SYMBOL<%d,%d>: \"-\"", _row, _col);
            break;
        case SYM_MUL:
            sprintf(result, "SYMBOL<%d,%d>: \"*\"", _row, _col);
            break;
        case SYM_NOT_EQ:
            sprintf(result, "SYMBOL<%d,%d>: \"!=\"", _row, _col);
            break;
        case SYM_PLUS:
            sprintf(result, "SYMBOL<%d,%d>: \"+\"", _row, _col);
            break;
        case SYM_SEMICOLON:
            sprintf(result, "SYMBOL<%d,%d>: \";\"", _row, _col);
            break;
        case SYM_SQ_CLOSE:
            sprintf(result, "SYMBOL<%d,%d>: \"]\"", _row, _col);
            break;
        case SYM_SQ_OPEN:
            sprintf(result, "SYMBOL<%d,%d>: \"[\"", _row, _col);
            break;
        case SYM_NOT:
            sprintf(result, "SYMBOL<%d,%d>: \"!\"", _row, _col);
            break;
        case SYM_AND:
            sprintf(result, "SYMBOL<%d,%d>: \"&&\"", _row, _col);
            break;
        case SYM_OR:
            sprintf(result, "SYMBOL<%d,%d>: \"||\"", _row, _col);
            break;
        case SYM_MOD:
            sprintf(result, "SYMBOL<%d,%d>: %%", _row, _col);
            break;
        default:
            sprintf(result, "ERROR<%d,%d>:Unrecognized Token:\"%s\"", _row, _col, _lexeme);
            break;
    }
    return result;
}


