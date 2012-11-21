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

public:
    Object(Image* img);
    ~Object();

    //Helper methods
    virtual bool update();  //Return false to destroy the object
    virtual void updateFrame();
    void draw(float32 fScaleFactor = 1.0);
    void offset(float32 x, float32 y)   {m_ptPos.x += x; m_ptPos.y += y;};
    void offset(Point pt)               {m_ptPos += pt;};
    void kill() {m_bDying = true;};    //Destroy sprite

    //Accessor methods
    void setNumFrames(uint16_t iNumFrames, bool bAnimateOnce = false);
    int16_t  getFrame() {return m_iCurFrame;};
    void  setFrame(int16_t iFrame) {m_iCurFrame = iFrame;}; //WARNING: Potentially dangerous
    uint32_t getWidth() {return m_iWidth;};
    uint32_t getHeight() {return m_iHeight;};
    void setCenter(Point ptCenter) {m_ptPos = ptCenter;};
    void setCenter(float32 x, float32 y)    {m_ptPos.x = x; m_ptPos.y = y;};
    void setPos(float32 x, float32 y)   {m_ptPos.x = x + m_iWidth/2.0; m_ptPos.y = y + m_iHeight/2.0;};
    void setPos(Point ptULCorner)   {setPos(ptULCorner.x, ptULCorner.y);};
    Point getCenter()  {return m_ptPos;};
    Point getVelocity() {return m_ptVel;};
    void setVelocity(Point pt)  {m_ptVel = pt;};
    void setVelocity(float32 x, float32 y)  {m_ptVel.x = x; m_ptVel.y = y;};
    uint32_t _getID()    {return m_Img->_getID();};   //For engine use

    Image* getImage()   {return m_Img;};
    void setImage(Image* img)   {m_Img = img;}; //Use with caution! No error-checking!

};

class retroObject : public Object
{
protected:
    string m_sName;   //Name, for use by the game code (the way you'll be able to tell what object's what)
    uint64_t m_iData;   //For use by game code

public:
    retroObject(Image* img);
    void setName(string sName)  {m_sName = sName;};
    void setName(char cName)  {m_sName.clear(); m_sName.push_back(cName);};
    string getName()    {return m_sName;};
    char getNameChar()  {return *(m_sName.begin());};

    uint64_t getData()  {return m_iData;};
    void    setData(uint64_t data) {m_iData = data;};
    void    addData(uint64_t iAdd)  {m_iData |= iAdd;};
    void    removeData(uint64_t iRem)   {m_iData &= ~iRem;};
    bool    isData(uint64_t iTest) {return(m_iData & iTest);};
};

//Objects specific to our game
class Brick : public retroObject
{
public:
    Brick(Image* img);
    void updateFrame();
};

class Door : public retroObject
{
public:
    Door(Image* img);
    void updateFrame();
};

class Dwarf : public retroObject
{
public:
    Dwarf(Image* img);
    void updateFrame();
};











#endif
