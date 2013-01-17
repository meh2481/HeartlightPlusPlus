/*
    Heartlight++ source - myEngine.h
    Copyright (c) 2012 Mark Hutcheson
*/
#ifndef MYENGINE_H
#define MYENGINE_H

#include "Engine.h"
#include <vector>

#define GRID_WIDTH      16
#define GRID_HEIGHT     16

#define LEVEL_WIDTH     20
#define LEVEL_HEIGHT    12

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   200

#define SCALE_FAC       2   //By what factor the retro gfx are scaled TODO: Variable

#define WIN_COUNT       14  //How many frames the dwarf jumps up and down until you go to the next level
#define DIE_COUNT       20  //How many frames the dwarf holds his head before he explodes
#define GAME_FRAMERATE  9   //How many fps the game normally runs at

#define FADE_NONE       0
#define FADE_IN         1
#define FADE_OUT        2
#define FADE_TIME       0.5

//Bitfields for object data
#define BOMB_EXPLODEDELAY1  1
#define BOMB_EXPLODEDELAY2  2
#define BALLOON_FLOATDELAY  4
#define FALLING_1           8
#define FALLING_2           16

#define RETRO   false   //If we're playing in retro mode or not TODO: Variable

class myEngine : public Engine
{
private:
    retroObject* m_levelGrid[LEVEL_WIDTH][LEVEL_HEIGHT];    //Object grid for the game
    retroObject* m_oldGrid[LEVEL_WIDTH][LEVEL_HEIGHT];  //And old positions for use when updating
    vector<string> m_vLevels;         //List of levels read in from LEVELS.HL
    map<uint32_t, bool> m_mLevelsBeaten;    //List of levels we've beaten
    uint32_t m_iCurrentLevel;    //And integer of it
    uint32_t m_iHeartsTotal;    //How many hearts total there are in this level
    uint32_t m_iCollectedHearts;    //how many hearts in this level we've collected
    uint16_t m_iWinningCount;   //Counts down when winning a level
    uint16_t m_iDyingCount; //Counts down when dying, before you explode_retro
    bool m_bTunnelMoved;    //Haven't moved in a tunnel (need this for shallow copy issues)
    //Variables for fading the screen to/from black
    float32 m_fEndFade;
    uint8_t m_iFade;
    bool m_bDebug;  //Whether to draw debug data and stuff or not
    HUD* m_hud;
    bool m_bSound, m_bMusic, m_bRad;    //Booleans for music settings and such
    bool m_bJumped; //For supercomplex movement stuff
    Rect m_rcViewScreen;    //Screen we can currently see
    bool m_bDragScreen;
    bool m_bScaleScreen;
    Point m_ptLastMousePos;

    list<physicsObject*> m_lSpheres_new;

    bool _moveToGridSquare_retro(int row, int col);   //Check this pos in the grid before we move there, and clear contents if we can (if return true, player has been destroyed)
    physicsObject* m_objTest;
    Cursor* m_cur;

protected:
    void frame();
    void draw();
    void init();
    void handleEvent(hgeInputEvent event);

public:
    myEngine(uint16_t iWidth, uint16_t iHeight, string sTitle);
    ~myEngine();

    void loadLevel_retro();   //Loads the current level m_iCurrLevel is pointing to into m_levelGrid[][]
    void loadLevel_new();       //Loads the current level as a physics-based fun play area thing
    bool loadLevels(string sFilename);  //Loads in all the levels from the specified text file into m_vLevels
    void loadLevelDirectory(string sFilePath);  //Loads all the level files in the specified folder
    void updateGrid_retro();  //Updates the level grid, moving objects around
    void updateGrid_new();  //Update objects and such
    void shoot_new(float32 x, float32 y);       //FIREN TEH LAZOR
    void place_new(float32 x, float32 y);
//    void checkSpheresHitting_new();
    void loadImages(string sListFilename);  //Loads all images listed in this file into memory, so we can batch load easily
    void loadSounds(string sListFilename);  //Loads all sounds listed in this file into memory
    void playSound(string sName);   //Plays a sound, with pitch shifting depending on framerate
    void explode_retro(uint16_t row, uint16_t col, bool bStartFrame1 = false);    //For handling bomb explosions tile by tile
    bool floatable_retro(retroObject* obj);   //If this object can be pushed upwards by a balloon
    void hudSignalHandler(string sSignal);  //For handling signals that come from the HUD
    bool isOnGround();  //See if the player is on the ground
};

void signalHandler(string sSignal); //Stub function for handling signals that come in from our HUD, and passing them on to myEngine


#endif
