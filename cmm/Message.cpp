//
//  Message.cpp
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#include "message.h"


void Message::print(MessageLevel messageLevel, const char *fmt, ...) {
    static char msg[maxlen];
    va_list args;
    va_start(args,fmt);
    vsnprintf(msg, maxlen, fmt, args);
    if (messageLevel == PANIC || messageLevel == ERROR) {
        ++_errorCount;
        fprintf(_ferr, "ERROR: %s\n", msg);
        if (messageLevel == PANIC)
            exit (-1);
    } else if (messageLevel == WARNING) {
        if (WARNING && _processLevel > SILENT) {
            ++_warningCount;
            fprintf(_ferr, "WARNING: %s\n", msg);
        }
    } else if (messageLevel <= _processLevel) {
        if (messageLevel == DBUG) {
            fprintf(_fdbg, "DEBUG: %s\n", msg);
        } else
            fprintf(stdout, "%s\n", msg);
    }
    va_end(args);
}