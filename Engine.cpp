/*
    Heartlight++ source - Engine.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "Engine.h"
ofstream errlog("err.log");

bool Engine::myFrameFunc()
{
    float dt = m_hge->Timer_GetDelta();
    m_fAccumulatedTime += dt;
    if(m_fAccumulatedTime >= m_fTargetTime)
    {
        m_fAccumulatedTime -= m_fTargetTime;
        return frame();
    }
    return false;
}

bool Engine::myRenderFunc()
{
    // Begin rendering
	m_hge->Gfx_BeginScene();

	// Clear screen with black color
	m_hge->Gfx_Clear(0);

    // Game-specific drawing
    draw();

    // End rendering and update the screen
	m_hge->Gfx_EndScene();
    return false;   //Keep going
}

Engine::Engine(uint16_t iWidth, uint16_t iHeight, string sTitle)
{
    m_hge = hgeCreate(HGE_VERSION);

	// Set up log file, frame function, render function and window title
	m_hge->System_SetState(HGE_LOGFILE, "hge.log");
	m_hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	m_hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	m_hge->System_SetState(HGE_TITLE, sTitle.c_str());
	m_hge->System_SetState(HGE_FPS, HGEFPS_VSYNC);

	// Set up video mode
	m_hge->System_SetState(HGE_WINDOWED, true);
	m_hge->System_SetState(HGE_SCREENWIDTH, iWidth);
	m_hge->System_SetState(HGE_SCREENHEIGHT, iHeight);
	m_hge->System_SetState(HGE_SCREENBPP, 32);

	if(!m_hge->System_Initiate())
	{
		errlog << "Error: " << m_hge->System_GetErrorMessage() << endl;
		exit(1);    //Abort
	}

	//Initialize engine stuff
	setFramerate(60);   //60 fps default
	m_fAccumulatedTime = 0.0;
	m_sprFill = new hgeSprite(0,0,0,64,64); //Initialize to blank sprite
}

Engine::~Engine()
{
    //Clean up our object list
    for(list<Object*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
        delete (*i);

    //Clean up our image map
    for(map<string, Image*>::iterator i = m_mImages.begin(); i != m_mImages.end(); i++)
        delete (i->second);    //Delete each image

    delete m_sprFill;
    // Clean up and shutdown
	m_hge->System_Shutdown();
	m_hge->Release();
}

void Engine::start()
{
    // Load all that we need to
    init();
    // Let's rock now!
    m_hge->System_Start();
}

void Engine::fillRect(Point p1, Point p2, uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha)
{
    fillRect(p1.x, p1.y, p2.x, p2.y, red, green, blue, alpha);
}

void Engine::fillRect(float32 x1, float32 y1, float32 x2, float32 y2, uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha)
{
    m_sprFill->SetColor(ARGB(alpha,red,green,blue));    //Set the color of the sprite
    m_sprFill->Render4V(x1, y1, x2, y1, x2, y2, x1, y2);    //And draw it
}

void Engine::fillRect(Rect rc, uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha)
{
    fillRect(rc.left, rc.top, rc.right, rc.bottom, red, green, blue, alpha);
}

Image* Engine::getImage(string sFilename)
{
    map<string, Image*>::iterator i = m_mImages.find(sFilename);
    if(i == m_mImages.end())   //This image isn't here yet; load
    {
        Image* img = new Image(sFilename);
        m_mImages[sFilename] = img; //Add to the map
        return img;
    }
    return i->second; //Return this image
}

void Engine::AddObject(Object* obj)
{
    m_lObjects.push_back(obj);
}

void Engine::UpdateObjects()
{
    for(list<Object*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
    {
        (*i)->Update();
    }
}

void Engine::DrawObjects(float fScale)
{
    for(list<Object*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
    {
        (*i)->Draw(fScale);
    }
}







