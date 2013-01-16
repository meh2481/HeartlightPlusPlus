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

void parallaxLayer::draw(Rect rcScreen, float fScaleFacX, float fScaleFacY)
{
    Rect rcImgPos;
    rcImgPos.set(0,0,image->getWidth(),image->getHeight());
    draw(rcScreen, rcImgPos, fScaleFacX, fScaleFacY);
}

void parallaxLayer::draw(Rect rcScreen, Rect rcImgPos, float fScaleFacX, float fScaleFacY)
{
    if(image == NULL)
        return;

    //TODO Make sure within screen

    Point ptDrawPos = pos;

    //Account for viewport position
    ptDrawPos.x -= rcScreen.left;
    ptDrawPos.y -= rcScreen.top;

    //Account for parallax and viewport scaling
    Point ptDifference;// = rcScreen.center();
    ptDifference.Set(rcScreen.width()/2.0, rcScreen.height()/2.0);
    ptDifference -= ptDrawPos;
    //ptDifference /= depth;
    ptDifference.x = ptDifference.x * depth;// - (depth)*rcScreen.width();// + rcScreen.width()*(1.0-depth);// + (1.0/depth - depth) * rcScreen.width();// * 1.0-fScaleFacX;
    ptDifference.y = ptDifference.y * depth;// + (1.0 - depth) * rcScreen.height() * 1.0-fScaleFacY;// * depth;// * depth;// * fScaleFacY;

    //Normalize
    ptDrawPos.Set(rcScreen.width()/2.0, rcScreen.height()/2.0);
    //ptDrawPos = rcScreen.center();
    ptDrawPos -= ptDifference;
    ptDrawPos.x *= fScaleFacX;
    ptDrawPos.y *= fScaleFacY;
    //if(depth != 1.0)
    //    ptDrawPos.x -= rcScreen.width()*(1.0-depth);//*(1.0-depth);//*fScaleFacX*depth;
    //ptDrawPos.y -= rcScreen.height()*(1.0-depth);

    //Scale up according to depth //TODO: Make this come out right for parallax layers
    Point ptScale = scale;
    //ptScale *= depth;
    ptScale.x *= depth * fScaleFacX;
    ptScale.y *= depth * fScaleFacY;


    //ptDrawPos.x -= rcScreen.width();
    //ptDrawPos.y -= rcScreen.height();

    image->drawCentered(ptDrawPos, rcImgPos, rot, ptScale.x, ptScale.y);
}














