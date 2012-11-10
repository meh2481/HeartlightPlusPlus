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

#define SCALE_FAC       2   //By what factor the retro gfx are scaled

#define WIN_COUNT       14  //How many frames the dwarf jumps up and down until you go to the next level
#define GAME_FRAMERATE  9   //How many fps the game normally runs at

class myEngine : public Engine
{
private:
    Image* m_imgHUD;
    retroObject* m_levelGrid[LEVEL_WIDTH][LEVEL_HEIGHT];    //Object grid for the game
    retroObject* m_oldGrid[LEVEL_WIDTH][LEVEL_HEIGHT];  //And old positions for use when updating
    vector<string> m_vLevels;         //List of levels read in from LEVELS.HL
    uint32_t m_iCurrentLevel;    //And integer of it
    uint32_t m_iHeartsTotal;    //How many hearts total there are in this level
    uint32_t m_iCollectedHearts;    //how many hearts in this level we've collected
    uint16_t m_iWinningCount;   //Counts down when winning a level
    bool m_bTunnelMoved;    //Haven't moved in a tunnel (need this for shallow copy issues)

    bool checkGrid(int row, int col);   //Check this pos in the grid before we move there, and clear contents if we can (if return true, player has been destroyed)

protected:
    void frame();
    void draw();
    void init();
    void handleEvent(hgeInputEvent event);

public:
    myEngine(uint16_t iWidth, uint16_t iHeight, string sTitle);
    ~myEngine();

    void loadLevel();   //Loads the current level m_iCurrLevel is pointing to into m_levelGrid[][]
    bool loadLevels(string sFilename);  //Loads in all the levels from the specified text file into m_vLevels
    void updateGrid();  //Updates the level grid, moving objects around
    void loadImages(string sListFilename);  //Loads all images listed in this file into memory, so we can batch load easily
    void playSound(string sFilename);   //Plays a sound, with pitch shifting depending on framerate
};



#endif
