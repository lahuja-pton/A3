/*--------------------------------------------------------------------*/
/* strp.c                                                             */
/* Author: Lakshit Ahuja                                              */
/* Implements a symbol table through a hash table.                    */
/*--------------------------------------------------------------------*/

#include "symtable.h"

/* array of all possible capacities of the hash table */
static const size_t SIZES[] = 
    {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};
/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount);
/* Expands the oSymTable capacity to the next size according to the 
   SIZES array, rehashing all bindings in the process */
static void SymTable_expand(SymTable_T oSymTable);

/* struct to represent hash table for symbol table ADT */
struct SymTable {
    /* index of the number of buckets / capacity of bindings in SIZES
       array */
    int iCapacity;
    /* number of bindings currently in the table */
    size_t numBindings;
    /* pointer to array of Binding pointers representing the buckets 
       of the hash table */
    struct Binding** buckets;
};
/* struct to represent a binding of the table */
struct Binding {
    /* key of the binding */
    const char* key; 
    /* value of the binding */
    const void* value; 
    /* pointer to next binding in the list */
    struct Binding* next;
};

/* returns new instance of the symbol table */
SymTable_T SymTable_new(void) {
    SymTable_T table = (SymTable_T)calloc(1, sizeof(struct SymTable));
    if (!table) return NULL;
    table->iCapacity = 0;
    table->buckets = (struct Binding**)
        calloc(SIZES[0], sizeof(struct Binding*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }
    return table;
}

/* frees all memory held by oSymTable */
void SymTable_free(SymTable_T oSymTable) {
    struct Binding *bind1, *bind2;
    size_t idx;
    assert(oSymTable != NULL);

    for (idx = 0; idx < SIZES[oSymTable->iCapacity]; idx++) {
        for (bind1 = oSymTable->buckets[idx]; bind1; bind1 = bind2) {
            bind2 = bind1->next;
            free((char*)bind1->key);
            free(bind1);
        }
    }
    free(oSymTable->buckets);
    free(oSymTable);
}

/* returns number of bindings held by oSymTable */
size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable->numBindings;
}

/* inserts new binding if the binding with key pcKey is not already 
   present in oSymTable. Otherwise, updates the value of the existing 
   binding to pvValue. Returns whether a new binding was created. */
int SymTable_put(SymTable_T oSymTable,
const char *pcKey, const void *pvValue) {
    struct Binding* bind;
    size_t numBuckets, hash;
    assert(oSymTable != NULL && pcKey);

    numBuckets = SIZES[oSymTable->iCapacity];
    hash = SymTable_hash(pcKey, numBuckets);
    for (bind = oSymTable->buckets[hash]; bind; bind = bind->next) {
        if (strcmp(bind->key, pcKey) == 0) return 0;
    }
    if (oSymTable->numBindings >= numBuckets)
        SymTable_expand(oSymTable);

    hash = SymTable_hash(pcKey, SIZES[oSymTable->iCapacity]);
    bind = (struct Binding*)calloc(1, sizeof(struct Binding));
    if (!bind) return 0;
    bind->key = (const char*)malloc(strlen(pcKey)+1);
    if (!bind->key) {
        free(bind);
        return 0;
    }
    strcpy((char*)bind->key, pcKey);
    bind->value = pvValue;
    bind->next = oSymTable->buckets[hash];
    oSymTable->buckets[hash] = bind;
    oSymTable->numBindings++;

    return 1;
}

/* updates value of binding associated with key pcKey to pvValue if 
   present in oSymTable */
void *SymTable_replace(SymTable_T oSymTable,
const char *pcKey, const void *pvValue) {
    struct Binding* bind;
    void* value;
    size_t hash;
    assert(oSymTable != NULL && pcKey);

    hash = SymTable_hash(pcKey, SIZES[oSymTable->iCapacity]);
    for (bind = oSymTable->buckets[hash]; bind; bind = bind->next) {
        if (strcmp((char*)bind->key, pcKey) == 0) {
            value = (void*)bind->value;
            bind->value = pvValue;
            return value;
        }
    }
    return NULL;
}

/* returns whether oSymTable holds a binding with key pcKey */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct Binding* bind;
    size_t hash;
    assert(oSymTable != NULL && pcKey);

    hash = SymTable_hash(pcKey, SIZES[oSymTable->iCapacity]);
    for (bind = oSymTable->buckets[hash]; bind; bind = bind->next) {
        if (strcmp((char*)bind->key, pcKey) == 0) return 1;
    }
    return 0;
}

/* returns the value associated with key pcKey if present in 
   oSymTable */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    struct Binding* bind;
    size_t hash;
    assert(oSymTable != NULL && pcKey);

    hash = SymTable_hash(pcKey, SIZES[oSymTable->iCapacity]);
    for (bind = oSymTable->buckets[hash]; bind; bind = bind->next) {
        if (strcmp((char*)bind->key, pcKey) == 0) 
            return (void*)bind->value;
    }
    return NULL;
}

/* removes binding associated with key pcKey if present in oSymTable */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    struct Binding *bind1, *bind2;
    void* value;
    size_t hash;
    assert(oSymTable != NULL && pcKey);

    bind1 = NULL;
    hash = SymTable_hash(pcKey, SIZES[oSymTable->iCapacity]);
    for (bind2 = oSymTable->buckets[hash]; bind2; bind2 = bind2->next) {
        if (strcmp(bind2->key, pcKey) == 0) {
            if (!bind1) oSymTable->buckets[hash] = bind2->next;
            else bind1->next = bind2->next;
            value = (void*)bind2->value;
            free((char*)bind2->key);
            free(bind2);
            oSymTable->numBindings--;
            return value;
        }
        bind1 = bind2;
    }
    return NULL;
}

/* runs pfApply with extra arguments pvExtra on all bindings within 
   oSymTable */
void SymTable_map(SymTable_T oSymTable, 
void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
const void *pvExtra) {
    struct Binding* bind;
    size_t idx = 0;
    assert(oSymTable != NULL && pfApply != NULL);

    while (idx < SIZES[oSymTable->iCapacity]) {
        for (bind = oSymTable->buckets[idx]; bind; bind = bind->next) 
            (*pfApply)(bind->key, (char*)bind->value, (void*)pvExtra);
        idx++;
    }
}

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

/* Expands the oSymTable capacity to the next size according to the 
   SIZES array, rehashing all bindings in the process */
static void SymTable_expand(SymTable_T oSymTable) {
    struct Binding *bind1, *bind2, **buckets;
    size_t hash, idx = 0, numBuckets;
    assert(oSymTable != NULL);

    int sizeLen = (int)(sizeof(SIZES) / sizeof(SIZES[0]) - 1)
    if (oSymTable->iCapacity == sizeLen) return;
    numBuckets = SIZES[oSymTable->iCapacity+1];
    buckets = (struct Binding**)
        calloc(numBuckets, sizeof(struct Binding*));
    if (!buckets) return;

    while (idx < SIZES[oSymTable->iCapacity]) {
        for (bind1 = oSymTable->buckets[idx]; bind1; bind1 = bind2) {
            bind2 = bind1->next; 
            hash = SymTable_hash(bind1->key, numBuckets);
            bind1->next = buckets[hash];
            buckets[hash] = bind1;
        }
        idx++;
    }

    free(oSymTable->buckets);
    oSymTable->buckets = buckets;
    oSymTable->iCapacity++;
}