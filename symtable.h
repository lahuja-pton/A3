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

/* frees all memory held by the oSymTable */
void SymTable_free(SymTable_T oSymTable);

/* returns number of bindings held by oSymTable */
size_t SymTable_getLength(SymTable_T oSymTable);

/* inserts new binding if the binding with key pcKey is not already 
   present in oSymTable. Otherwise, updates the value of the existing 
   binding to pvValue. Returns whether a new binding was created. */
int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/* updates value of binding associated with key pcKey to pvValue if 
   present in oSymTable */
void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/* returns whether oSymTable holds a binding with key pcKey */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* returns the value associated with key pcKey if present in 
   oSymTable */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* removes binding associated with key pcKey if present in oSymTable */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* runs pfApply with extra arguments pvExtra on all bindings within 
   oSymTable */
void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);

#endif