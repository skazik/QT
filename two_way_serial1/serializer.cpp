#include "serializer.h"

uint8_t calculate_crc_8(const std::vector<uint8_t>& data) {
    uint8_t crc = 0x00;  // Initial value
    uint8_t polynomial = 0x07;  // Standard CRC-8 polynomial: x^8 + x^2 + x^1 + 1

    for (auto byte : data) {
        crc ^= byte;  // XOR the byte into the CRC
        for (int i = 0; i < 8; ++i) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

static uint16_t calculate_crc_16(const std::vector<uint8_t>& data) {
    static constexpr uint16_t POLYNOMIAL = 0x8005;
    static constexpr uint16_t INITIAL_VALUE = 0xFFFF;
    uint16_t crc = INITIAL_VALUE;

    for (uint8_t byte : data) {
        crc ^= static_cast<uint16_t>(byte) << 8;

        for (int i = 0; i < 8; ++i) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

std::vector<uint8_t>
serialize_header(const SerializedWireMessageHeader& header) {
    std::vector<uint8_t> data;
    for (auto byte : header.start_transmission) data.insert(data.end(),byte);
    data.insert(data.end(), (uint8_t)header.message_type);
    return data;
}

inline uint8_t
calculate_header_crc(const SerializedWireMessageHeader& header) noexcept {
    std::vector<uint8_t> data = serialize_header(header);
    return calculate_crc_8(data);
}

inline uint16_t
calculate_message_crc(MessagePayloadType payload) noexcept {
    std::vector<uint8_t> data;
    for (int i = 0; i < (int) sizeof(MessagePayloadType); i++)
        data.insert(data.end(),payload[i]);
    return calculate_crc_16(data);
}

inline SerializedWireMessageHeader
make_header(SerialMessageType message_type) noexcept {
    auto header = SerializedWireMessageHeader{};
    header.start_transmission = kStartTransmissionHeader;
    header.message_type = message_type;
    header.crc = calculate_header_crc(header);
    return header;
}

SerializedMessage serialize_message(QString txt) {
    SerializedMessage msg;
    SerializedHeader hdr = {make_header(SerialMessageType::String)};
    msg.msg.header = hdr.hdr;
    memset(&msg.msg.payload, 0, sizeof (MessagePayloadType));
    std::strncpy((char*) &msg.msg.payload[0],
            txt.toStdString().c_str(),
            sizeof (MessagePayloadType));
    msg.msg.payload_crc = calculate_message_crc(msg.msg.payload);
    return msg;
}
