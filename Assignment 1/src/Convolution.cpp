#include "Convolution.h"

// Default constructor 
Convolution::Convolution() : kernel(nullptr), kernelHeight(0), kernelWidth(0), stride(1), padding(true) {
}

// Parametrized constructor for custom kernel and other parameters
Convolution::Convolution(double** customKernel, int kh, int kw, int stride_val, bool pad) : kernelHeight(kh), kernelWidth(kw), stride(stride_val), padding(pad){
    kernel = new double *[kernelHeight];
    for (int i = 0; i < kernelHeight; ++i){
        kernel[i] = new double [kernelWidth];
    }
    for (int i = 0; i < kernelHeight; ++i) {
        for (int j = 0; j < kernelWidth; j++) {
            kernel[i][j] = customKernel[i][j];
        }
    }
}

// Destructor
Convolution::~Convolution() {
    if(kernel!= nullptr){
        for(int i =0; i < kernelHeight; ++i){
            delete[] kernel[i];
        }
        delete[] kernel;
    }
}

// Copy constructor
Convolution::Convolution(const Convolution &other): kernelHeight(other.kernelHeight), kernelWidth(other.kernelWidth){
    kernel = new double*[kernelHeight];
    for (int i = 0; i < kernelHeight; ++i) {
        kernel[i] = new double[kernelWidth];
        for (int j = 0; j < kernelWidth; ++j) {
            kernel[i][j] = other.kernel[i][j];
        }
    }
}

// Copy assignment operator
Convolution& Convolution::operator=(const Convolution &other) {
    if (this == &other) {
        return *this; // self-assignment check
    }
    if (kernel != nullptr) {
        for (int i = 0; i < kernelHeight; ++i) {
            delete[] kernel[i];
        }
        delete[] kernel;
    }
    kernelHeight = other.kernelHeight;
    kernelWidth = other.kernelWidth;
    kernel = new double*[kernelHeight];
    for (int i = 0; i < kernelHeight; ++i) {
        kernel[i] = new double[kernelWidth];
        for (int j = 0; j < kernelWidth; ++j) {
            kernel[i][j] = other.kernel[i][j];
        }
    }
    return *this;
}


// Convolve Function: Responsible for convolving the input image with a kernel and return the convolved image.
ImageMatrix Convolution::convolve(const ImageMatrix& input_image) const {
    int inputHeight = input_image.get_height();
    int inputWidth = input_image.get_width();
    int outputHeight = (inputHeight - kernelHeight + (padding ? 2 : 0)) / stride + 1;
    int outputWidth = (inputWidth - kernelWidth + (padding ? 2 : 0)) / stride + 1;
    ImageMatrix output_image(outputHeight, outputWidth);
    for (int outputRow = 0; outputRow < outputHeight; outputRow++) {
        for (int outputCol = 0; outputCol < outputWidth; outputCol++) {

            int startRow = outputRow * stride - (padding ? 1 : 0);
            int startCol = outputCol * stride - (padding ? 1 : 0);

            double sum = 0.0;


            for (int kernelRow = 0; kernelRow < kernelHeight; kernelRow++) {
                for (int kernelCol = 0; kernelCol < kernelWidth; kernelCol++) {
                    // Check if the kernel position is within the input image boundaries
                    int inputRow = startRow + kernelRow;
                    int inputCol = startCol + kernelCol;
                    if (inputRow >= 0 && inputRow < inputHeight && inputCol >= 0 && inputCol < inputWidth) {
                        // Multiply the kernel value with the corresponding input image value
                        sum += kernel[kernelRow][kernelCol] * input_image.get_data(inputRow, inputCol);
                    }
                }
            }
            output_image.set_data(outputRow, outputCol, sum);
        }
    }

    return output_image;
}
