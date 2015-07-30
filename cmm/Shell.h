//
//  Shell.h
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#ifndef __CMM__Shell__
#define __CMM__Shell__

#include <stdio.h>
#include <stdbool.h>
#include "parser.h"

#define name    "C-- Compiler"
#define major_version	  "0"          /* version */
#define minor_version   "1"          /* revision */
#define sub     "0"          /* patch */
#define date    "20150718"   /* release */
#define author  "Roshan Gautam"
#define credits  "Jeff Jenness"

#define maxlength 	50
#define namelen 50
#define EXT ".cmm"

class Shell {
    Message _message;
    
    void processArgs(int argc, char *argv[]);
    char *version();
    void banner();
    bool isInteger(char *s);
    int getSwitch(char *s);
    
public:

    char srcfilename[namelen];
    char errfilename[namelen];
    char dbgfilename[namelen];
    bool stdinput = false;
    int tabWidthParam = 8; // default tab width
    
    Shell() {
        srcfilename[0] = '\0';
        errfilename[0] = '\0';
        dbgfilename[0] = '\0';
    }
    
    Shell(int argc, char *argv[]):Shell() {
        banner();       
        processArgs(argc, argv);
    }
    
    Message getMessage() {
        return _message;
    }
    
    char* getSourceFilename() {
        return srcfilename;
    }
    
    bool isStandardInput() {
        return stdinput;
    }
    
    int getTabWidthParam() {
        return tabWidthParam;
    }

};
#endif /* defined(__CMM__Shell__) */
