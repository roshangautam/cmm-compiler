//
//  parser.cpp
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#include "parser.h"


bool Parser::read() {  // Augmented Rule for Grammar
    _lookAhead = getToken();
    
    TranslationUnit();
    match(TOK_EOF);
    
    return (_message.getErrorCount() == 0);
}

void Parser::TranslationUnit() {
    _message.print(DBUG, "parser: in TranslationUnit()");
    
    // fill in with tokens, -1 must be the last one!
    static tokenType translationUnitFirstSet[] = {KW_EXTERN, KW_FLOAT, KW_INT, KW_VOID, (tokenType) - 1 };
    static tokenType parameterFirstSet[] = {KW_FLOAT, KW_INT, KW_VOID, (tokenType) - 1 };
    static tokenType followSet[] = {TOK_EOF, (tokenType) - 1 };
    
    // grammar rule
    //    { [ “extern” ] TypeSpecifier, identifier,
    //        ( [ “[”, integer, “]”],
    //        { “,”, identifier, [ “[”, integer, “]” ] }, “;”
    //         | “(”, [ Parameter, { “,”, Parameter } ], “)”,
    //         CompoundStatement
    //         )
    //    }
    
    if (synchronized(translationUnitFirstSet, followSet, "Expecting TranslationUnit")) {
        if (_lookAhead.getTokenType() == KW_EXTERN)
            match(KW_EXTERN);
        TypeSpecifier();
        match(TOK_IDENT);
        
        if(_lookAhead.getTokenType() == SYM_SQ_OPEN ||
           _lookAhead.getTokenType() == SYM_COMMA ||
           _lookAhead.getTokenType() == SYM_SEMICOLON) {  // VARIABLE DECLARATION
            
            if (_lookAhead.getTokenType() == SYM_SQ_OPEN) {  // ARRAY
                match(SYM_SQ_OPEN);
                match(LIT_INT);
                match(SYM_SQ_CLOSE); // MUST MATCH SQUARE BRACKET CLOSE
            }
            
            while(_lookAhead.getTokenType() == SYM_COMMA) {
                match(SYM_COMMA);
                match(TOK_IDENT);
                if(_lookAhead.getTokenType() == SYM_SQ_OPEN) {
                    match(SYM_SQ_OPEN);
                    match(LIT_INT);
                    match(SYM_SQ_CLOSE);
                }
            }
            
            match(SYM_SEMICOLON); // MUST MATCH SEMICOLON
            
        } else if(_lookAhead.getTokenType() == SYM_OPEN) { // FUNCTION DECLARATION OR DEFINITION
            match(SYM_OPEN);
            if(memberOf(_lookAhead.getTokenType(), parameterFirstSet)) {
                Parameter();
                while(_lookAhead.getTokenType() == SYM_COMMA) {
                    match(SYM_COMMA);
                    Parameter();
                }
            }
            if(_lookAhead.getTokenType() == SYM_CLOSE) {
                match(SYM_CLOSE);
                if(_lookAhead.getTokenType() == SYM_SEMICOLON) {
                    match(SYM_SEMICOLON); // FUNCTION DECLARATION
                } else {
                    CompoundStatement(); // FUNCTION DEFINITION
                }
            }
        }
    }
}

void Parser::TypeSpecifier() {

    //    “void”
    //    | “int”
    //    | “float”
    
    static tokenType firstSet[] = {KW_FLOAT, KW_INT, KW_VOID, (tokenType) -1};
    static tokenType followSet[] = {TOK_IDENT, (tokenType) -1};

    if(synchronized(firstSet, followSet, "Expecting TypeSpecifier")) {
        if(_lookAhead.getTokenType() == KW_FLOAT) {
            match(KW_FLOAT);
        } else if(_lookAhead.getTokenType() == KW_INT) {
            match(KW_INT);
        } else if(_lookAhead.getTokenType() == KW_VOID) {
            match(KW_VOID);
        }
    }
}

void Parser::Parameter() {
    
    //    TypeSpecifier, identifier, [ “[”, “]” ]
    
    static tokenType firstSet[] = {KW_FLOAT, KW_INT, KW_VOID, (tokenType)-1};
    static tokenType followSet[] = {SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    if(synchronized(firstSet, followSet, "Expecting Parameter")) {
        
        TypeSpecifier();
        
        match(TOK_IDENT);
        
        if(_lookAhead.getTokenType() == SYM_SQ_OPEN) {
            match(SYM_SQ_OPEN);
            match(SYM_SQ_CLOSE);
        }
    }
}

void Parser::CompoundStatement() {
    
    //“{”, { Declaration | Statement }, “}”
    
    static tokenType compoundStatementFirstSet[] = {SYM_CURLY_OPEN, (tokenType)-1};
    static tokenType declarationFirstSet[] = {KW_VOID, KW_INT, KW_FLOAT, (tokenType) - 1};
    static tokenType statementFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    static tokenType followSet[] = {TOK_EOF, KW_ELSE, SYM_CURLY_CLOSE, (tokenType) - 1};
    
    if (synchronized(compoundStatementFirstSet, followSet, "Expecting Compound Statement")) {
        match(SYM_CURLY_OPEN);
        while (memberOf(_lookAhead.getTokenType(), declarationFirstSet) ||
               memberOf(_lookAhead.getTokenType(), statementFirstSet) ) {
            
            if (memberOf(_lookAhead.getTokenType(), declarationFirstSet)) {
                Declaration();
            } else if(memberOf(_lookAhead.getTokenType(), statementFirstSet)) {
                Statement();
            }
            
        }
        match(SYM_CURLY_CLOSE);
    }
}

void Parser::Declaration() {
    
    //  TypeSpecifier, identifier, [ “[”, integer, “]” ],
    //    { “,”, identifier, [ “[”, integer, “]” ] }, “;”
    
    static tokenType firstSet[] = {KW_FLOAT, KW_INT, KW_VOID, (tokenType) -1};
    static tokenType followSet[] = {TOK_EOF, KW_ELSE, SYM_CURLY_CLOSE, (tokenType) - 1};

    if (synchronized(firstSet, followSet, "Expecting Declaration")) {
        TypeSpecifier();
        match(TOK_IDENT);
        if(_lookAhead.getTokenType() == SYM_SQ_OPEN){
            match(SYM_SQ_OPEN);
            match(LIT_INT);
            match(SYM_SQ_CLOSE);
        }
        while(_lookAhead.getTokenType() == SYM_COMMA) {
            match(SYM_COMMA);
            match(TOK_IDENT);
            if(_lookAhead.getTokenType() == SYM_SQ_OPEN) {
                match(SYM_SQ_OPEN);
                match(LIT_INT);
                if(_lookAhead.getTokenType() == SYM_SQ_CLOSE) {
                    match(SYM_SQ_CLOSE);
                }
            }
        }
        match(SYM_SEMICOLON);
    }
}

void Parser::Statement() {
    
    //    ExpressionStatement
    //    | CompoundStatement
    //    | SelectionStatement
    //    | RepetitionStatement
    //    | ReturnStatement
    
    static tokenType statementFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};

    static tokenType followSet[] = {SYM_CURLY_CLOSE, KW_ELSE, (tokenType) - 1};
    
    if (synchronized(statementFirstSet, followSet, "Expecting statement")) {
        if(_lookAhead.getTokenType() == SYM_CURLY_OPEN) {
            CompoundStatement();
        } else if(_lookAhead.getTokenType() == KW_IF) {
            SelectionStatement();
        } else if(_lookAhead.getTokenType() == KW_WHILE) {
            RepetitionStatement();
        } else if(_lookAhead.getTokenType() == KW_RETURN) {
            ReturnStatement();
        } else {
            ExpressionStatement();
        }
    }
}

void Parser::ExpressionStatement(){
    
    //    [ Expression, [ “=”, Expression ] ], “;”
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_CURLY_CLOSE, KW_ELSE, (tokenType) - 1};
    
    if (synchronized(firstSet, followSet, "Expecting Expression Statement")) {
        Expression();
        if(_lookAhead.getTokenType() == SYM_ASSIGN) {
            match(SYM_ASSIGN);
            Expression();
        }
    }
    match(SYM_SEMICOLON);
}

void Parser::SelectionStatement() {
    
    //    “if”, “(”, Expression, “)”, Statement, [ “else”, Statement ]
    
    static tokenType firstSet[] = {KW_IF, (tokenType) - 1};
    static tokenType followSet[] = {SYM_CURLY_CLOSE, KW_ELSE, (tokenType) - 1};
    
    if (synchronized(firstSet, followSet, "Expecting Selection Statement")) {
        match(KW_IF);
        match(SYM_OPEN);
        Expression();
        match(SYM_CLOSE);
        Statement();
        if (_lookAhead.getTokenType() == KW_ELSE) {
            match(KW_ELSE);
            Statement();
        }
    }
}

void Parser::RepetitionStatement() {
    
    //    “while”, “(”, Expression, “)”, Statement
    
    static tokenType firstSet[] = {KW_WHILE, (tokenType) - 1};
    static tokenType followSet[] = {SYM_CURLY_CLOSE, KW_ELSE, (tokenType) - 1};
    
    if (synchronized(firstSet, followSet, "Expecting Repetition Statement")) {
        match(KW_WHILE);
        match(SYM_OPEN);
        Expression();
        match(SYM_CLOSE);
        Statement();
    }
}

void Parser::ReturnStatement() {
    
    //    “return”, [ Expression ], “;”
    
    static tokenType firstSet[] = {KW_RETURN, (tokenType) - 1};
    static tokenType expressionFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    static tokenType followSet[] = {SYM_CURLY_CLOSE, KW_ELSE, (tokenType) - 1};
    
    if (synchronized(firstSet, followSet, "Expecting Return Statement")) {
        match(KW_RETURN);
        if (memberOf(_lookAhead.getTokenType(),expressionFirstSet)) {
            Expression();
        }
        match(SYM_SEMICOLON);
    }
}

void Parser::Expression() {
    
    //    AndExpression,
    //    { “||”, AndExpression }
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    static tokenType followSet[] = {SYM_CLOSE, SYM_SEMICOLON, SYM_SQ_CLOSE, SYM_COMMA, (tokenType) - 1};
//    static tokenType followSet[] = {SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    if(synchronized(firstSet, followSet, "expecting Expression")) {
        AndExpression();
        while (_lookAhead.getTokenType() == SYM_OR) {
            match(SYM_OR);
            AndExpression();
        }
    }
}

void Parser::AndExpression() {
    //    RelationExpression,
    //    { “&&”, RelationExpression }

    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    static tokenType followSet[] = {SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};

    if(synchronized(firstSet, followSet, "expecting AND Expression")) {
        RelationExpression();
        while (_lookAhead.getTokenType() == SYM_AND) {
            match(SYM_AND);
            RelationExpression();
        }
    }
}

void Parser::RelationExpression() {
    //    SimpleExpression,
    //    [ ( “<=” | “<” | “>=” | “>” | “==” | “!=” ),
    //     SimpleExpression ]
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    static tokenType followSet[] = {SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    static tokenType set[] = {SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, (tokenType) -1};
    if (synchronized(firstSet, followSet, "Expecting Relational Expression")) {
        SimpleExpression();
        if (memberOf(_lookAhead.getTokenType(), set)) {
            if(_lookAhead.getTokenType() == SYM_LESS_EQ) {
                match(SYM_LESS_EQ);
            } else if(_lookAhead.getTokenType() == SYM_LESS) {
                match(SYM_LESS);
            } else if(_lookAhead.getTokenType() == SYM_GREATER_EQ) {
                match(SYM_GREATER_EQ);
            } else if(_lookAhead.getTokenType() == SYM_GREATER) {
                match(SYM_GREATER);
            } else if(_lookAhead.getTokenType() == SYM_EQUAL) {
                match(SYM_EQUAL);
            } else if(_lookAhead.getTokenType() == SYM_NOT_EQ) {
                match(SYM_NOT_EQ);
            }
            SimpleExpression();
        }
    }

}

void Parser::SimpleExpression() {

    //    Term, { ( “+” | ”-“ ), Term }
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    static tokenType followSet[] = {SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};

    if (synchronized(firstSet, followSet, "Expecting Simple Expression")) {
        Term();
        while (_lookAhead.getTokenType() == SYM_PLUS ||
               _lookAhead.getTokenType() == SYM_MINUS) {
            if (_lookAhead.getTokenType() == SYM_PLUS) {
                match(SYM_PLUS);
            } else if(_lookAhead.getTokenType() == SYM_MINUS) {
                match(SYM_MINUS);
            }
            Term();
        }
    }
}

void Parser::Term() {
    
    //Factor, { ( “*” | ”/” | “%” ), Factor }
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    static tokenType followSet[] = {SYM_PLUS, SYM_MINUS, SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    if (synchronized(firstSet, followSet, "Expecting Term")) {
        Factor();
        while(_lookAhead.getTokenType() == SYM_MUL ||
              _lookAhead.getTokenType() == SYM_DIV ||
              _lookAhead.getTokenType() == SYM_MOD) {
            if(_lookAhead.getTokenType() == SYM_MUL) {
                match(SYM_MUL);
            } else if(_lookAhead.getTokenType() == SYM_DIV) {
                match(SYM_DIV);
            } else if(_lookAhead.getTokenType() == SYM_MOD) {
                match(SYM_MOD);
            }
            Factor();
        }
    }
}

void Parser::Factor() {
    
    //[ “+” | “-“ ], Value
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_MUL, SYM_DIV, SYM_MOD, SYM_PLUS, SYM_MINUS, SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    if (synchronized(firstSet, followSet, "Expecting Factor")) {
        if (_lookAhead.getTokenType() == SYM_PLUS) {
            match(SYM_PLUS);
        } else if (_lookAhead.getTokenType() == SYM_MINUS) {
            match(SYM_MINUS);
        }
        Value();
    }
 
}

void Parser::Value() {
    
    //    “(”, Expression, “)”
    //    | identifier, [ “[“, Expression, “]” | “(“, [ Expression, { “,”, Expression } ], “)” ]
    //    | integer
    //    | float
    //    | string
    
    static tokenType firstSet[] = {SYM_OPEN, TOK_IDENT, LIT_INT, LIT_FLOAT, LIT_STR, (tokenType) - 1};

    static tokenType expressionFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_MUL, SYM_DIV, SYM_MOD, SYM_PLUS, SYM_MINUS, SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    if (synchronized(firstSet, followSet, "Expecting Value")) {
        if (_lookAhead.getTokenType() == SYM_OPEN) {
            match(SYM_OPEN);
            Expression();
            match(SYM_CLOSE);
        } else if(_lookAhead.getTokenType() == TOK_IDENT) {
            match(TOK_IDENT);
            if(_lookAhead.getTokenType() == SYM_SQ_OPEN) {
                match(SYM_SQ_OPEN);
                Expression();
                match(SYM_SQ_CLOSE);
            } else if(_lookAhead.getTokenType() == SYM_OPEN){
                match(SYM_OPEN);
                if(memberOf(_lookAhead.getTokenType(), expressionFirstSet)){
                    Expression();
                    while(_lookAhead.getTokenType() == SYM_COMMA){
                        match(SYM_COMMA);
                        Expression();
                    }
                }
                match(SYM_CLOSE);
            }
        }
    } else if(_lookAhead.getTokenType() == LIT_INT) {
        match(LIT_INT);
    } else if(_lookAhead.getTokenType() == LIT_FLOAT) {
        match(LIT_FLOAT);
    } else if(_lookAhead.getTokenType() == LIT_STR) {
        match(LIT_STR);
    }
}