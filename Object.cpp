/*
    Heartlight++ source - Object.cpp
    Class for objects within the game
    Copyright (c) 2012 Mark Hutcheson
*/

#include "Object.h"

Object::Object(Image* img)
{
    if(img == NULL)
    {
        errlog << "Error: NULL image encountered in Object::Object()" << endl;
        exit(1);
    }
    layer = new parallaxLayer(img);
    m_ptPos.SetZero();
    m_ptVel.SetZero();
    //m_Img = img;
    m_iNumFrames = 1;
    m_iCurFrame = 0;
    m_iWidth = img->getWidth();
    m_iHeight = img->getHeight();
    m_ptVel.SetZero();
    m_bDying = false;
    m_bAnimateOnce = false;
    m_bAnimate = true;
}

Object::~Object()
{
    delete layer;
}

void Object::updateFrame()
{
    if(!m_bAnimate)
        return;
    m_iCurFrame++;
    if(m_iCurFrame >= m_iNumFrames)
    {
        if(m_bAnimateOnce)  //If we're only supposed to animate once, die
            kill();
        m_iCurFrame = 0;
    }
}

bool Object::update()
{
    return !m_bDying;
}

void Object::draw(Rect rcScreen)
{
    Rect rcImgPos = {0,m_iHeight*m_iCurFrame,m_iWidth,m_iHeight*(m_iCurFrame+1)};
    layer->pos = m_ptPos;//.Set(m_ptPos.x + getWidth()/2.0, m_ptPos.y + getHeight()/2.0);
    layer->draw(rcScreen, rcImgPos);
}

void Object::setNumFrames(uint16_t iNumFrames, bool bAnimateOnce)
{
    m_iNumFrames = iNumFrames;
    m_iHeight = layer->image->getHeight()/m_iNumFrames;
    m_bAnimateOnce = bAnimateOnce;
}

//-----------------------------------------------------------------------------------------------------------------------
retroObject::retroObject(Image* img) : Object(img)
{
    m_sName = "";
    m_iData = 0;
}

//-----------------------------------------------------------------------------------------------------------------------
//Brick::Brick(Image* img) : retroObject(img)
//{
//}

Door::Door(Image* img) : retroObject(img)
{
}

Dwarf::Dwarf(Image* img) : retroObject(img)
{
}

//void Brick::updateFrame()
//{
    //Don't update frame on bricks
//}

void Door::updateFrame()
{
    if(m_iCurFrame > 0)
        m_iCurFrame++;
    if(m_iCurFrame >= m_iNumFrames)
        m_iCurFrame = m_iNumFrames-1;
}

void Dwarf::updateFrame()
{
    //Don't update frame on dwarf
}

//-----------------------------------------------------------------------------------------------------------------------
physicsObject::physicsObject(Image* img) : Object(img)
{
    m_physicsBody = NULL;
}

physicsObject::~physicsObject()
{
}

void physicsObject::draw(Rect rcScreen)
{
    Rect rcImgPos = {0,m_iHeight*m_iCurFrame,m_iWidth,m_iHeight*(m_iCurFrame+1)};
    layer->pos = m_physicsBody->GetPosition();
    layer->pos *= SCALE_UP_FACTOR;
    layer->rot = m_physicsBody->GetAngle();
    layer->draw(rcScreen, rcImgPos);
}





