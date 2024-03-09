#include "EncodeMessage.h"
#include <bitset>
#include <iostream>


// Default Constructor
EncodeMessage::EncodeMessage() = default;

// Destructor
EncodeMessage::~EncodeMessage() = default;

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }

    return true;
}

int nthFibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;

    int a = 0, b = 1, c;
    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }

    return b;
}

std::string rightCircularShift(const std::string &message) {
    // Calculate the number of positions for the right circular shift
    size_t shiftAmount = message.length() / 2;

    // Create a new string to store the shifted message
    std::string shiftedMessage = message;

    for (size_t i = 0; i < message.length(); ++i) {
        // Calculate the new position after the right circular shift
        size_t newPosition = (i + shiftAmount) % message.length();

        // Copy the character to the new position
        shiftedMessage[newPosition] = message[i];
    }

    return shiftedMessage;
}

// Function to encode a message into an image matrix
ImageMatrix EncodeMessage::encodeMessageToImage(const ImageMatrix &img, const std::string &message,
                                                const std::vector<std::pair<int, int>> &positions) {
    ImageMatrix embeddedImage = img;
    std::string transformedMessage = message;
    const std::vector<std::pair<int, int>>& edgePixels = positions;
    std::string binaryString;

    for (int i = 0; i < message.size(); i++) {
        if (isPrime(i)) {
            int fibonacciNumber = nthFibonacci(i);
            int currentChar = static_cast<int>(static_cast<unsigned char>(message[i]));
            int adjustedChar = currentChar + fibonacciNumber;
            if (adjustedChar >= 127) {
                adjustedChar = 126;
            }
            if (adjustedChar <= 32) {
                adjustedChar += 32;
            }
            transformedMessage[i] = static_cast<char>(adjustedChar);
        }
    }
    std::string shiftedMessage = rightCircularShift(transformedMessage);
    for (auto character: shiftedMessage) {
        int ascii = static_cast<int>(character);
        std::string binaryValue = std::bitset<7>(ascii).to_string();
        binaryString += binaryValue;
    }
    int binaryStringSize = binaryString.size();
    int edgePixelSize = edgePixels.size();
    if (binaryStringSize > edgePixelSize) {
        binaryString = binaryString.substr(0, edgePixelSize);
    }
    for (int i = 0; i < binaryString.size(); ++i) {
        int x = positions[i].first;
        int y = positions[i].second;
        int pixel = embeddedImage.get_data(x, y);
        int lsb = pixel & 1;
        if (lsb) {
            if (binaryString[i] == '0') {
                pixel--;
                embeddedImage.set_data(x, y, pixel);
            }
        }
        if (!lsb) {
            if (binaryString[i] == '1') {
                pixel++;
                embeddedImage.set_data(x, y, pixel);
            }
        }
    }
    return embeddedImage;
}
