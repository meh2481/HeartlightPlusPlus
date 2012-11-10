/*
    Heartlight++ source - Object.cpp
    Class for objects within the game
    Copyright (c) 2012 Mark Hutcheson
*/

#include "Object.h"

Object::Object(Image* img)
{
    m_ptPos.x = m_ptPos.y = 0;
    m_Img = img;
    m_iNumFrames = 1;
    m_iCurFrame = 0;
    m_iWidth = img->getWidth();
    m_iHeight = img->getHeight();
    m_ptVel.SetZero();
    m_bDying = false;
    m_bAnimateOnce = false;
}

Object::~Object()
{

}

void Object::updateFrame()
{
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
    updateFrame();
    return !m_bDying;
}

void Object::draw(float fScaleFactor)
{
    Rect rcImgPos = {0,m_iHeight*m_iCurFrame,m_iWidth,m_iHeight*(m_iCurFrame+1)};
    m_Img->drawCentered(m_ptPos, rcImgPos, 0.0, fScaleFactor);
}

void Object::setNumFrames(uint16_t iNumFrames, bool bAnimateOnce)
{
    m_iNumFrames = iNumFrames;
    m_iHeight = m_Img->getHeight()/m_iNumFrames;
    m_bAnimateOnce = bAnimateOnce;
}

//-----------------------------------------------------------------------------------------------------------------------
retroObject::retroObject(Image* img) : Object(img)
{
    m_sName = "";
}

//-----------------------------------------------------------------------------------------------------------------------
Brick::Brick(Image* img) : retroObject(img)
{
}

Door::Door(Image* img) : retroObject(img)
{
}

Dwarf::Dwarf(Image* img) : retroObject(img)
{
}

void Brick::updateFrame()
{
    //Don't update frame on bricks
}

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









