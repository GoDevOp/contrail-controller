/*
 * Copyright (c) 2013 Juniper Networks, Inc. All rights reserved.
 */

#ifndef ctrlplane_parse_object_h
#define ctrlplane_parse_object_h
#include <stdint.h>
#include <string>
#include <string.h>
#include <map>
#ifndef __APPLE__
#include <endian.h>
#endif

static inline uint32_t get_short(const uint8_t *data) {
    uint32_t value = *data++;
    value <<= 8;
    value += *data;
    return value;
}

static inline uint64_t get_value_unaligned(const uint8_t *data, int size) {
    uint64_t value = 0;
    for (int i = 0; i < size; ++i) {
        if (i) value <<= 8;
        value += *data++;
    }
    return value;
}

static inline uint64_t get_value(const uint8_t *data, int size) {
    uint64_t value = -1;
    if (size == 1) {
        value = data[0];
    } else if (size == 2) {
        value = get_short(data);
    } else if ((size == 4) || (size == 8)) {
        value = 0;
        for (int i = 0; i < size; ++i) {
            if (i) value <<= 8;
            value += *data++;
        }
    }
    return value;
}

static inline void put_value(uint8_t *data, int size, uint64_t value) {
    for (int i = 0; i < size; i++) {
        int offset = (size - (i + 1)) * 8;
        if (offset >= (int)sizeof(value)*8) {
            *data++ = 0;
        } else {
            *data++ = ((value >> offset) & 0xff);
        }
    }
}

static inline double get_double(const uint8_t *data) {
    uint64_t *pp = (uint64_t *)data;
#ifndef __APPLE__
    uint64_t re = be64toh(*pp);
#else
    uint64_t re = *pp
#endif
    double *dp = (double *)&re;
    return *dp;
}

static inline void put_double(uint8_t *data, double value) {
    uint64_t *pp = (uint64_t *)data;
#ifndef __APPLE__
    *pp = htobe64(*((uint64_t *)&value));
#else
    *pp = *((uint64_t *)&value);
#endif

}

struct ParseErrorContext {
    ParseErrorContext() : error_code(0), error_subcode(0), data(NULL),
            data_size(0) {}
    int error_code;
    int error_subcode;
    std::string type_name;
    const uint8_t *data;
    int data_size;
};

class EncodeOffsets {
public:
    void SaveOffset(std::string, int);
    int FindOffset(const char *);
private:
    std::map<std::string, int> offsets_;
};

class ParseObject {
public:
    virtual ~ParseObject() { }
};

#endif
