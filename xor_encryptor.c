#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// Data conversion processing
void transform_buffer(unsigned char *buffer, size_t buffer_size, const char *salt) {
    size_t salt_size = 0;
    const char *temp = salt;
    while (*temp++) salt_size++;

    // Process in reverse order
    size_t end = buffer_size;
    while (end > 0) {
        end--;
        buffer[end] ^= salt[end % salt_size];
    }
}

// Output in hexadecimal format
void display_hex_format(unsigned char *bytes, size_t total) {
    char *output = (char*)malloc(total * 4 + 100);
    if (!output) return;

    char *ptr = output;
    *ptr++ = '"';

    size_t pos = 0;
    do {
        sprintf(ptr, "\\x%02X", bytes[pos]);
        ptr += 4;
        pos++;

        if (pos % 16 == 0 && pos < total) {
            *ptr++ = '"';
            *ptr++ = '\n';
            *ptr++ = '"';
        }
    } while (pos < total);

    *ptr++ = '"';
    *ptr++ = '\n';
    *ptr = '\0';

    printf("%s", output);
    free(output);
}

int main() {
    // ::::: Input your shellcode bytes here ::::::
    unsigned char payload[] = "";
	// ::::: Input your shellcode bytes here ::::::
	
	
    const char *cipher_key = "mick3y";

    // Length calculation
    size_t payload_length = 0;
    unsigned char *counter = payload;
    while (counter < payload + sizeof(payload) - 1) {
        payload_length++;
        counter++;
    }

    unsigned char *work_buffer = (unsigned char*)malloc(payload_length);
    if (work_buffer) {
        memcpy(work_buffer, payload, payload_length);
        transform_buffer(work_buffer, payload_length, cipher_key);

        // Output the results
        printf("\nTransformed payload:\n");
        display_hex_format(work_buffer, payload_length);

        free(work_buffer);
    }

    return 0;
}