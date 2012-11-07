/*
    Heartlight++ source - Image.h
    Class for drawing images to the screen easily
    Copyright (c) 2012 Mark Hutcheson
*/
#ifndef IMAGE_H
#define IMAGE_H

#include "globaldefs.h"

class Image
{
private:
    Image(){};  //Default constructor is uncallable

    HTEXTURE   m_hTex;
    hgeSprite* m_hSprite;
    string     m_sFilename;
    uint32_t m_iWidth, m_iHeight;

public:
    //Constructor/destructor
    Image(string sFilename);
    ~Image();

    //Accessor methods
    uint32_t GetWidth()     {return m_iWidth;};
    uint32_t GetHeight()    {return m_iHeight;};
    string GetFilename()    {return m_sFilename;};

    //public methods
    void Draw(Rect rcScreenPos);    //Stretch the image into this rect
    void Draw(Rect rcScreenPos, Rect rcImgPos); //Stretch part of the image into this rect
    void Draw(float32 x, float32 y);    //Draw the entire image with upper left corner at x,y
    void Draw(Point pt);                //Draw the entire image with upper left corner at pt
    void Draw(float32 x, float32 y, Rect rcImgPos); //Draw part of the image at x, y
    void Draw(Point pt, Rect rcImgPos); //Draw part of the image at pt
    void DrawCentered(float32 x, float32 y, float32 rotation = 0.0, float32 stretchFactor = 1.0);    //Center the entire image centered at x,y
    void DrawCentered(Point pt, float32 rotation = 0.0, float32 stretchFactor = 1.0);    //Center the entire image centered at pt
    void DrawCentered(float32 x, float32 y, Rect rcImgPos, float32 rotation = 0.0, float32 stretchFactor = 1.0);    //Center part of the image at x,y
    void DrawCentered(Point pt, Rect rcImgPos, float32 rotation = 0.0, float32 stretchFactor = 1.0);    //Center part of the image at pt
};


#endif



