/*
    Heartlight++ source - Engine.h
    Copyright (c) 2012 Mark Hutcheson
*/

#ifndef ENGINE_H
#define ENGINE_H

#include "globaldefs.h"
#include "Image.h"
#include "Object.h"

class Engine
{
private:
    Engine(){}; //Default constructor isn't callable

protected:
    //Variables for use by the engine
    HGE* m_hge;
    unsigned short m_iFramerate;
    float m_fAccumulatedTime;
    float m_fTargetTime;
    hgeSprite* m_sprFill;   //Sprite for filling a color

    //Engine-use class definitions
    friend bool FrameFunc();
    friend bool RenderFunc();
    bool myFrameFunc();
    bool myRenderFunc();

    //Classes to override in your own class definition
    virtual bool frame() = 0;   //Function that's called every frame
    virtual void draw() = 0;    //Actual function that draws stuff
    virtual void init() = 0;    //So we can load all our images and such

public:
    //Constructor/destructor
    Engine(int iWidth, int iHeight, string sTitle);
    ~Engine();

    //Methods
    void start();   //Runs HGE and doesn't exit until the engine ends
    void fillRect(Point p1, Point p2, uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha); //Fill the specified rect with the specified color
    void fillRect(float32 x1, float32 y1, float32 x2, float32 y2, uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha);
    void fillRect(Rect rc, uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha);

    //Accessor methods
    void setFramerate(unsigned short iFramerate)    {m_iFramerate = iFramerate; m_fTargetTime = 1.0/(float)(m_iFramerate);};
    unsigned short getFramerate()   {return m_iFramerate;};

};

bool FrameFunc();
bool RenderFunc();



#endif
