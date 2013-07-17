//Header for parallax scene layer stuff
//SceneLayer.h
#ifndef SCENELAYER_H
#define SCENELAYER_H

#include "Image.h"

//TODO Phase out Images in favor of parallaxLayers for sprites and such

class parallaxLayer
{
protected:
    parallaxLayer(){};

public:
    float32 depth;
    Point scale;
    Point pos;
    float32 rot;
    Image* image;
    Color col;

    parallaxLayer(Image* img);
    ~parallaxLayer();

    void draw();
    void draw(Rect rcImgPos);
};























#endif
