/*
    Heartlight++ source - myEngine.h
    Copyright (c) 2012 Mark Hutcheson
*/
#ifndef MYENGINE_H
#define MYENGINE_H

#include "Engine.h"
#include <vector>

#define GRID_WIDTH      32
#define GRID_HEIGHT     32

#define LEVEL_WIDTH     20
#define LEVEL_HEIGHT    12

class myEngine : public Engine
{
private:
    Image* m_imgHUD;
    Object* m_levelGrid[LEVEL_WIDTH][LEVEL_HEIGHT];    //Object grid for the game
    //list<string> m_lLevels;         //List of levels read in from LEVELS.HL
    //list<string>::iterator m_iCurrLevel;    //Current level we're playing

protected:
    bool frame();
    void draw();
    void init();

public:
list<string> m_lLevels;         //List of levels read in from LEVELS.HL
    list<string>::iterator m_iCurrLevel;    //Current level we're playing
    myEngine(uint16_t iWidth, uint16_t iHeight, string sTitle);
    ~myEngine();

    void loadLevel();   //Loads the current level m_iCurrLevel is pointing to into m_levelGrid[][]
    bool loadLevels(string sFilename);  //Loads in all the levels from the specified text file into m_lLevels
};



#endif
