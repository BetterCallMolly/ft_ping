#include "ft_ping.h"

/**
 * @brief Writes the entire ASCII table to the buffer
 * 
 * @param buffer The buffer to write to
 * @param size Number of bytes to write
 */
void generate_data(uint8_t *buffer, uint32_t size)
{
    if (!buffer)
        return ;

    if (size < 127*3) { // Fallback to a simple incrementing pattern to avoid memcpy overhead
        for (uint32_t i = 0; i < size; i++) {
            buffer[i] = i;
        }
    }

    static uint8_t ascii[127];
    static bool initialized = false;
    if (!initialized) {
        for (uint8_t i = 0; i < 127; i++) {
            ascii[i] = i;
        }
        initialized = true;
    }
    register uint_fast32_t _size = size; // pog optimization
    while (_size > 127) {
        memcpy(buffer, ascii, 127);
        buffer += 127;
        _size -= 127;
    }
    memcpy(buffer, ascii, _size);
}
