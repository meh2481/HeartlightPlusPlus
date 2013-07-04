/*
    Heartlight++ source - main.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "myEngine.h"

#if !(defined(_UNIX) || defined(__APPLE__)) 
#include <windows.h>
int CALLBACK WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow
)
#else
int main(int argc, char *argv[])
#endif
{
    errlog << "Starting program" << endl;
    myEngine* eng = new myEngine(SCREEN_WIDTH, SCREEN_HEIGHT, "Heartlight++"); //Create our engine

    eng->setFramerate(GAME_FRAMERATE);
    eng->start(); //Get the engine rolling
    delete eng;
    errlog << "Ending program happily" << endl;
	return 0;
}
