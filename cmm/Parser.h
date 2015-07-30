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
#include "scanner.h"


class Parser {
    Scanner _scanner;
    Message _message;
    Token _lookAhead;
    
    const char* tokenTypeString[36] = {
        "EOF",
        "identifier",
        
        "string literal",
        "int literal",
        "float literal",
        "char literal",
        
        "keyword extern",
        "keyword int",
        "keyword float",
        "keyword void",
        "keyword if",
        "keyword else",
        "keyword while",
        "keyword return",
        
        "symbol \"(\"",
        "symbol \")\"",
        "symbol \"+\"",
        "symbol \"-\"",
        "symbol \"*\"",
        "symbol \"/\"",
        "symbol \"<\"",
        "symbol \"<=\"",
        "symbol \">\"",
        "symbol \">=\"",
        "symbol \"==\"",
        "symbol \"!=\"",
        "symbol \";\"",
        "symbol \"=\"",
        "symbol \",\"",
        "symbol \"[\"",
        "symbol \"]\"",
        "symbol \"{\"",
        "symbol \"}\""
        "symbol \"!\"",
        "symbol \"%\"",
        "symbol \"&&\"",
        "symbol \"||\"",
    };
    
    Token getToken() {
        _scanner.read();
        while (_scanner.getToken().getTokenType() < 0) {
            _message.print(ERROR, "line %i: col %i: scanner: %s", _scanner.getToken().getRow(), _scanner.getToken().getCol(), _scanner.error());
            _scanner.read();
        }
        return _scanner.getToken();
    }
    
    void match(tokenType expected) {
        _message.print(DBUG, "parser: match: expecting %s", tokenTypeString[expected]);
        if (_lookAhead.getTokenType() == expected)
            _lookAhead = getToken();
        else {
            _message.print(ERROR, "line %i: col %i: parser: match : expecting %s: found %s", _lookAhead.getRow() , _lookAhead.getCol(), tokenTypeString[expected], tokenTypeString[_lookAhead.getTokenType()]);
        }
    }

    bool memberOf(tokenType element, tokenType* set) {
        bool found = false;
        int i = 0;
        while (!found && set[i] > 0)
            found = (element == set[i++]);
        return found;
    }
    
    bool synchronized(tokenType* firstSet, tokenType* followSet,const char* errMsg) {
        _message.print(DBUG, "parser: synchronize: %s", errMsg);
        bool synced = true;
        if (!memberOf(_lookAhead.getTokenType(), firstSet)) {
            
            _message.print(DBUG, "parser: synchronize: could not find %s", tokenTypeString[_lookAhead.getTokenType()]);
            _message.print(ERROR, "line %i: col %i: parser: %s: found %s",
                    _lookAhead.getRow(), _lookAhead.getCol(), errMsg, tokenTypeString[_lookAhead.getTokenType()]);
            
            while (_lookAhead.getTokenType() != TOK_EOF &&
                   !memberOf(_lookAhead.getTokenType(), firstSet) &&
                   !memberOf(_lookAhead.getTokenType(), followSet)) {                
//                _scanner.read();
//                _lookAhead = _scanner.getToken();
                _lookAhead = getToken();
            }
            
            if (!memberOf(_lookAhead.getTokenType(), firstSet))
                synced = false;
        }
        return synced;
    }
    
    
    void TranslationUnit();
    void TypeSpecifier();
    void Parameter();
    void CompoundStatement();
    void Declaration();
    void Statement();
    void ExpressionStatement();
    void SelectionStatement();
    void RepetitionStatement();
    void ReturnStatement();
    void Expression();
    void AndExpression();
    void RelationExpression();
    void SimpleExpression();
    void Term();
    void Factor();
    void Value();
    
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
    
    Message getMessage() {
        return _message;
    }
    
    bool read();
};
#endif /* defined(__CMM__parser__) */
