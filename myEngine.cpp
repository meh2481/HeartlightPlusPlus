/*
    Heartlight++ source - myEngine.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "myEngine.h"

//For HGE-based functions to be able to call our Engine class functions - Note that this means there can be no more than one Engine at a time
static myEngine* g_pGlobalEngine;

bool frameFunc()
{
    return g_pGlobalEngine->_myFrameFunc();
}

bool renderFunc()
{
    return g_pGlobalEngine->_myRenderFunc();
}

void signalHandler(string sSignal)
{
    g_pGlobalEngine->hudSignalHandler(sSignal);
}

void fillRect(Rect rc, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    g_pGlobalEngine->fillRect(rc, red, green, blue, alpha);
}

myEngine::myEngine(uint16_t iWidth, uint16_t iHeight, string sTitle) : Engine(iWidth, iHeight, sTitle)
{
    g_pGlobalEngine = this;
    m_iCurrentLevel = 0;
    m_iWinningCount = 0;
    m_iDyingCount = 0;
    m_iFade = FADE_NONE;
    m_bDebug = false;
    m_bSound = true;
    m_bMusic = true;
    m_bRad  = false;
}

myEngine::~myEngine()
{
    delete m_cur;
    delete m_hud;
}

void myEngine::frame()
{
    if(m_iFade == FADE_IN)
        return; //Don't do anything if fading in

    updateGrid_retro();

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
            m_mLevelsBeaten[m_iCurrentLevel] = true;    //We've beaten this level
            m_iCurrentLevel++;  //Go to next level
            if(m_iCurrentLevel >= m_vLevels.size())
                m_iCurrentLevel = 0;
            if(RETRO)
                loadLevel_retro();
            else
                loadLevel_new();
        }
    }
    if(m_iDyingCount)
    {
        if(!(--m_iDyingCount))
        {
            //Start fading to black
            m_fEndFade = getTime() + FADE_TIME;
            m_iFade = FADE_OUT;
        }
        else if(m_iDyingCount == DIE_COUNT/2)   //Done dying; explode_retro
        {
            //Make gnome explode_retro
            for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
            {
                for(uint16_t col = 0; col < LEVEL_WIDTH; col++)
                {
                    if(m_levelGrid[col][row] == NULL)
                        continue;
                    if(m_levelGrid[col][row]->getNameChar() == '*')
                    {
                        //Make explosion
                        m_levelGrid[col][row]->kill();
                        m_levelGrid[col][row] = new retroObject(getImage("o_explode"));
                        m_oldGrid[col][row] = m_levelGrid[col][row];
                        m_levelGrid[col][row]->setName('X');
                        m_levelGrid[col][row]->setNumFrames(7);
                        m_levelGrid[col][row]->setPos(col*GRID_WIDTH*SCALE_FAC, row*GRID_HEIGHT*SCALE_FAC);
                        addObject(m_levelGrid[col][row]);
                        playSound("o_explode");  //Make explosion sound too
                    }
                }
            }
        }
        else if(m_iDyingCount > DIE_COUNT/2)
        {
            //Make gnome hold his head and rock back and forth
            for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
            {
                for(uint16_t col = 0; col < LEVEL_WIDTH; col++)
                {
                    if(m_levelGrid[col][row] == NULL)
                        continue;
                    if(m_levelGrid[col][row]->getNameChar() == '*')
                    {
                        m_levelGrid[col][row]->setFrame(m_iDyingCount % 2 + 4);
                    }
                }
            }
        }
    }

    updateObjects();    //Update the objects in the game
}

void myEngine::draw()
{
    //Just draw all objects
    drawObjects();

    //Draw debug stuff if we should
    if(m_bDebug)
    {
        for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
        {
            for(uint16_t col = 0; col < LEVEL_WIDTH; col++)
            {
                if(m_levelGrid[col][row] != NULL)   //Draw a green box in this grid square if it isn't vacant
                {
                    Rect rc = {col*GRID_WIDTH*SCALE_FAC, row*GRID_HEIGHT*SCALE_FAC, (col+1)*GRID_WIDTH*SCALE_FAC, (row+1)*GRID_HEIGHT*SCALE_FAC};
                    fillRect(rc, 0, 255, 0, 100);
                }
            }
        }
    }

    //If fading, draw black overlay
    if(m_iFade != FADE_NONE)
    {
        float32 fCurTime = getTime();
        float32 fTimeLeft = m_fEndFade - fCurTime;
        if(fTimeLeft <= 0)
        {
            if(m_iFade == FADE_IN)
                m_iFade = FADE_NONE;
            else if(m_iFade == FADE_OUT)
            {
                m_iFade = FADE_IN;
                m_fEndFade = fCurTime + fTimeLeft + FADE_TIME;
                fTimeLeft = m_fEndFade - fCurTime;
                if(RETRO)
                    loadLevel_retro();
                else
                    loadLevel_new();      //Reload this level
            }
        }
        uint8_t iFinalAlpha = 0;
        if(m_iFade == FADE_IN)
            iFinalAlpha = (fTimeLeft / FADE_TIME) * (float32)(255);
        else if(m_iFade == FADE_OUT)
            iFinalAlpha = ((FADE_TIME - fTimeLeft) / FADE_TIME) * (float32)(255);
        fillRect(getScreenRect(), 0,0,0, iFinalAlpha);

    }

    //Update our HUD
    HUDTextbox* tex = (HUDTextbox*)m_hud->getChild("curlevel");
    if(tex != NULL)
        tex->setText(m_iCurrentLevel+1);
    tex = (HUDTextbox*)m_hud->getChild("levelstotal");
    if(tex != NULL)
        tex->setText(m_vLevels.size());
    tex = (HUDTextbox*)m_hud->getChild("levelsbeaten");
    if(tex != NULL)
        tex->setText(m_mLevelsBeaten.size());
    tex = (HUDTextbox*)m_hud->getChild("heartsgotten");
    if(tex != NULL)
        tex->setText(m_iCollectedHearts);
    tex = (HUDTextbox*)m_hud->getChild("heartstotal");
    if(tex != NULL)
        tex->setText(m_iHeartsTotal);
    HUDToggle* tog = (HUDToggle*)m_hud->getChild("defeated");
    if(tog != NULL)
    {
        map<uint32_t, bool>::iterator itCurLev = m_mLevelsBeaten.find(m_iCurrentLevel);
        if(itCurLev != m_mLevelsBeaten.end())   //If this item is here
            tog->setEnabled(true);  //This level is beaten
        else
            tog->setEnabled(false); //We haven't beaten this level yet
    }


    //Draw our HUD
    m_hud->draw(getTime());
}

void myEngine::init()
{
    //Load all images, so we can scale all of them up from the start
    loadImages("res/gfx/orig.xml");

    //Now scale all the images up
    scaleImages(SCALE_FAC);

    //Load all sounds as well
    loadSounds("res/sfx/orig.xml");

    loadLevelDirectory("res/levels");

    m_cur = new Cursor("res/gfx/new/cursorpoint.png");
    m_cur->setHotSpot(47,15);//m_cur->getWidth()/2.0, m_cur->getHeight()/2.0);
    m_cur->setType(CURSOR_BREATHE);
    setCursor(m_cur);

    if(RETRO)
        loadLevel_retro();
    else
        loadLevel_new();

    m_hud = new HUD("levelhud");
    //errlog << "Creating HUD" << endl;
    m_hud->create("res/hud/hud.xml");
    //errlog << "Done creating HUD" << endl;
    m_hud->setScale(SCALE_FAC);
    m_hud->setSignalHandler(signalHandler);

    //if(m_bMusic)
    //    playMusic("o_mus_menu"); //Start playing menu music

    //for(int i = 0; i < 20; i++)
    //{
    //    physicsObject* obj = new physicsObject(getImage("o_metalwall"));
    //    b2BodyDef def;
    //    def.type = b2_dynamicBody;
    //    def.position.Set(8 * SCALE_DOWN_FACTOR * SCALE_FAC + i * obj->getWidth()/2.0 * SCALE_DOWN_FACTOR * SCALE_FAC, 8 * SCALE_DOWN_FACTOR * SCALE_FAC);
    //    b2Body* bod = createBody(&def);
    //    //bod->SetAngularVelocity(2.0);
    //    obj->addBody(bod);
    //    b2PolygonShape dynamicBox;
    //    dynamicBox.SetAsBox(obj->getWidth() * SCALE_DOWN_FACTOR/2.0, obj->getHeight() * SCALE_DOWN_FACTOR/2.0);
    //    b2FixtureDef fixtureDef;
    //    fixtureDef.shape = &dynamicBox;
    //    fixtureDef.density = 1.0f;
    //    fixtureDef.friction = 0.3f;
    //    obj->addFixture(&fixtureDef);
    //    addObject(obj);
    //    m_objTest = obj;
    //}

    //Create physics boundary if not in retro mode
    if(!RETRO)
    {
        b2BodyDef grounddef;
        grounddef.position.SetZero();
        b2Body* groundBody = createBody(&grounddef);
        b2ChainShape worldBox;
        Rect rcScreen = getScreenRect();
        rcScreen.bottom = LEVEL_HEIGHT*GRID_HEIGHT*SCALE_FAC;    //Account for size of HUD on bottom of screen
        rcScreen.scale(SCALE_DOWN_FACTOR);
        b2Vec2 vertices[5];
        vertices[0].Set(rcScreen.left, rcScreen.top);
        vertices[1].Set(rcScreen.right, rcScreen.top);
        vertices[2].Set(rcScreen.right, rcScreen.bottom);
        vertices[3].Set(rcScreen.left, rcScreen.bottom);
        vertices[4].Set(rcScreen.left, rcScreen.top);
        worldBox.CreateChain(vertices, 5);
        groundBody->CreateFixture(&worldBox, 0.0);
    }
}

void myEngine::loadImages(string sListFilename)
{
    //  File format:
    //
    //  <?xml version="1.0" encoding="UTF-8"?>
    //  <images>
    //      <image name="o_balloon" path="res/gfx/orig/balloon.png" />
    //      ...
    //  </images>

    //If you wish to reference the image in your code, use the text in the "name" field

    XMLDocument* doc = new XMLDocument();
    doc->LoadFile(sListFilename.c_str());

    XMLElement* elem = doc->FirstChildElement("images");
    if(elem == NULL) return;
    elem = elem->FirstChildElement("image");
    if(elem == NULL) return;
    for(;;) //Load all elements
    {
        const char* cPath = elem->Attribute("path");
        if(cPath == NULL) return;
        const char* cName = elem->Attribute("name");
        if(cName == NULL) return;
        createImage(cPath, cName);  //Create this image

        //Move to the next sibling
        elem = elem->NextSiblingElement();
        if(elem == NULL) break;
    }
    delete doc;
}

void myEngine::loadSounds(string sListFilename)
{
    //  File format:
    //
    //  <?xml version="1.0" encoding="UTF-8"?>
    //  <sounds>
    //      <sound name="o_applause" path="res/sfx/orig/applause.ogg" />
    //      ...
    //  </sounds>

    //If you wish to reference the sound in your code, use the text in the "name" field

    XMLDocument* doc = new XMLDocument();
    doc->LoadFile(sListFilename.c_str());

    XMLElement* elem = doc->FirstChildElement("sounds");
    if(elem == NULL) return;
    elem = elem->FirstChildElement("sound");
    if(elem == NULL) return;
    for(;;) //Load all elements
    {
        const char* cPath = elem->Attribute("path");
        if(cPath == NULL) return;
        const char* cName = elem->Attribute("name");
        if(cName == NULL) return;
        createSound(cPath, cName);  //Create this sound

        //Move to the next sibling
        elem = elem->NextSiblingElement();
        if(elem == NULL) break;
    }
    delete doc;
}

void myEngine::hudSignalHandler(string sSignal)
{
    if(sSignal == "soundtoggle")
    {
        m_bSound = !m_bSound;
    }
    else if(sSignal == "musictoggle")
    {
        m_bMusic = !m_bMusic;
        if(!m_bMusic)
            pauseMusic();
        else
            playMusic("o_mus_menu");
    }
    else if(sSignal == "radtoggle")
    {
        m_bRad = !m_bRad;       //Toggle this... TODO: Find out what this even does in the original game, and do whatever it is it does
    }
    else
        errlog << "Warning: Unrecognized signal \"" << sSignal << "\" received from HUD." << endl;

    playSound("o_toggle");  //Play sound for toggling hud item
}

void myEngine::handleEvent(hgeInputEvent event)
{
    m_hud->event(event);    //Let our HUD handle any events it needs to
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
                        if(RETRO)
                            loadLevel_retro();
                        else
                            loadLevel_new();
                    }
                    else if(!RETRO)
                    {
                        b2Body* bod = m_objTest->getBody();
                        b2Vec2 force;
                        force.Set(1000,0);
                        bod->ApplyForceToCenter(force);
                    }
                    break;

                case HGEK_LEFT:
                    if(keyDown(HGEK_SPACE)) //Space-left goes to previous level
                    {
                        if(m_iCurrentLevel == 0)
                            m_iCurrentLevel = m_vLevels.size();
                        m_iCurrentLevel--;
                        if(RETRO)
                            loadLevel_retro();
                        else
                            loadLevel_new();
                    }
                    else if(!RETRO)
                    {
                        b2Body* bod = m_objTest->getBody();
                        b2Vec2 force;
                        force.Set(-1000,0);
                        bod->ApplyForceToCenter(force);
                    }
                    break;

                case HGEK_ESCAPE:
                    //Make gnome die
                    if(!m_iDyingCount)
                        m_iDyingCount = DIE_COUNT;
                    break;

                case HGEK_F11:      //F11: Decrease fps
                    setFramerate(std::max(getFramerate()-1.0,0.0));
                    break;

                case HGEK_F12:      //F12: Increase fps
                    setFramerate(getFramerate()+1.0);
                    break;

                case HGEK_V:
                    m_bDebug = !m_bDebug;
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


void myEngine::loadLevelDirectory(string sFilePath)
{
    //Get directory listing
    ttvfs::VFSHelper vfs;
    vfs.Prepare();
    ttvfs::StringList slFiles;
    ttvfs::GetFileList(sFilePath.c_str(), slFiles);

    for(ttvfs::StringList::iterator il = slFiles.begin(); il != slFiles.end(); il++)
    {
        errlog << "Loading level file \"" << *il << "\"" << endl;
        if(!loadLevels(sFilePath + "/" + (*il)))    //Load each file in this folder
            errlog << "Warning: Malformed file. May not load some levels properly." << endl;
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
        //Spin through characters, ignoring them, until we hit the next '{' character
        for(;;)
        {
            if(infile.eof() || infile.fail())   //Hit end of file
                break;
            char c = infile.get();
            if(c == '{')
            {
                c=infile.get(); //Skip over \n char
                if(c != '\n' &&
                   c != '\r')
                {
                    infile.putback(c);
                }
                c=infile.get(); //Skip over \n char
                if(c != '\n' &&
                   c != '\r')
                {
                    infile.putback(c);
                }
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

void myEngine::playSound(string sName)
{
    if(m_bSound)    //If sounds are off, don't play them
        Engine::playSound(sName, 100, 0, (float32)(getFramerate()/(float32)(GAME_FRAMERATE)));    //Pitchshift depending on framerate. For fun.
}









