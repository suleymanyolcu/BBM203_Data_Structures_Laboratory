// DecodeMessage.cpp

#include "DecodeMessage.h"
// Default constructor
DecodeMessage::DecodeMessage() = default ;
    // Nothing specific to initialize here


// Destructor
DecodeMessage::~DecodeMessage() = default;
    // Nothing specific to clean up



std::string DecodeMessage::decodeFromImage(const ImageMatrix& image, const std::vector<std::pair<int, int>>& edgePixels) {
    std::string binaryString;
    std::string decodedMessage;
    for (const auto& pixel : edgePixels) {
        int pixelValue = image.get_data(pixel.first, pixel.second);
        int lsb = pixelValue & 1; // Get the LSB (rightmost bit)
        binaryString += std::to_string(lsb);
    }
    while (binaryString.size() % 7 != 0) {
        binaryString = "0" + binaryString;
    }
    for (size_t i = 0; i < binaryString.size(); i += 7) {
        std::string segment = binaryString.substr(i, 7);
        int decimalValue = std::stoi(segment, nullptr, 2);
        if (decimalValue <= 32) {
            decimalValue += 33;
        }
        if (decimalValue >= 127) {
            decimalValue = 126;
        }
        char decodedChar = static_cast<char>(decimalValue);
        decodedMessage += decodedChar;
    }
    return decodedMessage;

}

