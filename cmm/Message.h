//
//  Message.h
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#ifndef __CMM__Message__
#define __CMM__Message__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define maxlen 256

typedef enum {
    PANIC = -3,
    ERROR = -2,
    WARNING = -1,
    SILENT,
    CONCISE,
    NORMAL,
    VERBOSE,
    DBUG
} MessageLevel;


class Message {
    MessageLevel _processLevel;
    bool _warnings;
    int _warningCount;
    int _errorCount;
    FILE* _fdbg;
    FILE* _ferr;
public:
    
    Message() {
        _processLevel = NORMAL;
        _warnings = true;
        _warningCount = 0;
        _errorCount = 0;
        _fdbg = stdout;
        _ferr = stderr;
    }
    
    Message(MessageLevel level):Message() {
        _processLevel = level;
        print(DBUG, "Error Handler Initialized");        
    }
    
    void setProcessLevel(MessageLevel level) {
        _processLevel = level;
    }

    MessageLevel getProcessLevel() {
        return _processLevel;
    }
    
    void setDebugFile(FILE* debug) {
        _fdbg = debug;
    }
    
    void setErrorFile(FILE* error) {
        _ferr = error;
    }
    
    void enableWarnings() {
        _warnings = true;
    }
    
    void disableWarnings() {
        _warnings = false;
    }
    
    bool isWarningsEnabled() {
        return _warnings;
    }
    
    int getErrorCount() {
        return _errorCount;
    }
    
    int getWarningCount() {
        return _warningCount;
    }
    
    void print(MessageLevel messageLevel, const char *fmt, ...);
};

#endif /* defined(__CMM__Message__) */
