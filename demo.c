/* 
   U64 DATABASE
   (C) MARIO EMMANUEL 2024
   demo.c
   DEMO FILE
   SEE LICENSE FILE FOR LICENSE DETAILS
*/  

#include "db.h"
#include "persistence.h"
#include "record.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    size_t i;
    
    Database *db = (Database *)malloc(sizeof(Database));
    if (db == NULL || init_database(db) != U64_OK) {
        return 1;
    }
    
    printf("Size before creating item 100: %d\n", getsize_db(db));

    unsigned char value1[] = {0x01, 0x02, 0x03, 0x04};
    if (create_record(db, 100, value1, sizeof(value1)) != U64_OK) {
        return 1;
    }
    
    printf("Size after creating item 100: %d\n", getsize_db(db));
    
    unsigned char value2[] = {0x11, 0x12, 0x13, 0x14};
    if (create_record(db, 200, value2, sizeof(value2)) != U64_OK) {
        return 1;
    }
    
    printf("Size after creating item 200: %d\n", getsize_db(db));

    size_t size;
    const unsigned char* read_value = read_record(db, 100, &size);
    if (read_value == NULL) {
        return 1;
    }
    printf("Read key 100: ");
    for (i = 0; i < size; i++) {
        printf("%02x ", read_value[i]);
    }
    printf("\n");

    unsigned char value3[] = {0x21, 0x22, 0x23};
    if (update_record(db, 100, value3, sizeof(value3)) != U64_OK) {
        return 1;
    }

    read_value = read_record(db, 100, &size);
    if (read_value == NULL) {
        return 1;
    }
    printf("Read key 100 after update: ");
    for (i = 0; i < size; i++) {
        printf("%02x ", read_value[i]);
    }
    printf("\n");

    if (delete_record(db, 100) != U64_OK) {
        return 1;
    }

    printf("Size after delete: %d\n", getsize_db(db));

    const unsigned char* read_index_value = read_record_by_index(db, 0, &size);
    if (read_index_value == NULL) {
        return 1;
    }
    printf("Read by index 0: ");
    for (i = 0; i < size; i++) {
        printf("%02x ", read_index_value[i]);
    }
    printf("\n");

    if (write_db(db, "database.dat") != U64_OK) {
        return 1;
    }

    Database* db2 = (Database *)malloc( sizeof(Database) );
    if (db2 == NULL || init_database(db2) != U64_OK) {
        return 1;
    }
    if (read_db(db2, "database.dat") != U64_OK) {
        return 1;
    }

    printf("Size of db2 after reading from file: %d\n", getsize_db(db2));

    return 0;
}
