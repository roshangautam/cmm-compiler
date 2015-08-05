//
//  symbolTable.cpp
//  cmm
//
//  Created by Roshan Gautam on 8/1/15.
//  Copyright (c) 2015 Roshan Gautam. All rights reserved.
//

#include "symbolTable.h"

void SymbolTable::dump() {
	int i;
	Scope *scope;
	Entry *entry;
    cerr << "SYMBOL-TABLE:" << SPC << "{" << SEP;
	scope = _currentScope;
	while (scope != NULL) {
		fprintf(stderr, "%sscope(%d)%s{%s", TAB, scope->getId(), SPC, SEP);
		for (i = 0; i < MAXENT; ++i)
			if (scope->getEntry(i) != NULL) {
                entry = scope->getEntry(i);
                cerr << TABS << "hash(" << i << ")" << SPC << "{\"" << entry->getSymbol() << "\"";
				entry = entry->getNextEntry();
				while (entry != NULL) {
                    cerr << ",\"" << entry->getSymbol() << "\"";
					entry = entry->getNextEntry();
				}
                cerr << "}" << SEP ;
			}
        cerr << TABS << "}" << SEP ;
		scope = scope->getNextScope();
	}
    cerr << "}" << EOL;
}

// returns true if found, false otherwise
bool SymbolTable::isDefined(string symbol) {
	return searchTable(symbol) != NULL;
}

// returns true if sym not defined in current scope, false otherwise
bool SymbolTable::define(string symbol, string definition) {
    Entry *entry = NULL;
	if (searchScope(_currentScope, symbol) == NULL) {
		int index = hash(symbol);
        entry = new Entry();
        entry->setSymbol(symbol);
        entry->setDefinition(definition);
        entry->setNextEntry(_currentScope->getEntry(index));
        _currentScope->setEntry(index, entry);
	}
	return entry != NULL;
}

// returns true if sym is found in any scope, false otherwise
bool SymbolTable::reDefine(string symbol, string definition) {
	Entry *entry = searchTable(symbol);
	if (entry != NULL)
        entry->setDefinition(definition);
	return entry != NULL;
}

// returns the symbol definition if found in any scope, NULL otherwise
string SymbolTable::lookup(string symbol) {
    string defintion;
	Entry *entry = searchTable(symbol);
	if (entry != NULL)
        return entry->getDefinition();
	return defintion;
}

void SymbolTable::remove(string symbol) {
    Entry *entry = searchScope(_currentScope, symbol);
    Entry *nextEntry = entry->getNextEntry();
    entry = nextEntry;
}
