/* 
   U64 DATABASE
   (C) MARIO EMMANUEL 2024
   db.c
   DATABASE RELATED FUNCTIONS
   SEE LICENSE FILE FOR LICENSE DETAILS
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

void init_database(Database* db) {
    db->size = 0;
    db->limit = U64_INITIAL_CAPACITY;
    db->keys = (unsigned long *)malloc(sizeof(unsigned long)*db->limit);
    db->values = (unsigned char **)malloc(sizeof(unsigned char *)*db->limit);
    db->value_sizes = (size_t *)malloc(sizeof(size_t)*db->limit);
    if(db->keys == NULL || db->values == NULL || db->value_sizes == NULL) {
        fprintf(stderr, "ERROR init_database: Can not allocate memory");
        exit(-1);
    }
}

int getlimit_db(Database* db) {
    return db->limit;
}

int getsize_db(Database* db) {
    return db->size;
}

int grow_db(Database* db) {
    int new_limit;
    unsigned long *new_keys;
    unsigned char **new_values;
    size_t *new_value_sizes;

    new_limit = db->limit * 2;
    if (new_limit < U64_INITIAL_CAPACITY) {
        new_limit = U64_INITIAL_CAPACITY;
    }

    new_keys = (unsigned long *)realloc(db->keys, sizeof(unsigned long)*new_limit);
    if (new_keys == NULL) {
        return -1;
    }
    db->keys = new_keys;

    new_values = (unsigned char **)realloc(db->values, sizeof(unsigned char *)*new_limit);
    if (new_values == NULL) {
        return -1;
    }
    db->values = new_values;

    new_value_sizes = (size_t *)realloc(db->value_sizes, sizeof(size_t)*new_limit);
    if (new_value_sizes == NULL) {
        return -1;
    }
    db->value_sizes = new_value_sizes;
    db->limit = new_limit;

    return 0;
}

int binary_search_db(Database* db, unsigned long key) {
    int low = 0, high = db->size - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (db->keys[mid] == key) {
            return mid;
        } else if (db->keys[mid] < key) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return low;
}
