/*
    Heartlight++ source - myEngine.h
    Copyright (c) 2012 Mark Hutcheson
*/

#include "Engine.h"

class myEngine : public Engine
{
private:

protected:
    bool frame();
    void draw();

public:
    myEngine(int iWidth, int iHeight, string sTitle);
    ~myEngine();
};
