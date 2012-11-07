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

bool myEngine::frame()
{
    return false;   //Do nothing for now
}

void myEngine::draw()
{
    fillRect(32,32,64,64,255,0,0,255);  //For now, just draw a red box to make sure everything's good
    m_myImg->Draw(64,64);
}

void myEngine::init()
{
    m_myImg = new Image("res/gfx/new/rock.png");
}

myEngine::myEngine(int iWidth, int iHeight, string sTitle) : Engine(iWidth, iHeight, sTitle)
{
    g_pGlobalEngine = this;
    m_myImg = NULL;
}

myEngine::~myEngine()
{
    delete m_myImg;
}
