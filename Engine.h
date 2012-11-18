/*
    Heartlight++ source - Engine.h
    Copyright (c) 2012 Mark Hutcheson
*/

#ifndef ENGINE_H
#define ENGINE_H

#include "globaldefs.h"
#include "Image.h"
#include "Object.h"
#include <map>

class Engine
{
private:

    //Variables for use by the engine
    HGE* m_hge;
    float32 m_fFramerate;
    float32 m_fAccumulatedTime;
    float32 m_fTargetTime;
    hgeSprite* m_sprFill;   //Sprite for filling a color
    map<string, Image*> m_mImages;  //Image handler
    map<string, HEFFECT> m_mSounds; //Sound handler
    multimap<uint32_t, Object*> m_mObjects;       //Object handler
    HCHANNEL m_MusicChannel;        //Sound channel we play our music on
    bool m_bFirstMusic; //Don't stop a previous song playing if there is none
    bool m_bQuitting;   //Stop the game if this turns true

    //Engine-use function definitions
    friend bool frameFunc();
    friend bool renderFunc();
    bool myFrameFunc();
    bool myRenderFunc();
    HEFFECT getEffect(string sFilename);

    Engine(){}; //Default constructor isn't callable

protected:

    //Classes to override in your own class definition
    virtual void frame() = 0;   //Function that's called every frame
    virtual void draw() = 0;    //Actual function that draws stuff
    virtual void init() = 0;    //So we can load all our images and such
    virtual void handleEvent(hgeInputEvent event) = 0;  //Function that's called for each HGE input event

public:
    //Constructor/destructor
    Engine(uint16_t iWidth, uint16_t iHeight, string sTitle);
    ~Engine();

    //Methods
    void start();   //Runs HGE and doesn't exit until the engine ends
    void fillRect(Point p1, Point p2, uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha); //Fill the specified rect with the specified color
    void fillRect(float32 x1, float32 y1, float32 x2, float32 y2, uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha);
    void fillRect(Rect rc, uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha);
    Image* getImage(string sFilename);  //Retrieves an image from the filename, creating it if need be
    void addObject(Object* obj);    //Add an object to the object handler
    void updateObjects();           //update all objects in the game
    void drawObjects(float fScale = 1.0);   //draw all objects in the game
    void clearObjects();    //Destroy all objects, freeing memory
    virtual void playSound(string sFilename, int volume = 100, int pan = 0, float pitch = 1.0);     //Play a sound
    void playMusic(string sFilename, int volume = 100, int pan = 0, float pitch = 1.0);     //Play looping music
    int32_t randInt(int32_t min, int32_t max) {return m_hge->Random_Int(min, max);};  //Get a random integer
    bool keyDown(int32_t keyCode);  //Test and see if a key is currently pressed
    void quit() {m_bQuitting = true;};  //Stop the engine and quit nicely
    void scaleImages(uint16_t scaleFac);    //scale all images by scaleFac
    float32 getTime()      {return m_hge->Timer_GetTime();}; //Get the time the engine's been running
    Rect getScreenRect()    {Rect rc = {0,0,getWidth(),getHeight()}; return rc;};

    //Accessor methods
    void setFramerate(float32 fFramerate);
    float32 getFramerate()   {return m_fFramerate;};
    uint16_t getWidth() {return m_hge->System_GetState(HGE_SCREENWIDTH);};
    uint16_t getHeight() {return m_hge->System_GetState(HGE_SCREENHEIGHT);};

};

bool frameFunc();
bool renderFunc();



#endif
