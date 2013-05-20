#ifndef BALLGUN_H
#define BALLGUN_H

#include "Object.h"

//Standard gun that fires small bullets; little to no kickback
class ballGun
{
protected:
    Point   m_ptCursorPos;
    list<physicsObject*> m_lFired;
    bool    m_bAutomatic;
    bool    m_bMouseDown;
    float32 m_fLastFired;
    float32 m_fCurTime;

public:
    string      icon;   //Icon, to be drawn in upper left corner of screen
    string      gun;    //Gun image, to be drawn over obj
    string      bullet; //Image for bullet we'll create
    Object*     obj;    //Object that has gun
    uint32_t    num;    //How many objects can be created before some start being destroyed
    float32     delay;  //How much time between shots

    ballGun();
    ~ballGun();

    virtual void mouseDown(float32 x, float32 y);
    virtual void mouseUp(float32 x, float32 y);
    virtual void mouseMove(float32 x, float32 y)     {m_ptCursorPos.Set(x,y);};
    virtual void fire();
    void draw(Rect rcScreen);
    void clear();
    virtual void update(float32 fCurTime);
};

//Gun that creates an object where the mouse is clicked
class placeGun : public ballGun
{
public:
    placeGun();
    ~placeGun();

    //void mouseDown(float32 x, float32 y);
    void fire();

    float32 dist;   //Max distance you can place an object at
};

//Gun that fires large objects, with lots of kickback (enough to propel you through the air)
class blastGun : public ballGun
{
public:
    blastGun();
    ~blastGun();

    //void mouseDown(float32 x, float32 y);
    void fire();
};

//Gun that fires a spread of smaller balls
class shotgun : public ballGun
{
public:
    shotgun();
    ~shotgun();

    //void mouseDown(float32 x, float32 y);
    void fire();
};

//Gun that fires a ton of bullets liek a baws
class machineGun : public ballGun
{
public:
    machineGun();
    ~machineGun();

    //void mouseDown(float32 x, float32 y);
    void fire();
};

//Telekinesis powahz!
class teleGun : public ballGun
{
public:
    teleGun();
    ~teleGun();

    void mouseDown(float32 x, float32 y);
    void mouseUp(float32 x, float32 y);
    void mouseMove(float32 x, float32 y);
    void fire();
};

class superGun : public ballGun
{
public:
    superGun();
    ~superGun();

    void fire();
};

//#ifndef SCALE_FAC
//#define SCALE_FAC 1
//#endif
#define MAX_CREATED_SPHERES     10
#define SHOOT_VEL               1000.0
#define MASS_FAC                1000.0
#define DEFAULT_GUN_LENGTH      1.0
#define GUN_ICON_DRAW_OFFSET    5

extern b2Body*  createBody(b2BodyDef* def);
extern Image*   getImage(string sName);
extern void     addObject(Object* obj);
extern void     playSound(string sName, int volume = 100, int pan = 0, float32 pitch = 1.0);
void pew();
























#endif
