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
    
    bool externDef = false; // extern keyword protection
    string type; // identifier type information container
    string identifier; // identifier attribute for functions
    while ( synchronized(translationUnitFirstSet, followSet, "Expected TranslationUnit") ) {
        
        if ( _lookAhead.getTokenType() == KW_EXTERN ) {
            match(KW_EXTERN);
            externDef = true;
        }
        
        type = TypeSpecifier();
        
        identifier = _lookAhead.getLexeme();
        
        if (_lookAhead.getTokenType() == TOK_IDENT)
            if(!_symbolTable->define(_lookAhead.getLexeme(), type))
                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Redifinition of '%s'", _lookAhead.getRow() , _lookAhead.getCol(), _lookAhead.getLexeme());
        
        match(TOK_IDENT);
        if( _lookAhead.getTokenType() == SYM_SQ_OPEN ||
            _lookAhead.getTokenType() == SYM_COMMA ||
            _lookAhead.getTokenType() == SYM_SEMICOLON ) {  // VARIABLE DECLARATION
            
            if (type == "v") {
                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Variable has incomplete type 'void'", _lookAhead.getRow() , _lookAhead.getCol());
                _symbolTable->remove(identifier);
            }
            
            if ( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {  // ARRAY
                
                match(SYM_SQ_OPEN);
                match(LIT_INT);
                if (_lookAhead.getTokenType() == SYM_SQ_CLOSE) {
                    match(SYM_SQ_CLOSE); // MUST MATCH SQUARE BRACKET CLOSE
                }

                if (type != "v") {
                    _symbolTable->reDefine(identifier, type == "i" ? "I" : "F");
                }
            }

            while ( _lookAhead.getTokenType() == SYM_COMMA ) {
                
                match(SYM_COMMA);
                
                identifier = _lookAhead.getLexeme();
                
                if (_lookAhead.getTokenType() == TOK_IDENT)
                    if(type != "v" &&
                       !_symbolTable->define(_lookAhead.getLexeme(), type))
                        _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Redifinition of '%s'", _lookAhead.getRow() , _lookAhead.getCol(), _lookAhead.getLexeme());
                
                match(TOK_IDENT);
                
                if( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {
                    match(SYM_SQ_OPEN);
                    match(LIT_INT);
                    if (_lookAhead.getTokenType() == SYM_SQ_CLOSE) {
                        match(SYM_SQ_CLOSE);
                    }
                    
                    if (type != "v") {
                        _symbolTable->reDefine(identifier, type == "i" ? "I" : "F");
                    }
                }
            }
            
            match(SYM_SEMICOLON);

        } else if ( _lookAhead.getTokenType() == SYM_OPEN ) { // FUNCTION DECLARATION OR DEFINITION

            _symbolTable->openScope();
            
            match(SYM_OPEN);
            
            if ( memberOf(_lookAhead.getTokenType(), parameterFirstSet) ) {
                
                type += Parameter();
                
                while( _lookAhead.getTokenType() == SYM_COMMA ) {
                    match(SYM_COMMA);
                    type += Parameter();
                }
                
            } else {
                type += "v";
            }
            
            match(SYM_CLOSE);
            
            _symbolTable->reDefine(identifier, type);

            if ( _lookAhead.getTokenType() == SYM_SEMICOLON )
                match(SYM_SEMICOLON); // FUNCTION DECLARATION
            else {
                if (externDef) {
                    _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Function defition can not be of 'extern' type", _lookAhead.getRow() , _lookAhead.getCol());
                }
                CompoundStatement(identifier); // FUNCTION DEFINITION
            }
//            _symbolTable->dump();            
            _symbolTable->closeScope();
        }
    }

    _message.print(DBUG, "PARSER: End of TranslationUnit()\n");
}

string Parser::TypeSpecifier() {

    _message.print(DBUG, "PARSER: In TypeSpecifier()\n");
    
    //    “void”
    //    | “int”
    //    | “float”
    
    static tokenType firstSet[] = {KW_FLOAT, KW_INT, KW_VOID, (tokenType) -1};
    
    static tokenType followSet[] = {TOK_IDENT, (tokenType) -1};

    string type;
    
    if ( synchronized(firstSet, followSet, "Expected TypeSpecifier") ) {
        if ( _lookAhead.getTokenType() == KW_FLOAT ) {
            match(KW_FLOAT);
            type = "f";
        } else if ( _lookAhead.getTokenType() == KW_INT ) {
            match(KW_INT);
            type = "i";
        } else if ( _lookAhead.getTokenType() == KW_VOID ) {
            match(KW_VOID);
            type = "v";
        }
    }
    _message.print(DBUG, "PARSER: End of TypeSpecifier()\n");
    
    return type;
}

string Parser::Parameter() {

    _message.print(DBUG, "PARSER: In Parameter()\n");
    
    //    TypeSpecifier, identifier, [ “[”, “]” ]
    
    static tokenType firstSet[] = {KW_FLOAT, KW_INT, KW_VOID, (tokenType)-1};
    
    static tokenType followSet[] = {SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    string type;
    string identifier;
    
    if ( synchronized(firstSet, followSet, "Expected Parameter") ) {
        
        type = TypeSpecifier();
        
        identifier = _lookAhead.getLexeme();
        
        if (_lookAhead.getTokenType() == TOK_IDENT)
            if(type != "v" && // make sure its not void type
               !_symbolTable->define(_lookAhead.getLexeme(), type))
                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Redifinition of '%s'", _lookAhead.getRow() , _lookAhead.getCol(), _lookAhead.getLexeme());
        
        if (type == "v") {
            _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Variable has incomplete type 'void'", _lookAhead.getRow() , _lookAhead.getCol());
            _symbolTable->remove(identifier);
        }
        
        match(TOK_IDENT);
        
        if ( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {
            match(SYM_SQ_OPEN);
            match(SYM_SQ_CLOSE);
            
            if (type != "v") {
                _symbolTable->reDefine(identifier, type == "i" ? "I" : "F");
            }
        }
    }
    
    _message.print(DBUG, "PARSER: End of Parameter()\n");
    return type;
}

void Parser::CompoundStatement(string functionName) {

    _message.print(DBUG, "PARSER: In CompoundStatement()\n");
    
    //“{”, { Declaration | Statement }, “}”
    
    static tokenType compoundStatementFirstSet[] = {SYM_CURLY_OPEN, (tokenType)-1};
    
    static tokenType declarationFirstSet[] = {KW_VOID, KW_INT, KW_FLOAT, (tokenType) - 1};
    
    static tokenType statementFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {TOK_EOF, KW_ELSE, SYM_CURLY_CLOSE, KW_VOID, KW_INT, KW_FLOAT, SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    if ( synchronized(compoundStatementFirstSet, followSet, "Expected Compound Statement") ) {
        
        match(SYM_CURLY_OPEN);
        
        while ( memberOf(_lookAhead.getTokenType(), declarationFirstSet) ||
                memberOf(_lookAhead.getTokenType(), statementFirstSet) ) {
            
            if ( memberOf(_lookAhead.getTokenType(), declarationFirstSet) ) {
                Declaration();
            } else if ( memberOf(_lookAhead.getTokenType(), statementFirstSet) ) {
                Statement(functionName);
            }
        }
        match(SYM_CURLY_CLOSE);            

    }
    _message.print(DBUG, "PARSER: End of CompoundStatement()\n");
}

void Parser::Declaration() {
    // repeat same typing as TranslationUnit
    _message.print(DBUG, "PARSER: In Declaration()\n");
    
    //  TypeSpecifier, identifier, [ “[”, integer, “]” ],
    //    { “,”, identifier, [ “[”, integer, “]” ] }, “;”
    
    static tokenType firstSet[] = {KW_FLOAT, KW_INT, KW_VOID, (tokenType) -1};
    
    static tokenType followSet[] = {TOK_EOF, KW_ELSE, SYM_CURLY_CLOSE, KW_VOID, KW_INT, KW_FLOAT, SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    string type;
    string identifier;
    
    if ( synchronized(firstSet, followSet, "Expected Declaration") ) {
        
        type = TypeSpecifier();
        identifier = _lookAhead.getLexeme();
        
        if (_lookAhead.getTokenType() == TOK_IDENT)
            if(type != "v" &&
               !_symbolTable->define(_lookAhead.getLexeme(), type))
                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Redifinition of '%s'", _lookAhead.getRow() , _lookAhead.getCol(), identifier.c_str());
        
        if (type == "v") {
            _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Variable has incomplete type 'void'", _lookAhead.getRow() , _lookAhead.getCol());
        }
        
        match(TOK_IDENT);
        
        if ( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {
            
            match(SYM_SQ_OPEN);
            match(LIT_INT);
            match(SYM_SQ_CLOSE);
            
            if (type != "v") {
                _symbolTable->reDefine(identifier, type == "i" ? "I" : "F");
            }
        }
        
        while ( _lookAhead.getTokenType() == SYM_COMMA ) {
            
            match(SYM_COMMA);
            
            identifier = _lookAhead.getLexeme();
            
            if (_lookAhead.getTokenType() == TOK_IDENT)
                if(type != "v" &&
                   !_symbolTable->define(_lookAhead.getLexeme(), type))
                    _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Redifinition of '%s'", _lookAhead.getRow() , _lookAhead.getCol(), identifier.c_str());
            
            match(TOK_IDENT);
            
            if ( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {
                
                match(SYM_SQ_OPEN);
                match(LIT_INT);
                match(SYM_SQ_CLOSE);
                if (type != "v") {
                    _symbolTable->reDefine(identifier, type == "i" ? "I" : "F");
                }
            }
        }
        
        match(SYM_SEMICOLON);
    }
    _message.print(DBUG, "PARSER: End of Declaration()\n");
}

void Parser::Statement(string functionName) {
    
    _message.print(DBUG, "PARSER: In Statement()\n");
    
    //    ExpressionStatement
    //    | CompoundStatement
    //    | SelectionStatement
    //    | RepetitionStatement
    //    | ReturnStatement
    
    static tokenType statementFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, SYM_SEMICOLON, (tokenType) - 1};

    static tokenType followSet[] = {TOK_EOF, KW_ELSE, SYM_CURLY_CLOSE, KW_VOID, KW_INT, KW_FLOAT, SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    if ( synchronized(statementFirstSet, followSet, "Expected Statement") ) {
        if ( _lookAhead.getTokenType() == SYM_CURLY_OPEN ) {
            _symbolTable->openScope();
            CompoundStatement(functionName);
//            _symbolTable->dump();
            _symbolTable->closeScope();
        } else if ( _lookAhead.getTokenType() == KW_IF ) {
            SelectionStatement(functionName);
        } else if ( _lookAhead.getTokenType() == KW_WHILE ) {
            RepetitionStatement(functionName);
        } else if ( _lookAhead.getTokenType() == KW_RETURN ) {
            ReturnStatement(functionName);
        } else {
            ExpressionStatement();
        }
    }
    _message.print(DBUG, "PARSER: End of Statement()\n");
}

void Parser::ExpressionStatement() {

    _message.print(DBUG, "PARSER: In ExpressionStatement()\n");
    
    //    [ Expression, [ “=”, Expression ] ], “;”
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, SYM_SEMICOLON, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {TOK_EOF, KW_ELSE, SYM_CURLY_CLOSE, KW_VOID, KW_INT, KW_FLOAT, SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    string type, type1;
    
    if ( synchronized(firstSet, followSet, "Expected Expression Statement") ) {
        
        type = Expression();

        if( _lookAhead.getTokenType() == SYM_ASSIGN ) {
            match(SYM_ASSIGN);

            type1 = Expression();

            // check that type1 and type2 are assignment compatible
            if(!((type == "f" && type1 == "i") ||
               (type == "i" && type1 == "i") ||
               (type == "f" && type1 == "f"))) {
                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Incompatible assignment", _lookAhead.getRow() , _lookAhead.getCol());
            }
        }
        
        match(SYM_SEMICOLON);
    }
    
    
    _message.print(DBUG, "PARSER: End of ExpressionStatement()\n");
}

void Parser::SelectionStatement(string functionName) {
    
    _message.print(DBUG, "PARSER: In SelectionStatement()\n");
    
    //    “if”, “(”, Expression, “)”, Statement, [ “else”, Statement ]
    
    static tokenType firstSet[] = {KW_IF, (tokenType) - 1};
    
    static tokenType followSet[] = {TOK_EOF, KW_ELSE, SYM_CURLY_CLOSE, KW_VOID, KW_INT, KW_FLOAT, SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    
    if ( synchronized(firstSet, followSet, "Expected Selection Statement") ) {
        
        match(KW_IF);
        match(SYM_OPEN);
        
        if (Expression() != "i") {
            _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Invalid expression", _lookAhead.getRow() , _lookAhead.getCol());
        }
        
        match(SYM_CLOSE);
        Statement(functionName);
        
        if (_lookAhead.getTokenType() == KW_ELSE) {
            match(KW_ELSE);
            Statement(functionName);
        }
        
    }
    _message.print(DBUG, "PARSER: End of SelectionStatement()\n");
}

void Parser::RepetitionStatement(string functionName) {
    
    _message.print(DBUG, "PARSER: In RepetitionStatement()\n");
    
    //    “while”, “(”, Expression, “)”, Statement
    
    static tokenType firstSet[] = {KW_WHILE, (tokenType) - 1};
    
    static tokenType followSet[] = {TOK_EOF, KW_ELSE, SYM_CURLY_CLOSE, KW_VOID, KW_INT, KW_FLOAT, SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    if ( synchronized(firstSet, followSet, "Expected Repetition Statement") ) {
        
        match(KW_WHILE);
        match(SYM_OPEN);
        if (Expression() != "i") {
            _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Invalid expression", _lookAhead.getRow() , _lookAhead.getCol());
        }
        match(SYM_CLOSE);
        Statement(functionName);
        
    }
    
    _message.print(DBUG, "PARSER: End of RepetitionStatement()\n");
}

void Parser::ReturnStatement(string functionName) {
    
    _message.print(DBUG, "PARSER: In ReturnStatement()\n");
    
    //    “return”, [ Expression ], “;”
    
    static tokenType firstSet[] = {KW_RETURN, (tokenType) - 1};
    
    static tokenType expressionFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {TOK_EOF, KW_ELSE, SYM_CURLY_CLOSE, KW_VOID, KW_INT, KW_FLOAT, SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_CURLY_OPEN, KW_IF, KW_WHILE, KW_RETURN, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    string type = "v"; // return statements defaults to void
    
    if ( synchronized(firstSet, followSet, "Expected Return Statement") ) {
        
        match(KW_RETURN);
        
        if ( memberOf(_lookAhead.getTokenType(),expressionFirstSet) ) {
            type = Expression();
        }
        
        string def = _symbolTable->lookup(functionName);
        if (!def.empty())
            def = def.substr(0,1) == "v" ? "void" : (def.substr(0,1) == "i" ? "int" : "float");

        if(def.empty() || def.substr(0,1) != type) {
            _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Function '%s' shoule return '%s' value", _lookAhead.getRow() , _lookAhead.getCol(),functionName.c_str(),def.c_str());
        }
    
        match(SYM_SEMICOLON);
    }
    _message.print(DBUG, "PARSER: End of ReturnStatement()\n");
}

string Parser::Expression() {
    
    _message.print(DBUG, "PARSER: In Expression()\n");
    
    //    AndExpression,
    //    { “||”, AndExpression }
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    string type, type1;
    
    if ( synchronized(firstSet, followSet, "Expected Expression") ) {
        
        type = AndExpression();

        while ( _lookAhead.getTokenType() == SYM_OR ) {
            match(SYM_OR);
            type = "i";
            type1 = AndExpression();
            if (!(type == "i" && type1 == "i"))
                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Conditional statement must return either 0 or 1", _lookAhead.getRow() , _lookAhead.getCol());
        }
    }
    _message.print(DBUG, "PARSER: End of Expression()\n");
    return type;
}

string Parser::AndExpression() {
    
    _message.print(DBUG, "PARSER: In AndExpression()\n");
    
    //    RelationExpression,
    //    { “&&”, RelationExpression }

    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    static tokenType followSet[] = {SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};

    string type, type1;
    
    if ( synchronized(firstSet, followSet, "Expected AND Expression") ) {
        
        type = RelationExpression();
        
        while ( _lookAhead.getTokenType() == SYM_AND ) {
            type = "i";
            match(SYM_AND);
            type1 = RelationExpression();
            if (!(type == "i" && type1 == "i"))
                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Conditional statement must return either 0 or 1", _lookAhead.getRow() , _lookAhead.getCol());
        }
    }
    _message.print(DBUG, "PARSER: End of AndExpression()\n");
    return type;
}

string Parser::RelationExpression() {
    
    _message.print(DBUG, "PARSER: In RelationExpression()\n");
    
    //    SimpleExpression,
    //    [ ( “<=” | “<” | “>=” | “>” | “==” | “!=” ),
    //     SimpleExpression ]
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    static tokenType set[] = {SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, (tokenType) -1};
    
    string type, type1;
    
    if ( synchronized(firstSet, followSet, "Expected Relational Expression") ) {
        
        type = SimpleExpression();

        if ( memberOf(_lookAhead.getTokenType(), set) ) {
            
            //check to make sure both types of simpleexpression are numeric
            type = "i";
            
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
            
            type1 = SimpleExpression();
            
            if(!((type == "f" && type1 == "i") ||
                 (type == "i" && type1 == "i") ||
                 (type == "f" && type1 == "f") ||
                 (type == "i" && type1 == "f"))) {
                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Incompatible comparision", _lookAhead.getRow() , _lookAhead.getCol());
            }
        }
    }
    _message.print(DBUG, "PARSER: End of RelationExpression()\n");
    return type;
}

string Parser::SimpleExpression() {
    
    _message.print(DBUG, "PARSER: In SimpleExpression()\n");

    //    Term, { ( “+” | ”-“ ), Term }
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};

    string type, type1;
    
    if ( synchronized(firstSet, followSet, "Expected Simple Expression") ) {
        
        type = Term();

        while ( _lookAhead.getTokenType() == SYM_PLUS ||
                _lookAhead.getTokenType() == SYM_MINUS ) {
            
            if ( _lookAhead.getTokenType() == SYM_PLUS ) {
                match(SYM_PLUS);
            } else if ( _lookAhead.getTokenType() == SYM_MINUS ) {
                match(SYM_MINUS);
            }
            
            type1 = Term();

            if((type == "f" && type1 == "i") ||
               (type == "f" && type1 == "f") ||
               (type == "i" && type1 == "f")) {
                type = "f";
            } else if(type == "i" && type1 == "i") {
                type = "i";
            } else {
                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Incompatible operation", _lookAhead.getRow() , _lookAhead.getCol());
            }
        }
    }
    
    _message.print(DBUG, "PARSER: End of SimpleExpression()\n");
    return type;
}

string Parser::Term() {
    
    _message.print(DBUG, "PARSER: In Term()\n");
    
    //Factor, { ( “*” | ”/” | “%” ), Factor }
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};

    static tokenType followSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    string type, type1;
    
    if ( synchronized(firstSet, followSet, "Expected Term") ) {
        
        type = Factor();
        
        while ( _lookAhead.getTokenType() == SYM_MUL ||
               _lookAhead.getTokenType() == SYM_DIV ||
               _lookAhead.getTokenType() == SYM_MOD ) {
            bool isModOperation = false;
            if ( _lookAhead.getTokenType() == SYM_MUL) {
                match(SYM_MUL);
            } else if(_lookAhead.getTokenType() == SYM_DIV) {
                match(SYM_DIV);
            } else if(_lookAhead.getTokenType() == SYM_MOD) {
                match(SYM_MOD);
                isModOperation = true;
            }
            
            type1 = Factor();
            
            if((type == "f" && type1 == "i") ||
               (type == "f" && type1 == "f") ||
               (type == "i" && type1 == "f")) {
                type = "f";
                if (isModOperation) {
                    type = "i";
                }
            } else if(type == "i" && type1 == "i") {
                type = "i";
            } else {
                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Incompatible operation", _lookAhead.getRow() , _lookAhead.getCol());
            }
        }
    }
    _message.print(DBUG, "PARSER: End of Term()\n");
    return type;
}

string Parser::Factor() {
    
    _message.print(DBUG, "PARSER: In Factor()\n");
    
    //[ “+” | “-“ ], Value
    
    static tokenType firstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_MUL, SYM_DIV, SYM_MOD, SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    string type;
    
    if ( synchronized(firstSet, followSet, "Expected Factor") ) {
        
        bool isNumeric = false;
        
        if ( _lookAhead.getTokenType() == SYM_PLUS ) {
            match(SYM_PLUS);
            isNumeric = true;
        } else if ( _lookAhead.getTokenType() == SYM_MINUS ) {
            match(SYM_MINUS);
            isNumeric = true;
        }
        
        type = Value();
        
        if (isNumeric &&
            (type != "i" || type != "f")) {
            _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Invalid operation", _lookAhead.getRow() , _lookAhead.getCol());
        }
    }
    
     _message.print(DBUG, "PARSER: End of Factor()\n");
    return type;
}

string Parser::Value() {
    
    _message.print(DBUG, "PARSER: In Value()\n");
    
    //    “(”, Expression, “)”
    //    | identifier, [ “[“, Expression, “]” | “(“, [ Expression, { “,”, Expression } ], “)” ]
    //    | integer
    //    | float
    //    | string
    
    static tokenType firstSet[] = {SYM_OPEN, TOK_IDENT, LIT_INT, LIT_FLOAT, LIT_STR, (tokenType) - 1};

    static tokenType expressionFirstSet[] = {SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_OPEN, LIT_INT, LIT_FLOAT, LIT_STR, TOK_IDENT, (tokenType) - 1};
    
    static tokenType followSet[] = {SYM_MUL, SYM_DIV, SYM_MOD, SYM_PLUS, SYM_MINUS, SYM_NOT, SYM_LESS_EQ, SYM_LESS, SYM_GREATER_EQ, SYM_GREATER, SYM_EQUAL, SYM_NOT_EQ, SYM_AND, SYM_OR, SYM_ASSIGN, SYM_SEMICOLON, SYM_OPEN, SYM_SQ_OPEN, SYM_COMMA, SYM_CLOSE, (tokenType) - 1};
    
    string type, identifier;
    
    if ( synchronized(firstSet, followSet, "Expected Value") ) {

        if ( _lookAhead.getTokenType() == SYM_OPEN ) {
            
            match(SYM_OPEN);
            type = Expression();
            match(SYM_CLOSE);
            
        } else if ( _lookAhead.getTokenType() == TOK_IDENT ) {
            
            identifier = _lookAhead.getLexeme();
            type = _symbolTable->lookup(identifier);

            if(!_symbolTable->isDefined(_lookAhead.getLexeme()))
                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Use of undeclared identifier '%s'", _lookAhead.getRow() , _lookAhead.getCol(), identifier.c_str());
            
            match(TOK_IDENT);

            if ( _lookAhead.getTokenType() == SYM_SQ_OPEN ) {
                // check ident in symbol table to make sure it is an array
                if (type != "I" ||
                    type != "F")
                        _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Identifier %s must be an array", _lookAhead.getRow() , _lookAhead.getCol(), identifier.c_str());
                
                std::transform(type.begin(), type.end(), type.begin(), ::tolower);
                // return type is now the lowcase value of array type
                match(SYM_SQ_OPEN);
                if(Expression() != "i")
                        _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Array index must be an integer", _lookAhead.getRow() , _lookAhead.getCol());
                //make sure the expression type is integer
                match(SYM_SQ_CLOSE);
                
            } else if( _lookAhead.getTokenType() == SYM_OPEN ) {
                
                if (type.length() < 2) { //function is at least two "vv"
                    _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Not a function", _lookAhead.getRow() , _lookAhead.getCol());
                }
                
                string tempType = type;
                
                if (!type.empty())
                    type = type.substr(0,1);
                

                match(SYM_OPEN);
                
                if ( memberOf(_lookAhead.getTokenType(), expressionFirstSet) ) {
                    
                    string expType = Expression();
                    
                    if (!tempType.empty() && expType != tempType.substr(1,1))
                        _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Parameter type mismatch a", _lookAhead.getRow() , _lookAhead.getCol());

                    int formalParamCount = !tempType.empty() ?  (int) (tempType.length() - 1) : 0;
                    int paramCount = 1;
                    
                    while ( _lookAhead.getTokenType() == SYM_COMMA ) {
                        paramCount++;
                        match(SYM_COMMA);
                        string expType1 = Expression();
                        if (paramCount > formalParamCount)
                            _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Too many parameters", _lookAhead.getRow() , _lookAhead.getCol());
                        else
                            if(!tempType.empty() && expType1 != tempType.substr(paramCount,1))
                                _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Parameter type mismatch b", _lookAhead.getRow() , _lookAhead.getCol());
                    }
                    if(paramCount < formalParamCount)
                        _message.print(ERROR, "SEMANTIC-ANALYZER: Semantic issue on line: %i col: %i. Too few parameters", _lookAhead.getRow() , _lookAhead.getCol());
                    
                }
                
                match(SYM_CLOSE);
            }
        } else if ( _lookAhead.getTokenType() == LIT_INT ) {
            match(LIT_INT);
            type = "i";
        } else if ( _lookAhead.getTokenType() == LIT_FLOAT ) {
            match(LIT_FLOAT);
            type = "f";
        } else if ( _lookAhead.getTokenType() == LIT_STR ) {
            match(LIT_STR);
            type = "s";
        }
    }
     _message.print(DBUG, "PARSER: End of Value()\n");
    return type;
}