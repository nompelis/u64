/* 
   U64 DATABASE
   (C) MARIO EMMANUEL 2024
   persistence.c
   PERSITENCE RELATED FUNCTIONS
   SEE LICENSE FILE FOR LICENSE DETAILS
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "db.h"

#define U64_FILE_MAGIC "u64v2"
#define U64_FILE_MAGIC_SIZE 5
#define U64_FILE_VERSION_MAJOR 2
#define U64_FILE_VERSION_MINOR 0
#define U64_FILE_VERSION (((uint16_t)U64_FILE_VERSION_MAJOR << 8) | U64_FILE_VERSION_MINOR)

static void uint16_to_big_endian(uint16_t value, unsigned char bytes[2]) {
    bytes[0] = (unsigned char)((value >> 8) & 0xff);
    bytes[1] = (unsigned char)(value & 0xff);
}

static uint16_t uint16_from_big_endian(const unsigned char bytes[2]) {
    return (uint16_t)(((uint16_t)bytes[0] << 8) |
                      (uint16_t)bytes[1]);
}

static void uint32_to_big_endian(uint32_t value, unsigned char bytes[4]) {
    bytes[0] = (unsigned char)((value >> 24) & 0xff);
    bytes[1] = (unsigned char)((value >> 16) & 0xff);
    bytes[2] = (unsigned char)((value >> 8) & 0xff);
    bytes[3] = (unsigned char)(value & 0xff);
}

static uint32_t uint32_from_big_endian(const unsigned char bytes[4]) {
    return ((uint32_t)bytes[0] << 24) |
           ((uint32_t)bytes[1] << 16) |
           ((uint32_t)bytes[2] << 8) |
           (uint32_t)bytes[3];
}

static void uint64_to_big_endian(uint64_t value, unsigned char bytes[8]) {
    bytes[0] = (unsigned char)((value >> 56) & 0xff);
    bytes[1] = (unsigned char)((value >> 48) & 0xff);
    bytes[2] = (unsigned char)((value >> 40) & 0xff);
    bytes[3] = (unsigned char)((value >> 32) & 0xff);
    bytes[4] = (unsigned char)((value >> 24) & 0xff);
    bytes[5] = (unsigned char)((value >> 16) & 0xff);
    bytes[6] = (unsigned char)((value >> 8) & 0xff);
    bytes[7] = (unsigned char)(value & 0xff);
}

static uint64_t uint64_from_big_endian(const unsigned char bytes[8]) {
    return ((uint64_t)bytes[0] << 56) |
           ((uint64_t)bytes[1] << 48) |
           ((uint64_t)bytes[2] << 40) |
           ((uint64_t)bytes[3] << 32) |
           ((uint64_t)bytes[4] << 24) |
           ((uint64_t)bytes[5] << 16) |
           ((uint64_t)bytes[6] << 8) |
           (uint64_t)bytes[7];
}

static int write_uint32_big_endian(FILE* file, uint32_t value) {
    unsigned char bytes[4];
    uint32_to_big_endian(value, bytes);
    return fwrite(bytes, sizeof(bytes), 1, file) == 1 ? 0 : -1;
}

static int write_uint16_big_endian(FILE* file, uint16_t value) {
    unsigned char bytes[2];
    uint16_to_big_endian(value, bytes);
    return fwrite(bytes, sizeof(bytes), 1, file) == 1 ? 0 : -1;
}

static int write_uint64_big_endian(FILE* file, uint64_t value) {
    unsigned char bytes[8];
    uint64_to_big_endian(value, bytes);
    return fwrite(bytes, sizeof(bytes), 1, file) == 1 ? 0 : -1;
}

static int read_uint16_big_endian(FILE* file, uint16_t* value) {
    unsigned char bytes[2];
    if (fread(bytes, sizeof(bytes), 1, file) != 1) {
        return -1;
    }
    *value = uint16_from_big_endian(bytes);
    return 0;
}

static int read_uint32_big_endian(FILE* file, uint32_t* value) {
    unsigned char bytes[4];
    if (fread(bytes, sizeof(bytes), 1, file) != 1) {
        return -1;
    }
    *value = uint32_from_big_endian(bytes);
    return 0;
}

static int read_uint64_big_endian(FILE* file, uint64_t* value) {
    unsigned char bytes[8];
    if (fread(bytes, sizeof(bytes), 1, file) != 1) {
        return -1;
    }
    *value = uint64_from_big_endian(bytes);
    return 0;
}

static int write_header(FILE* file) {
    if (fwrite(U64_FILE_MAGIC, U64_FILE_MAGIC_SIZE, 1, file) != 1) {
        return -1;
    }
    return write_uint16_big_endian(file, U64_FILE_VERSION);
}

static int read_header(FILE* file) {
    char magic[U64_FILE_MAGIC_SIZE];
    uint16_t version;

    if (fread(magic, U64_FILE_MAGIC_SIZE, 1, file) != 1) {
        return -1;
    }
    if (memcmp(magic, U64_FILE_MAGIC, U64_FILE_MAGIC_SIZE) != 0) {
        return -1;
    }
    if (read_uint16_big_endian(file, &version) != 0) {
        return -1;
    }
    if (version != U64_FILE_VERSION) {
        return -1;
    }
    return 0;
}

static void clear_loaded_values(Database* db, int count) {
    int i;

    for (i = 0; i < count; ++i) {
        free(db->values[i]);
        db->values[i] = NULL;
        db->value_sizes[i] = 0;
    }
    db->size = 0;
}

int read_db(Database* db, const char* filename) {
    int i;
    int loaded_values;
    uint32_t size;
    uint64_t value_size;
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return U64_ERROR;
    }

    if (read_header(file) != 0) {
        fclose(file);
        return U64_ERROR;
    }
    if (read_uint32_big_endian(file, &size) != 0) {
        fclose(file);
        return U64_ERROR;
    }
    if (size > INT_MAX) {
        fclose(file);
        return U64_ERROR;
    }
    while (db->limit < (int)size) {
        if (grow_db(db) != 0) {
            fclose(file);
            return U64_ERROR;
        }
    }
    db->size = 0;
    for (i = 0; i < (int)size; ++i) {
        if (read_uint64_big_endian(file, &db->keys[i]) != 0) {
            fclose(file);
            return U64_ERROR;
        }
    }
    for (i = 0; i < (int)size; ++i) {
        if (read_uint64_big_endian(file, &value_size) != 0) {
            fclose(file);
            return U64_ERROR;
        }
        if ((uint64_t)((size_t)value_size) != value_size) {
            fclose(file);
            return U64_ERROR;
        }
        db->value_sizes[i] = (size_t)value_size;
    }

    loaded_values = 0;
    for (i = 0; i < (int)size; ++i) {
        db->values[i] = (unsigned char*)malloc(db->value_sizes[i]);
        if (db->values[i] == NULL) {
            clear_loaded_values(db, loaded_values);
            fclose(file);
            return U64_ERROR;
        }
        if (fread(db->values[i], sizeof(unsigned char), db->value_sizes[i], file) != db->value_sizes[i]) {
            loaded_values++;
            clear_loaded_values(db, loaded_values);
            fclose(file);
            return U64_ERROR;
        }
        loaded_values++;
    }

    db->size = (int)size;
    fclose(file);
    return U64_OK;
}

int write_db(Database* db, const char* filename) {
    int i;
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return U64_ERROR;
    }

    if (db->size < 0) {
        fclose(file);
        return U64_ERROR;
    }
    if (write_header(file) != 0) {
        fclose(file);
        return U64_ERROR;
    }
    if (write_uint32_big_endian(file, (uint32_t)db->size) != 0) {
        fclose(file);
        return U64_ERROR;
    }
    for (i = 0; i < db->size; ++i) {
        if (write_uint64_big_endian(file, db->keys[i]) != 0) {
            fclose(file);
            return U64_ERROR;
        }
    }
    for (i = 0; i < db->size; ++i) {
        if ((size_t)((uint64_t)db->value_sizes[i]) != db->value_sizes[i]) {
            fclose(file);
            return U64_ERROR;
        }
        if (write_uint64_big_endian(file, (uint64_t)db->value_sizes[i]) != 0) {
            fclose(file);
            return U64_ERROR;
        }
    }

    for (i = 0; i < db->size; ++i) {
        if (fwrite(db->values[i], sizeof(unsigned char), db->value_sizes[i], file) != db->value_sizes[i]) {
            fclose(file);
            return U64_ERROR;
        }
    }

    fclose(file);
    return U64_OK;
}
