//
//  parser.cpp
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#include "parser.h"


bool Parser::read() {  // Augmented Rule for Grammar
    _message.print(DBUG, "PARSER: In AugmentedRule()\n");
    
    _lookAhead = getToken();
    
    TranslationUnit();
    match(TOK_EOF);
    
    return (_message.getErrorCount() == 0);
}

void Parser::TranslationUnit() {
    
    _message.print(DBUG, "PARSER: In TranslationUnit()\n");
    
    // fill with tokens, -1 must be the last one!
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
    
    while ( synchronized(translationUnitFirstSet, followSet, "Expected TranslationUnit") ) {
        
        if ( _lookAhead.getTokenType() == KW_EXTERN )
            match(KW_EXTERN);
        
        TypeSpecifier();
        match(TOK_IDENT);
        
        if( _lookAhead.getTokenType() == SYM_SQ_OPEN ||
            _lookAhead.getTokenType() == SYM_COMMA ||
            _lookAhead.getTokenType() == SYM_SEMICOLON ) {  // VARIABLE DECLARATION
            
            if ( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {  // ARRAY
                
                match(SYM_SQ_OPEN);
                match(LIT_INT);
                if (_lookAhead.getTokenType() == SYM_SQ_CLOSE) {
                    match(SYM_SQ_CLOSE); // MUST MATCH SQUARE BRACKET CLOSE
                }
            }
            
            while ( _lookAhead.getTokenType() == SYM_COMMA ) {
                
                match(SYM_COMMA);
                match(TOK_IDENT);
                
                if( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {
                    match(SYM_SQ_OPEN);
                    match(LIT_INT);
                    if (_lookAhead.getTokenType() == SYM_SQ_CLOSE) {
                        match(SYM_SQ_CLOSE);
                    }
                }
            }
            
            if (_lookAhead.getTokenType() == SYM_SEMICOLON) {
                match(SYM_SEMICOLON);
            }

        } else if ( _lookAhead.getTokenType() == SYM_OPEN ) { // FUNCTION DECLARATION OR DEFINITION
            
            match(SYM_OPEN);
            
            if ( memberOf(_lookAhead.getTokenType(), parameterFirstSet) ) {
                
                Parameter();
                
                while( _lookAhead.getTokenType() == SYM_COMMA ) {
                    match(SYM_COMMA);
                    Parameter();
                }
            }
            if ( _lookAhead.getTokenType() == SYM_CLOSE ) {
                
                match(SYM_CLOSE);
                
                if ( _lookAhead.getTokenType() == SYM_SEMICOLON ) {
                    match(SYM_SEMICOLON); // FUNCTION DECLARATION
                } else {
                    CompoundStatement(); // FUNCTION DEFINITION
                }
            }
        }
    }
}

void Parser::TypeSpecifier() {

    _message.print(DBUG, "PARSER: In TypeSpecifier()\n");
    
    //    “void”
    //    | “int”
    //    | “float”
    
    static tokenType firstSet[] = {KW_FLOAT, KW_INT, KW_VOID, (tokenType) -1};
    
    static tokenType followSet[] = {TOK_IDENT, (tokenType) -1};

    if ( synchronized(firstSet, followSet, "Expected TypeSpecifier") ) {
        if ( _lookAhead.getTokenType() == KW_FLOAT ) {
            match(KW_FLOAT);
        } else if ( _lookAhead.getTokenType() == KW_INT ) {
            match(KW_INT);
        } else if ( _lookAhead.getTokenType() == KW_VOID ) {
            match(KW_VOID);
        }
    }
}

void Parser::Parameter() {

    _message.print(DBUG, "PARSER: In Parameter()\n");
    
    //    TypeSpecifier, identifier, [ “[”, “]” ]
    
    static tokenType firstSet[] = {KW_FLOAT, KW_INT, KW_VOID, (tokenType)-1};
    
    static tokenType followSet[] = {SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    if ( synchronized(firstSet, followSet, "Expected Parameter") ) {
        
        TypeSpecifier();
        
        match(TOK_IDENT);
        
        if ( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {
            match(SYM_SQ_OPEN);
            if (_lookAhead.getTokenType() == SYM_SQ_CLOSE) {
                match(SYM_SQ_CLOSE);
            }
        }
    }
}

void Parser::CompoundStatement() {

    _message.print(DBUG, "PARSER: In CompoundStatement()\n");
    
    //“{”, { Declaration | Statement }, “}”
    
    static tokenType compoundStatementFirstSet[] = {SYM_CURLY_OPEN, (tokenType)-1};
    
    static tokenType declarationFirstSet[] = {KW_VOID, KW_INT, KW_FLOAT, (tokenType) - 1};
    
    static tokenType statementFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {TOK_EOF, KW_ELSE, SYM_CURLY_CLOSE, (tokenType) - 1}; 
    
    if ( synchronized(compoundStatementFirstSet, followSet, "Expected Compound Statement") ) {
        
        match(SYM_CURLY_OPEN);
        
        while ( memberOf(_lookAhead.getTokenType(), declarationFirstSet) ||
                memberOf(_lookAhead.getTokenType(), statementFirstSet) ) {
            
            if ( memberOf(_lookAhead.getTokenType(), declarationFirstSet) ) {
                Declaration();
            } else if ( memberOf(_lookAhead.getTokenType(), statementFirstSet) ) {
                Statement();
            }
            
        }
        
        if (_lookAhead.getTokenType() == SYM_CURLY_CLOSE) {
            match(SYM_CURLY_CLOSE);            
        }
    }
}

void Parser::Declaration() {
    
    _message.print(DBUG, "PARSER: In Declaration()\n");
    
    //  TypeSpecifier, identifier, [ “[”, integer, “]” ],
    //    { “,”, identifier, [ “[”, integer, “]” ] }, “;”
    
    static tokenType firstSet[] = {KW_FLOAT, KW_INT, KW_VOID, (tokenType) -1};
    
    static tokenType followSet[] = {TOK_EOF, KW_ELSE, SYM_CURLY_CLOSE, (tokenType) - 1};

    if ( synchronized(firstSet, followSet, "Expected Declaration") ) {
        
        TypeSpecifier();
        match(TOK_IDENT);
        
        if ( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {
            
            match(SYM_SQ_OPEN);
            match(LIT_INT);
            
            if (_lookAhead.getTokenType() == SYM_SQ_CLOSE) {
                match(SYM_SQ_CLOSE);
            }
        }
        
        while ( _lookAhead.getTokenType() == SYM_COMMA ) {
            
            match(SYM_COMMA);
            match(TOK_IDENT);
            
            if ( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {
                match(SYM_SQ_OPEN);
                match(LIT_INT);
                if( _lookAhead.getTokenType() == SYM_SQ_CLOSE ) {
                    match(SYM_SQ_CLOSE);
                }
            }
        }
        
        if (_lookAhead.getTokenType() == SYM_SEMICOLON) {
            match(SYM_SEMICOLON);
        }
    }
}

void Parser::Statement() {
    
    _message.print(DBUG, "PARSER: In Statement()\n");
    
    //    ExpressionStatement
    //    | CompoundStatement
    //    | SelectionStatement
    //    | RepetitionStatement
    //    | ReturnStatement
    
    static tokenType statementFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, SYM_SEMICOLON, (tokenType) - 1};

    static tokenType followSet[] = {SYM_CURLY_CLOSE, KW_ELSE, (tokenType) - 1};
    
    if ( synchronized(statementFirstSet, followSet, "Expected Statement") ) {
        if ( _lookAhead.getTokenType() == SYM_CURLY_OPEN ) {
            CompoundStatement();
        } else if ( _lookAhead.getTokenType() == KW_IF ) {
            SelectionStatement();
        } else if ( _lookAhead.getTokenType() == KW_WHILE ) {
            RepetitionStatement();
        } else if ( _lookAhead.getTokenType() == KW_RETURN ) {
            ReturnStatement();
        } else {
            ExpressionStatement();
        }
    }
}

void Parser::ExpressionStatement() {

    _message.print(DBUG, "PARSER: In ExpressionStatement()\n");
    
    //    [ Expression, [ “=”, Expression ] ], “;”
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, SYM_SEMICOLON, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_CURLY_CLOSE, KW_ELSE, (tokenType) - 1};
    
    if ( synchronized(firstSet, followSet, "Expected Expression Statement") ) {
        
        Expression();
        
        if( _lookAhead.getTokenType() == SYM_ASSIGN ) {
            match(SYM_ASSIGN);
            Expression();
        }
    }
    
    if (_lookAhead.getTokenType() == SYM_SEMICOLON) {
        match(SYM_SEMICOLON);
    }
}

void Parser::SelectionStatement() {
    
    _message.print(DBUG, "PARSER: In SelectionStatement()\n");
    
    //    “if”, “(”, Expression, “)”, Statement, [ “else”, Statement ]
    
    static tokenType firstSet[] = {KW_IF, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_CURLY_CLOSE, KW_ELSE, (tokenType) - 1};
    
    if ( synchronized(firstSet, followSet, "Expected Selection Statement") ) {
        
        match(KW_IF);
        match(SYM_OPEN);
        Expression();

        if (_lookAhead.getTokenType() == SYM_CLOSE) {
            match(SYM_CLOSE);
            Statement();
        }
        
        if (_lookAhead.getTokenType() == KW_ELSE) {
            match(KW_ELSE);
            Statement();
        }
        
    }
}

void Parser::RepetitionStatement() {
    
    _message.print(DBUG, "PARSER: In RepetitionStatement()\n");
    
    //    “while”, “(”, Expression, “)”, Statement
    
    static tokenType firstSet[] = {KW_WHILE, (tokenType) - 1};
    static tokenType followSet[] = {SYM_CURLY_CLOSE, KW_ELSE, (tokenType) - 1};
    
    if ( synchronized(firstSet, followSet, "Expected Repetition Statement") ) {
        
        match(KW_WHILE);
        match(SYM_OPEN);
        Expression();
        match(SYM_CLOSE);
        Statement();
        
    }
}

void Parser::ReturnStatement() {
    
    _message.print(DBUG, "PARSER: In ReturnStatement()\n");
    
    //    “return”, [ Expression ], “;”
    
    static tokenType firstSet[] = {KW_RETURN, (tokenType) - 1};
    
    static tokenType expressionFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_CURLY_CLOSE, KW_ELSE, (tokenType) - 1};
    
    if ( synchronized(firstSet, followSet, "Expected Return Statement") ) {
        
        match(KW_RETURN);
        
        if ( memberOf(_lookAhead.getTokenType(),expressionFirstSet) ) {
            Expression();
        }
        
        if (_lookAhead.getTokenType() == SYM_SEMICOLON) {
            match(SYM_SEMICOLON);
        }
    }
}

void Parser::Expression() {
    
    _message.print(DBUG, "PARSER: In Expression()\n");
    
    //    AndExpression,
    //    { “||”, AndExpression }
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    if ( synchronized(firstSet, followSet, "Expected Expression") ) {
        
        AndExpression();
        
        while ( _lookAhead.getTokenType() == SYM_OR ) {
            match(SYM_OR);
            AndExpression();
        }
    }
}

void Parser::AndExpression() {
    
    _message.print(DBUG, "PARSER: In AndExpression()\n");
    
    //    RelationExpression,
    //    { “&&”, RelationExpression }

    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    static tokenType followSet[] = {SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};

    if ( synchronized(firstSet, followSet, "Expected AND Expression") ) {
        RelationExpression();
        
        while ( _lookAhead.getTokenType() == SYM_AND ) {
            match(SYM_AND);
            RelationExpression();
        }
    }
}

void Parser::RelationExpression() {
    
    _message.print(DBUG, "PARSER: In RelationExpression()\n");
    
    //    SimpleExpression,
    //    [ ( “<=” | “<” | “>=” | “>” | “==” | “!=” ),
    //     SimpleExpression ]
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    static tokenType set[] = {SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, (tokenType) -1};
    
    if ( synchronized(firstSet, followSet, "Expected Relational Expression") ) {
        
        SimpleExpression();
        
        if ( memberOf(_lookAhead.getTokenType(), set) ) {
            
            if ( _lookAhead.getTokenType() == SYM_LESS_EQ ) {
                match(SYM_LESS_EQ);
            } else if( _lookAhead.getTokenType() == SYM_LESS) {
                match(SYM_LESS);
            } else if( _lookAhead.getTokenType() == SYM_GREATER_EQ ) {
                match(SYM_GREATER_EQ);
            } else if( _lookAhead.getTokenType() == SYM_GREATER ) {
                match(SYM_GREATER);
            } else if( _lookAhead.getTokenType() == SYM_EQUAL ) {
                match(SYM_EQUAL);
            } else if( _lookAhead.getTokenType() == SYM_NOT_EQ ) {
                match(SYM_NOT_EQ);
            }
            
            SimpleExpression();
            
        }
    }

}

void Parser::SimpleExpression() {
    
    _message.print(DBUG, "PARSER: In SimpleExpression()\n");

    //    Term, { ( “+” | ”-“ ), Term }
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};

    if ( synchronized(firstSet, followSet, "Expected Simple Expression") ) {
        
        Term();
        
        while ( _lookAhead.getTokenType() == SYM_PLUS ||
                _lookAhead.getTokenType() == SYM_MINUS ) {
            
            if ( _lookAhead.getTokenType() == SYM_PLUS ) {
                match(SYM_PLUS);
            } else if ( _lookAhead.getTokenType() == SYM_MINUS ) {
                match(SYM_MINUS);
            }
            
            Term();
        }
    }
}

void Parser::Term() {
    
    _message.print(DBUG, "PARSER: In Term()\n");
    
    //Factor, { ( “*” | ”/” | “%” ), Factor }
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};

    static tokenType followSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    if ( synchronized(firstSet, followSet, "Expected Term") ) {
        
        Factor();
        
        while ( _lookAhead.getTokenType() == SYM_MUL ||
               _lookAhead.getTokenType() == SYM_DIV ||
               _lookAhead.getTokenType() == SYM_MOD ) {
            
            if ( _lookAhead.getTokenType() == SYM_MUL) {
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
    
    _message.print(DBUG, "PARSER: In Factor()\n");
    
    //[ “+” | “-“ ], Value
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_MUL, SYM_DIV, SYM_MOD, SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    if ( synchronized(firstSet, followSet, "Expected Factor") ) {
        
        if ( _lookAhead.getTokenType() == SYM_PLUS ) {
            match(SYM_PLUS);
        } else if ( _lookAhead.getTokenType() == SYM_MINUS ) {
            match(SYM_MINUS);
        }
        
        Value();
    }
 
}

void Parser::Value() {
    
    _message.print(DBUG, "PARSER: In Value()\n");
    
    //    “(”, Expression, “)”
    //    | identifier, [ “[“, Expression, “]” | “(“, [ Expression, { “,”, Expression } ], “)” ]
    //    | integer
    //    | float
    //    | string
    
    static tokenType firstSet[] = {SYM_OPEN, TOK_IDENT, LIT_INT, LIT_FLOAT, LIT_STR, (tokenType) - 1};

    static tokenType expressionFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_MUL, SYM_DIV, SYM_MOD, SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    if ( synchronized(firstSet, followSet, "Expected Value") ) {
        
        if ( _lookAhead.getTokenType() == SYM_OPEN ) {
            
            match(SYM_OPEN);
            Expression();
            match(SYM_CLOSE);
            
        } else if ( _lookAhead.getTokenType() == TOK_IDENT ) {
            
            match(TOK_IDENT);
            
            if ( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {
                
                match(SYM_SQ_OPEN);
                Expression();
                match(SYM_SQ_CLOSE);
                
            } else if( _lookAhead.getTokenType() == SYM_OPEN ) {
                
                match(SYM_OPEN);
                
                if ( memberOf(_lookAhead.getTokenType(), expressionFirstSet) ) {
                    
                    Expression();
                    
                    while ( _lookAhead.getTokenType() == SYM_COMMA ) {
                        match(SYM_COMMA);
                        Expression();
                    }
                    
                }
                
                match(SYM_CLOSE);
            }
        } else if ( _lookAhead.getTokenType() == LIT_INT ) {
            match(LIT_INT);
        } else if ( _lookAhead.getTokenType() == LIT_FLOAT ) {
            match(LIT_FLOAT);
        } else if ( _lookAhead.getTokenType() == LIT_STR ) {
            match(LIT_STR);
        }
    }
}