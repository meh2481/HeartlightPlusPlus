/*
    Heartlight++ source - Engine.h
    Copyright (c) 2012 Mark Hutcheson
*/

#ifndef ENGINE_H
#define ENGINE_H

#include "globaldefs.h"
#include "Image.h"
#include "Object.h"
#include "Text.h"
#include "hud.h"
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
    map<string, string> m_mImageNames;  //And names of images
    map<string, HEFFECT> m_mSounds; //Sound handler
    map<string, string> m_mSoundNames; //And names of sounds
    multimap<uint32_t, Object*> m_mObjects;       //Object handler
    HCHANNEL m_MusicChannel;        //Sound channel we play our music on
    bool m_bFirstMusic; //Don't stop a previous song playing if there is none
    string m_sLastMusic;    //Last song we played, so we can pause/resume songs instead of restarting them
    bool m_bQuitting;   //Stop the game if this turns true
    uint16_t m_iImgScaleFac;    //How much images are scaled up by

    //Engine-use function definitions
    friend bool frameFunc();
    friend bool renderFunc();
    bool _myFrameFunc();
    bool _myRenderFunc();
    HEFFECT _getEffect(string sName);

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
    void fillRect(Point p1, Point p2, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha); //Fill the specified rect with the specified color
    void fillRect(float32 x1, float32 y1, float32 x2, float32 y2, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    void fillRect(Rect rc, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    Image* getImage(string sName);  //Retrieves an image from the name
    void createImage(string sPath, string sName); //Creates an image with this name from this file path
    void createSound(string sPath, string sName);   //Creates a sound from this name and file path
    void addObject(Object* obj);    //Add an object to the object handler
    void updateObjects();           //update all objects in the game
    void drawObjects(float32 fScale = 1.0);   //draw all objects in the game
    void clearObjects();    //Destroy all objects, freeing memory
    void clearImages();     //Free memory associated with the images in our image map
    virtual void playSound(string sName, int volume = 100, int pan = 0, float32 pitch = 1.0);     //Play a sound
    void playMusic(string sName, int volume = 100, int pan = 0, float32 pitch = 1.0);     //Play looping music, or resume paused music
    void pauseMusic();                                                                     //Pause music that's currently playing
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
