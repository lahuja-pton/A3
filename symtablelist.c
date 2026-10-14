#include "symtable.h"

struct SymTable {size_t numBindings; struct Node* first;};
struct Node {const char* key; const void* value; struct Node* next;};

SymTable_T SymTable_new(void) {
    SymTable_T table = (SymTable_T)calloc(1, sizeof(struct SymTable));
    return table;
}

void SymTable_free(SymTable_T oSymTable) {
    struct Node *node1, *node2;
    assert(oSymTable != NULL);

    node1 = oSymTable->first;
    free(oSymTable);
    if (node1 == NULL) return;
    node2 = node1->next;
    while (node2) {
        free((char*)node1->key);
        free(node1);
        node1 = node2;
        node2 = node2->next;
    }
    free(node1);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable->numBindings;
}

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
        if (node == NULL) return 0;
        node->key = (const char*)malloc(strlen(pcKey)+1);
        strcpy((char*)node->key, pcKey);
        node->value = pvValue;
        node->next = oSymTable->first;
        oSymTable->first = node;
        oSymTable->numBindings++;
        return 1;
}

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
            free(node2);
            oSymTable->numBindings--;
            return value;
        }
        node1 = node2;
        node2 = node2->next;
    }
    return NULL;
}

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
