/*
    Heartlight++ source - myEngine.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "myEngine.h"

//For HGE-based functions to be able to call our Engine class functions - Note that this means there can be no more than one Engine at a time
static myEngine* g_pGlobalEngine;

bool frameFunc()
{
    return g_pGlobalEngine->myFrameFunc();
}

bool renderFunc()
{
    return g_pGlobalEngine->myRenderFunc();
}

void myEngine::frame()
{
    updateGrid();
    updateObjects();
    if(m_iWinningCount) //If winning a level, make dwarf jump up and down
    {
        if(--m_iWinningCount)
        {
            //Make gnome jump up and down
            for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
            {
                for(uint16_t col = 0; col < LEVEL_WIDTH; col++)
                {
                    if(m_levelGrid[col][row] == NULL)
                        continue;
                    if(m_levelGrid[col][row]->getNameChar() == '*')
                    {
                        m_levelGrid[col][row]->setFrame(m_iWinningCount % 2 + 6);
                    }
                }
            }
        }
        else    //End of winning count
        {
            m_iCurrentLevel++;  //Go to next level
            if(m_iCurrentLevel >= m_vLevels.size())
                m_iCurrentLevel = 0;
            loadLevel();
        }
    }
}

void myEngine::draw()
{
    //Just draw all objects
    drawObjects();
    //And bottom bar
    m_imgHUD->draw(0,getHeight() - m_imgHUD->getHeight());
}

void myEngine::init()
{
    //Load all images, so we can scale all of them up from the start
    loadImages("res/gfx/orig.txt");

    m_imgHUD = getImage("res/gfx/orig/bottombar.png");  //Hang onto this image

    //Now scale all the images up
    scaleImages(SCALE_FAC);

    if(!loadLevels("res/LEVELS.HL"))
    {
        errlog << "Aborting..." << endl;
        exit(1);    //Abort
    }
    loadLevel();
    //m_imgHUD->scale(SCALE_FAC);
    //playMusic("res/sfx/orig/menu_music.ogg"); //Start playing menu music
}

myEngine::myEngine(uint16_t iWidth, uint16_t iHeight, string sTitle) : Engine(iWidth, iHeight, sTitle)
{
    g_pGlobalEngine = this;
    m_imgHUD = NULL;
    m_iCurrentLevel = 0;
    m_iWinningCount = 0;
}

myEngine::~myEngine()
{
}

void myEngine::loadImages(string sListFilename)
{
    ifstream infile(sListFilename.c_str());
    if(infile.fail())
    {
        errlog << "Couldn't open image list file " << sListFilename << " for reading. Abort." << endl;
        exit(1);
    }
    while(!infile.fail() && !infile.eof())
    {
        char name[256];
        infile.getline(name, 256);
        string s(name);
        if(s.size())
            getImage(s);    //load this image
    }
    infile.close();
}

void myEngine::handleEvent(hgeInputEvent event)
{
    switch(event.type)
    {
        //Key pressed
        case INPUT_KEYDOWN:
            switch(event.key)
            {
                case HGEK_RIGHT:
                    if(keyDown(HGEK_SPACE)) //Space-right goes to next level
                    {
                        m_iCurrentLevel++;
                        if(m_iCurrentLevel >= m_vLevels.size())
                            m_iCurrentLevel = 0;
                        loadLevel();
                    }
                    break;

                case HGEK_LEFT:
                    if(keyDown(HGEK_SPACE)) //Space-left goes to previous level
                    {
                        if(m_iCurrentLevel == 0)
                            m_iCurrentLevel = m_vLevels.size();
                        m_iCurrentLevel--;
                        loadLevel();
                    }
                    break;

                case HGEK_ESCAPE:
                    //For now, just reload level
                    loadLevel();
                    break;

                case HGEK_F11:      //F11: Decrease fps
                    setFramerate(std::max(getFramerate()-1.0,0.0));
                    break;

                case HGEK_F12:      //F12: Increase fps
                    setFramerate(getFramerate()+1.0);
                    break;
            }
            break;

        //Key released
        case INPUT_KEYUP:
            switch(event.key)
            {

            }
            break;
    }
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
    if(m_iCurrentLevel >= m_vLevels.size()) //At the end, when we shouldn't be
    {
        errlog << "No levels loaded! Abort. " << endl;
        exit(1);
    }
    clearObjects(); //If there's any memory hanging around with objects, clear it out
    m_iHeartsTotal = 0; //No hearts in this level yet
    m_iCollectedHearts = 0;     //And we've collected none
    //And clear out our map
    for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
    {
        for(uint16_t col = 0; col < LEVEL_WIDTH; col++)
            m_levelGrid[col][row] = NULL;
    }

    string s = m_vLevels[m_iCurrentLevel];
    string::iterator it = s.begin();

    //Loop through, creating all objects
    for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
    {
        //errlog << endl;
        for(uint16_t col = 0; col < LEVEL_WIDTH+2; col++)   //+2 here for newlines. Rgh newlines
        {
            char cObj = *it++;
            bool bSkip = false;

            //Create object
            switch(cObj)
            {
                case ' ':
                    break;  //Ignore spaces

                case '$':   //Heart
                    m_iHeartsTotal++;   //Increment the total number of hearts in this level
                    m_levelGrid[col][row] = new retroObject(heart);
                    m_levelGrid[col][row]->setNumFrames(6);
                    m_levelGrid[col][row]->setFrame(randInt(0,5));    //randomize the frame it starts at
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '@':   //rock
                    m_levelGrid[col][row] = new retroObject(rock);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '.':   //grass
                    m_levelGrid[col][row] = new retroObject(grass);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '*':   //the dwarf
                    m_levelGrid[col][row] = new Dwarf(dwarf);
                    m_levelGrid[col][row]->setNumFrames(8);
                    //Set to facing left if on right side of the screen
                    if(col >= LEVEL_WIDTH/2)
                        m_levelGrid[col][row]->setFrame(1);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '!':   //exit door
                    m_levelGrid[col][row] = new Door(exitdoor);
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '&':   //bomb
                    m_levelGrid[col][row] = new retroObject(bomb);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '0':   //balloon
                    m_levelGrid[col][row] = new retroObject(balloon);
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setFrame(randInt(0,3));
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '=':   //plasma
                    m_levelGrid[col][row] = new retroObject(plasma);
                    m_levelGrid[col][row]->setNumFrames(8);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '#':   //brick wall
                    m_levelGrid[col][row] = new Brick(brick);
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setFrame(m_iCurrentLevel % 4);  //Color depends on level number, like original game
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '%':   //metal wall
                    m_levelGrid[col][row] = new retroObject(metalwall);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;
                case '<':   //left tunnel
                    m_levelGrid[col][row] = new retroObject(Ltunnel);
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '>':   //right tunnel
                    m_levelGrid[col][row] = new retroObject(Rtunnel);
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
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

    //Check and see if there are no hearts in this level
    if(m_iCollectedHearts == m_iHeartsTotal)
    {
        //Open all doors
        for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
        {
            for(uint16_t col = 0; col < LEVEL_WIDTH; col++)
            {
                if(m_levelGrid[col][row] == NULL)
                    continue;
                if(m_levelGrid[col][row]->getNameChar() == '!')
                {
                    m_levelGrid[col][row]->setFrame(1);
                    playSound("res/sfx/orig/door_open.ogg");
                }
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
    m_iCurrentLevel = 0;   //Start from top
    if(m_iCurrentLevel == m_vLevels.size()) //No levels?
    {
        errlog << "No levels in file " << sFilename << endl;
        return false;
    }
    return true;
}

bool myEngine::checkGrid(int row, int col)
{
    if(row < 0 ||
       col < 0 ||
       row >= LEVEL_HEIGHT ||
       col >= LEVEL_WIDTH)
        return false; //We're at the edge of the map -- ignore
    retroObject* obj = m_levelGrid[col][row];
    if(obj == NULL || keyDown(HGEK_SPACE))
        return false; //Nothing to do here
    switch(obj->getNameChar())
    {
        case '$':   //Heart
            m_iCollectedHearts++;
            if(m_iCollectedHearts == m_iHeartsTotal)    //Collected all hearts; open the doors
            {
                //Open all doors
                for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
                {
                    for(uint16_t col = 0; col < LEVEL_WIDTH; col++)
                    {
                        if(m_levelGrid[col][row] == NULL)
                            continue;
                        if(m_levelGrid[col][row]->getNameChar() == '!')
                        {
                            m_levelGrid[col][row]->setFrame(1);
                            playSound("res/sfx/orig/door_open.ogg");
                        }
                    }
                }
            }
            else    //Otherwise, play getting heart noise
                playSound("res/sfx/orig/get_heart.ogg");
            m_levelGrid[col][row]->kill();
            m_levelGrid[col][row] = NULL;
            m_oldGrid[col][row] = NULL;
            break;

        case '.':   //Grass- just destroy
            m_levelGrid[col][row]->kill();
            m_levelGrid[col][row] = NULL;
            m_oldGrid[col][row] = NULL;
            break;

        case '!':
            if(m_levelGrid[col][row]->getFrame() == 3)  //If open
            {
                m_levelGrid[col][row]->kill();
                m_levelGrid[col][row] = NULL;
                m_oldGrid[col][row] = NULL;
                m_iWinningCount = WIN_COUNT+2;  //+2 here because we'll miss a couple frames
                playSound("res/sfx/orig/applause.ogg");
            }
            break;

        //Tunnels
        case '>':   //Tunnel to the right
        {
            //See if tunnel is obstructed
            bool bObstructed = true;
            for(uint16_t coltest = col+1; coltest < LEVEL_WIDTH; coltest++)
            {
                if(coltest == LEVEL_WIDTH)
                    break;
                retroObject* tunnelTest = m_oldGrid[coltest][row];
                if(tunnelTest == NULL || tunnelTest->getNameChar() == '.')
                {
                    bObstructed = false;    //We can go this way if it's just grass or empty space
                    break;
                }
                else if(tunnelTest->getNameChar() != '>')
                    break;  //Obstructed
            }
            if(bObstructed)
                break;  //We can't go through this tunnel (also will prevent you from going the wrong way through tunnels)

            //Enter the tunnel
            obj->setImage(getImage("res/gfx/orig/tunnelR_up.png"));
            obj->setName(">*"); //Player is in here
            m_bTunnelMoved = true;  //Entered this tunnel
            playSound("res/sfx/orig/subway.ogg");

            //Destroy the current dwarf object
            for(uint32_t i = 0; i < LEVEL_HEIGHT; i++)
            {
                for(uint32_t j = 0; j < LEVEL_WIDTH; j++)
                {
                    if(m_levelGrid[j][i] != NULL && m_levelGrid[j][i]->getNameChar() == '*')
                    {
                        m_levelGrid[j][i]->kill();
                        m_levelGrid[j][i] = NULL;
                    }
                }
            }
            return true;
        }

        case '<':   //Tunnel to the left
        {
            //See if tunnel is obstructed
            bool bObstructed = true;
            for(uint16_t coltest = col-1; coltest > 0; coltest--)
            {
                if(coltest <= 0)
                    break;
                retroObject* tunnelTest = m_oldGrid[coltest][row];
                if(tunnelTest == NULL || tunnelTest->getNameChar() == '.')
                {
                    bObstructed = false;    //We can go this way if it's just grass or empty space
                    break;
                }
                else if(tunnelTest->getNameChar() != '<')
                    break;  //Obstructed
            }
            if(bObstructed)
                break;  //We can't go through this tunnel (also will prevent you from going the wrong way through tunnels)

            //Enter the tunnel
            obj->setImage(getImage("res/gfx/orig/tunnelL_up.png"));
            obj->setName("<*"); //Player is in here
            playSound("res/sfx/orig/subway.ogg");

            //Destroy the current dwarf object
            for(uint32_t i = 0; i < LEVEL_HEIGHT; i++)
            {
                for(uint32_t j = 0; j < LEVEL_WIDTH; j++)
                {
                    if(m_levelGrid[j][i] != NULL && m_levelGrid[j][i]->getNameChar() == '*')
                    {
                        m_levelGrid[j][i]->kill();
                        m_levelGrid[j][i] = NULL;
                    }
                }
            }

            return true;
        }

        case '@':
        case '&':   //Push bombs or rocks
        case '0':   //or balloons
            if(col > 0 &&
               m_levelGrid[col-1][row] != NULL &&
               m_levelGrid[col-1][row]->getNameChar() == '*' &&
               m_levelGrid[col-1][row]->getFrame() < 2 &&
               keyDown(HGEK_RIGHT) && !keyDown(HGEK_SPACE))    //Pushing to the right
            {
                if(col < LEVEL_WIDTH-1 && m_levelGrid[col+1][row] == NULL)  //There's an empty space for us to push it
                {
                    //Push it over
                    m_levelGrid[col+1][row] = m_levelGrid[col][row];
                    m_levelGrid[col+1][row]->offset(GRID_WIDTH*SCALE_FAC,0);
                    m_levelGrid[col][row] = NULL;
                    m_oldGrid[col][row] = NULL;
                }
            }
            else if(col < LEVEL_WIDTH-1 &&
                    m_levelGrid[col+1][row] != NULL &&
                    m_levelGrid[col+1][row]->getNameChar() == '*' &&
                    m_levelGrid[col+1][row]->getFrame() < 2 &&
                    keyDown(HGEK_LEFT) && !keyDown(HGEK_SPACE))    //Pushing to the left
            {
                if(col > 0 && m_levelGrid[col-1][row] == NULL)
                {
                    m_levelGrid[col-1][row] = m_levelGrid[col][row];
                    m_levelGrid[col-1][row]->offset(-GRID_WIDTH*SCALE_FAC,0);
                    m_levelGrid[col][row] = NULL;
                    m_oldGrid[col][row] = NULL;
                }
            }
            break;

    }
    return false;
}

void myEngine::explode(uint16_t row, uint16_t col, bool bStartFrame1)
{
    if(row < 0 ||
       row > LEVEL_HEIGHT-1 ||
       col < 0 ||
       col > LEVEL_WIDTH-1)
    {
        return;     //Off grid = nothing
    }
    retroObject* obj = m_levelGrid[col][row];
    if(obj == NULL) //If null, go ahead and create explosion
    {
        m_levelGrid[col][row] = new retroObject(getImage("res/gfx/orig/explosion.png"));
        m_levelGrid[col][row]->setName('X');
        m_levelGrid[col][row]->setNumFrames(7);
        if(bStartFrame1)
            m_levelGrid[col][row]->setFrame(1); //Skip first frame
        m_levelGrid[col][row]->setPos(col*GRID_WIDTH*SCALE_FAC, row*GRID_HEIGHT*SCALE_FAC);
        addObject(m_levelGrid[col][row]);
    }
    else
    {
        switch(obj->getNameChar())
        {
            case '.':
            case '@':
            case '=':
            case '#':
            case '$':
            case '!':
            case '*':
                obj->kill();
                m_levelGrid[col][row] = new retroObject(getImage("res/gfx/orig/explosion.png"));
                m_levelGrid[col][row]->setName('X');
                m_levelGrid[col][row]->setNumFrames(7);
                if(bStartFrame1)
                    m_levelGrid[col][row]->setFrame(1); //Skip first frame
                m_levelGrid[col][row]->setPos(col*GRID_WIDTH*SCALE_FAC, row*GRID_HEIGHT*SCALE_FAC);
                addObject(m_levelGrid[col][row]);
                break;
            case '&':
                if(bStartFrame1)
                    obj->setVelocity(0,42);
                else
                    obj->setVelocity(0,36);
                return;

        }
    }
}

void myEngine::updateGrid() //Workhorse for updating the objects in the game
{
    m_bTunnelMoved = false;
    for(int row = 0; row < LEVEL_HEIGHT; row++)
    {
        for(int col = 0; col < LEVEL_WIDTH; col++)
            m_oldGrid[col][row] = m_levelGrid[col][row];
    }

    for(int row = 0; row < LEVEL_HEIGHT; row++)
    {
        for(int col = 0; col < LEVEL_WIDTH; col++)
        {
            retroObject* obj = m_oldGrid[col][row];
            if(obj == NULL)
                continue;   //ignore empty spaces

            string s = obj->getName();
            switch(*(s.begin()))
            {
                case 'x':   //exploding bomb
                    obj->kill();
                    m_levelGrid[col][row] = new retroObject(getImage("res/gfx/orig/explosion.png"));
                    m_levelGrid[col][row]->setName('X');
                    m_levelGrid[col][row]->setNumFrames(7);
                    m_levelGrid[col][row]->setFrame(1); //Skip first frame
                    m_levelGrid[col][row]->setPos(col*GRID_WIDTH*SCALE_FAC, row*GRID_HEIGHT*SCALE_FAC);
                    addObject(m_levelGrid[col][row]);
                    explode(row+1, col, true);
                    explode(row, col+1, true);
                    explode(row-1, col);
                    explode(row, col-1);
                case 'X':   //explosion
                    if(obj->getFrame() == 6)
                    {
                        obj->kill();
                        m_levelGrid[col][row] = NULL;
                    }
                    break;
                case '$':   //heart
                case '@':   //rock
                case '&':   //bomb
                    if(obj->getVelocity().y == 36)
                    {
                        //Explode
                        obj->setImage(getImage("res/gfx/orig/bombexplode.png"));
                        obj->setName('x');
                        playSound("res/sfx/orig/explode.ogg");
                        break;
                    }
                    else if(obj->getVelocity().y == 42)
                    {
                        obj->setVelocity(0,36);
                        break;
                    }

                    if((row == LEVEL_HEIGHT-1 || m_oldGrid[col][row+1] != NULL) && obj->getVelocity().y > 0)   //Hitting something
                    {
                        if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1]->getNameChar() == '*') //Hit player
                        {
                            //kill player
                            if(m_levelGrid[col][row+1]->getFrame() < 4) //But only if not dying or winning
                            {
                                m_levelGrid[col][row+1]->kill();
                                m_levelGrid[col][row+1] = new retroObject(getImage("res/gfx/orig/explosion.png"));
                                m_levelGrid[col][row+1]->setName('X');
                                m_levelGrid[col][row+1]->setNumFrames(7);
                                m_levelGrid[col][row+1]->setPos(col*GRID_WIDTH*SCALE_FAC, (row+1)*GRID_HEIGHT*SCALE_FAC);
                                addObject(m_levelGrid[col][row+1]);
                                playSound("res/sfx/orig/explode.ogg");
                            }
                            float32 fYvel = obj->getVelocity().y;
                            if(fYvel > 0)
                                obj->setVelocity(0,fYvel-1);  //Hitting something, stop falling

                        }
                        else  //Hit ground
                        {
                            if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1]->getNameChar() == '.' && obj->getVelocity().y > 1) //Hit grass
                            {
                                if(*(s.begin()) == '$') //Heart hit
                                    playSound("res/sfx/orig/heart_hit_grass.ogg");
                                else
                                    playSound("res/sfx/orig/hit_grass.ogg");
                            }
                            else if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1]->getNameChar() == '&' && obj->getVelocity().y > 1)    //Hit a bomb
                            {
                                //Explode
                                m_oldGrid[col][row+1]->setVelocity(0,36);
                                //m_oldGrid[col][row+1]->setImage(getImage("res/gfx/orig/bombexplode.png"));
                                //m_oldGrid[col][row+1]->setName('x');    //Explode
                                //playSound("res/sfx/orig/explode.ogg");
                                if(*(s.begin()) == '&')    //bomb hit
                                {
                                    //Explode
                                    //obj->setVelocity(0,36);
                                    obj->setImage(getImage("res/gfx/orig/bombexplode.png"));
                                    obj->setName('x');
                                    playSound("res/sfx/orig/explode.ogg");
                                }
                            }
                            else if(obj->getVelocity().y > 1)
                            {
                                if(*(s.begin()) == '$') //Heart hit
                                {
                                    char cName[256];
                                    sprintf(cName, "res/sfx/orig/heart_hit%d.ogg", randInt(1,4));
                                    playSound(string(cName));   //Random hit noise
                                }
                                else if(*(s.begin()) == '&')    //bomb hit
                                {
                                    //Explode
                                    obj->setImage(getImage("res/gfx/orig/bombexplode.png"));
                                    obj->setName('x');
                                    playSound("res/sfx/orig/explode.ogg");
                                }
                                else    //rock hit
                                    playSound("res/sfx/orig/rock_hit.ogg");
                            }
                            float32 fYvel = obj->getVelocity().y;
                            if(fYvel > 0)
                                obj->setVelocity(0,fYvel-1);  //Hitting something, stop falling
                        }
                    }
                    else if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1] == NULL)   //Fall down
                    {
                        obj->offset(0,GRID_HEIGHT*SCALE_FAC);
                        m_levelGrid[col][row+1] = obj;
                        m_levelGrid[col][row] = NULL;
                        obj->setVelocity(0,2);
                    }
                    if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1] != NULL)   //Check and see if we should move over to fall
                    {
                        //Check and see if we're on top of an object we can slide off of
                        char cObj = m_levelGrid[col][row+1]->getNameChar();
                        if(cObj == '$' ||
                           cObj == '@' ||
                           cObj == '&' ||
                           cObj == '=' ||
                           cObj == '#' ||
                           cObj == 'x' ||
                           cObj == 'X')
                        {
                            //Check left side first, as per game behavior
                            if(col > 0 && m_levelGrid[col-1][row] == NULL && m_levelGrid[col-1][row+1] == NULL && obj->getVelocity().y == 0)
                            {
                                if(row < 1 || m_levelGrid[col-1][row-1] == NULL || (m_levelGrid[col-1][row-1]->getNameChar() != '$' &&
                                                                                    m_levelGrid[col-1][row-1]->getNameChar() != '&' &&
                                                                                    m_levelGrid[col-1][row-1]->getNameChar() != '@'))    //Also check above for falling objects
                                {
                                    m_levelGrid[col-1][row] = obj;
                                    obj->offset(-GRID_WIDTH*SCALE_FAC,0);
                                    m_levelGrid[col][row] = NULL;   //Move over
                                }
                            }
                            //Then check right side
                            else if(col < LEVEL_WIDTH-1 && m_levelGrid[col+1][row] == NULL && m_levelGrid[col+1][row+1] == NULL && obj->getVelocity().y == 0)
                            {
                                if(row < 1 || m_levelGrid[col+1][row-1] == NULL || (m_levelGrid[col+1][row-1]->getNameChar() != '$' &&
                                                                                    m_levelGrid[col+1][row-1]->getNameChar() != '&' &&
                                                                                    m_levelGrid[col+1][row-1]->getNameChar() != '@'))    //Also check above for falling objects
                                {
                                    m_levelGrid[col+1][row] = obj;
                                    obj->offset(GRID_WIDTH*SCALE_FAC,0);
                                    m_levelGrid[col][row] = NULL;   //Move over
                                }
                            }
                        }
                    }
                    break;

                case '*':   //dwarf
                    if(obj->getFrame() > 3)
                        break;
                    //Move the player if pressing keys
                    if(keyDown(HGEK_RIGHT))
                    {
                        if(checkGrid(row, col+1))
                            break;
                        if(!keyDown(HGEK_SPACE) && col+1 < LEVEL_WIDTH && m_levelGrid[col+1][row] == NULL)
                        {
                            m_levelGrid[col+1][row] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->offset(GRID_WIDTH*SCALE_FAC, 0);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->getFrame();
                            if(iFrame > 1)
                                iFrame = 0;
                            else
                                iFrame = 2;
                            if(col+1 >= LEVEL_WIDTH || m_oldGrid[col+1][row] != NULL)
                                iFrame = 0;
                            obj->setFrame(iFrame);
                        }
                    }
                    else if(keyDown(HGEK_LEFT))
                    {
                        if(checkGrid(row, col-1))
                            break;
                        if(!keyDown(HGEK_SPACE) && col > 0 && m_levelGrid[col-1][row] == NULL)
                        {
                            m_levelGrid[col-1][row] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->offset(-GRID_WIDTH*SCALE_FAC, 0);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->getFrame();
                            if(iFrame > 1)
                                iFrame = 1;
                            else
                                iFrame = 3;
                            if(col <= 0 || m_oldGrid[col-1][row] != NULL)
                                iFrame = 1;
                            obj->setFrame(iFrame);
                        }
                    }
                    else if(keyDown(HGEK_DOWN))
                    {
                        checkGrid(row+1, col);
                        if(!keyDown(HGEK_SPACE) && row+1 < LEVEL_HEIGHT && m_levelGrid[col][row+1] == NULL)
                        {
                            m_levelGrid[col][row+1] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->offset(0, GRID_HEIGHT*SCALE_FAC);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->getFrame();
                            if(iFrame > 1)
                                iFrame -= 2;
                            else
                                iFrame += 2;
                            if(row+1 >= LEVEL_HEIGHT || m_oldGrid[col][row+1] != NULL)
                                if(iFrame > 1)
                                iFrame -= 2;
                            obj->setFrame(iFrame);
                        }
                    }
                    else if(keyDown(HGEK_UP))
                    {
                        checkGrid(row-1, col);
                        if(!keyDown(HGEK_SPACE) && row > 0 && m_levelGrid[col][row-1] == NULL)
                        {
                            m_levelGrid[col][row-1] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->offset(0, -GRID_HEIGHT*SCALE_FAC);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->getFrame();
                            if(iFrame > 1)
                                iFrame -= 2;
                            else
                                iFrame += 2;
                            if(row <= 0 || m_oldGrid[col][row-1] != NULL)
                                if(iFrame > 1)
                                iFrame -= 2;
                            obj->setFrame(iFrame);
                        }
                    }
                    else    //Set to stopped frame
                    {
                        int16_t iFrame = obj->getFrame();
                        if(iFrame >= 2)
                            obj->setFrame(iFrame-2);
                    }
                    break;

                case '0':   //balloon
                    //TODO
                    break;

                case '<':   //Left tunnel
                {
                    if(s == "<*")   //Has the player inside
                    {
                        //Move player left
                        retroObject* nextTunnel = m_levelGrid[col-1][row];  //Assume this is good, since we'll test for it before entering a tunnel
                        if(nextTunnel == NULL || nextTunnel->getNameChar() == '.')  //We can place player down in empty spaces or grass
                        {
                            if(nextTunnel != NULL)
                                nextTunnel->kill();
                            //Create dwarf
                            m_levelGrid[col-1][row] = new Dwarf(getImage("res/gfx/orig/dwarf.png"));
                            m_levelGrid[col-1][row]->setNumFrames(8);
                            m_levelGrid[col-1][row]->setFrame(1);
                            m_levelGrid[col-1][row]->setPos((col-1) * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                            m_levelGrid[col-1][row]->setName('*');
                            addObject(m_levelGrid[col-1][row]);
                            obj->setName('<');
                            obj->setImage(getImage("res/gfx/orig/tunnelL.png"));

                        }
                        else if(nextTunnel->getNameChar() == '<')    //Next tunnel
                        {
                            nextTunnel->setName("<*");  //Put player inside this next tunnel
                            obj->setName('<');  //Move player out of this tunnel
                            //Swap the images of each
                            Image* img = obj->getImage();
                            obj->setImage(nextTunnel->getImage());
                            nextTunnel->setImage(img);
                        }
                    }
                    break;
                }

                case '>':   //Right tunnel
                {
                    if(s == ">*" && !m_bTunnelMoved)   //Has the player inside
                    {
                        m_bTunnelMoved = true;
                        //Move player right
                        retroObject* nextTunnel = m_oldGrid[col+1][row];  //Assume this is good, since we'll test for it before entering a tunnel
                        if(nextTunnel == NULL || nextTunnel->getNameChar() == '.')  //We can place player down in empty spaces or grass
                        {
                            if(nextTunnel != NULL)
                                nextTunnel->kill();
                            //Create dwarf
                            m_levelGrid[col+1][row] = new Dwarf(getImage("res/gfx/orig/dwarf.png"));
                            m_levelGrid[col+1][row]->setNumFrames(8);
                            m_levelGrid[col+1][row]->setPos((col+1) * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                            m_levelGrid[col+1][row]->setName('*');
                            addObject(m_levelGrid[col+1][row]);
                            obj->setName('>');
                            obj->setImage(getImage("res/gfx/orig/tunnelR.png"));

                        }
                        else if(nextTunnel->getNameChar() == '>')    //Next tunnel
                        {
                            nextTunnel->setName(">*");  //Put player inside this next tunnel
                            obj->setName('>');  //Move player out of this tunnel
                            //Swap the images of each
                            Image* img = obj->getImage();
                            obj->setImage(nextTunnel->getImage());
                            nextTunnel->setImage(img);
                        }
                    }
                    break;
                }
            }
        }
    }
}

void myEngine::playSound(string sFilename)
{
    Engine::playSound(sFilename, 100, 0, (float32)(getFramerate()/(float)(GAME_FRAMERATE)));    //Pitchshift depending on framerate. For fun.
}









