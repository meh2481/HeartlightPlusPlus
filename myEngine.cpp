/*
    Heartlight++ source - myEngine.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "myEngine.h"

//For HGE-based functions to be able to call our Engine class functions - Note that this means there can be no more than one Engine at a time
static myEngine* g_pGlobalEngine;

bool FrameFunc()
{
    return g_pGlobalEngine->myFrameFunc();
}

bool RenderFunc()
{
    return g_pGlobalEngine->myRenderFunc();
}

void myEngine::frame()
{
    UpdateGrid();
    UpdateObjects();
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
                    if(m_levelGrid[col][row]->GetNameChar() == '*')
                    {
                        m_levelGrid[col][row]->SetFrame(m_iWinningCount % 2 + 6);
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
    DrawObjects();
    //And bottom bar
    m_imgHUD->Draw(0,getHeight() - m_imgHUD->GetHeight());
}

void myEngine::init()
{
    //Load all images, so we can scale all of them up from the start
    loadImages("res/gfx/orig.txt");

    m_imgHUD = getImage("res/gfx/orig/bottombar.png");  //Hang onto this image

    //Now scale all the images up
    ScaleImages(SCALE_FAC);

    if(!loadLevels("res/LEVELS.HL"))
    {
        errlog << "Aborting..." << endl;
        exit(1);    //Abort
    }
    loadLevel();
    //m_imgHUD->Scale(SCALE_FAC);
    //PlayMusic("res/sfx/orig/menu_music.ogg"); //Start playing menu music
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
                    setFramerate(std::max(getFramerate()-1,1));
                    break;

                case HGEK_F12:      //F12: Increase fps
                    setFramerate(getFramerate()+1);
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
    ClearObjects(); //If there's any memory hanging around with objects, clear it out
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
                    m_levelGrid[col][row]->SetNumFrames(6);
                    m_levelGrid[col][row]->SetFrame(randInt(0,5));    //randomize the frame it starts at
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
                    break;

                case '@':   //rock
                    m_levelGrid[col][row] = new retroObject(rock);
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
                    break;

                case '.':   //grass
                    m_levelGrid[col][row] = new retroObject(grass);
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
                    break;

                case '*':   //the dwarf
                    m_levelGrid[col][row] = new Dwarf(dwarf);
                    m_levelGrid[col][row]->SetNumFrames(8);
                    //Set to facing left if on right side of the screen
                    if(col >= LEVEL_WIDTH/2)
                        m_levelGrid[col][row]->SetFrame(1);
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
                    break;

                case '!':   //exit door
                    m_levelGrid[col][row] = new Door(exitdoor);
                    m_levelGrid[col][row]->SetNumFrames(4);
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
                    break;

                case '&':   //bomb
                    m_levelGrid[col][row] = new retroObject(bomb);
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
                    break;

                case '0':   //balloon
                    m_levelGrid[col][row] = new retroObject(balloon);
                    m_levelGrid[col][row]->SetNumFrames(4);
                    m_levelGrid[col][row]->SetFrame(randInt(0,3));
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
                    break;

                case '=':   //plasma
                    m_levelGrid[col][row] = new retroObject(plasma);
                    m_levelGrid[col][row]->SetNumFrames(8);
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
                    break;

                case '#':   //brick wall
                    m_levelGrid[col][row] = new Brick(brick);
                    m_levelGrid[col][row]->SetNumFrames(4);
                    m_levelGrid[col][row]->SetFrame(m_iCurrentLevel % 4);  //Color depends on level number, like original game
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
                    break;

                case '%':   //metal wall
                    m_levelGrid[col][row] = new retroObject(metalwall);
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
                    break;
                case '<':   //left tunnel
                    m_levelGrid[col][row] = new retroObject(Ltunnel);
                    m_levelGrid[col][row]->SetNumFrames(4);
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
                    break;

                case '>':   //right tunnel
                    m_levelGrid[col][row] = new retroObject(Rtunnel);
                    m_levelGrid[col][row]->SetNumFrames(4);
                    m_levelGrid[col][row]->SetPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->SetName(cObj);
                    AddObject(m_levelGrid[col][row]);
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
                if(m_levelGrid[col][row]->GetNameChar() == '!')
                {
                    m_levelGrid[col][row]->SetFrame(1);
                    PlaySound("res/sfx/orig/door_open.ogg");
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

bool myEngine::CheckGrid(int row, int col)
{
    if(row < 0 ||
       col < 0 ||
       row >= LEVEL_HEIGHT ||
       col >= LEVEL_WIDTH)
        return false; //We're at the edge of the map -- ignore
    retroObject* obj = m_levelGrid[col][row];
    if(obj == NULL || keyDown(HGEK_SPACE))
        return false; //Nothing to do here
    switch(obj->GetNameChar())
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
                        if(m_levelGrid[col][row]->GetNameChar() == '!')
                        {
                            m_levelGrid[col][row]->SetFrame(1);
                            PlaySound("res/sfx/orig/door_open.ogg");
                        }
                    }
                }
            }
            else    //Otherwise, play getting heart noise
                PlaySound("res/sfx/orig/get_heart.ogg");
            m_levelGrid[col][row]->Kill();
            m_levelGrid[col][row] = NULL;
            m_oldGrid[col][row] = NULL;
            break;

        case '.':   //Grass- just destroy
            m_levelGrid[col][row]->Kill();
            m_levelGrid[col][row] = NULL;
            m_oldGrid[col][row] = NULL;
            break;

        case '!':
            if(m_levelGrid[col][row]->GetFrame() == 3)  //If open
            {
                m_levelGrid[col][row]->Kill();
                m_levelGrid[col][row] = NULL;
                m_oldGrid[col][row] = NULL;
                m_iWinningCount = WIN_COUNT+2;  //+2 here because we'll miss a couple frames
                PlaySound("res/sfx/orig/applause.ogg");
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
                if(tunnelTest == NULL || tunnelTest->GetNameChar() == '.')
                {
                    bObstructed = false;    //We can go this way if it's just grass or empty space
                    break;
                }
                else if(tunnelTest->GetNameChar() != '>')
                    break;  //Obstructed
            }
            if(bObstructed)
                break;  //We can't go through this tunnel (also will prevent you from going the wrong way through tunnels)

            //Enter the tunnel
            obj->setImage(getImage("res/gfx/orig/tunnelR_up.png"));
            obj->SetName(">*"); //Player is in here
            m_bTunnelMoved = true;  //Entered this tunnel
            PlaySound("res/sfx/orig/subway.ogg");

            //Destroy the current dwarf object
            for(uint32_t i = 0; i < LEVEL_HEIGHT; i++)
            {
                for(uint32_t j = 0; j < LEVEL_WIDTH; j++)
                {
                    if(m_levelGrid[j][i] != NULL && m_levelGrid[j][i]->GetNameChar() == '*')
                    {
                        m_levelGrid[j][i]->Kill();
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
                if(tunnelTest == NULL || tunnelTest->GetNameChar() == '.')
                {
                    bObstructed = false;    //We can go this way if it's just grass or empty space
                    break;
                }
                else if(tunnelTest->GetNameChar() != '<')
                    break;  //Obstructed
            }
            if(bObstructed)
                break;  //We can't go through this tunnel (also will prevent you from going the wrong way through tunnels)

            //Enter the tunnel
            obj->setImage(getImage("res/gfx/orig/tunnelL_up.png"));
            obj->SetName("<*"); //Player is in here
            PlaySound("res/sfx/orig/subway.ogg");

            //Destroy the current dwarf object
            for(uint32_t i = 0; i < LEVEL_HEIGHT; i++)
            {
                for(uint32_t j = 0; j < LEVEL_WIDTH; j++)
                {
                    if(m_levelGrid[j][i] != NULL && m_levelGrid[j][i]->GetNameChar() == '*')
                    {
                        m_levelGrid[j][i]->Kill();
                        m_levelGrid[j][i] = NULL;
                    }
                }
            }

            return true;
        }

    }
    return false;
}

void myEngine::UpdateGrid() //Workhorse for updating the objects in the game
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

            string s = obj->GetName();
            switch(*(s.begin()))
            {
                case '$':   //heart
                case '@':   //rock  (both heart and rock behave the same way as far as falling and such)
                    if(row == LEVEL_HEIGHT-1 || m_oldGrid[col][row+1] != NULL )   //Hitting something
                    {
                        if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1]->GetNameChar() == '*') //Hit player
                        {
                            if(obj->GetVelocity().y > 0)   //Falling on top of player
                            {
                                //TODO kill player
                            }
                        }
                        else if(obj->GetVelocity().y > 0)   //Hit ground
                        {
                            if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1]->GetNameChar() == '.') //Hit grass
                            {
                                if(*(s.begin()) == '$') //Heart hit
                                    PlaySound("res/sfx/orig/heart_hit_grass.ogg");
                                else
                                    PlaySound("res/sfx/orig/hit_grass.ogg");
                            }
                            else
                            {
                                if(*(s.begin()) == '$') //Heart hit
                                    PlaySound("res/sfx/orig/heart_hit1.ogg");   //TODO random hit noise
                                else
                                    PlaySound("res/sfx/orig/rock_hit.ogg");
                            }
                            obj->SetVelocity(0,0);  //Hitting something, stop falling
                        }
                    }
                    else if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1] == NULL)   //Fall down
                    {
                        obj->Offset(0,GRID_HEIGHT*SCALE_FAC);
                        m_levelGrid[col][row+1] = obj;
                        m_levelGrid[col][row] = NULL;
                        obj->SetVelocity(0,1);
                    }
                    //TODO Check and see if we should move over to fall
                    break;

                case '*':   //dwarf
                    //Move the player if pressing keys
                    if(keyDown(HGEK_RIGHT))
                    {
                        if(CheckGrid(row, col+1))
                            break;
                        if(!keyDown(HGEK_SPACE) && col+1 < LEVEL_WIDTH && m_levelGrid[col+1][row] == NULL)
                        {
                            m_levelGrid[col+1][row] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->Offset(GRID_WIDTH*SCALE_FAC, 0);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->GetFrame();
                            if(iFrame > 1)
                                iFrame = 0;
                            else
                                iFrame = 2;
                            if(col+1 >= LEVEL_WIDTH || m_oldGrid[col+1][row] != NULL)
                                iFrame = 0;
                            obj->SetFrame(iFrame);
                        }
                    }
                    else if(keyDown(HGEK_LEFT))
                    {
                        if(CheckGrid(row, col-1))
                            break;
                        if(!keyDown(HGEK_SPACE) && col > 0 && m_levelGrid[col-1][row] == NULL)
                        {
                            m_levelGrid[col-1][row] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->Offset(-GRID_WIDTH*SCALE_FAC, 0);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->GetFrame();
                            if(iFrame > 1)
                                iFrame = 1;
                            else
                                iFrame = 3;
                            if(col <= 0 || m_oldGrid[col-1][row] != NULL)
                                iFrame = 1;
                            obj->SetFrame(iFrame);
                        }
                    }
                    else if(keyDown(HGEK_DOWN))
                    {
                        CheckGrid(row+1, col);
                        if(!keyDown(HGEK_SPACE) && row+1 < LEVEL_HEIGHT && m_levelGrid[col][row+1] == NULL)
                        {
                            m_levelGrid[col][row+1] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->Offset(0, GRID_HEIGHT*SCALE_FAC);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->GetFrame();
                            if(iFrame > 1)
                                iFrame -= 2;
                            else
                                iFrame += 2;
                            if(row+1 >= LEVEL_HEIGHT || m_oldGrid[col][row+1] != NULL)
                                if(iFrame > 1)
                                iFrame -= 2;
                            obj->SetFrame(iFrame);
                        }
                    }
                    else if(keyDown(HGEK_UP))
                    {
                        CheckGrid(row-1, col);
                        if(!keyDown(HGEK_SPACE) && row > 0 && m_levelGrid[col][row-1] == NULL)
                        {
                            m_levelGrid[col][row-1] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->Offset(0, -GRID_HEIGHT*SCALE_FAC);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->GetFrame();
                            if(iFrame > 1)
                                iFrame -= 2;
                            else
                                iFrame += 2;
                            if(row <= 0 || m_oldGrid[col][row-1] != NULL)
                                if(iFrame > 1)
                                iFrame -= 2;
                            obj->SetFrame(iFrame);
                        }
                    }
                    else    //Set to stopped frame
                    {
                        int16_t iFrame = obj->GetFrame();
                        if(iFrame >= 2)
                            obj->SetFrame(iFrame-2);
                    }
                    break;

                case '&':   //bomb
                    //TODO
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
                        if(nextTunnel == NULL || nextTunnel->GetNameChar() == '.')  //We can place player down in empty spaces or grass
                        {
                            if(nextTunnel != NULL)
                                nextTunnel->Kill();
                            //Create dwarf
                            m_levelGrid[col-1][row] = new Dwarf(getImage("res/gfx/orig/dwarf.png"));
                            m_levelGrid[col-1][row]->SetNumFrames(8);
                            m_levelGrid[col-1][row]->SetFrame(1);
                            m_levelGrid[col-1][row]->SetPos((col-1) * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                            m_levelGrid[col-1][row]->SetName('*');
                            AddObject(m_levelGrid[col-1][row]);
                            obj->SetName('<');
                            obj->setImage(getImage("res/gfx/orig/tunnelL.png"));

                        }
                        else if(nextTunnel->GetNameChar() == '<')    //Next tunnel
                        {
                            nextTunnel->SetName("<*");  //Put player inside this next tunnel
                            obj->SetName('<');  //Move player out of this tunnel
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
                        if(nextTunnel == NULL || nextTunnel->GetNameChar() == '.')  //We can place player down in empty spaces or grass
                        {
                            if(nextTunnel != NULL)
                                nextTunnel->Kill();
                            //Create dwarf
                            m_levelGrid[col+1][row] = new Dwarf(getImage("res/gfx/orig/dwarf.png"));
                            m_levelGrid[col+1][row]->SetNumFrames(8);
                            m_levelGrid[col+1][row]->SetPos((col+1) * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                            m_levelGrid[col+1][row]->SetName('*');
                            AddObject(m_levelGrid[col+1][row]);
                            obj->SetName('>');
                            obj->setImage(getImage("res/gfx/orig/tunnelR.png"));

                        }
                        else if(nextTunnel->GetNameChar() == '>')    //Next tunnel
                        {
                            nextTunnel->SetName(">*");  //Put player inside this next tunnel
                            obj->SetName('>');  //Move player out of this tunnel
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

void myEngine::PlaySound(string sFilename)
{
    Engine::PlaySound(sFilename, 100, 0, (float32)(getFramerate()/(float)(GAME_FRAMERATE)));    //Pitchshift depending on framerate. For fun.
}









