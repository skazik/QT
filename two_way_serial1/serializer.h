#ifndef SERIALIZER_H
#define SERIALIZER_H
#include <cstring>
#include "mainwindow.h"

namespace serializer {

constexpr int kVelocityByteArraySize = 20;

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

struct VelocityTarget {
    float velocity;           // = static_cast<float>(action.normalized.y)
    float acceleration;       // = 0.0
    float max_velocity;       // = 1.0
    float max_acceleration;   // = static_cast<float>(bend_acceleration)
};

SerializedMessage serialize_message(QString txt);
void save_delay(QTextStream &out, int &sec);
QString save_command(QString &cmd, QTextStream &out, int &sec);
std::vector<std::string> parseCSV(const std::string& csv);
QString translate_key_to_cmd(int key);
QString translate_script_cmd(std::string input);
void serialize_velocity(uint8_t *result);
std::string deserialize_velocity(const uint8_t *byte_array);
}

#endif // SERIALIZER_H
