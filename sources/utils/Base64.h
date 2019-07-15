#ifndef _BASE_64
#define _BASE_64

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "StringEx.h"

class Base64
{
public:
    Base64();
    void EncodeBase64(const unsigned char *data,  size_t input_length,  unsigned long &output_length, std::string &encodedString);
    unsigned char *DecodeBase64(const char *data,  size_t input_length,  unsigned long &output_length);
private:
    void DeallocateDecodingTable();
};

#endif
