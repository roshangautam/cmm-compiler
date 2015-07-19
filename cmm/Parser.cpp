//
//  parser.cpp
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#include "Parser.h"


bool Parser::read() {
    do {
        _scanner.read();
        _message.print(NORMAL, "%s", _scanner.getToken().getFormattedLexeme());
    } while (_scanner.getToken().getTokenType() != TOK_EOF);
    return true;
}