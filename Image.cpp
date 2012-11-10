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
    m_hscaledTex = 0;

    if(!m_hTex) //Failed to load texture
    {
        errlog << "Error loading image " << sFilename << endl;
        m_hSprite = NULL;
        m_iWidth = m_iHeight = 32;
        //Keep going, it'll just create a white image instead
    }
    else
    {
        m_iWidth = hge->Texture_GetWidth(m_hTex, true);
        m_iHeight = hge->Texture_GetHeight(m_hTex, true);
        errlog << "Creating image " << sFilename << " Width: " << m_iWidth << " Height: " << m_iHeight << endl;
    }

    m_hSprite = new hgeSprite(m_hTex, 0, 0, m_iWidth, m_iHeight);

    hge->Release();
}

Image::~Image()
{
    errlog << "Destroying image " << m_sFilename << endl;
    HGE* hge = hgeCreate(HGE_VERSION);
    delete m_hSprite;
    hge->Texture_Free(m_hTex);
    if(m_hscaledTex)
        hge->Texture_Free(m_hscaledTex);
    hge->Release();
}

void Image::draw(Rect rcScreenPos)
{
    Rect rcImg = {0,0,m_iWidth,m_iHeight};
    draw(rcScreenPos, rcImg);
}

void Image::draw(Rect rcScreenPos, Rect rcImgPos)
{
    if(m_hSprite == NULL)
    {
        errlog << "NULL hgeSprite in Image::draw()" << endl;
        return;
    }
    m_hSprite->SetTextureRect(rcImgPos.left, rcImgPos.top, rcImgPos.width(), rcImgPos.height());
    m_hSprite->RenderStretch(rcScreenPos.left, rcScreenPos.top, rcScreenPos.right, rcScreenPos.bottom);
}

void Image::draw(float32 x, float32 y)
{
    Rect rcScr = {x, y, m_iWidth+x, m_iHeight+y};
    draw(rcScr);
}

void Image::draw(Point pt)
{
    draw(pt.x, pt.y);
}

void Image::draw(float32 x, float32 y, Rect rcImgPos)
{
    Rect rcScr = {x, y, m_iWidth+x, m_iHeight+y};
    draw(rcScr, rcImgPos);
}

void Image::draw(Point pt, Rect rcImgPos)
{
    draw(pt.x, pt.y, rcImgPos);
}

void Image::drawCentered(float32 x, float32 y, float32 rotation, float32 stretchFactor)
{
    Rect rcImg = {0,0,m_iWidth,m_iHeight};
    drawCentered(x, y, rcImg, rotation, stretchFactor);
}

void Image::drawCentered(Point pt, float32 rotation, float32 stretchFactor)
{
    drawCentered(pt.x, pt.y, rotation, stretchFactor);
}

void Image::drawCentered(float32 x, float32 y, Rect rcImgPos, float32 rotation, float32 stretchFactor)
{
    if(m_hSprite == NULL)
    {
        errlog << "NULL hgeSprite in Image::drawCentered()" << endl;
        return;
    }
    m_hSprite->SetTextureRect(rcImgPos.left, rcImgPos.top, rcImgPos.width(), rcImgPos.height());
    m_hSprite->RenderEx(x - rcImgPos.width()/2.0, y - rcImgPos.height()/2.0, rotation, stretchFactor);
}

void Image::drawCentered(Point pt, Rect rcImgPos, float32 rotation, float32 stretchFactor)
{
    drawCentered(pt.x, pt.y, rcImgPos, rotation, stretchFactor);
}

//Stretch this image onto a larger one, without interpolation
void Image::scale(uint16_t iScaleFac)
{
    if(iScaleFac < 1)   //Don't scale up any if 0
        return;

    HGE* hge = hgeCreate(HGE_VERSION);
    if(m_hscaledTex)
    {
        hge->Texture_Free(m_hscaledTex);
        m_hscaledTex = 0;
        m_iWidth = hge->Texture_GetWidth(m_hTex, true);
        m_iHeight = hge->Texture_GetHeight(m_hTex, true);
        if(iScaleFac == 1)  //Rescale down to the original image
        {
            m_hSprite->SetTexture(m_hTex);
            m_hSprite->SetTextureRect(0,0,m_iWidth,m_iHeight);
            return; //Done
        }
    }
    else if(iScaleFac == 1)
    {
        //Nothing to do if there's no previous texture and we're just reverting
        hge->Release();
        return;
    }
    m_hscaledTex = hge->Texture_Create(m_iWidth * iScaleFac, m_iHeight * iScaleFac);  //Create a new texture of the right size
    DWORD* src = hge->Texture_Lock(m_hTex); //Grab our original texture data
    DWORD* dest = hge->Texture_Lock(m_hscaledTex, false);   //And our new data

    //Now loop through and copy over, scaling up
    for(uint32_t x = 0; x < m_iWidth; x++)
    {
        for(uint32_t y = 0; y < m_iHeight; y++)
        {
            DWORD srcPixel = src[y*m_iWidth+x]; //Grab this pixel
            //loop and make all the neighboring pixels this color too (Yeah, I don't understand this code either)
            for(uint16_t i = 0; i < iScaleFac; i++)
            {
                for(uint16_t j = 0; j < iScaleFac; j++)
                    dest[(iScaleFac*y+j)*m_iWidth*iScaleFac+(x*iScaleFac)+i] = srcPixel;
            }
        }
    }

    hge->Texture_Unlock(m_hTex);    //Unlock both textures
    hge->Texture_Unlock(m_hscaledTex);
    m_hSprite->SetTexture(m_hscaledTex);   //Set to the new texture
    m_iWidth *= iScaleFac;              //Set to new dimensions
    m_iHeight *= iScaleFac;
    m_hSprite->SetTextureRect(0,0,m_iWidth,m_iHeight);  //Set the sprite to this new texture rectangle
    //hge->Texture_Free(m_hTex);  //Clean up old texture
    //m_hTex = scaledTex; //Save new texture
    hge->Release();
}



















