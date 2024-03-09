#include "ImageSharpening.h"

// Default constructor
ImageSharpening::ImageSharpening() : kernel_height(3),kernel_width(3) {
    blurring_kernel = new double *[kernel_height];
    for (int i = 0; i < kernel_height; ++i){
        blurring_kernel[i] = new double [kernel_width];
    }
    for (int i = 0; i < kernel_height; ++i) {
        for (int j = 0; j < kernel_width; j++) {
            blurring_kernel[i][j] = static_cast<double>(1.0/9.0);
        }
    }

}

ImageSharpening::~ImageSharpening() {
    if(blurring_kernel != nullptr){
        for(int i =0; i < kernel_height; ++i){
            delete[] blurring_kernel[i];
        }
        delete[] blurring_kernel;
    }


}

ImageMatrix ImageSharpening::sharpen(const ImageMatrix &input_image, double k) {
    ImageSharpening kernel;
    Convolution blurring(blurring_kernel,
                         kernel_height,kernel_width,1,true);
    ImageMatrix BlurredImg = blurring.convolve(input_image);
    ImageMatrix SharpImg = input_image+(input_image-BlurredImg)*k;
    int height = SharpImg.get_height();
    int width = SharpImg.get_width();
    for(int i = 0 ; i<height; i++){
        for(int j = 0 ; j<width; j++){
            if (SharpImg.get_data(i,j)>255){
                SharpImg.set_data(i,j,255);
            }
            if (SharpImg.get_data(i,j)<0){
                SharpImg.set_data(i,j,0);
            }
        }
    }
    return SharpImg;
}

