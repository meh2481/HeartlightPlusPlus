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
    m_iWidth = img->GetWidth();
    m_iHeight = img->GetHeight();
    m_ptVel.SetZero();
    m_bDying = false;
}

Object::~Object()
{

}

void Object::UpdateFrame()
{
    m_iCurFrame++;
    if(m_iCurFrame >= m_iNumFrames)
        m_iCurFrame = 0;
}

bool Object::Update()
{
    UpdateFrame();
    return !m_bDying;
}

void Object::Draw(float fScaleFactor)
{
    Rect rcImgPos = {0,m_iHeight*m_iCurFrame,m_iWidth,m_iHeight*(m_iCurFrame+1)};
    m_Img->DrawCentered(m_ptPos, rcImgPos, 0.0, fScaleFactor);
}

void Object::SetNumFrames(uint16_t iNumFrames)
{
    m_iNumFrames = iNumFrames;
    m_iHeight = m_Img->GetHeight()/m_iNumFrames;
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

void Brick::UpdateFrame()
{
    //Don't update frame on bricks
}

void Door::UpdateFrame()
{
    if(m_iCurFrame > 0)
        m_iCurFrame++;
    if(m_iCurFrame >= m_iNumFrames)
        m_iCurFrame = m_iNumFrames-1;
}

void Dwarf::UpdateFrame()
{
    //Don't update frame on dwarf
}









