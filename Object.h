/*
    Heartlight++ source - Object.h
    Class for objects within the game
    Copyright (c) 2012 Mark Hutcheson
*/

#ifndef OBJECT_H
#define OBJECT_H

#include "globaldefs.h"
#include "Image.h"

class Object
{
private:
    Object(){};

protected:
    Image* m_Img;
    uint16_t m_iNumFrames;
    uint16_t m_iCurFrame;
    uint32_t m_iWidth, m_iHeight;
    Point m_ptPos;

    //Helper methods
    virtual void UpdateFrame();

public:
    Object(Image* img);
    ~Object();

    //Helper methods
    virtual void Update();
    void Draw(float fScaleFactor = 1.0);

    //Accessor methods
    void SetNumFrames(uint16_t iNumFrames);
    int16_t  GetFrame() {return m_iCurFrame;};
    void  SetFrame(int16_t iFrame) {m_iCurFrame = iFrame;}; //WARNING: Potentially dangerous
    uint32_t GetWidth() {return m_iWidth;};
    uint32_t GetHeight() {return m_iHeight;};
    void SetCenter(Point ptCenter) {m_ptPos = ptCenter;};
    void SetCenter(float32 x, float32 y)    {m_ptPos.x = x; m_ptPos.y = y;};
    void SetPos(float32 x, float32 y)   {m_ptPos.x = x + m_iWidth/2.0; m_ptPos.y = y + m_iHeight/2.0;};
    void SetPos(Point ptULCorner)   {SetPos(ptULCorner.x, ptULCorner.y);};
    Point GetCenter()  {return m_ptPos;};


};

//Objects specific to our game
class Brick : public Object
{
protected:
    void UpdateFrame();
public:
    Brick(Image* img);
};

class Door : public Object
{
protected:
    void UpdateFrame();
public:
    Door(Image* img);
};

class Dwarf : public Object
{
protected:
    void UpdateFrame();
public:
    Dwarf(Image* img);
};











#endif
