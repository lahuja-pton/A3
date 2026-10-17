/*--------------------------------------------------------------------*/
/* str.h                                                              */
/* Author: Lakshit Ahuja                                              */
/* Interface for symbol table functions.                              */
/*--------------------------------------------------------------------*/

#ifndef SYM_TABLE_H
#define SYM_TABLE_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* defines new type as pointer to SymTable for abstraction */
typedef struct SymTable* SymTable_T;

/* returns new instance of the symbol table */
SymTable_T SymTable_new(void);

/* frees all memory held by the symbol table */
void SymTable_free(SymTable_T oSymTable);

/* returns number of bindings held by the table */
size_t SymTable_getLength(SymTable_T oSymTable);

/* inserts new binding if the binding is not already present. Otherwise,
   updates the value of the existing binding */
int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/* updates value of binding associated with provided key if present */
void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/* returns whether the table holds a binding with the provided key */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* returns the value associated with the provided key if present 
   in the table */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* removes binding associated with key if present */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* runs the provided function on all bindings within the table */
void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);

#endif