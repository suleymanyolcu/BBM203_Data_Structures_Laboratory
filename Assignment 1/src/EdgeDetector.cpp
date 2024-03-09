// EdgeDetector.cpp

#include "EdgeDetector.h"
#include <cmath>


// Default constructor
EdgeDetector::EdgeDetector() : sobeloperator (nullptr),operator_height(0), operator_width(0) {

}

// Destructor
EdgeDetector::~EdgeDetector() {
    if (sobeloperator != nullptr) {
        for (int i = 0; i < operator_height; ++i) {
            delete[] sobeloperator[i];
        }
        delete[] sobeloperator;
    }

}

// Detect Edges using the given algorithm
std::vector<std::pair<int, int>> EdgeDetector::detectEdges(const ImageMatrix &input_image) {
    std::vector<std::pair<int, int>> EdgePixels;
    double sum_magnitude = 0.0;
    const double *Gx[3] = {
            new double[3]{-1.0, 0.0, 1.0},
            new double[3]{-2.0, 0.0, 2.0},
            new double[3]{-1.0, 0.0, 1.0}
    };
    const double *Gy[3] = {
            new double[3]{-1.0, -2.0, -1.0},
            new double[3]{0.0, 0.0, 0.0},
            new double[3]{1.0, 2.0, 1.0}
    };
    ImageMatrix gx_matrix(Gx, 3, 3);
    ImageMatrix gy_matrix(Gy, 3, 3);
    Convolution gx_convolution(gx_matrix.get_data(), 3, 3, 1, true);
    Convolution gy_convolution(gy_matrix.get_data(), 3, 3, 1, true);
    ImageMatrix Ix = gx_convolution.convolve(input_image);
    ImageMatrix Iy = gy_convolution.convolve(input_image);
    for (int i = 0; i < input_image.get_height(); i++) {
        for (int j = 0; j < input_image.get_width(); j++) {
            double magnitude = std::sqrt(std::pow(Ix.get_data(i,j),2)+std::pow(Iy.get_data(i,j),2));
                    sum_magnitude += magnitude;
        }
    }
    double threshold = sum_magnitude / static_cast<double>(input_image.get_height() * input_image.get_width());
    for (int i = 0; i < input_image.get_height(); i++) {
        for (int j = 0; j < input_image.get_width(); j++) {
            double magnitude = std::sqrt(std::pow(Ix.get_data(i, j), 2) + std::pow(Iy.get_data(i, j), 2));
            if (magnitude > threshold) {
                EdgePixels.emplace_back(i, j);
            }
        }
    }
    return EdgePixels;
}

