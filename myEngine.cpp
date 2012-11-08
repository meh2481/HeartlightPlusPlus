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
    UpdateObjects();
    return false;   //Do nothing for now
}

void myEngine::draw()
{
    //Just draw all objects
    DrawObjects();
}

void myEngine::init()
{
    Image* img = getImage("res/gfx/orig/rock.png");
    img->Scale(2);
    for(int i = 0; i < 20; i++)
    {
        for(int j = 0; j < 12; j++)
        {
            Object* obj = new Object(img);
            obj->SetPos(i*obj->GetWidth(), j*obj->GetHeight());
            AddObject(obj);
        }
    }
}

myEngine::myEngine(uint16_t iWidth, uint16_t iHeight, string sTitle) : Engine(iWidth, iHeight, sTitle)
{
    g_pGlobalEngine = this;
    m_myImg = NULL;
}

myEngine::~myEngine()
{
}
