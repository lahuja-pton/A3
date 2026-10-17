/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Lakshit Ahuja                                              */
/* Implements a symbol table through a linked list.                   */
/*--------------------------------------------------------------------*/

#include "symtable.h"

/* struct to represent linked list for symbol table ADT, holding the 
   size and the first node of the linked list */
struct SymTable {size_t numBindings; struct Node* first;};
/* struct to represent a node of the linked list, holding the key-value
   pair and a pointer to the next node */
struct Node {const char* key; const void* value; struct Node* next;};

/* returns new instance of the symbol table */
SymTable_T SymTable_new(void) {
    return (SymTable_T)calloc(1, sizeof(struct SymTable));
}

/* frees all memory held by the symbol table */
void SymTable_free(SymTable_T oSymTable) {
    struct Node *node1, *node2;
    assert(oSymTable != NULL);

    for (node1 = oSymTable->first; node1; node1 = node2) {
        node2 = node1->next;
        free((char*)node1->key);
        free(node1);
    }
    free(oSymTable);
}

/* returns number of bindings held by the table */
size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable->numBindings;
}

/* inserts new binding if the binding is not already present. Otherwise,
   updates the value of the existing binding */
int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {
        struct Node* node;
        assert(oSymTable != NULL && pcKey);

        node = oSymTable->first;
        while (node) {
            if (strcmp(node->key, pcKey) == 0) return 0;
            node = node->next;
        }
        
        node = (struct Node*)calloc(1, sizeof(struct Node));
        if (!node) return 0;
        node->key = (const char*)malloc(strlen(pcKey)+1);
        if (!node->key) {
            free(node);
            return 0;
        }
        strcpy((char*)node->key, pcKey);
        node->value = pvValue;
        node->next = oSymTable->first;
        oSymTable->first = node;
        oSymTable->numBindings++;
        return 1;
}

/* updates value of binding associated with provided key if present */
void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {
        struct Node* node;
        void* value;
        assert(oSymTable != NULL && pcKey);

        node = oSymTable->first;
        while (node) {
            if (strcmp((char*)node->key, pcKey) == 0) {
                value = (void*)node->value;
                node->value = pvValue;
                return value;
            }
            node = node->next;
        }
        return NULL;
}

/* returns whether the table holds a binding with the provided key */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct Node* node;
    assert(oSymTable != NULL && pcKey);

    node = oSymTable->first;
    while (node) {
        if (strcmp((char*)node->key, pcKey) == 0) return 1;
        node = node->next;
    }
    return 0;
}

/* returns the value associated with the provided key if present 
   in the table */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    struct Node* node;
    assert(oSymTable != NULL && pcKey);

    node = oSymTable->first;
    while (node) {
        if (strcmp((char*)node->key, pcKey) == 0) 
            return (void*)node->value;
        node = node->next;
    }
    return NULL;
}

/* removes binding associated with key if present */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    struct Node *node1 = NULL, *node2;
    void* value;
    assert(oSymTable != NULL && pcKey);

    node2 = oSymTable->first;
    while (node2) {
        if (strcmp(node2->key, pcKey) == 0) {
            if (node1 == NULL) oSymTable->first = node2->next;
            else node1->next = node2->next;
            value = (void*)node2->value;
            free((char*)node2->key);
            free(node2);
            oSymTable->numBindings--;
            return value;
        }
        node1 = node2;
        node2 = node2->next;
    }
    return NULL;
}

/* runs the provided function on all bindings within the table */
void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
        struct Node* node;
        assert(oSymTable != NULL && pfApply != NULL);

        node = oSymTable->first;
        while (node) {
            (*pfApply)(node->key, (char*)node->value, (void*)pvExtra);
            node = node->next;
        }
}
