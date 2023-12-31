#include "icmp.h"
#include <stdio.h>

bool compute_icmp_checksum(t_icmp_packet *packet)
{
    if (!packet)
        return false;
    packet->checksum = checksum(serialize_icmp_packet(packet), EMPTY_PACKET_SIZE + packet->size);
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
    if (packet->type != ECHO_REQUEST && packet->type != ECHO_REPLY)
        return ;
    printf("ICMP packet @ %p:\n", packet);
    printf("    type: 0x%02x\n", packet->type);
    printf("    code: 0x%02x\n", packet->code);
    printf("    checksum: 0x%04x\n", packet->checksum);
    printf("    identifier: 0x%04x\n", packet->identifier);
    printf("    seq_number: 0x%04x\n", packet->sequence_number);
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

/**
 * @brief Checks if the received checksum is valid
 * 
 * @param packet Received packet
 * @return True if the received checksum is equal to the computed checksum, false otherwise
 */
bool compare_checksum(t_icmp_packet *packet)
{
    uint16_t received_checksum = packet->checksum;
    packet->checksum = 0;
    compute_icmp_checksum(packet);
    if (received_checksum != packet->checksum) {
        fprintf(stderr, "ft_ping: warning: remote host returned an invalid checksum (got: %#04x, expected: %#04x)\n", received_checksum, packet->checksum);
        return false;
    }
    return true;
}

t_icmp_packet generate_base_packet(uint32_t size)
{
    t_icmp_packet packet = {0};
    packet.type = ECHO_REQUEST;
    packet.code = 0;
    packet.identifier = get_echo_identifier();
    generate_data(packet.data, size);
    packet.size = size;
    return packet;
}