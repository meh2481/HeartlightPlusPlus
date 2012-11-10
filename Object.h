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
    bool m_bAnimateOnce;
    uint16_t m_iCurFrame;
    uint32_t m_iWidth, m_iHeight;
    Point m_ptPos;
    Point m_ptVel;  //Velocity
    bool m_bDying;  //If sprite should be destroyed or no

    //Helper methods
    virtual void UpdateFrame();

public:
    Object(Image* img);
    ~Object();

    //Helper methods
    virtual bool Update();  //Return false to destroy the object
    void Draw(float fScaleFactor = 1.0);
    void Offset(float32 x, float32 y)   {m_ptPos.x += x; m_ptPos.y += y;};
    void Offset(Point pt)               {m_ptPos += pt;};
    void Kill() {m_bDying = true;};    //Destroy sprite

    //Accessor methods
    void SetNumFrames(uint16_t iNumFrames, bool bAnimateOnce = false);
    int16_t  GetFrame() {return m_iCurFrame;};
    void  SetFrame(int16_t iFrame) {m_iCurFrame = iFrame;}; //WARNING: Potentially dangerous
    uint32_t GetWidth() {return m_iWidth;};
    uint32_t GetHeight() {return m_iHeight;};
    void SetCenter(Point ptCenter) {m_ptPos = ptCenter;};
    void SetCenter(float32 x, float32 y)    {m_ptPos.x = x; m_ptPos.y = y;};
    void SetPos(float32 x, float32 y)   {m_ptPos.x = x + m_iWidth/2.0; m_ptPos.y = y + m_iHeight/2.0;};
    void SetPos(Point ptULCorner)   {SetPos(ptULCorner.x, ptULCorner.y);};
    Point GetCenter()  {return m_ptPos;};
    Point GetVelocity() {return m_ptVel;};
    void SetVelocity(Point pt)  {m_ptVel = pt;};
    void SetVelocity(float32 x, float32 y)  {m_ptVel.x = x; m_ptVel.y = y;};
    uint32_t getID()    {return m_Img->GetID();};   //For engine use

    Image* getImage()   {return m_Img;};
    void setImage(Image* img)   {m_Img = img;}; //Use with caution! No error-checking!

};

class retroObject : public Object
{
protected:
    string m_sName;   //Name, for use by your game code (the way you'll be able to tell what object's what)

public:
    retroObject(Image* img);
    void SetName(string sName)  {m_sName = sName;};
    void SetName(char cName)  {m_sName.clear(); m_sName.push_back(cName);};
    string GetName()    {return m_sName;};
    char GetNameChar()  {return *(m_sName.begin());};
};

//Objects specific to our game
class Brick : public retroObject
{
protected:
    void UpdateFrame();
public:
    Brick(Image* img);
};

class Door : public retroObject
{
protected:
    void UpdateFrame();
public:
    Door(Image* img);
};

class Dwarf : public retroObject
{
protected:
    void UpdateFrame();
public:
    Dwarf(Image* img);
};











#endif
