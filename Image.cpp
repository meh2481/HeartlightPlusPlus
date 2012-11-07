/*
    Heartlight++ source - Image.cpp
    Class for drawing images to the screen easily
    Copyright (c) 2012 Mark Hutcheson
*/

#include "Image.h"

Image::Image(string sFilename)
{
    m_sFilename = sFilename;

    HGE* hge = hgeCreate(HGE_VERSION);
    m_hTex = hge->Texture_Load(sFilename.c_str());

    if(!m_hTex) //Failed to load texture
    {
        errlog << "Error loading " << sFilename << endl;
        m_hSprite = NULL;
        m_iWidth = m_iHeight = 0;
        return; //Abort
    }

    m_iWidth = hge->Texture_GetWidth(m_hTex, true);
    m_iHeight = hge->Texture_GetHeight(m_hTex, true);
    errlog << "Creating image " << sFilename << " Width: " << m_iWidth << " Height: " << m_iHeight << endl;

    m_hSprite = new hgeSprite(m_hTex, 0, 0, m_iWidth, m_iHeight);

    hge->Release();
}

Image::~Image()
{
    errlog << "Destroying image " << m_sFilename << endl;
    HGE* hge = hgeCreate(HGE_VERSION);
    delete m_hSprite;
    hge->Texture_Free(m_hTex);
    hge->Release();
}

void Image::Draw(Rect rcScreenPos)
{
    Rect rcImg = {0,0,m_iWidth,m_iHeight};
    Draw(rcScreenPos, rcImg);
}

void Image::Draw(Rect rcScreenPos, Rect rcImgPos)
{
    if(m_hSprite == NULL)
    {
        errlog << "NULL hgeSprite in Draw()" << endl;
        return;
    }
    m_hSprite->SetTextureRect(rcImgPos.left, rcImgPos.top, rcImgPos.width(), rcImgPos.height());
    m_hSprite->RenderStretch(rcScreenPos.left, rcScreenPos.top, rcScreenPos.right, rcScreenPos.bottom);
}

void Image::Draw(float32 x, float32 y)
{
    Rect rcScr = {x, y, m_iWidth+x, m_iHeight+y};
    Draw(rcScr);
}

void Image::Draw(Point pt)
{
    Draw(pt.x, pt.y);
}

void Image::Draw(float32 x, float32 y, Rect rcImgPos)
{
    Rect rcScr = {x, y, m_iWidth+x, m_iHeight+y};
    Draw(rcScr, rcImgPos);
}

void Image::Draw(Point pt, Rect rcImgPos)
{
    Draw(pt.x, pt.y, rcImgPos);
}

void Image::DrawCentered(float32 x, float32 y, float32 rotation, float32 stretchFactor)
{
    Rect rcImg = {0,0,m_iWidth,m_iHeight};
    DrawCentered(x, y, rcImg, rotation, stretchFactor);
}

void Image::DrawCentered(Point pt, float32 rotation, float32 stretchFactor)
{
    DrawCentered(pt.x, pt.y, rotation, stretchFactor);
}

void Image::DrawCentered(float32 x, float32 y, Rect rcImgPos, float32 rotation, float32 stretchFactor)
{
    if(m_hSprite == NULL)
        return;
    m_hSprite->SetTextureRect(rcImgPos.left, rcImgPos.top, rcImgPos.width(), rcImgPos.height());
    m_hSprite->RenderEx(x, y, rotation, stretchFactor);
}

void Image::DrawCentered(Point pt, Rect rcImgPos, float32 rotation, float32 stretchFactor)
{
    DrawCentered(pt.x, pt.y, rcImgPos, rotation, stretchFactor);
}












