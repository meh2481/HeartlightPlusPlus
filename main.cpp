/*
    Heartlight++ source - main.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "myEngine.h"

int main(int argc, char *argv[])
{
    myEngine eng(SCREEN_WIDTH*SCALE_FAC, SCREEN_HEIGHT*SCALE_FAC, "Heartlight++"); //Create our engine

    eng.setFramerate(GAME_FRAMERATE);
    eng.start(); //Get the engine rolling
	return 0;
}
