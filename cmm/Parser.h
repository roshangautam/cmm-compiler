//
//  parser.h
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#ifndef __CMM__parser__
#define __CMM__parser__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "Scanner.h"

class Parser {
    Scanner _scanner;
    Message _message;
    Token _lookAhead;
    
    Token getToken() {
        _scanner.read();
        while (_scanner.getToken().getTokenType() < 0) {
            _message.print(ERROR, "line %i: col %i: scanner: %s", _scanner.getToken().getRow(), _scanner.getToken().getCol(), _scanner.error());
            _scanner.read();
        }
        return _scanner.getToken();
    }
    
    void match(tokenType expected) {
//        message(DEBUG, "parser: match: expecting %s", _scanner.tokenTypeString(expected));
        if (_lookAhead.getTokenType() == expected)
            _lookAhead = getToken();
        else {
//            message(ERROR, "line %i: col %i: parser: expecting %s: found %s",
//                    lookAhead.getRow() , lookAhead.getCol(), tokenTypeString[expected], tokenTypeString[lookAhead.token]);
        }
    }

    bool memberOf(tokenType element, tokenType* set) {
        bool found = false;
        int i = 0;
        while (!found && set[i] > 0)
            found = (element == set[i++]);
        return found;
    }
    
    void synchronize(tokenType* firstSet, tokenType* followSet,const char* errMsg) {
        _message.print(DBUG, "parser: synchronize: %s", errMsg);
        if (!memberOf(_lookAhead.getTokenType(), firstSet)) {
//            message(DEBUG, "parser: synchronize: could not find %s", tokenTypeString[lookAhead.token]);
//            message(ERROR, "line %i: col %i: parser: %s: found %s",
//                    lookAhead.row, lookAhead.col, errMsg, tokenTypeString[lookAhead.token]);
            while (_lookAhead.getTokenType() != TOK_EOF && !memberOf(_lookAhead.getTokenType(), firstSet) && !memberOf(_lookAhead.getTokenType(), followSet)) {
                _scanner.read();
                _lookAhead = _scanner.getToken();
            }
        }
    }
    
    void resynchronize(tokenType* syncSet) {
        while (_lookAhead.getTokenType() != TOK_EOF && !memberOf(_lookAhead.getTokenType(), syncSet)) {
            _scanner.read();
            _lookAhead = _scanner.getToken();
        }
    }
    
    void TranslationUnit();
    void TypeSpecifier();
    void Parameter();
    void CompoundStatement();
    void Declarations();
    void Statements();
    void ExpressionStatement();
    void SelectionStatement();
    void RepetitionStatement();
    void ReturnStatement();
    void Expression();
    void SimpleExpression();
    void Term();
    void Factor();
    
public:
    Parser(FILE *fin, int tabSize, Message message) {
        _message = message;
        _message.print(DBUG, "Parser:Initialized");        
        _scanner = Scanner(fin, message);
        _scanner.setTabWidth(tabSize);
    }
    
    Scanner getScanner() {
        return _scanner;
    }
    
    bool read();
};
#endif /* defined(__CMM__parser__) */
