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

void parallaxLayer::draw(Rect rcScreen, Rect rcImgPos, float32 fScaleFacX, float32 fScaleFacY)
{
    if(image == NULL)
        return;

    //TODO Make sure within screen

    Point ptDrawPos = pos;

    //Account for viewport position
    ptDrawPos.x -= rcScreen.left;
    ptDrawPos.y -= rcScreen.top;

    //Account for parallax and viewport scaling
    Point ptDifference;
    ptDifference.Set(rcScreen.width()/2.0, rcScreen.height()/2.0);
    ptDifference -= ptDrawPos;
    ptDifference.x = ptDifference.x * depth;
    ptDifference.y = ptDifference.y * depth;

    //Normalize
    ptDrawPos.Set(rcScreen.width()/2.0, rcScreen.height()/2.0);
    ptDrawPos -= ptDifference;
    ptDrawPos.x *= fScaleFacX;
    ptDrawPos.y *= fScaleFacY;

    //Scale up according to depth //TODO: Make this come out right for parallax layers
    Point ptScale = scale;
    ptScale.x *= depth * fScaleFacX;
    ptScale.y *= depth * fScaleFacY;

    image->drawCentered(ptDrawPos, rcImgPos, rot, ptScale.x, ptScale.y);
}














