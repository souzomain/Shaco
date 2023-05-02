#ifndef _BASE64_H
#define _BASE64_H

#include "Defines.h"

char* base64_encode(const unsigned char* data, size_t input_length, size_t* output_length);
unsigned char* base64_decode(const char* encoded_data, size_t input_length, size_t* output_length);

#endif
