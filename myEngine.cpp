/*
    Heartlight++ source - myEngine.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "myEngine.h"

//For our engine functions to be able to call our Engine class functions - Note that this means there can be no more than one Engine at a time
//TODO: Think of workaround? How does everything communicate now?
static myEngine* g_pGlobalEngine;

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
    m_bJumped = false;
	m_bRetroPhys = true;
	m_bRetroGfx = false;
	m_bRetroSfx = true;
    //m_rcViewScreen.set(0,0,getWidth(),getHeight());
//    m_bDragScreen = false;
//    m_bScaleScreen = false;
}

myEngine::~myEngine()
{
    delete m_cur;
    delete m_hud;
    //delete testObj;
    //delete shipObj;
    //delete objImg;
    //delete objLayer;
    //delete objSeg;
    //delete myObj;
}

void myEngine::frame()
{
    if(m_iFade == FADE_LOAD)                //Waiting to load next level
    {
        m_iFade = FADE_IN;                  //Start fading in
        m_fEndFade = getTime() + FADE_TIME;
        loadLevel_retro();                  //Reload this level
    }

    if(m_iFade == FADE_IN)
        return; //Don't do anything if fading in

    if(m_bRetroPhys)
        updateGrid_retro();
    else
        updateGrid_new();

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
            if(m_bRetroPhys)
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
                        m_levelGrid[col][row]->setPos(col*GRID_WIDTH, row*GRID_HEIGHT);
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
    //glLoadIdentity();
    //glTranslatef( 0.0f, 0.0f, MAGIC_ZOOM_NUMBER);


    //glDisable( GL_LIGHTING );   //Don't care about lighting for rendering 2D objects
    //hideCursor();
    drawObjects();
    if(m_iCurGun != m_lGuns.end())
        (*m_iCurGun)->draw();

    //Draw debug stuff if we should
    if(m_bDebug)
    {
        for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
        {
            for(uint16_t col = 0; col < LEVEL_WIDTH; col++)
            {
                if(m_levelGrid[col][row] != NULL)   //Draw a green box in this grid square if it isn't vacant
                {
                    Rect rc = {col*GRID_WIDTH, row*GRID_HEIGHT, (col+1)*GRID_WIDTH, (row+1)*GRID_HEIGHT};
                    fillRect(rc, 0, 255, 0, 100);
                }
            }
        }
    }

    //If fading, draw black overlay TODO: Generic fading HUD item or such
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
                m_iFade = FADE_LOAD;    //Can't reload a level within the draw cycle, since that involves cleaning up image memory. Wait until
                                        // our next frame, then clean up
            }
        }
        uint8_t iFinalAlpha = 255;  //Avoid full visibility when done fading out
        if(m_iFade == FADE_IN)
            iFinalAlpha = (fTimeLeft / FADE_TIME) * (float32)(255);
        else if(m_iFade == FADE_OUT)
            iFinalAlpha = ((FADE_TIME - fTimeLeft) / FADE_TIME) * (float32)(255);
        else if(m_iFade == FADE_NONE)
            iFinalAlpha = 0;    //Avoid black frame at end of fading
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
    
    //objLayer->depth = 2.0f;
    //cout << "Objlayer: " << objLayer->depth << endl;
    m_hud->draw(getTime());
    //myObj->draw();
    //o3d->render();
    //objLayer->scale.x += 0.001;
    //objLayer->scale.y += 0.004;
    //objLayer->rot += 0.05;//PI/4.0;

    //objLayer->pos.x = 200;
    //objLayer->pos.y = 100;

    //fillRect(m_rcViewScreen, 255, 0, 0, 100);   //DEBUG: Draw red rectangle of portion of screen we're looking at
  
    //Draw our HUD
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    //Draw lightning testing stuff
    Image* img = getImage("test_arc");
    //img->setColor(133,207,255);
    #define LIGHTNING_WIDTH 10
    #define LIGHTNING_HEIGHT  40
    for(int i = 0; i < LIGHTNING_NUM-1; i++)
    {
      Point ul, ur, bl, br;
      Point pos = m_ptLightningTestPos[i];
      pos.y += 300;
      pos.x = i*LIGHTNING_WIDTH;
      pos.x += 50;
      ul.x = bl.x = pos.x;
      ur.x = br.x = pos.x + LIGHTNING_WIDTH;
      ul.y = pos.y;
      bl.y = pos.y + LIGHTNING_HEIGHT;
      //
      pos = m_ptLightningTestPos[i+1];
      pos.y += 300;
      ur.y = pos.y;
      br.y = pos.y + LIGHTNING_HEIGHT;
      img->draw4V(ul, ur, bl, br);
      //img->drawCentered(pos);
    }
    
    //Update
    vector<Point> ptTemp;
    ptTemp.reserve(LIGHTNING_NUM);
    for(int i = 0; i < LIGHTNING_NUM; i++)
    {
      m_ptLightningTestPos[i].y += (rand() % 31) - 15;
      if(m_ptLightningTestPos[i].y > 70)
        m_ptLightningTestPos[i].y = 70;
      if(m_ptLightningTestPos[i].y < -70)
        m_ptLightningTestPos[i].y = -70;
      ptTemp[i].y = m_ptLightningTestPos[i].y;
    }
    
    for(int i = 2; i < LIGHTNING_NUM-2; i++)
    {
      float iTot = 0.0;
      for(int j = i-2; j < i+3; j++)
        iTot += m_ptLightningTestPos[j].y;
      ptTemp[i].y = iTot / 5.0;
    }
    for(int i = 0; i < LIGHTNING_NUM; i++)
    {
      m_ptLightningTestPos[i].y = ptTemp[i].y;
    }
    //Ends are fixed in place
    m_ptLightningTestPos[0].y = m_ptLightningTestPos[LIGHTNING_NUM-1].y = 0;
    m_ptLightningTestPos[1].y = m_ptLightningTestPos[2].y / 2.0;
    m_ptLightningTestPos[LIGHTNING_NUM-2].y = m_ptLightningTestPos[LIGHTNING_NUM-3].y / 2.0;
    //m_ptLightningTestPos[2].y /= 2.0;
    //
    
}

void myEngine::init()
{
    m_ptLightningTestPos.reserve(LIGHTNING_NUM);
    for(int i = 0; i < LIGHTNING_NUM; i++)
      m_ptLightningTestPos[i].x = m_ptLightningTestPos[i].y = 0;
    //objImg = new Image("res/hud/logo.png");
    //parallaxLayer* objLayer = new parallaxLayer(objImg);
    //physSegment* objSeg = new physSegment();
    //objSeg->layer = objLayer;
    //objSeg->obj3D = new Object3D("res/3D/spaceship2.obj", "res/3D/spaceship2.png");
    //myObj = new obj();
    //myObj->addSegment(objSeg);
    //objLayer->scale.x = 0.5;
    //objLayer->scale.y = 0.4;
    //objLayer->rot = PI/4.0;
    //objLayer->pos.x = 200;
    //objLayer->pos.y = 100;
    //objLayer->depth = 2.0f;
    //objSeg->obj3D->pos.x = -0.8;
    //objLayer->col.set(0.0f, 1.0f, 1.0f, 0.0f);
    //Interpolate* inter = new Interpolate(&(objLayer->col.a));
    //inter->setMinVal(0.0f, false);
    //inter->setMaxVal(1.0f, false);
    //inter->calculateIncrement(1.0f, 1.0f);
    //inter->setDelay(10.0f);
    //addInterpolation(inter);
    //inter = new Interpolate(&(objSeg->obj3D->angle));
	  //objSeg->obj3D->rot.y = 1.0;
    //inter->calculateIncrement(180.0f, 1.0f);
    //addInterpolation(inter);
    m_lightningColor.set(0.0f, 0.0f, 0.0f, 1.0f);
   Interpolate* interp = new Interpolate(&(m_lightningColor.r));
   interp->setMinVal(0.0f, false);
   interp->setMaxVal(1.0f, false);
   interp->calculateIncrement(1.0f, 1.0f);
   addInterpolation(interp);
   interp = new Interpolate(&(m_lightningColor.g));
   interp->setMinVal(0.0f, false);
   interp->setMaxVal(1.0f, false);
   interp->calculateIncrement(1.0f, 2.0f);
   addInterpolation(interp);
   interp = new Interpolate(&(m_lightningColor.b));
   interp->setMinVal(0.0f, false);
   interp->setMaxVal(1.0f, false);
   interp->calculateIncrement(1.0f, 3.0f);
   addInterpolation(interp);
   
    lastMousePos.Set(getWidth()/2.0, getHeight()/2.0);
    setCursorPos(getWidth()/2.0, getHeight()/2.0);
    hideCursor(); //Start in retro mode without a cursor

    //Load all images, so we can scale all of them up from the start
	if(m_bRetroGfx)
		loadImages("res/gfx/orig.xml");
	else
		loadImages("res/gfx/new.xml");
    
        //testObj = new Object3D("cs/trixie2.obj", "cs/trixie.png");
    //shipObj = new Object3D("res/3D/spaceship2.obj", "res/3D/spaceship2.png");

    //Now scale all the images up
//    scaleImages(SCALE_FAC);
    //Load all sounds as well
    loadSounds("res/sfx/orig.xml");

    loadLevelDirectory("res/levels");

    m_cur = new Cursor("res/cursor/cursor1.xml");
    m_cur->setHotSpot(/*47,15);*/m_cur->getWidth()/2.0, m_cur->getHeight()/2.0);
    //m_cur->setType(CURSOR_COLOR);
    setCursor(m_cur);

    //Create guns and such
    ballGun* gun = new ballGun();
    gun->bullet = "o_rock";
    gun->gun = "n_gun";
    //gun.obj
    m_lGuns.push_back(gun);
  
    gun = new placeGun();
    gun->bullet = "o_rock";
    m_lGuns.push_back(gun);
  
    gun = new blastGun();
    gun->bullet = "o_rock";
    gun->gun = "n_blastgun";
    m_lGuns.push_back(gun);
  
    gun = new shotgun();
    gun->bullet = "o_rock";
    gun->gun = "n_shotgun";
    m_lGuns.push_back(gun);
  
    gun = new machineGun();
    gun->bullet = "o_rock";
    gun->gun = "n_gun";
    m_lGuns.push_back(gun);
  
    gun = new teleGun();
    //gun->bullet = "o_rock";
    m_lGuns.push_back(gun);
  
    gun = new superGun();
    gun->bullet = "o_rock";
    gun->gun = "n_blastgun";
    m_lGuns.push_back(gun);
  
    if(m_bRetroPhys)
    {
        loadLevel_retro();
        m_iCurGun = m_lGuns.end();
    }
    else
    {
        m_iCurGun = m_lGuns.begin();
        loadLevel_new();
    }

    m_hud = new HUD("levelhud");
    m_hud->create("res/hud/hud.xml");
//    m_hud->setScale(SCALE_FAC);
    m_hud->setSignalHandler(signalHandler);

    setGravity(0.0, 9.8);

    //if(m_bMusic)
    //    playMusic("o_mus_menu"); //Start playing menu music

    //Create physics boundary if not in retro mode
    //if(!m_bRetroPhys)
    //{
        b2BodyDef grounddef;
        grounddef.position.SetZero();
        b2Body* groundBody = createBody(&grounddef);
        b2ChainShape worldBox;
        Rect rcScreen = getScreenRect();
        rcScreen.bottom = LEVEL_HEIGHT*GRID_HEIGHT;    //Account for size of HUD on bottom of screen
        rcScreen.scale(SCALE_DOWN_FACTOR);
        b2Vec2 vertices[5];
        vertices[0].Set(rcScreen.left, rcScreen.top);
        vertices[1].Set(rcScreen.right, rcScreen.top);
        vertices[2].Set(rcScreen.right, rcScreen.bottom);
        vertices[3].Set(rcScreen.left, rcScreen.bottom);
        vertices[4].Set(rcScreen.left, rcScreen.top);
        worldBox.CreateChain(vertices, 5);
        groundBody->CreateFixture(&worldBox, 0.0);
        
    if(!m_bRetroPhys)
        setFramerate(60); //YAY 60 fps!
    //}
    
    //Keep track of current resolution
    //m_lResolutions = getAvailableResolutions();
    //TESTING
    resolution r;
    r.w = 1280;
    r.h = 768;
    m_lResolutions.push_back(r);
    r.h = 720;
    m_lResolutions.push_back(r);
    r.h = 600;
    m_lResolutions.push_back(r);
    
	/*for(list<resolution>::iterator i = m_lResolutions.begin(); i != m_lResolutions.end(); i++)
	{
		//Make sure this resolution is an even multiple of 320x200
		if(i->w % SCREEN_WIDTH || i->h % SCREEN_HEIGHT)	//If a modulus returns nonzero, not an even multiple
		{
			list<resolution>::iterator j = i;
			j--;
			m_lResolutions.erase(i);
			i = j;
		}
		else	//If both remainders are still zero, it still may not be an even scaling (1:2 ratio or such)
		{
			int iMul = i->w / SCREEN_WIDTH;
			if(SCREEN_HEIGHT * iMul != i->h)
			{
				list<resolution>::iterator j = i;
				j--;
				m_lResolutions.erase(i);
				i = j;
			}
		}
	}*/
    iCurResolution = m_lResolutions.end();
    iCurResolution--;
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

float fXpos = 0.0;
float fYpos = 0.0;
bool bBlur = true;
float32 scale_amt = 1.0;

void myEngine::handleEvent(SDL_Event event)
{
    static bool isMouseDown = false;
    m_hud->event(event);    //Let our HUD handle any events it needs to
    switch(event.type)
    {
        //Key pressed
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
                case SDLK_RIGHT:
                    if(keyDown(SDLK_SPACE)) //Space-right goes to next level
                    {
                        m_iCurrentLevel++;
                        if(m_iCurrentLevel >= m_vLevels.size())
                            m_iCurrentLevel = 0;
                        if(m_bRetroPhys)
                            loadLevel_retro();
                        else
                            loadLevel_new();
                    }
                    //else if(!m_bRetroPhys)
                    //{
                    //    b2Body* bod = m_objTest->getBody();
                    //    b2Vec2 force;
                    //    force.Set(1000,0);
                    //    bod->ApplyForceToCenter(force);
                    //}
                    break;

                case SDLK_LEFT:
                    if(keyDown(SDLK_SPACE)) //Space-left goes to previous level
                    {
                        if(m_iCurrentLevel == 0)
                            m_iCurrentLevel = m_vLevels.size();
                        m_iCurrentLevel--;
                        if(m_bRetroPhys)
                            loadLevel_retro();
                        else
                            loadLevel_new();
                    }
                    //else if(!m_bRetroPhys)
                    //{
                    //    b2Body* bod = m_objTest->getBody();
                    //    b2Vec2 force;
                    //    force.Set(-1000,0);
                    //    bod->ApplyForceToCenter(force);
                    //}
                    break;

                case SDLK_ESCAPE:
                    //Make gnome die
                    if(m_bRetroPhys)
                    {
                        if(!m_iDyingCount)
                            m_iDyingCount = DIE_COUNT;
                    }
                    else
                        quit(); //TODO: Menu blah blah blah
                    break;

                case SDLK_F11:      //F11: Decrease fps
                    setFramerate(std::max(getFramerate()-1.0,0.0));
                    break;

                case SDLK_F12:      //F12: Increase fps
                    setFramerate(getFramerate()+1.0);
                    break;

                case SDLK_v:
                    m_bDebug = !m_bDebug;
                    break;
                  
                case SDLK_r:
                    toggleRetro();
                    break;
				
				case SDLK_t:
					m_bRetroGfx = !m_bRetroGfx;
					if(m_bRetroGfx)
					{
						loadImages("res/gfx/orig.xml");
						setImagePixellated();
					}
					else
					{
						loadImages("res/gfx/new.xml");
						setImageBlurred();
					}
					reloadImages();
					//Reload level as well
					if(m_bRetroPhys)
						loadLevel_retro();
					else
						loadLevel_new();
					break;

                case SDLK_5:   //Refresh cursor XML
                    m_cur->loadFromXML("res/cursor/cursor1.xml");
                    loadLevelDirectory("res/levels");                
                    break;

                //case SDLK_0:
                //    m_rcViewScreen.set(0,0,getWidth(),getHeight());
                //    break;
                
                case SDLK_1:
                {
                    bool bChange = false;
                    iCurResolution++;
                    if(iCurResolution == m_lResolutions.end())
                      iCurResolution = m_lResolutions.begin();
                    changeScreenResolution(iCurResolution->w, iCurResolution->h);
                    glPopMatrix();
                    glPushMatrix();
                    float32 scale_x = (float32)iCurResolution->w / (float32)SCREEN_WIDTH;
                    float32 scale_y = (float32)iCurResolution->h / (float32)SCREEN_HEIGHT;
                    scale_amt = scale_x;
                    if(scale_y < scale_amt)
                    {
                      scale_amt = scale_y;
                      bChange = true;
                    }
                    //Scale up as well
                    glTranslatef((scale_amt-1.0)*(float32)iCurResolution->w/(float32)iCurResolution->h, 1.0f - scale_amt, 0.0);
                    glScalef(scale_amt,scale_amt,1.0);
                    if(scale_y > scale_amt)
                    {
                      glTranslatef(0.0, -0.16666/scale_amt, 0.0); //Magic numbers ftw
                    }
                    else if(bChange)
                    {
                      //glTranslatef(0.0f, -((GLfloat)((iCurResolution->h % SCREEN_HEIGHT)))/((GLfloat)iCurResolution->h * scale_amt), 0.0);
                    }
                    break;
                }//728, 720, 600
                
                case SDLK_RETURN:
                    if(keyDown(SDLK_LALT) || keyDown(SDLK_RALT))
                    {
                      toggleFullscreen();
                      glPopMatrix();
                      glPushMatrix();
                      glTranslatef((scale_amt-1.0)*(float32)getWidth()/(float32)getHeight(), -(scale_amt-1.0), 0.0);
                      glScalef(scale_amt,scale_amt,1.0);
                    }
                    break;
                    
                case SDLK_EQUALS:
                    if(m_iCurGun != m_lGuns.end())
                        (*m_iCurGun)->clear();
                    if(m_iCurGun != m_lGuns.end())
                        m_iCurGun++;
                    if(m_iCurGun == m_lGuns.end())
                        m_iCurGun = m_lGuns.begin();
                    if(m_iCurGun != m_lGuns.end())
                        (*m_iCurGun)->mouseMove(getCursorPos().x, getCursorPos().y);
                    break;

                case SDLK_MINUS:
                    if(m_iCurGun != m_lGuns.end())
                        (*m_iCurGun)->clear();
                    if(m_iCurGun == m_lGuns.begin())
                        m_iCurGun = m_lGuns.end();
                    if(m_iCurGun != m_lGuns.begin())
                        m_iCurGun--;
                    if(m_iCurGun != m_lGuns.end())
                        (*m_iCurGun)->mouseMove(getCursorPos().x, getCursorPos().y);
                    break;
            }
            break;

        //Key released
        case SDL_KEYUP:
            switch(event.key.keysym.sym)
            {

            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == SDL_BUTTON_LEFT)
            {
				isMouseDown = true;
                if(m_iCurGun != m_lGuns.end())
                {
                    (*m_iCurGun)->mouseDown(event.button.x, event.button.y);
                }
            }
			//RMB zooms in/out
            else if(event.button.button == SDL_BUTTON_RIGHT)
            {
              cout << "scale amt: " << scale_amt << endl;
				
				//Reset back to centered
				/*glPopMatrix();
				glPushMatrix();
				//Center on upper left corner
				glTranslatef((scale_amt-1.0)*getWidth()/getHeight(), -(scale_amt-1.0), 0.0);
				//Scale up
				glScalef(scale_amt,scale_amt,1.0);
				//Reset pan variables
				fXpos = fYpos = 0.0f;*/
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if(event.button.button == SDL_BUTTON_LEFT)
            {
				isMouseDown = false;
                if(m_iCurGun != m_lGuns.end())
                {
                    (*m_iCurGun)->mouseUp(event.button.x, event.button.y);
                }
            }
            break;

        case SDL_MOUSEMOTION:
            if(m_iCurGun != m_lGuns.end())
            {
                (*m_iCurGun)->mouseMove(event.motion.x,event.motion.y);
            }
            if(isMouseDown)
            {
              glTranslatef(event.motion.xrel/(scale_amt*1000.0), -event.motion.yrel/(scale_amt*1000.0), 0.0);
              fXpos += event.motion.xrel/(scale_amt*1000.0);
              fYpos -= event.motion.yrel/(scale_amt*1000.0);
              //cout << "pos: " << fXpos << ", " << fYpos << endl;
            }
/*            if(m_bDragScreen)
            {
                m_rcViewScreen.offset(m_ptLastMousePos.x - (float32)event.x, m_ptLastMousePos.y - (float32)event.y);
                //cout << "Offset screen " << m_ptLastMousePos.x - event.x << ", " << m_ptLastMousePos.y - event.y << endl;
                m_ptLastMousePos.Set(event.x, event.y);
                //cout << "Screen pos: " << m_rcViewScreen.left << ", " << m_rcViewScreen.top << ", " << m_rcViewScreen.right << ", " << m_rcViewScreen.bottom << endl;
                //cout << "Screen scale: " << (float32)getWidth()/m_rcViewScreen.width() << ", " << (float32)getHeight()/m_rcViewScreen.height() << endl;
            }
            else if(m_bScaleScreen)
            {
                m_rcViewScreen.right += m_ptLastMousePos.x - (float32)event.x;
                m_rcViewScreen.bottom += m_ptLastMousePos.y - (float32)event.y;
                //cout << "Offset screen corner " << m_ptLastMousePos.x - event.x << ", " << m_ptLastMousePos.y - event.y << endl;
                m_ptLastMousePos.Set(event.x, event.y);
                //cout << "Screen pos: " << m_rcViewScreen.left << ", " << m_rcViewScreen.top << ", " << m_rcViewScreen.right << ", " << m_rcViewScreen.bottom << endl;
                //cout << "Screen scale: " << (float32)getWidth()/m_rcViewScreen.width() << ", " << (float32)getHeight()/m_rcViewScreen.height() << endl;
            }*/
            break;

        //TODO
        //case INPUT_MOUSEWHEEL:
			//if(event.wheel < 0)
			//	glScalef(1.0/2.0,1.0/2.0,1.0);
			//else
			//	glScalef((float32)event.wheel,(float32)event.wheel,1.0);
				/*
            m_rcViewScreen.right -= (float32)event.wheel * getWidth()/getHeight();
            m_rcViewScreen.bottom -= (float32)event.wheel;
            m_rcViewScreen.left += (float32)event.wheel * getWidth()/getHeight();
            m_rcViewScreen.top += (float32)event.wheel;
            //cout << "Screen pos: " << m_rcViewScreen.left << ", " << m_rcViewScreen.top << ", " << m_rcViewScreen.right << ", " << m_rcViewScreen.bottom << endl;
            //cout << "Screen scale: " << (float32)getWidth()/m_rcViewScreen.width() << ", " << (float32)getHeight()/m_rcViewScreen.height() << endl;*/
        //    break;
    }
	
	/*Uint8 ms = SDL_GetMouseState(NULL, NULL);
	if(SDL_BUTTON(SDL_BUTTON_WHEELUP) & ms)
		cout << "wheel up" << endl;
	if(SDL_BUTTON(SDL_BUTTON_WHEELDOWN) & ms)
		cout << "wheel down" << endl;*/
}


void myEngine::loadLevelDirectory(string sFilePath)
{
    m_vLevels.clear();
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

void myEngine::toggleRetro()
{
  if(m_iFade != FADE_NONE)
    return; //Don't allow retro-mode-changing if fading (causes crash)
  m_bRetroPhys = !m_bRetroPhys;
  //Reload the current level and set the framerate
  if(m_bRetroPhys)
  {
    loadLevel_retro();
    setFramerate(GAME_FRAMERATE);
    m_iCurGun = m_lGuns.end();
    hideCursor();
  }
  else
  {
    loadLevel_new();
    setFramerate(60);
    m_iCurGun = m_lGuns.begin();     
    showCursor();                 
  }  
}

void myEngine::playSound(string sName)
{
    if(m_bSound)    //If sounds are off, don't play them
        Engine::playSound(sName, 100, 0, (float32)(getFramerate()/(float32)(GAME_FRAMERATE)));    //Pitchshift depending on framerate. For fun.
}

void playSound(string sName, int volume, int pan, float32 pitch)
{
    g_pGlobalEngine->Engine::playSound(sName, volume, pan, pitch);
}

b2Body* createBody(b2BodyDef* def)
{
    return g_pGlobalEngine->createBody(def);
}

Image* getImage(string sName)
{
    return g_pGlobalEngine->getImage(sName);
}

void addObject(Object* obj)
{
    g_pGlobalEngine->addObject(obj);
}



