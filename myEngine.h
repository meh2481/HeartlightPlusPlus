/*
    Heartlight++ source - myEngine.h
    Copyright (c) 2012 Mark Hutcheson
*/
#ifndef MYENGINE_H
#define MYENGINE_H

#include "Engine.h"

class myEngine : public Engine
{
private:
    Image* m_myImg;

protected:
    bool frame();
    void draw();
    void init();

public:
    myEngine(uint16_t iWidth, uint16_t iHeight, string sTitle);
    ~myEngine();
};



#endif
