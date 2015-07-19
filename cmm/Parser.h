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
public:

    Parser() {
        _message.print(D_BUG, "Parser:Initialized");
    }
    
    Parser(FILE *fin, int tabSize) : Parser() {
        _scanner = Scanner(fin);
        _scanner.setTabWidth(tabSize);
    }
    
    Scanner getScanner() {
        return _scanner;
    }
    
    bool read();
};
#endif /* defined(__CMM__parser__) */
