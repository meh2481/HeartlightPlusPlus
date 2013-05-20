//Source for parallaxLayers and drawing them
//SceneLayer.cpp
#include "SceneLayer.h"

parallaxLayer::parallaxLayer(Image* img)
{
    depth = 1.0f;
    scale.x = scale.y = 1.0f;
    rot = 0.0f;
    image = img;
    pos.SetZero();
}

parallaxLayer::~parallaxLayer()
{
}

void parallaxLayer::draw(Rect rcScreen, float32 fScaleFacX, float32 fScaleFacY)
{
    Rect rcImgPos;
    rcImgPos.set(0,0,image->getWidth(),image->getHeight());
    draw(rcScreen, rcImgPos, fScaleFacX, fScaleFacY);
}

void parallaxLayer::draw(Rect rcScreen, Rect rcImgPos, float32 fScaleFacX, float32 fScaleFacY)  //TODO: Why do I even have scalefacx/y here?
{
    if(image == NULL)
        return;

    //TODO Make sure within screen

    Point ptDrawPos = pos;

    //Account for viewport position
    ptDrawPos.x -= rcScreen.left;
    ptDrawPos.y -= rcScreen.top;

    //TODO: Account for parallax scrolling

    image->drawCentered(ptDrawPos, rcImgPos, rot, scale.x, scale.y);
}














