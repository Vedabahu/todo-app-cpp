#include "uuid.hpp"
#include <iomanip>
#include <random>
#include <sstream>

std::string generate_uuid()
{
    thread_local static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<> dis(0, 255);

    unsigned char bytes[16];

    for (int i = 0; i < 16; ++i)
    {
        bytes[i] = static_cast<unsigned char>(dis(gen));
    }

    // Set version (4) and variant bits
    bytes[6] = (bytes[6] & 0x0F) | 0x40; // Version 4
    bytes[8] = (bytes[8] & 0x3F) | 0x80; // Variant

    std::stringstream ss;

    for (int i = 0; i < 16; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);

        if (i == 3 || i == 5 || i == 7 || i == 9)
            ss << "-";
    }

    return ss.str();
}