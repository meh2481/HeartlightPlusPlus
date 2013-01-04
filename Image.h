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
    HTEXTURE   m_hscaledTex;    //If we've scaled it
    hgeSprite* m_hSprite;
    string     m_sFilename;
    uint32_t m_iWidth, m_iHeight;
    uint32_t m_iID;  //Used to differentiate objects easily so can draw in batches
                    // (it's faster to draw one image several times in a row than it is to draw images in random order)
    uint16_t m_iScaleFac;

public:
    //Constructor/destructor
    Image(string sFilename);
    ~Image();

    //Accessor methods
    uint32_t getWidth()     {return m_iWidth;};
    uint32_t getHeight()    {return m_iHeight;};
    string getFilename()    {return m_sFilename;};
    uint32_t _getID()    {return m_iID;};    //For engine use
    void _setID(uint32_t id) {m_iID = id;};

    //public methods
    void draw(Rect rcScreenPos);    //Stretch the image into this rect
    void draw(Rect rcScreenPos, Rect rcImgPos); //Stretch part of the image into this rect
    void draw(float32 x, float32 y);    //draw the entire image with upper left corner at x,y
    void draw(Point pt);                //draw the entire image with upper left corner at pt
    void draw(float32 x, float32 y, Rect rcImgPos); //draw part of the image at x, y
    void draw(Point pt, Rect rcImgPos); //draw part of the image at pt
    void drawCentered(float32 x, float32 y, float32 rotation = 0.0, float32 stretchFactor = 1.0);    //Center the entire image centered at x,y
    void drawCentered(Point pt, float32 rotation = 0.0, float32 stretchFactor = 1.0);    //Center the entire image centered at pt
    void drawCentered(float32 x, float32 y, Rect rcImgPos, float32 rotation = 0.0, float32 stretchFactor = 1.0);    //Center part of the image at x,y
    void drawCentered(Point pt, Rect rcImgPos, float32 rotation = 0.0, float32 stretchFactor = 1.0);    //Center part of the image at pt

    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);  //Set the image to this color
    void setColor(DWORD dwCol);
    void scale(uint16_t iScaleFac); //Scales this image up by a given factor from the original, without interpolation. (scale(1) to reset to original size)
    void setHotSpot(float32 x, float32 y)   {m_hSprite->SetHotSpot(x,y);};
    Point getHotSpot()                      {Point pt; m_hSprite->GetHotSpot(&pt.x, &pt.y); return pt;};
    void setHotSpot(Point pt)               {setHotSpot(pt.x, pt.y);};
};


#endif



