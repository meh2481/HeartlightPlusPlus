/*
    Heartlight++ source - main.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "myEngine.h"

#ifdef _WIN32
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
#ifdef __APPLE__
    errlog << "Starting FreeImage" << endl;
    FreeImage_Initialise();
#endif
    myEngine* eng = new myEngine(1000, 600, "Heartlight++"); //Create our engine

    eng->setFramerate(GAME_FRAMERATE);
    eng->start(); //Get the engine rolling
    delete eng;
#ifdef __APPLE__
    errlog << "Closing FreeImage" << endl;
    FreeImage_DeInitialise();
#endif
    errlog << "Ending program happily" << endl;
    return 0;
}
