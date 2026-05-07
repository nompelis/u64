/* 
   U64 DATABASE
   (C) MARIO EMMANUEL 2024
   db.h
   DATABASE HEADER FILE
   SEE LICENSE FILE FOR LICENSE DETAILS
*/  

#ifndef _U64_DATABASE_H
#define _U64_DATABASE_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t *keys;
    unsigned char **values;
    size_t *value_sizes;
    int size;
    int limit;
} Database;

#define U64_INITIAL_CAPACITY 32
#define U64_OK 0
#define U64_ERROR -1

int init_database(Database* db);
int getlimit_db(Database* db);
int getsize_db(Database* db);
int binary_search_db(Database* db, uint64_t key);
int grow_db(Database* db);

#endif 
