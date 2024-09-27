#ifndef SERIALIZER_H
#define SERIALIZER_H
#include <cstring>
#include "mainwindow.h"

// serialization
// serialization
// serialization
static constexpr auto kStartTransmissionHeaderSize = 3U;
static constexpr std::array<uint8_t, kStartTransmissionHeaderSize> kStartTransmissionHeader = {
    {66u, 103u, 110u}};
enum class SerialMessageType : uint8_t {
    Unknown = 0,
    DriveControllerCommand = 1,
    PoseTrajectoryCommand = 2,
    /* ClockSynchronizationRequest = 3,*/
    /* ClockSynchronizationReponse = 4,*/
    ResetErrorsRequest = 5,
    HardwareMechanismsState = 6,
    LifecycleEvent = 7,
    DriveControllerState = 8,
    Log = 9,
    String = 10,
    Msp430StringCommand = 11,
    Msp430Log = 12,
    Msp430UpdatePacket = 13
};
typedef uint8_t HeaderChecksumType;
typedef uint16_t PayloadChecksumType;
typedef uint8_t MessagePayloadType[8];

struct SerializedWireMessageHeader {
    std::array<uint8_t, 3> start_transmission;
    SerialMessageType message_type;
    HeaderChecksumType crc;
};

//union SerializedPayloadType {
//    uint8_t byte_array[8];
//    MessagePayloadType pld;
//};

struct SerializedWireMessage {
    SerializedWireMessageHeader header;
    MessagePayloadType payload;
    PayloadChecksumType payload_crc;
};

union SerializedHeader {
    struct SerializedWireMessageHeader hdr;
    char * byte_array[sizeof(SerializedWireMessageHeader)];
};

union SerializedMessage {
    struct SerializedWireMessage msg;
    char * byte_array[sizeof(SerializedWireMessage)];
};

SerializedMessage serialize_message(QString txt);

#endif // SERIALIZER_H
