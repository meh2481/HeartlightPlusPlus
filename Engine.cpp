/*
    Heartlight++ source - Engine.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "Engine.h"
ofstream errlog("err.log");

bool Engine::_myFrameFunc()
{
    //Handle input events from SDL
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        handleEvent(event);
        //See if cursor has moved
        if(event.type == SDL_MOUSEMOTION)
        {
            m_ptCursorPos.x = event.motion.x;
            m_ptCursorPos.y = event.motion.y;
        }
        if(event.type == SDL_QUIT)
            return true;
    }

    float32 fCurTime = (float32)SDL_GetTicks()/1000.0;
    //m_fAccumulatedTime += dt;
    if(m_fAccumulatedTime <= fCurTime)
    {
        m_fAccumulatedTime += m_fTargetTime;
        //Box2D wants fixed timestep, so we use target framerate here instead of actual elapsed time
        m_physicsWorld->Step(m_fTargetTime, VELOCITY_ITERATIONS, PHYSICS_ITERATIONS);
        m_cursor->update(m_fTargetTime);
        frame();
        _myRenderFunc();
    }

    if(m_fAccumulatedTime + m_fTargetTime * 3.0 < fCurTime)    //We've gotten far too behind; we could have a huge FPS jump if the load lessens
        m_fAccumulatedTime = fCurTime;   //Drop any frames past this

    return m_bQuitting;
}

bool Engine::_myRenderFunc()
{
    // Begin rendering by clearing the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Start out at default location/zoom
    glLoadIdentity( );
    glTranslatef( 0.0f, 0.0f, MAGIC_ZOOM_NUMBER);

    // Game-specific drawing
    draw();
    if(m_cursor != NULL)    //Draw cursor if it's there
        m_cursor->draw(m_ptCursorPos);

    // End rendering and update the screen
    SDL_GL_SwapBuffers();
    return false;   //Keep going
}

Engine::Engine(uint16_t iWidth, uint16_t iHeight, string sTitle)
{
    b2Vec2 gravity(0.0, 9.8);  //Vector for our world's gravity
    m_physicsWorld = new b2World(gravity);
    m_cursor = NULL;
    m_ptCursorPos.SetZero();
    m_physicsWorld->SetAllowSleeping(true);
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    /*m_hge = hgeCreate(HGE_VERSION);

	// Set up log file, frame function, render function and window title
	m_hge->System_SetState(HGE_LOGFILE, "hge.log");
	m_hge->System_SetState(HGE_FRAMEFUNC, frameFunc);
	m_hge->System_SetState(HGE_RENDERFUNC, renderFunc);
	m_hge->System_SetState(HGE_TITLE, sTitle.c_str());
	m_hge->System_SetState(HGE_FPS, HGEFPS_VSYNC);
	m_hge->System_SetState(HGE_SHOWSPLASH, false);

	// Set up video mode
	m_hge->System_SetState(HGE_WINDOWED, true);
	m_hge->System_SetState(HGE_SCREENWIDTH, iWidth);
	m_hge->System_SetState(HGE_SCREENHEIGHT, iHeight);
	m_hge->System_SetState(HGE_SCREENBPP, 32);

	if(!m_hge->System_Initiate())
	{
		errlog << "Error: " << m_hge->System_GetErrorMessage() << endl;
		exit(1);    //Abort
	}*/
	setup_sdl();
    setup_opengl();

	//Initialize engine stuff
	setFramerate(60);   //60 fps default
	m_fAccumulatedTime = 0.0;
	//m_sprFill = new hgeSprite(0,0,0,64,64); //Initialize to blank sprite
	m_bFirstMusic = true;
	m_bQuitting = false;
	srand(SDL_GetTicks());  //Not as random as it could be... narf
	m_iImgScaleFac = 0;
}

Engine::~Engine()
{
    //Clear up our object map
    clearObjects();

    //Clean up our image map
    clearImages();

    //TODO Any form of OpenGL cleanup?

    //Clean up our sound effects
    /*for(map<string, HEFFECT>::iterator i = m_mSounds.begin(); i != m_mSounds.end(); i++)
    {
        errlog << "Freeing sound effect \"" << i->first << "\"" << endl;
        m_hge->Effect_Free(i->second);
    }*/

    //delete m_sprFill;
    // Clean up and shutdown
	//m_hge->System_Shutdown();
	//m_hge->Release();
	delete m_physicsWorld;
}

void Engine::clearObjects()
{
    //Clean up our object list
    for(multimap<float32, Object*>::iterator i = m_mObjects.begin(); i != m_mObjects.end(); i++)
    {
        b2Body* bod = i->second->getBody();
        if(bod != NULL)
            m_physicsWorld->DestroyBody(bod);
        delete (*i).second;
    }
    m_mObjects.clear();
}

void Engine::clearImages()
{
    //Note that this clears the memory associated with the images, not the filenames. We can reload images as the need arises
    for(map<string, Image*>::iterator i = m_mImages.begin(); i != m_mImages.end(); i++)
        delete (i->second);    //Delete each image
    m_mImages.clear();
}

void Engine::start()
{
    // Load all that we need to
    init();
    // Let's rock now!
    while(!_myFrameFunc());
    //m_hge->System_Start();
}

void Engine::fillRect(Point p1, Point p2, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    fillRect(p1.x, p1.y, p2.x, p2.y, red, green, blue, alpha);
}

void Engine::fillRect(float32 x1, float32 y1, float32 x2, float32 y2, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
//    m_sprFill->SetColor(ARGB(alpha,red,green,blue));    //Set the color of the sprite
//    m_sprFill->Render4V(x1, y1, x2, y1, x2, y2, x1, y2);    //And draw it
}

void Engine::fillRect(Rect rc, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    fillRect(rc.left, rc.top, rc.right, rc.bottom, red, green, blue, alpha);
}

Image* Engine::getImage(string sName)
{
    map<string, Image*>::iterator i = m_mImages.find(sName);
    if(i == m_mImages.end())   //This image isn't here; load it
    {
        Image* img = new Image(m_mImageNames[sName]);   //Create this image
        m_mImages[sName] = img; //Add to the map
        img->_setID(m_mImages.size());   //For now, just numbering 0...n will work for an ID
//        img->scale(m_iImgScaleFac); //Scale this image on creation
        return img;
    }
    return i->second; //Return this image
}

void Engine::createImage(string sPath, string sName)
{
    m_mImageNames[sName] = sPath;
}

void Engine::createSound(string sPath, string sName)
{
    m_mSoundNames[sName] = sPath;
}

/*HEFFECT Engine::_getEffect(string sName)
{
    map<string, HEFFECT>::iterator i = m_mSounds.find(sName);
    if(i == m_mSounds.end())   //This sound isn't here yet; load
    {
        errlog << "Loading sound effect \"" << sName << "\" from file \"" << m_mSoundNames[sName] << "\"" << endl;
        HEFFECT eff = m_hge->Effect_Load(m_mSoundNames[sName].c_str());
        m_mSounds[sName] = eff; //Add to the map
        return eff;
    }
    return i->second; //Return this sound
}*/

void Engine::addObject(Object* obj)
{
    if(obj == NULL) return;
    pair<float32, Object*> objPair;
    objPair.first = obj->_getDepthID();
    objPair.second = obj;
    m_mObjects.insert(objPair);
}

void Engine::updateObjects()
{
    for(multimap<float32, Object*>::iterator i = m_mObjects.begin(); i != m_mObjects.end(); i++)
    {
        if(!(*i).second->update())  //Remove this object if it returns true
        {
            multimap<float32, Object*>::iterator j = i;
            j--;                    //Hang onto map item before this before deleting, since the erase() method seems to do undefined things with
                                    // the original iterator. At least Valgrind thinks so.
            b2Body* bod = i->second->getBody();
            if(bod != NULL)
                m_physicsWorld->DestroyBody(bod);
            delete (*i).second;
            m_mObjects.erase(i);
            i = j;
        }
    }

    //Update all object frames also (outside loop so frames aren't put out of sync)
    for(multimap<float32, Object*>::iterator i = m_mObjects.begin(); i != m_mObjects.end(); i++)
        (*i).second->updateFrame();
}

void Engine::drawObjects(Rect rcScreen)
{
    //m_hge->Gfx_SetTransform(0,0,0,0,0,(float32)getWidth()/rcScreen.width(), (float32)getHeight()/rcScreen.height());
    for(multimap<float32, Object*>::iterator i = m_mObjects.begin(); i != m_mObjects.end(); i++)
    {
        (*i).second->draw(rcScreen, (float32)getWidth()/rcScreen.width(), (float32)getHeight()/rcScreen.height());
    }
    //m_hge->Gfx_SetTransform();
}

void Engine::playSound(string sName, int volume, int pan, float32 pitch)
{
    //HEFFECT eff = _getEffect(sName);
    //m_hge->Effect_PlayEx(eff,volume,pan,pitch);
}

void Engine::pauseMusic()
{
    //if(!m_bFirstMusic)
    //    m_hge->Channel_Pause(m_MusicChannel);
}

void Engine::playMusic(string sName, int volume, int pan, float32 pitch)
{
    /*HEFFECT eff = _getEffect(sName); //Can take a while, depending on the song
    //if(!m_bFirstMusic)
    //{
        if(sName == m_sLastMusic)
        {
            m_hge->Channel_Resume(m_MusicChannel);
            return;
        }
        else
            m_hge->Channel_Stop(m_MusicChannel);
    }
    m_sLastMusic = sName;
    m_MusicChannel = m_hge->Effect_PlayEx(eff,volume,pan,pitch,true);
    m_bFirstMusic = false;*/
}

bool Engine::keyDown(int32_t keyCode)
{
    //TODO Keep actual list
    Uint8 *keystate = SDL_GetKeyState(NULL);
    return(keystate[keyCode]);
    //return(m_hge->Input_GetKeyState(keyCode));
}

/*void Engine::scaleImages(uint16_t scaleFac)
{
    for(map<string, Image*>::iterator i = m_mImages.begin(); i != m_mImages.end(); i++)
        i->second->scale(scaleFac);
    m_iImgScaleFac = scaleFac;
}*/

void Engine::setFramerate(float32 fFramerate)
{
    if(m_fFramerate == 0.0)
        m_fAccumulatedTime = (float32)SDL_GetTicks()/1000.0;   //If we're stuck at 0fps for a while, this number could be huge, which would cause unlimited fps for a bit
    m_fFramerate = fFramerate;
    if(m_fFramerate == 0.0)
        m_fTargetTime = FLT_MAX;    //Avoid division by 0
    else
        m_fTargetTime = 1.0 / m_fFramerate;
}

void Engine::setCursor(Cursor* cur)
{
    //if(m_cursor != NULL)
    //    delete m_cursor;
    m_cursor = cur;
}

void Engine::setup_sdl()
{
  const SDL_VideoInfo* video;

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
  	fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }

  // Quit SDL properly on exit
  atexit(SDL_Quit);

  // Get the current video information
  video = SDL_GetVideoInfo();
  if(video == NULL)
  {
  	fprintf(stderr, "Couldn't get video information: %s\n", SDL_GetError());
    exit(1);
  }

  // Set the minimum requirements for the OpenGL window
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  //TODO: Set icon for window
  //SDL_Surface *image;
  //image = IMG_Load("res/icon.png");
  SDL_WM_SetCaption("Heartlight++", NULL);
  //SDL_WM_SetIcon(image, NULL);
  //SDL_FreeSurface(image);

  // Create SDL window
  if(SDL_SetVideoMode(m_iWidth, m_iHeight, video->vfmt->BitsPerPixel, SDL_OPENGL) == 0)
  {
  	fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
    exit(1);
  }
}

//Set up OpenGL
void Engine::setup_opengl()
{
	// Make the viewport
  glViewport(0, 0, m_iWidth, m_iHeight);

  // Set the camera projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective( 45.0f, (GLfloat)m_iWidth/(GLfloat)m_iHeight, 0.1f, 100.0f );

  glMatrixMode(GL_MODELVIEW);

  // set the clear color to grey
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_TEXTURE_2D);
  glLoadIdentity();

  //Enable image transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glShadeModel( GL_SMOOTH );
  glClearDepth( 1.0f );
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}






