#include "Base64.h"
#include "shaco_stdlib.h"

#define BASE64_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

char* base64_encode(const unsigned char* data, size_t input_length, size_t* output_length) {
    size_t encoded_length = 4 * ((input_length + 2) / 3);
    char* encoded_data = (char*)shaco_malloc(encoded_length + 1);

    if (encoded_data == NULL) 
        return NULL;
    

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = BASE64_CHARS[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = BASE64_CHARS[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = BASE64_CHARS[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = BASE64_CHARS[(triple >> 0 * 6) & 0x3F];
    }

    for (size_t i = 0; i < (3 - input_length % 3) % 3; i++) {
        encoded_data[encoded_length - 1 - i] = '=';
    }

    encoded_data[encoded_length] = '\0';

    if (output_length != NULL) {
        *output_length = encoded_length;
    }

    return encoded_data;
}

unsigned char* base64_decode(const char* encoded_data, size_t input_length, size_t* output_length) {
    size_t decoded_length = (input_length / 4) * 3;
    if (input_length > 0 && encoded_data[input_length - 1] == '=') {
        decoded_length--;
    }
    if (input_length > 1 && encoded_data[input_length - 2] == '=') {
        decoded_length--;
    }

    unsigned char* decoded_data = (unsigned char*)shaco_malloc(decoded_length + 1);
    if (decoded_data == NULL) 
        return NULL;
    

    unsigned char b[4];
    for (size_t i = 0, j = 0; i < input_length;) {
        for (size_t k = 0; k < 4; k++) {
            b[k] = 0;
            while (i < input_length && b[k] == 0) {
                b[k] = (unsigned char)encoded_data[i++];
                b[k] = (unsigned char)((b[k] < 43 || b[k] > 122) ? 0 : (unsigned char)BASE64_CHARS[b[k] - 43]);
                if (b[k]) {
                    b[k] = (unsigned char)((b[k] == '+') ? 62 : ((b[k] == '/') ? 63 : ((b[k] == '=') ? 0 : b[k] - 62)));
                }
            }
        }

        decoded_data[j++] = (unsigned char)((b[0] << 2) + ((b[1] & 0x30) >> 4));
        decoded_data[j++] = (unsigned char)(((b[1] & 0x0F) << 4) + ((b[2] & 0x3C) >> 2));
        decoded_data[j++] = (unsigned char)(((b[2] & 0x03) << 6) + b[3]);
    }

    decoded_data[decoded_length] = '\0';

    if (output_length != NULL) 
        *output_length = decoded_length;
    

    return decoded_data;
}
