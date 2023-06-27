

uint8_t gen4_checksum(uint8_t *buffer, uint8_t length) {
    uint16_t temp;
    uint8_t checksum = 0;
    for (temp = 0; temp < length; temp++)
        checksum += *(buffer + temp);
}