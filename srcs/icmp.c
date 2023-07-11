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
 * @param packet Packet to disassemble
 */
void disasm_icmp_packet(t_icmp_packet *packet, bool disasm_data)
{
    printf("ICMP packet @ %p:\n", packet);
    printf("    type: 0x%02x\n", packet->header.type);
    printf("    code: 0x%02x\n", packet->header.code);
    printf("    checksum: 0x%04x\n", packet->header.checksum);
    printf("    rest: 0x%02x 0x%02x 0x%02x 0x%02x\n", packet->header.rest[0], packet->header.rest[1], packet->header.rest[2], packet->header.rest[3]);
    printf("    timestamp: 0x%08x\n", packet->timestamp);
    if (!disasm_data)
        return ;
    printf("    data: ");
    for (size_t i = 0; i < MAX_SIZE; i++) {
        printf("0x%02x ", packet->data[i]);
    }
    printf("\n");
}