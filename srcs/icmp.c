#include "icmp.h"
#include <stdio.h>

bool compute_icmp_checksum(t_icmp_packet *packet)
{
    if (!packet)
    
        return false;
    packet->checksum = checksum(serialize_icmp_packet(packet), sizeof(t_icmp_packet) - 4);
    return packet->checksum != 0;
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
    // if (packet->type != ECHO_REQUEST && packet->type != ECHO_REPLY)
    //     return ;
    printf("ICMP packet @ %p:\n", packet);
    printf("    type: 0x%02x\n", packet->type);
    printf("    code: 0x%02x\n", packet->code);
    printf("    checksum: 0x%04x\n", packet->checksum);
    printf("    identifier: 0x%04x\n", packet->identifier);
    printf("    seq_number: 0x%04x\n", packet->sequence_number);
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
    return (uint8_t *)&packet->type; // small hack to return the address of the first byte of the packet
}