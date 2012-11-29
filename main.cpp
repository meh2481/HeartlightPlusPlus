/*
    Heartlight++ source - main.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "myEngine.h"

int main(int argc, char *argv[])
{
    errlog << "Starting program" << endl;
    myEngine* eng = new myEngine(SCREEN_WIDTH*SCALE_FAC, SCREEN_HEIGHT*SCALE_FAC, "Heartlight++"); //Create our engine

    eng->setFramerate(GAME_FRAMERATE);
    eng->start(); //Get the engine rolling
    delete eng;
    errlog << "Ending program happily" << endl;
	return 0;
}
