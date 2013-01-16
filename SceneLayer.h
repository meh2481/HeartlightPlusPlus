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

    parallaxLayer(Image* img);
    ~parallaxLayer();

    void draw(Rect rcScreen, float fScaleFacX = 1.0, float fScaleFacY = 1.0);
    void draw(Rect rcScreen, Rect rcImgPos, float fScaleFacX = 1.0, float fScaleFacY = 1.0);
};























#endif
