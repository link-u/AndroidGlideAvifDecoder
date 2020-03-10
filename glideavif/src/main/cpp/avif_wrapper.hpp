//
// Created by gc on 20/03/10.
//

#ifndef ANDROIDGLIDEAVIFDECODER_AVIF_WRAPPER_HPP
#define ANDROIDGLIDEAVIFDECODER_AVIF_WRAPPER_HPP

#include <avif/avif.h>
#include <memory>

struct AvifDecoderDestroyer final {
    void operator()(avifDecoder *decoder) noexcept {
        avifDecoderDestroy(decoder);
    }
};

using avifDecoderPtr = std::unique_ptr<avifDecoder, AvifDecoderDestroyer>;

struct AvifRGBImageFree final {
    void operator()(avifRGBImage *im) noexcept {
        avifRGBImageFreePixels(im);
        delete im;
    }
};

using avifRGBImagePtr = std::unique_ptr<avifRGBImage, AvifRGBImageFree>;


#endif //ANDROIDGLIDEAVIFDECODER_AVIF_WRAPPER_HPP
