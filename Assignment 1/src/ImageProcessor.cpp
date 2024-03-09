#include <iostream>
#include "ImageProcessor.h"

ImageProcessor::ImageProcessor() = default;

ImageProcessor::~ImageProcessor() = default;


std::string ImageProcessor::decodeHiddenMessage(const ImageMatrix &img) {
    std::string hiddenMessage;
    ImageSharpening imageSharpening;
    ImageMatrix SharpenedImage = imageSharpening.sharpen(img,2);
    EdgeDetector edgeDetector;
    this -> edgePixels = edgeDetector.detectEdges(SharpenedImage);
    DecodeMessage decodeMessage;
    hiddenMessage= decodeMessage.decodeFromImage(SharpenedImage,edgePixels);
    return hiddenMessage;

}

ImageMatrix ImageProcessor::encodeHiddenMessage(const ImageMatrix &img, const std::string &message) {
    ImageSharpening sharpener;
    ImageMatrix sharpenedImage = sharpener.sharpen(img,2);
    EdgeDetector edgeDetector;
    this -> edgePixels = edgeDetector.detectEdges(sharpenedImage);
    EncodeMessage encodeMessage;
    ImageMatrix embeddedImage = encodeMessage.encodeMessageToImage(img,message,edgePixels);
    return embeddedImage;
}
