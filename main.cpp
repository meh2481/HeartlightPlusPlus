/*
    Heartlight++ source - main.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "myEngine.h"

int main(int argc, char *argv[])
{
    myEngine eng(800, 600, "Heartlight++"); //Create our engine

    eng.start(); //Get the engine rolling
	return 0;
}
