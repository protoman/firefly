#include "TiledMapTexture.h"

TiledMapTexture::TiledMapTexture() {}

TiledMapTexture::~TiledMapTexture()
{
    image_data.freeGraphic();
}

void TiledMapTexture::set_image_data(const st_imageData& new_image_data) {
    if (!image_data.is_null()) {
        image_data.freeGraphic();
    }
    image_data = new_image_data;
}


