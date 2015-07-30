//
//  Scanner.cpp
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#include "scanner.h"


int Scanner::setTabWidth(int t) {
    return _tabSize = t < 1 ? TAB_WIDTH : t;
}

int Scanner::getTabWidth() {
    return _tabSize;
}

int Scanner::getLinesRead() {
    return _token.getRow() - 1;
}

void Scanner::read() {
    int c;
    c = nextCh();
    while (isspace((char)c) ||  // we dont care about spaces
           ((char)c == '/' &&  // oh look , it can be the starting of a comment
            (peekCh() == '*' || // it can be the starting of a multiple line comment
             peekCh() == '/'))) { // OR, it can be the starting of a single line comment
        if (c == '/' && peekCh() == '*') { // look we actually found the starting of a multi line comment
            _token.setRow(_row);
            _token.setCol(_col);// gotta remember the line and column number where the comment starts for error handling
            c = nextCh();
            while (( c = nextCh()) >= TOK_EOF) {  // i am hungry, nom nom, eat everything
               if (c == '*' && peekCh() == '/') { // Oo we found the end of the comment, can't eat anymore
                   c = nextCh(); // lets fetch another character
                   break; // and get out of here, ciao.
               }
            }
            if (c < TOK_EOF) { // Look we reached the end of file. Either everything has been commented out or the comment was never terminated. I am full, can't move anymore
               _token.setTokenType(ERR_BADCOMMENT);
               _token.setLexeme(_buffer);
                return;
            }
        } else if (c == '/' && peekCh() == '/') { // this is single line comment
            _token.setRow(_row);
            c = nextCh();
            while ((c = nextCh()) >= TOK_EOF && _token.getRow() == _row); // eat the line
        }
        c = nextCh();
    }
    _token.setRow(_row);
    _token.setCol(_col); // lets remeber where we are
    if (c < 0)
        _token.setTokenType(TOK_EOF);
    else {
        _buffer[0] = '\0';            // initialize the buffer
        strnapp(_buffer, (char)c, BUFF_LEN);
        
        // process all other lexemes
        if (isalpha((char)c) || c == '_') { // if it starts with a character or _
            while (isalnum((char)peekCh()) || peekCh() == '_') // and followed by any number of either character or digit or  _
                c = nextCh();
            _token.setTokenType(searchKeyWord(_buffer)); // it can be a keyword or an identifier
        } else if (isdigit((char)c) || c == '.') { // if it starts with a digit or .
            if (isdigit((char)c)) { // if it started with a digit
                while (isdigit((char)peekCh())) // and followed by any number of digits
                    c = nextCh();
                _token.setTokenType(LIT_INT); // lets default to integer first
                if (peekCh() == '.') // if its followed by . it can be a float
                    c = nextCh();
            }
            if (c == '.') { // its a floating point value, check for bad float
                if (isspace(peekCh()) &&
                    (_token.getTokenType() == TOK_EOF ||
                     _token.getTokenType() != LIT_INT)) {
                    _token.setTokenType(ERR_BADINPUT);
                } else {
                    while (isdigit((char)peekCh()))
                        c = nextCh();
                    if (toupper((char)peekCh()) == 'E') { //handles both cases e and E
                        c = nextCh();
                        if ((char)peekCh() == '+' || (char)peekCh() == '-')
                            c = nextCh();
                        if (!isdigit((char)peekCh())) {
                            _token.setTokenType(ERR_BADFLOAT);
                            _token.setLexeme(_buffer);
                            return;
                        }
                        while (isdigit((char)peekCh()))
                            c = nextCh();
                    }
                    _token.setTokenType(LIT_FLOAT);
                }
            }
        } else if (c == '"') {
            _buffer[0] = '\0';
            while ((c = nextCh()) != '"') {
                if (c < 0 || c == '\n' || c == '\r') {
                    _token.setTokenType(ERR_BADSTR);
                    _buffer[ strlen(_buffer) - 1 ] = '\0';
                    _token.setLexeme(_buffer);
                    return;
                }
            }
            _buffer[strlen(_buffer)-1] = '\0';
            _token.setTokenType(LIT_STR);
        } else if (c == ',') {
            _token.setTokenType(SYM_COMMA);
        } else if (c == ';') {
            _token.setTokenType(SYM_SEMICOLON);
        } else if (c == '+') {
            _token.setTokenType(SYM_PLUS);
        } else if (c == '-') {
            _token.setTokenType(SYM_MINUS);
        } else if (c == '*') {
            _token.setTokenType(SYM_MUL);
        } else if (c == '/') {
            _token.setTokenType(SYM_DIV);
        } else if (c == '%') {
            _token.setTokenType(SYM_MOD);
        } else if (c == '=') {
            if (peekCh() == '=') {
                c = nextCh();
                _token.setTokenType(SYM_EQUAL);
            } else
                _token.setTokenType(SYM_ASSIGN);
        } else if (c == '!') {
            if (peekCh() != '=') {
                c = nextCh();
                _token.setTokenType(SYM_NOT);
            } else {
                c = nextCh();
                _token.setTokenType(SYM_NOT_EQ);
            }
        } else if (c == '<') {
            if (peekCh() == '=') {
                c = nextCh();
                _token.setTokenType(SYM_LESS_EQ);
            } else
                _token.setTokenType(SYM_LESS);
        } else if (c == '>') {
            if (peekCh() == '=') {
                c = nextCh();
                _token.setTokenType(SYM_GREATER_EQ);
            } else
                _token.setTokenType(SYM_GREATER);
        } else if (c == '&') {
            if (peekCh() == '&') {
                c = nextCh();
                _token.setTokenType(SYM_AND);
            } else
                _token.setTokenType(ERR_BADINPUT);
        } else if (c == '|') {
            if (peekCh() == '|') {
                c = nextCh();
                _token.setTokenType(SYM_OR);
            } else
                _token.setTokenType(ERR_BADINPUT);
        } else if (c == '(') {
            _token.setTokenType(SYM_OPEN);
        } else if (c == ')') {
            _token.setTokenType(SYM_CLOSE);
        } else if (c == '[') {
            _token.setTokenType(SYM_SQ_OPEN);
        } else if (c == ']') {
            _token.setTokenType(SYM_SQ_CLOSE);
        } else if (c == '{') {
            _token.setTokenType(SYM_CURLY_OPEN);
        } else if (c == '}') {
            _token.setTokenType(SYM_CURLY_CLOSE);
        } else if (c == '\'') {
            _buffer[0] = '\0';
            c = nextCh();
            if (peekCh() != '\'') {
                _token.setTokenType(ERR_BADINPUT);
                _buffer[0] = peekCh();
                _token.setLexeme(_buffer);
                return;
            }
            c = nextCh();
            _buffer[strlen(_buffer)-1] = '\0';
            _token.setTokenType(LIT_CHAR);
        } else {
            _token.setTokenType(ERR_BADINPUT);
        }
        _token.setLexeme(_buffer);
    }
    _message.print(DBUG, "scanner: Found %s", getToken().getFormattedLexeme());
}

const char* Scanner::error() {
    static const char *msg;
    
    switch (_token.getTokenType()) {
        case ERR_BUFFEROVRFLW:
            msg = "buffer overflow";
            break;
        case ERR_BADINPUT:
            msg = "unrecognized input character";
            break;
        case ERR_BADSTR:
            msg = "unterminated string literal";
            break;
        case ERR_BADFLOAT:
            msg = "unrecognized float literal";
            break;
        case ERR_BADCOMMENT:
            msg = "unterminated comment";
            break;
        default:
            msg = "unknown error code";
    }
    
    return msg;
}