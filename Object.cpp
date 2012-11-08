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

void Object::Update()
{
    UpdateFrame();
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














