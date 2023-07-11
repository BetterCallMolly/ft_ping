#include "icmp.h"
#include <stdio.h>

bool compute_icmp_checksum(t_icmp_packet *packet)
{
    int32_t sum = checksum((uint8_t *)(&packet->header), sizeof(packet->header));
    packet->header.checksum = sum;
    return true;
}

/**
 * @brief Debug function that prints the content of an ICMP packet to stderr
 * 
 * @note This function asserts the passed packet is not NULL and that it's a ECHO_REQUEST / ECHO_REPLY packet
 * @param packet Packet to disassemble
 */
void disasm_icmp_packet(t_icmp_packet *packet, bool disasm_data)
{
    if (!packet)
        return ;
    if (packet->header.type != ECHO_REQUEST && packet->header.type != ECHO_REPLY)
        return ;
    printf("ICMP packet @ %p:\n", packet);
    printf("    type: 0x%02x\n", packet->header.type);
    printf("    code: 0x%02x\n", packet->header.code);
    printf("    checksum: 0x%04x\n", packet->header.checksum);
    printf("    identifier: 0x%02x 0x%02x\n", packet->header.rest[0], packet->header.rest[1]);
    printf("    seq_number: 0x%02x 0x%02x\n", packet->header.rest[2], packet->header.rest[3]);
    printf("    timestamp: 0x%08x\n", packet->timestamp);
    if (!disasm_data)
        return ;
    printf("    data: ");
    for (size_t i = 0; i < packet->size; i++) {
        printf("0x%02x ", packet->data[i]);
    }
    printf("\n");
}

/**
 * @brief Converts an ICMP packet to a byte array
 * 
 * @param packet Packet to serialize
 * @return uint8_t* Serialized packet, which is allocated on the heap
 * @note The returned pointer must be freed by the caller
 */
uint8_t *serialize_icmp_packet(t_icmp_packet *packet)
{
    if (!packet)
        return NULL;
    uint8_t *buffer = malloc(sizeof(t_icmp_header) + (sizeof(uint32_t) * 2) + packet->size);
    if (!buffer)
        return NULL;
    memcpy(buffer, packet, sizeof(t_icmp_packet));
    return buffer;
}