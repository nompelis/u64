/* 
   U64 DATABASE
   (C) MARIO EMMANUEL 2024
   record.c
   RECORD RELATED FUNCTIONS
   SEE LICENSE FILE FOR LICENSE DETAILS
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

int create_record(Database* db, uint64_t key, const unsigned char* value, size_t value_size) {

    int i;
    unsigned char* stored_value;

    if (db->size >= db->limit) {
        if (grow_db(db) != 0) {
            return U64_ERROR;
        }
    }

    int index = binary_search_db(db, key);

    if (index < db->size && db->keys[index] == key) {
        return U64_ERROR;
    }

    stored_value = (unsigned char*)malloc(value_size);
    if (stored_value == NULL) {
        return U64_ERROR;
    }
    memcpy(stored_value, value, value_size);

    for (i = db->size; i > index; --i) {
        db->keys[i] = db->keys[i - 1];
        db->values[i] = db->values[i - 1];
        db->value_sizes[i] = db->value_sizes[i - 1];
    }

    db->keys[index] = key;
    db->values[index] = stored_value;
    db->value_sizes[index] = value_size;
    db->size++;
    return U64_OK;
}

const unsigned char* read_record(Database* db, uint64_t key, size_t* value_size) {
    int index = binary_search_db(db, key);
    if (index < db->size && db->keys[index] == key) {
        *value_size = db->value_sizes[index];
        return db->values[index];
    }
    return NULL;
}

int update_record(Database* db, uint64_t key, const unsigned char* value, size_t value_size) {
    int index = binary_search_db(db, key);
    unsigned char* stored_value;

    if (index < db->size && db->keys[index] == key) {
        stored_value = (unsigned char*)malloc(value_size);
        if (stored_value == NULL) {
            return U64_ERROR;
        }
        memcpy(stored_value, value, value_size);
        free(db->values[index]);  /* Free the old value memory */
        db->values[index] = stored_value;
        db->value_sizes[index] = value_size;
        return U64_OK;
    } else {
        return U64_ERROR;
    }
}

int delete_record(Database* db, uint64_t key) {
    int i;
    int index = binary_search_db(db, key);
    if (index < db->size && db->keys[index] == key) {
        free(db->values[index]);  /* Free the value memory */
        for (i = index; i < db->size - 1; ++i) {
            db->keys[i] = db->keys[i + 1];
            db->values[i] = db->values[i + 1];
            db->value_sizes[i] = db->value_sizes[i + 1];
        }
        db->size--;
        return U64_OK;
    } else {
        return U64_ERROR;
    }
}

const unsigned char* read_record_by_index(Database* db, int index, size_t* value_size) {
    if (index >= 0 && index < db->size) {
        *value_size = db->value_sizes[index];
        return db->values[index];
    }
    return NULL;
}
