/*
    Heartlight++ source - Object.h
    Class for objects within the game
    Copyright (c) 2012 Mark Hutcheson
*/

#ifndef OBJECT_H
#define OBJECT_H

#include "globaldefs.h"
#include "Image.h"
#include "3DObject.h"
#include "SceneLayer.h"

class Object
{
private:
    Object(){};

protected:
    //Image* m_Img;
    uint16_t m_iNumFrames;
    bool m_bAnimateOnce;
    uint16_t m_iCurFrame;
    uint32_t m_iWidth, m_iHeight;
    Point m_ptPos;
    Point m_ptVel;  //Velocity
    bool m_bDying;  //If sprite should be destroyed or no
    bool m_bAnimate;    //If should animate frames or not

public:
    parallaxLayer* layer;   //TODO List of these, for multiple images per object

    Object(Image* img);
    ~Object();

    //Helper methods
    virtual bool update();  //Return false to destroy the object
    virtual void updateFrame();
    virtual void draw();
    virtual void offset(float32 x, float32 y)   {m_ptPos.x += x; m_ptPos.y += y;};
    virtual void offset(Point pt)               {m_ptPos += pt;};
    void kill() {m_bDying = true;};    //Destroy sprite

    //Accessor methods
    void setNumFrames(uint16_t iNumFrames, bool bAnimateOnce = false);
    void setAnimate(bool b)         {m_bAnimate = b;};
    bool getAnimate()               {return m_bAnimate;};
    int16_t  getFrame() {return m_iCurFrame;};
    void  setFrame(int16_t iFrame) {m_iCurFrame = iFrame;}; //WARNING: Potentially dangerous
    uint32_t getWidth() {return m_iWidth * layer->scale.x;};
    uint32_t getHeight() {return m_iHeight * layer->scale.y;};
    void setCenter(Point ptCenter) {m_ptPos = ptCenter;};
    void setCenter(float32 x, float32 y)    {m_ptPos.x = x; m_ptPos.y = y;};
    void setPos(float32 x, float32 y)   {m_ptPos.x = x + m_iWidth*layer->scale.x/2.0; m_ptPos.y = y + m_iHeight*layer->scale.y/2.0;};
    void setPos(Point ptULCorner)   {setPos(ptULCorner.x, ptULCorner.y);};
    virtual Point getCenter()  {return m_ptPos;};
    Point getVelocity() {return m_ptVel;};
    void setVelocity(Point pt)  {m_ptVel = pt;};
    void setVelocity(float32 x, float32 y)  {m_ptVel.x = x; m_ptVel.y = y;};
    virtual float32 _getDepthID() {return layer->depth;};  //For engine use - what is drawn in what order
    virtual b2Body* getBody() {return NULL;};

    Image* getImage()   {return layer->image;};
    void setImage(Image* img)   {layer->image = img;}; //Use with caution! No error-checking!
//    parallaxLayer* getLayer()   {return layer;};

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
    virtual float32 _getDepthID() {return layer->image->_getID();};   //Draw these together for speed
};

//Objects specific to our game
/*class Brick : public retroObject
{
public:
    Brick(Image* img);
    void updateFrame();
};*/

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

#define VELOCITY_ITERATIONS 8
#define PHYSICS_ITERATIONS 3
#define SCALE_UP_FACTOR 16.0
#define SCALE_DOWN_FACTOR 0.0625

class physSegment
{
public:
    b2Body*         body;
    parallaxLayer*  layer;
    Object3D*       obj3D;

    physSegment();
    ~physSegment();

};

class obj
{
protected:
    list<physSegment*> segments;

public:
    obj();
    ~obj();
    
    Point pos;
    float32 rot;

    void draw();
    void addSegment(physSegment* seg);

};

class physicsObject : public Object
{
protected:
    b2Body* m_physicsBody;  //TODO: Multiple of these for destructible objects

public:
    physicsObject(Image* img);
    ~physicsObject();

    void addFixture(b2FixtureDef* def)   {m_physicsBody->CreateFixture(def);};  //Add a physics fixture to this object
    void addBody(b2Body* body)   {body->SetUserData(this); m_physicsBody = body;};  //TODO Better way of doing this

    virtual void draw();
    virtual Point getCenter()    {Point pt = m_physicsBody->GetWorldCenter();pt*=SCALE_UP_FACTOR;return pt;};
    virtual b2Body* getBody() {return m_physicsBody;};

};









#endif
