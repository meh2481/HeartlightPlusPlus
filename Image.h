/*
    Heartlight++ source - Image.h
    Class for drawing images to the screen easily
    Copyright (c) 2012 Mark Hutcheson
*/
#ifndef IMAGE_H
#define IMAGE_H

#include "globaldefs.h"

//TODO: Should be variable or such
//extern int SCREEN_WIDTH;//  800.0
//extern int SCREEN_HEIGHT;// 600.0

//TODO: Figure out what causes this and calculate mathematically? Or just live with it?
#define MAGIC_ZOOM_NUMBER -2.414213562373095

class Color
{
public:
	float32 r,g,b,a;
	Color();

	void from256(int32_t ir, int32_t ig, int32_t ib, int32_t a = 255);
	void set(float32 fr, float32 fg, float32 fb, float32 fa = 1.0) {r=fr;g=fg;b=fb;a=fa;};
};

class Image
{
private:
    Image(){};  //Default constructor is uncallable

    GLuint   m_hTex;
//    hgeSprite* m_hSprite;
    Point m_ptHotSpot;
    string     m_sFilename;
    uint32_t m_iWidth, m_iHeight;
#ifdef __APPLE__
    uint32_t m_iRealWidth, m_iRealHeight;
#endif
    uint32_t m_iID;  //TODO: Remove/isolate/optimize
//    uint16_t m_iScaleFac;

    void _load(string sFilename);

public:
    Color m_col;
    
    //Constructor/destructor
    Image(string sFilename);
    ~Image();
    
	//Engine use functions
    void _reload();  //Reload memory associated with this image
	void _setFilename(string s) {m_sFilename = s;};
    uint32_t _getID()    {return m_iID;};    //For engine use
    void _setID(uint32_t id) {m_iID = id;};

    //Accessor methods
    uint32_t getWidth()     {return m_iWidth;};
    uint32_t getHeight()    {return m_iHeight;};
    string getFilename()    {return m_sFilename;};

    //public methods
    void draw(Rect rcDrawPos);
    void draw(Rect rcDrawPos, Rect rcImgPos);
    void draw(float32 x, float32 y);    //draw the entire image with upper left corner at x,y
    void draw(Point pt);                //draw the entire image with upper left corner at pt
    void draw(float32 x, float32 y, Rect rcImgPos); //draw part of the image at x, y
    void draw(Point pt, Rect rcImgPos); //draw part of the image at pt
    void draw4V(Point ul, Point ur, Point bl, Point br);  //render the image at arbitrary quad
    void drawCentered(float32 x, float32 y, float32 rotation = 0.0, float32 stretchFactorx = 1.0, float32 stretchFactory = 1.0);    //Center the entire image centered at x,y
    void drawCentered(Point pt, float32 rotation = 0.0, float32 stretchFactorx = 1.0, float32 stretchFactory = 1.0);    //Center the entire image centered at pt
    void drawCentered(float32 x, float32 y, Rect rcImgPos, float32 rotation = 0.0, float32 stretchFactorx = 1.0, float32 stretchFactory = 1.0);    //Center part of the image at x,y
    void drawCentered(Point pt, Rect rcImgPos, float32 rotation = 0.0, float32 stretchFactorx = 1.0, float32 stretchFactory = 1.0);    //Center part of the image at pt

    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {m_col.from256(r,g,b,a);};  //Set the image to this color
    void setColor(DWORD dwCol);
    void setColor(Color col)    {m_col = col;};
    Color getColor()            {return m_col;};
    //void setColor(float32 r, float32 g, float32 b, float32 a = 1.0) {m_col.set(r,g,b,a);};
//    void scale(uint16_t iScaleFac); //Scales this image up by a given factor from the original, without interpolation. (scale(1) to reset to original size)
    void setHotSpot(float32 x, float32 y)   {m_ptHotSpot.x=x;m_ptHotSpot.y=y;};
    Point getHotSpot()                      {return m_ptHotSpot;};
    void setHotSpot(Point pt)               {setHotSpot(pt.x, pt.y);};
};

//Image reloading handler functions
void reloadImages();
void _addImgReload(Image* img);
void _removeImgReload(Image* img);

//Misc image functions
void setImageBlurred();		//Draw images blurred
void setImagePixellated();	//Draw images pixellated




#endif



