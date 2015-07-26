//
//  Shell.cpp
//  CMM
//
//  Created by Roshan Gautam on 7/18/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#include "shell.h"

char* Shell::version() {
    static char v[maxlength];
    sprintf(v, "V%s.%s.%s-%s",major_version,minor_version,sub,date);
    return v;
}

void Shell::banner() {
    _message.print(NORMAL, "%s %s Author - (%s) , Re-written in C++ by: %s", name, version(), credits, author);
}

bool Shell::isInteger(char *s) {
    bool isInt = true;
    int i = 0;
    if (s[i] == '+' || s[i] == '-')
        ++i;
    while (i < strlen(s) && isInt)
        isInt = isInt && isdigit(s[i++]);
    return isInt;
}

int Shell::getSwitch(char *s) {
    int i;
    for (i=0; i < strlen(s); ++i)
        s[i] = tolower(s[i]);
    if (strcmp(s, "+") == 0 || strcmp(s, "1") == 0 || strcmp(s, "on") == 0 || strcmp(s, "true") == 0)
        return 1;
    else if (strcmp(s, "-") == 0 || strcmp(s, "0") == 0 || strcmp(s, "off") == 0 || strcmp(s, "false") == 0)
        return 0;
    else
        return -1;
}

void Shell::processArgs(int argc, char *argv[]) {
    int i;
    char opt[maxlength], *arg;
    for (i=1; i<argc; i++) {
        strncpy(opt, argv[i], maxlength-1);
        opt[maxlength-1] = '\0';
        if (opt[0] == '-') {
            if ((arg = strchr(opt,':')) || (arg = strchr(opt,'=')))
                *(arg++) = '\0';
            if (strcmp(opt, "--help") == 0 || strcmp(opt, "-h") == 0 || strcmp(opt, "-?") == 0) {
                banner();
                printf("usage: cmc [options] filename [options]\n");
                printf("options:\n");
                printf("    --help        print this help (or -h -?)\n");
                printf("    --version     print out current version (or -V)\n");
                printf("    --silent      run silently: implies -W:off (or -s)\n");
                printf("    --verbose     turn on verbose messages (or -v)\n");
                printf("    --debug       turn on DBUGging messages (or -d)\n");
                printf("    --debugfile:F print DBUG messages to file F: implies -d (or -D:F)\n");
                printf("    --warnings:S  switch on/off warning messages: default 'on' (or -W:S)\n");
                printf("    --errorfile:F print error messages to file F (or -E:F)\n");
                printf("    --messages:N  set message print level: N = 0-4: default 2 (or -M:N)\n");
                printf("    --stdin       read source from standard input (or --)\n");
                printf("    --tabs:N      set tab width of input: N > 0: default 8 (or -T:N)\n");
                printf("switches (S) turn 'on' using 1/+/on/true or 'off' using 0/-/off/false\n\n");
                exit(0);
            } else if (strcmp(opt, "--version") == 0   || strcmp(opt, "-V") == 0) {
                printf("%s\n", version());
                exit(0);
            } else if (strcmp(opt, "--silent") == 0    || strcmp(opt, "-s") == 0) {
                _message.setProcessLevel(SILENT);
            } else if (strcmp(opt, "--verbose") == 0     || strcmp(opt, "-v") == 0) {
                _message.setProcessLevel(VERBOSE);
            } else if (strcmp(opt, "--debug") == 0     || strcmp(opt, "-d") == 0) {
                _message.setProcessLevel(DBUG);
            } else if (strcmp(opt, "--debugfile") == 0 || strcmp(opt, "-D") == 0) {
                if (arg == NULL || arg[0] == '\0') {
                    _message.print(WARNING, "command-line: filename missing: not redirecting DBUG messages");
                } else {
                    strncpy(dbgfilename, arg, namelen);
                    _message.print(DBUG, "command-line: redirecting DBUG messages to \"%s\"", dbgfilename);
                    _message.setProcessLevel(DBUG);
                    _message.setDebugFile(fopen(dbgfilename, "w"));
                }
            } else if (strcmp(opt, "--warnings") == 0  || strcmp(opt, "-W") == 0) {
                if (arg == NULL || arg[0] == '\0') {
                    _message.print(WARNING, "command-line: warning switch missing: setting to '%s'", _message.isWarningsEnabled() ? "on" : "off");
                } else {
                    int s = getSwitch(arg);
                    if (s > 0)
                        _message.enableWarnings();
                    else if (s == 0)
                        _message.disableWarnings();
                    else {
                        _message.print(WARNING, "command-line: warning switch '%s' invalid: setting to '%s'", arg, _message.isWarningsEnabled() ? "on" : "off");
                    }
                }
            } else if (strcmp(opt, "--errorfile") == 0 || strcmp(opt, "-E") == 0) {
                if (arg == NULL || arg[0] == '\0') {
                    _message.print(WARNING, "command-line: filename missing: not redirecting error messages");
                } else {
                    strncpy(errfilename, arg, namelen);
                    _message.print(DBUG, "command-line: redirecting error messages to \"%s\"", errfilename);
                    _message.setErrorFile(fopen(errfilename, "w"));
                }
            } else if (strcmp(opt, "--messages") == 0  || strcmp(opt, "-M") == 0) {
                if (arg == NULL || arg[0] == '\0') {
                    _message.print(WARNING, "command-line: message level missing: setting to %d", _message.getProcessLevel());
                } else if (!isInteger(arg)) {
                    _message.print(WARNING, "command-line: message level '%s' invalid: setting to %d", arg, _message.getProcessLevel());
                } else {
                    MessageLevel l = (MessageLevel)atoi(arg);
                    if (l < SILENT || l > DBUG) {
                    _message.print(WARNING, "command-line: message level %d out of range: setting to %d", l, _message.getProcessLevel());
                    } else {
                        _message.setProcessLevel((MessageLevel)l);
                    }
                }
            } else if (strcmp(opt, "--stdin") == 0     || strcmp(opt, "--") == 0) {
                stdinput = true;
            } else if (strcmp(opt, "--tabs") == 0      || strcmp(opt, "-T") == 0) {
                if (arg == NULL || arg[0] == '\0') {
                    _message.print(WARNING, "command-line: tab width missing: setting to %d", tabWidthParam);
                } else if (!isInteger(arg)) {
                    _message.print(WARNING, "command-line: tab width missing: setting to %d",arg, tabWidthParam);
                } else {
                    int t = atoi(arg);
                    if (t <= 0) {
                        _message.print(WARNING, "command-line: tab width %d out of range: setting to %d", t, tabWidthParam);
                    }
                    else
                        tabWidthParam = t;
                }
            } else {
                banner();
                _message.print(PANIC, "command-line: unknown option: '%s'", argv[i]);
            }
            _message.print(DBUG, "command-line: processed option '%s'", argv[i]);
        } else if (srcfilename[0] == '\0') {
            strncpy(srcfilename, argv[i], namelen-1);
            srcfilename[namelen-1] = '\0';
            if (strncmp(srcfilename+strlen(srcfilename)-strlen(EXT), EXT, strlen(EXT)) != 0)
                strncat(srcfilename, EXT, strlen(EXT));
            _message.print(DBUG, "command-line: processed filename \"%s\"", srcfilename);
        } else {
            banner();
            _message.print(PANIC, "command-line: too many filenames");
        }
    }
}