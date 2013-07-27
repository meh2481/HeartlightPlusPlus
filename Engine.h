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
#include "Cursor.h"
#include "3DObject.h"
#include "Interpolate.h"
#include <map>

struct resolution
{
  int w;
  int h;
} ;

class Engine
{
private:
    //Variables for use by the engine
//    HGE* m_hge;
    b2World* m_physicsWorld;
    Cursor* m_cursor;
    Point m_ptCursorPos;
    bool  m_bShowCursor;
    float32 m_fFramerate;
    float32 m_fAccumulatedTime;
    float32 m_fTargetTime;
    map<string, Image*> m_mImages;  //Image handler
    map<string, string> m_mImageNames;  //And names of images
//    map<string, HEFFECT> m_mSounds; //Sound handler
    map<string, string> m_mSoundNames; //And names of sounds
    multimap<float32, Object*> m_mObjects;       //Object handler
    list<Interpolate*> m_lInterpolations;  //Keep track of stuff that's interpolating
//    HCHANNEL m_MusicChannel;        //Sound channel we play our music on
    bool m_bFirstMusic; //Don't stop a previous song playing if there is none
    string m_sLastMusic;    //Last song we played, so we can pause/resume songs instead of restarting them
    bool m_bQuitting;   //Stop the game if this turns true
    uint16_t m_iImgScaleFac;    //How much images are scaled up by
    uint16_t m_iWidth, m_iHeight;
    Uint8 *m_iKeystates;    //Keep track of keys that are pressed/released so we can poll as needed
    SDL_Rect** m_rcScreenModes; //Screen modes that are available
    int m_iNumScreenModes;      //Number of screen modes that are available
    bool m_bFullscreen;
    float32 m_fLastCycle;     //When the last cycle was

    //Engine-use function definitions
    bool _frame();
    void _render();
    
    void _interpolations(float32 dt); //update any interpolating variables
//    HEFFECT _getEffect(string sName);
    void setup_sdl();
    void setup_opengl();

    Engine(){}; //Default constructor isn't callable

protected:

    //Classes to override in your own class definition
    virtual void frame() = 0;   //Function that's called every frame
    virtual void draw() = 0;    //Actual function that draws stuff
    virtual void init() = 0;    //So we can load all our images and such
    virtual void handleEvent(SDL_Event event) = 0;  //Function that's called for each SDL input event

public:
    //Constructor/destructor
    Engine(uint16_t iWidth, uint16_t iHeight, string sTitle);
    ~Engine();

    //Methods
    void start();   //Runs engine and doesn't exit until the engine ends
    void fillRect(Point p1, Point p2, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha); //Fill the specified rect with the specified color
    void fillRect(float32 x1, float32 y1, float32 x2, float32 y2, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    void fillRect(Rect rc, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    Image* getImage(string sName);  //Retrieves an image from the name
    void createImage(string sPath, string sName); //Creates an image with this name from this file path
    void createSound(string sPath, string sName);   //Creates a sound from this name and file path
    void addObject(Object* obj);    //Add an object to the object handler
    void updateObjects();           //update all objects in the game
    void drawObjects();   //draw all objects in the game
    void clearObjects();    //Destroy all objects, freeing memory
    void clearImages();     //Free memory associated with the images in our image map
    virtual void playSound(string sName, int volume = 100, int pan = 0, float32 pitch = 1.0);     //Play a sound
    void playMusic(string sName, int volume = 100, int pan = 0, float32 pitch = 1.0);     //Play looping music, or resume paused music
    void pauseMusic();                                                                     //Pause music that's currently playing
    bool keyDown(int32_t keyCode);  //Test and see if a key is currently pressed
    void quit() {m_bQuitting = true;};  //Stop the engine and quit nicely
//    void scaleImages(uint16_t scaleFac);    //scale all images by scaleFac
    float32 getTime()      {return (float32)SDL_GetTicks()/1000.0;}; //Get the time the engine's been running
    Rect getScreenRect()    {Rect rc = {0,0,getWidth(),getHeight()}; return rc;};
    b2Body* createBody(b2BodyDef* bdef) {return m_physicsWorld->CreateBody(bdef);};
    void setCursor(Cursor* cur);
    Point getCursorPos()    {return m_ptCursorPos;};
    void setCursorPos(int32_t x, int32_t y);
    void setCursorPos(Point ptPos)  {setCursorPos(ptPos.x, ptPos.y);};
    void showCursor()   {m_bShowCursor = true;};
    void hideCursor()   {m_bShowCursor = false;};
    void setGravity(Point ptGravity)    {m_physicsWorld->SetGravity(ptGravity);};
    void setGravity(float32 x, float32 y)   {setGravity(Point(x,y));};
    void changeScreenResolution(float32 w, float32 h);  //Change resolution mid-game and reload OpenGL textures as needed
    void toggleFullscreen();                            //Switch between fullscreen/windowed modes
    list<resolution> getAvailableResolutions();         //Get available fullscreen resolutions
    void addInterpolation(Interpolate* inter);
    void clearInterpolations();

    //Accessor methods
    void setFramerate(float32 fFramerate);
    float32 getFramerate()   {return m_fFramerate;};
    uint16_t getWidth() {return m_iWidth;};
    uint16_t getHeight() {return m_iHeight;};

};

//bool frameFunc();
//bool renderFunc();



#endif
