/*
    Heartlight++ source - myEngine.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "myEngine.h"

//For HGE-based functions to be able to call our Engine class functions - Note that this means there can be no more than one Engine at a time
static myEngine* g_pGlobalEngine;

bool FrameFunc()
{
    if(g_pGlobalEngine->m_hge->Input_GetKeyState(HGEK_SPACE))
    {
        if(g_pGlobalEngine->m_hge->Input_KeyDown(HGEK_RIGHT))
        {
            g_pGlobalEngine->m_intCurrLevel++;
            if(g_pGlobalEngine->m_intCurrLevel == g_pGlobalEngine->m_vLevels.size())
            {
                g_pGlobalEngine->m_intCurrLevel = 0;
            }
            g_pGlobalEngine->loadLevel();
        }
        if(g_pGlobalEngine->m_hge->Input_KeyDown(HGEK_LEFT))
        {
            if(g_pGlobalEngine->m_intCurrLevel == 0)
            {
                g_pGlobalEngine->m_intCurrLevel = g_pGlobalEngine->m_vLevels.size();

            }
            g_pGlobalEngine->m_intCurrLevel--;
            g_pGlobalEngine->loadLevel();
        }
    }
    return g_pGlobalEngine->myFrameFunc();
}

bool RenderFunc()
{
    return g_pGlobalEngine->myRenderFunc();
}

bool myEngine::frame()
{
    UpdateObjects();
    return false;   //Do nothing for now
}

void myEngine::draw()
{
    //Just draw all objects
    DrawObjects();
    //And bottom bar
    m_imgHUD->Draw(0,getHeight() - m_imgHUD->GetHeight());
}

void myEngine::init()
{
    //Load all images and scale up
    Image* heart = getImage("res/gfx/orig/heart.png");
    heart->Scale(2);
    Image* rock = getImage("res/gfx/orig/rock.png");
    rock->Scale(2);
    Image* grass = getImage("res/gfx/orig/grass.png");
    grass->Scale(2);
    Image* dwarf = getImage("res/gfx/orig/dwarf.png");
    dwarf->Scale(2);
    Image* exitdoor = getImage("res/gfx/orig/door.png");
    exitdoor->Scale(2);
    Image* bomb = getImage("res/gfx/orig/bomb.png");
    bomb->Scale(2);
    Image* balloon = getImage("res/gfx/orig/balloon.png");
    balloon->Scale(2);
    Image* plasma = getImage("res/gfx/orig/plasma.png");
    plasma->Scale(2);
    Image* brick = getImage("res/gfx/orig/brick.png");
    brick->Scale(2);
    Image* metalwall = getImage("res/gfx/orig/metalwall.png");
    metalwall->Scale(2);
    Image* Ltunnel = getImage("res/gfx/orig/tunnelL.png");
    Ltunnel->Scale(2);
    Image* Rtunnel = getImage("res/gfx/orig/tunnelR.png");
    Rtunnel->Scale(2);

    if(!loadLevels("res/LEVELS.HL"))
    {
        errlog << "Aborting..." << endl;
        exit(1);    //Abort
    }
    loadLevel();
    setFramerate(10);
    m_imgHUD = getImage("res/gfx/orig/bottombar.png");
    m_imgHUD->Scale(2);
    /*Image* img = getImage("res/gfx/orig/plasma.png");
    img->Scale(2);
    for(int i = 0; i < 20; i++)
    {
        for(int j = 0; j < 12; j++)
        {
            Object* obj = new Object(img);
            obj->SetNumFrames(8);
            obj->SetPos(i*obj->GetWidth(), j*obj->GetHeight());
            AddObject(obj);
        }
    }*/
    //PlayMusic("res/sfx/orig/menu_music.ogg"); //Start playing menu music
    //PlaySound("res/sfx/orig/explode.ogg");
}

myEngine::myEngine(uint16_t iWidth, uint16_t iHeight, string sTitle) : Engine(iWidth, iHeight, sTitle)
{
    g_pGlobalEngine = this;
    m_imgHUD = NULL;
    m_intCurrLevel = 0;
}

myEngine::~myEngine()
{
}

void myEngine::loadLevel()
{
    //Grab all images
    Image* heart = getImage("res/gfx/orig/heart.png");
    Image* rock = getImage("res/gfx/orig/rock.png");
    Image* grass = getImage("res/gfx/orig/grass.png");
    Image* dwarf = getImage("res/gfx/orig/dwarf.png");
    Image* exitdoor = getImage("res/gfx/orig/door.png");
    Image* bomb = getImage("res/gfx/orig/bomb.png");
    Image* balloon = getImage("res/gfx/orig/balloon.png");
    Image* plasma = getImage("res/gfx/orig/plasma.png");
    Image* brick = getImage("res/gfx/orig/brick.png");
    Image* metalwall = getImage("res/gfx/orig/metalwall.png");
    Image* Ltunnel = getImage("res/gfx/orig/tunnelL.png");
    Image* Rtunnel = getImage("res/gfx/orig/tunnelR.png");
    if(m_intCurrLevel >= m_vLevels.size()) //At the end, when we shouldn't be
    {
        errlog << "No levels loaded! Abort. " << endl;
        exit(1);
    }
    ClearObjects(); //If there's any memory hanging around with objects, clear it out

    string s = m_vLevels[m_intCurrLevel];
    string::iterator it = s.begin();

    //Loop through, creating all objects
    for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
    {
        //errlog << endl;
        for(uint16_t col = 0; col < LEVEL_WIDTH+2; col++)
        {
            char cObj = *it++;
            bool bSkip = false;

            //Create object
            Object* obj;
            switch(cObj)
            {
                case ' ':
                    break;  //Ignore spaces

                case '$':   //Heart
                    obj = new Object(heart);
                    obj->SetNumFrames(6);
                    obj->SetFrame(randInt(0,5));    //randomize the frame it starts at
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;

                case '@':   //rock
                    obj = new Object(rock);
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;

                case '.':   //grass
                    obj = new Object(grass);
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;

                case '*':   //the dwarf
                    obj = new Dwarf(dwarf);
                    obj->SetNumFrames(8);
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;

                case '!':   //exit door
                    obj = new Door(exitdoor);
                    obj->SetNumFrames(4);
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;

                case '&':   //bomb
                    obj = new Object(bomb);
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;

                case '0':   //balloon
                    obj = new Object(balloon);
                    obj->SetNumFrames(4);
                    obj->SetFrame(randInt(0,3));
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;

                case '=':   //plasma
                    obj = new Object(plasma);
                    obj->SetNumFrames(8);
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;

                case '#':   //brick wall
                    obj = new Brick(brick);
                    obj->SetNumFrames(4);
                    obj->SetFrame(m_intCurrLevel % 4);  //Color depends on level number, like original game
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;

                case '%':   //metal wall
                    obj = new Object(metalwall);
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;
                case '<':   //left tunnel
                    obj = new Object(Ltunnel);
                    obj->SetNumFrames(4);
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;

                case '>':   //right tunnel
                    obj = new Object(Rtunnel);
                    obj->SetNumFrames(4);
                    obj->SetPos(col * GRID_WIDTH, row * GRID_HEIGHT);
                    AddObject(obj);
                    break;

                case '\n':
                    bSkip = true;
                    break;

                default:
                    errlog << "Warning: Invalid char \'" << cObj << "\'. Ignoring..." << endl;
                    break;  //Ignore
            }
            if(bSkip)
            {
                bSkip = false;
                break;  //Skip to next line
            }
        }
    }
}

bool myEngine::loadLevels(string sFilename)
{
    ifstream infile(sFilename.c_str(), ios_base::in | ios_base::binary);
    if(infile.fail())
    {
        errlog << "Error: Unable to open level definition file " << sFilename << endl;
        return false;    //Abort
    }
    //Loop until we hit eof or fail
    while(!infile.eof() && !infile.fail())
    {
        static int iLevelNum = 0;
        iLevelNum++;
        //Spin through characters, ignoring them, until we hit the next '{' character
        for(;;)
        {
            if(infile.eof() || infile.fail())   //Hit end of file
                break;
            char c = infile.get();
            if(c == '{')
            {
                c=infile.get(); //Skip over \n char
                c=infile.get(); //Skip over \n char
                break;
            }
        }

        if(infile.eof() || infile.fail())   //Hit end of file
            break;

        //Now read in the string until we hit '}' (we don't care about the string length here)
        string s;
        for(;;)
        {
            if(infile.eof() || infile.fail())   //Hit end of file
            {
                errlog << "No final terminating } character at end of " << sFilename << endl;
                return false;
            }
            char c = infile.get();
            if(c == '\r' || c == '\t')
                continue;
            if(c == '}')
                break;
            s.push_back(c); //Tack this onto the end of the string
        }

        //Add this string to our list of levels
        m_vLevels.push_back(s);
    }

    //Done
    infile.close();
    m_intCurrLevel = 0;   //Start from top
    if(m_intCurrLevel == m_vLevels.size()) //No levels?
    {
        errlog << "No levels in file " << sFilename << endl;
        return false;
    }
    return true;
}












