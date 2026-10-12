#include "symtable.h"

struct SymTable {size_t size; struct Node* first;};
struct Node {const char* key; const void* value; struct Node* next;};

SymTable_T SymTable_new(void) {
    return (SymTable_T)calloc(1, sizeof(SymTable_T));
}

void SymTable_free(SymTable_T oSymTable) {
    struct Node *p, *q;
    assert(oSymTable);

    p = oSymTable->first;
    free(oSymTable);
    if (p == NULL) return;
    q = p->next;
    while (q) {
        free((char*)p->key);
        free(p);
        p = q;
        q = q->next;
    }
    free(p);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable);
    return oSymTable->size;
}

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {
        struct Node* p;
        assert(oSymTable);

        p = oSymTable->first;
        while (p) {
            if (strcmp(p->key, pcKey) == 0) return 0;
            p = p->next;
        }
        p = (struct Node*)calloc(1, sizeof(struct Node));
        p->key = (const char*)malloc(strlen(pcKey)+1);
        strcpy((char*)p->key, pcKey);
        p->value = pvValue;
        p->next = oSymTable->first;
        oSymTable->first = p;
        oSymTable->size++;
        return 1;
}

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {
        struct Node* p;
        assert(oSymTable);

        p = oSymTable->first;
        while (p) {
            if (strcmp((char*)p->key, pcKey) == 0) {
                void* value = (void*)p->value;
                p->value = pvValue;
                return value;
            }
            p = p->next;
        }
        return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct Node* p;
    assert(oSymTable);

    p = oSymTable->first;
    while (p) {
        if (strcmp((char*)p->key, pcKey) == 0) return 1;
        p = p->next;
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    struct Node* p;
    assert(oSymTable);

    p = oSymTable->first;
    while (p) {
        if (strcmp((char*)p->key, pcKey) == 0) return (void*)p->value;
        p = p->next;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    struct Node *p = NULL, *q;
    assert(oSymTable);

    q = oSymTable->first;
    while (q) {
        if (strcmp(q->key, pcKey) == 0) {
            if (p == NULL) oSymTable->first = NULL;
            else p->next = q->next;
            void* value = q->value;
            free(q);
            oSymTable->size--;
            return value;
        }
        p = q;
        q = q->next;
    }
    return NULL;
}

void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
        struct Node* p;
        assert(oSymTable);

        p = oSymTable->first;
        while (p) {
            (*pfApply)(p->key, p->value, pvExtra);
            p = p->next;
        }
}
