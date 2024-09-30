#include "serializer.h"
#include <QTime>
#include <functional> // for std::hash
#include <algorithm> // For std::transform
#include <cctype>    // For std::tolower

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

void save_delay(QTextStream &out, int &sec) {
    if (sec > 0)
    {
        QTime currentTime = QTime::currentTime();
        int delay = currentTime.hour() * 3600 + currentTime.minute() * 60 + currentTime.second() - sec;
        if (delay > 1) {
            out << "," << delay;
        } // else the default 1 sec will be used
        out << endl;
        sec = 0;
    }
}

QString save_command(QString &cmd, QTextStream &out, int &sec) {
    save_delay(out, sec);
    QString rec{""};
    if (const char *tmp = strstr(cmd.toStdString().c_str(), "*")) {
        switch (*(++tmp)) {
        case 'K': rec = "OK"; break;
        case 'r': rec = "RearButton"; break;
        case 'U': rec = "Up"; break;
        case 'D': rec = "Down"; break;
        case 'R': rec = "Right"; break;
        case 'L': rec = "Left"; break;
        default:
            qWarning("wrong format of the SCM command!");
            return rec;
        }
    }
    out << rec;
    QTime currentTime = QTime::currentTime();
    sec = currentTime.hour() * 3600 + currentTime.minute() * 60 + currentTime.second();
    return rec;
}

std::vector<std::string> parseCSV(const std::string& csv) {
    std::vector<std::string> result;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < csv.size(); ++i) {
        char c = csv[i];

        if (c == '"') {
            // Toggle quote status
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            // If we hit a comma and we are not inside quotes, add the field to the result
            result.push_back(field);
            field.clear(); // Start a new field
        } else {
            // Otherwise, add the character to the current field
            field += c;
        }
    }

    // Add the last field
    result.push_back(field);

    return result;
}

QString translate_key_to_cmd(int key) {
    QString str_to_send{"*"};
    switch (key) {
        case Qt::Key_Up:    str_to_send += 'U'; break;
        case Qt::Key_Down:  str_to_send += 'D'; break;
        case Qt::Key_Left:  str_to_send += 'L'; break;
        case Qt::Key_Right: str_to_send += 'R'; break;
        case Qt::Key_Return:
        case Qt::Key_Enter: str_to_send += 'K'; break;
        case Qt::Key_End:   str_to_send += 'r'; break;
        default: return (QString{""});
    }
    return str_to_send;
}

constexpr size_t hashString(const char* str, size_t hash = 14695981039346656037ull) {
    return (*str == '\0') ? hash : hashString(str + 1, (hash ^ static_cast<size_t>(*str)) * 1099511628211ull);
}

std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}

QString translate_script_cmd(std::string input) {
    size_t hashValue = hashString(toLowerCase(input).c_str());

    switch (hashValue) {
    case hashString("u"):
    case hashString("up"):
        return translate_key_to_cmd(Qt::Key_Up);
    case hashString("d"):
    case hashString("down"):
    case hashString("dn"):
        return translate_key_to_cmd(Qt::Key_Down);
    case hashString("l"):
    case hashString("left"):
    case hashString("lt"):
        return translate_key_to_cmd(Qt::Key_Left);
    case hashString("ri"):
    case hashString("right"):
    case hashString("rt"):
        return translate_key_to_cmd(Qt::Key_Right);
    case hashString("k"):
    case hashString("ok"):
        return translate_key_to_cmd(Qt::Key_Enter);
    case hashString("re"):
    case hashString("rb"):
    case hashString("rear"):
    case hashString("rbutton"):
    case hashString("rearbutton"):
        return translate_key_to_cmd(Qt::Key_End);
    default:
        break;
    }
    return QString{""};
}
