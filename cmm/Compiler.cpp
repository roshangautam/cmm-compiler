//
//  Compiler.cpp
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#include "Shell.h"

int main(int argc, char *argv[]) {

    FILE *fin;

    Shell shell = Shell(argc, argv);
    
    if (shell.getSourceFilename()[0] == '\0') {
        fin = stdin;
        shell.getMessage().print(NORMAL, "compiler: No source file provided. Reading from standard input");
    } else {
        if (shell.isStandardInput()) {
            shell.getMessage().print(WARNING, "compiler: ignoring option --stdin: using source file \"%s\"", shell.getSourceFilename());
        }
        fin = fopen(shell.getSourceFilename(), "r");
        if (!fin) {
            shell.getMessage().print(PANIC, "compiler: file \"%s\" not found", shell.getSourceFilename());
        }
        shell.getMessage().print(VERBOSE, "compiler: reading from source file \"%s\"", shell.getSourceFilename());
    }
    
    Parser parser = Parser(fin, shell.getTabWidthParam());
    
    if (parser.read()) {
        shell.getMessage().print(NORMAL, "%i lines processed", parser.getScanner().getLinesRead());
    } else {
        shell.getMessage().print(NORMAL, "%i errors: %i warnings: compile failed", shell.getMessage().getErrorCount(), shell.getMessage().getWarningCount());
    }
    return 0;
}
