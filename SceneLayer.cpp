//Source for parallaxLayers and drawing them
//SceneLayer.cpp
#include "SceneLayer.h"

parallaxLayer::parallaxLayer(Image* img)
{
    depth = 0.0f;
    scale.x = scale.y = 1.0f;
    rot = 0.0f;
    image = img;
    alpha = 1.0f;
    pos.SetZero();
}

parallaxLayer::~parallaxLayer()
{
}

void parallaxLayer::draw()
{
    Rect rcImgPos;
    rcImgPos.set(0,0,image->getWidth(),image->getHeight());
    draw(rcImgPos);
}

void parallaxLayer::draw(Rect rcImgPos)
{
    if(image == NULL)
        return;
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, depth);
    //if(depth != 0.0f)
    //  cout << "Depth: " << depth << endl;
    float32 fAlpha = image->getAlpha();
    image->setAlpha(alpha);
    image->drawCentered(pos, rcImgPos, rot, scale.x, scale.y);
    image->setAlpha(fAlpha);
    glPopMatrix();
}














