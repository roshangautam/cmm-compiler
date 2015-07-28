//
//  parser.cpp
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#include "parser.h"


bool Parser::read() {
    _lookAhead = getToken();
    
    TranslationUnit();
    match(TOK_EOF);
    
    return (_message.getErrorCount() == 0);
}

void Parser::TranslationUnit() {
    _message.print(DBUG, "parser: in TranslationUnit()");
    
    // fill in with tokens, -1 must be the last one!
    static tokenType firstSet[] = { KW_FLOAT, KW_INT, KW_VOID, (tokenType) - 1 };

    // grammar
    //    { [ “extern” ] TypeSpecifier, identifier,
    //        ( [ “[”, integer, “]”],
    //        { “,”, identifier, [ “[”, integer, “]” ] }, “;”
    //         | “(”, [ Parameter, { “,”, Parameter } ], “)”,
    //         CompoundStatement
    //         )
    //    }
    // fill in the rule here
    
    while(_lookAhead.getTokenType() == KW_EXTERN ||
          memberOf(_lookAhead.getTokenType(), firstSet)) {
        
        if (_lookAhead.getTokenType() == KW_EXTERN)
            match(KW_EXTERN);
        TypeSpecifier();
        match(TOK_IDENT);
        
        if(_lookAhead.getTokenType() == SYM_SQ_OPEN ||
           _lookAhead.getTokenType() == SYM_COMMA ||
           _lookAhead.getTokenType() == SYM_SEMICOLON) {
            
            if (_lookAhead.getTokenType() == SYM_SQ_OPEN) {
                match(SYM_SQ_OPEN);
                match(LIT_INT);
                if (_lookAhead.getTokenType() == SYM_SQ_CLOSE) {
                    match(SYM_SQ_CLOSE);
                }
            }
            
            while(_lookAhead.getTokenType() == SYM_COMMA) {
                match(SYM_COMMA);
                match(TOK_IDENT);
                if(_lookAhead.getTokenType() == SYM_SQ_OPEN) {
                    match(SYM_SQ_OPEN);
                    match(LIT_INT);
                    if(_lookAhead.getTokenType() == SYM_SQ_CLOSE) {
                        match(SYM_SQ_CLOSE);
                    } else {
                        //illegal array
                    }
                }
            }
            if(_lookAhead.getTokenType() == SYM_SEMICOLON) {
                match(SYM_SEMICOLON);
            }
        } else if(_lookAhead.getTokenType() == SYM_OPEN) {
            match(SYM_OPEN);
            if(memberOf(_lookAhead.getTokenType(), firstSet)) {
                Parameter();
                while(_lookAhead.getTokenType() == SYM_COMMA) {
                    match(SYM_COMMA);
                    Parameter();
                }
            }
            if(_lookAhead.getTokenType() == SYM_CLOSE){
                match(SYM_CLOSE);
                if(_lookAhead.getTokenType() == SYM_SEMICOLON) {
                    match(SYM_SEMICOLON); // its a function declaration (prototype)
                } else {
                    CompoundStatement(); // its a function definition
                }
            }
        } else {
            // something's not right
        }
    }
}

void Parser::TypeSpecifier(){
    static tokenType firstSet[] = {KW_EXTERN, KW_FLOAT, KW_INT, KW_VOID, (tokenType)-1};
    static tokenType followSet[] = {TOK_IDENT, (tokenType)-1};
    synchronize(firstSet, followSet, "expecting typespecifier");
    
    if(memberOf(_lookAhead.getTokenType(), firstSet)){
        if(_lookAhead.getTokenType() == KW_EXTERN){
            match(KW_EXTERN);
        }
        else if(_lookAhead.getTokenType() == KW_FLOAT){
            match(KW_FLOAT);
        }
        else if(_lookAhead.getTokenType() == KW_INT){
            match(KW_INT);
        }
        else if(_lookAhead.getTokenType() == KW_VOID){
            match(KW_VOID);
        }
    }
}

void Parser::Parameter(){
    static tokenType firstSet[] = {KW_EXTERN, KW_FLOAT, KW_INT, KW_VOID, (tokenType)-1};
    static tokenType followSet[] = {SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    synchronize(firstSet, followSet, "expecting parameter");
    
    if(memberOf(_lookAhead.getTokenType(), firstSet)){
        TypeSpecifier();
        if(_lookAhead.getTokenType() == TOK_IDENT){
            match(TOK_IDENT);
        }
        if(_lookAhead.getTokenType() == SYM_SQ_OPEN){
            match(SYM_SQ_OPEN);
            if(_lookAhead.getTokenType() == SYM_SQ_CLOSE){
                match(SYM_SQ_CLOSE);
            }
        }
    }
}

void Parser::CompoundStatement(){
    if(_lookAhead.getTokenType() == SYM_CURLY_OPEN) {
        match(SYM_CURLY_OPEN);
    }
    while(_lookAhead.getTokenType() == KW_EXTERN ||
          _lookAhead.getTokenType() == KW_FLOAT ||
          _lookAhead.getTokenType() == KW_INT ||
          _lookAhead.getTokenType() == KW_VOID ||
          _lookAhead.getTokenType() == SYM_OPEN ||
          _lookAhead.getTokenType() == SYM_SEMICOLON ||
          _lookAhead.getTokenType() == LIT_INT ||
          _lookAhead.getTokenType() == LIT_FLOAT ||
          _lookAhead.getTokenType() == LIT_CHAR ||
          _lookAhead.getTokenType() == LIT_STR ||
          _lookAhead.getTokenType() == SYM_CURLY_OPEN ||
          _lookAhead.getTokenType() == KW_IF ||
          _lookAhead.getTokenType() == KW_WHILE ||
          _lookAhead.getTokenType() == KW_RETURN){
        if(_lookAhead.getTokenType() == KW_EXTERN || _lookAhead.getTokenType() == KW_FLOAT || _lookAhead.getTokenType() == KW_INT ||
           _lookAhead.getTokenType() == KW_VOID){
            Declarations();
        }
        else{
            Statements();
        }
        if(_lookAhead.getTokenType() == SYM_CURLY_CLOSE){
            match(SYM_CURLY_CLOSE);
        }
    }
}

void Parser::Declarations(){
    while(_lookAhead.getTokenType() == KW_EXTERN || _lookAhead.getTokenType() == KW_FLOAT || _lookAhead.getTokenType() == KW_INT || _lookAhead.getTokenType() == KW_VOID){
        TypeSpecifier();
        match(TOK_IDENT);
        if(_lookAhead.getTokenType() == SYM_SQ_OPEN){
            match(SYM_SQ_OPEN);
            match(LIT_INT);
            if(_lookAhead.getTokenType() == SYM_SQ_CLOSE){
                match(SYM_SQ_CLOSE);
            }
        }
        while(_lookAhead.getTokenType() == SYM_COMMA){
            match(SYM_COMMA);
            match(TOK_IDENT);
            if(_lookAhead.getTokenType() == SYM_SQ_OPEN){
                match(SYM_SQ_OPEN);
                match(LIT_INT);
                if(_lookAhead.getTokenType() == SYM_SQ_CLOSE){
                    match(SYM_SQ_CLOSE);
                }
            }
        }
        if(_lookAhead.getTokenType() == SYM_SEMICOLON){
            match(SYM_SEMICOLON);
        }
    }
}

void Parser::Statements(){
    static tokenType firstSet[] = {SYM_OPEN, TOK_IDENT, LIT_INT, LIT_FLOAT, LIT_CHAR, LIT_STR, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, (tokenType) - 1};
    static tokenType followSet[] = {SYM_CURLY_CLOSE, KW_ELSE, (tokenType) - 1};
    synchronize(firstSet, followSet, "expecting Statement");
    while(memberOf(_lookAhead.getTokenType(), firstSet)){
        if(_lookAhead.getTokenType() == SYM_CURLY_OPEN){
            CompoundStatement();
        }
        else if(_lookAhead.getTokenType() == KW_IF){
            SelectionStatement();
        }
        else if(_lookAhead.getTokenType() == KW_WHILE){
            RepetitionStatement();
        }
        else if(_lookAhead.getTokenType() == KW_RETURN){
            ReturnStatement();
        }
        else{
            ExpressionStatement();
        }
    }
}

void Parser::ExpressionStatement(){
    static tokenType firstSet[] = {SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_CHAR, LIT_STR, (tokenType) - 1};
    if(memberOf(_lookAhead.getTokenType(), firstSet)){
        Expression();
        if(_lookAhead.getTokenType() == SYM_ASSIGN){
            match(SYM_ASSIGN);
            Expression();
        }
    }
    if(_lookAhead.getTokenType() == SYM_SEMICOLON){
        match(SYM_SEMICOLON);
    }
}

void Parser::SelectionStatement(){
    if(_lookAhead.getTokenType() == KW_IF){
        match(KW_IF);
        if(_lookAhead.getTokenType() == SYM_OPEN){
            match(SYM_OPEN);
            Expression();
            if(_lookAhead.getTokenType() == SYM_CLOSE){
                match(SYM_CLOSE);
                Statements();
            }
            if(_lookAhead.getTokenType() == KW_ELSE){
                match(KW_ELSE);
                Statements();
            }
        }
    }
}

void Parser::RepetitionStatement(){
    if(_lookAhead.getTokenType() == KW_WHILE){
        match(KW_WHILE);
        if(_lookAhead.getTokenType() == SYM_OPEN){
            match(SYM_OPEN);
            Expression();
            if(_lookAhead.getTokenType() == SYM_CLOSE){
                match(SYM_CLOSE);
                Statements();
            }
        }
    }
}

void Parser::ReturnStatement(){
    static tokenType firstSet[] = {SYM_OPEN, TOK_IDENT, LIT_INT, LIT_FLOAT, LIT_CHAR, LIT_STR, (tokenType) - 1};
    if(_lookAhead.getTokenType() == KW_RETURN){
        match(KW_RETURN);
    }
    if(memberOf(_lookAhead.getTokenType(), firstSet)){
        Expression();
    }
    match(SYM_SEMICOLON);
}

void Parser::Expression(){
    static tokenType firstSet[] = {SYM_OPEN, TOK_IDENT, LIT_INT, LIT_FLOAT, LIT_CHAR, LIT_STR, (tokenType) - 1};
    static tokenType followSet[] = {SYM_CLOSE, SYM_SEMICOLON, SYM_SQ_CLOSE, SYM_COMMA, (tokenType) - 1};
    synchronize(firstSet, followSet, "expecting Expression");
    if(memberOf(_lookAhead.getTokenType(), firstSet)){
        SimpleExpression();
        if(_lookAhead.getTokenType() == SYM_LESS_EQ) {
            match(SYM_LESS_EQ);
            SimpleExpression();
        } else if(_lookAhead.getTokenType() == SYM_LESS){
            match(SYM_LESS);
            SimpleExpression();
        } else if(_lookAhead.getTokenType() == SYM_GREATER_EQ){
            match(SYM_GREATER_EQ);
            SimpleExpression();
        } else if(_lookAhead.getTokenType() == SYM_GREATER){
            match(SYM_GREATER);
            SimpleExpression();
        } else if(_lookAhead.getTokenType() == SYM_EQUAL){
            match(SYM_EQUAL);
            SimpleExpression();
        } else if(_lookAhead.getTokenType() == SYM_NOT_EQ){
            match(SYM_NOT_EQ);
            SimpleExpression();
        }
    }
}

void Parser::SimpleExpression(){
    static tokenType firstSet[] = {SYM_OPEN, TOK_IDENT, LIT_INT, LIT_FLOAT, LIT_CHAR, LIT_STR, (tokenType) - 1};
    if(memberOf(_lookAhead.getTokenType(), firstSet)){
        Term();
        while(_lookAhead.getTokenType() == SYM_PLUS || _lookAhead.getTokenType() == SYM_MINUS){
            if(_lookAhead.getTokenType() == SYM_PLUS){
                match(SYM_PLUS);
                Term();
            }
            else if(_lookAhead.getTokenType() == SYM_MINUS){
                match(SYM_MINUS);
                Term();
            }
        }
    }
}

void Parser::Term(){
    static tokenType firstSet[] = {SYM_OPEN, TOK_IDENT, LIT_INT, LIT_FLOAT, LIT_CHAR, LIT_STR, (tokenType) - 1};
    if(memberOf(_lookAhead.getTokenType(), firstSet)){
        Factor();
        while(_lookAhead.getTokenType() == SYM_MUL || _lookAhead.getTokenType() == SYM_DIV){
            if(_lookAhead.getTokenType() == SYM_MUL){
                match(SYM_MUL);
                Factor();
            }
            else if(_lookAhead.getTokenType() == SYM_DIV){
                match(SYM_DIV);
                Factor();
            }
        }
    }
}

void Parser::Factor(){
    static tokenType firstSet[] = {SYM_OPEN, TOK_IDENT, LIT_INT, LIT_FLOAT, LIT_CHAR, LIT_STR, (tokenType) - 1};
    
    if(memberOf(_lookAhead.getTokenType(), firstSet)){
        if(_lookAhead.getTokenType() == SYM_OPEN){
            match(SYM_OPEN);
            Expression();
            if(_lookAhead.getTokenType() == SYM_CLOSE){
                match(SYM_CLOSE);
            }
        }
        else if(_lookAhead.getTokenType() == TOK_IDENT){
            match(TOK_IDENT);
            if(_lookAhead.getTokenType() == SYM_SQ_OPEN){
                match(SYM_SQ_OPEN);
                Expression();
                if(_lookAhead.getTokenType() == SYM_SQ_CLOSE){
                    match(SYM_SQ_CLOSE);
                }
                if(_lookAhead.getTokenType() == SYM_OPEN){
                    match(SYM_OPEN);
                    if(memberOf(_lookAhead.getTokenType(), firstSet)){
                        Expression();
                        while(_lookAhead.getTokenType() == SYM_COMMA){
                            match(SYM_COMMA);
                            Expression();
                        }
                    }
                }
                if(_lookAhead.getTokenType() == SYM_CLOSE){
                    match(SYM_CLOSE);
                }
            }
        }
        else if(_lookAhead.getTokenType() == LIT_INT){
            match(LIT_INT);
        }
        else if(_lookAhead.getTokenType() == LIT_FLOAT){
            match(LIT_FLOAT);
        }
        else if(_lookAhead.getTokenType() == LIT_CHAR){
            match(LIT_CHAR);
        }
        else if(_lookAhead.getTokenType() == LIT_STR){
            match(LIT_STR);
        }
    }
}