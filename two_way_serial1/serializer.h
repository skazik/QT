#ifndef SERIALIZER_H
#define SERIALIZER_H
#include <cstring>
#include "mainwindow.h"

namespace serializer {

constexpr int kVelocityByteArraySize = 4;

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
    Msp430UpdatePacket = 13,
    SerialCommand = 14
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
    float velocity;           // range [-5.0..5.0]
    float acceleration;       // range [-5.0..5.0]
    float max_velocity;       // range [-5.0..5.0]
    float max_acceleration;   // range [-5.0..5.0]
};

struct serialized_float {
    uint8_t reserved : 1; // reserved for for alignment and future use
    uint8_t sign     : 1; // if set to 1 then negative, othervise positive
    uint8_t integer  : 3; // range [0..7]
    uint8_t fraction : 3; // range [0..7]
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
