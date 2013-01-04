/*
    Heartlight++ source - globaldefs.h
    Global definitions for data types
    Copyright (c) 2012 Mark Hutcheson
*/
#ifndef GLOBALDEFS_H
#define GLOBALDEFS_H

#include <string>
#include <fstream>
using namespace std;
#include "hge.h"
#include "hgesprite.h"
#include "hgeparticle.h"
#include "Box2D.h"
#include "tinyxml2.h"
using namespace tinyxml2;
#include "VFS.h"
#include "VFSTools.h"

#define Point b2Vec2    //Our point structure
#define pi 3.1415926535 //Close enough for my calculations

class Rect {
public:
    float32 left,top,right,bottom;
    float32 width() {return right-left;};
    float32 height() {return bottom-top;};
    void    offset(float32 x, float32 y)    {left+=x;right+=x;top+=y;bottom+=y;};
    void    offset(Point pt)                {offset(pt.x,pt.y);};
    Point   center() {Point pt; pt.x = (right-left)/2.0 + right; pt.y = (bottom-top)/2.0 + top; return pt;};
    void    center(float32* x, float32* y)    {Point pt = center(); *x = pt.x; *y = pt.y;};
    void    scale(float32 fScale) {left*=fScale;right*=fScale;top*=fScale;bottom*=fScale;};
};

extern ofstream errlog;

//Helper functions
string stripCommas(string s);       //Strip all the commas from s, leaving spaces in their place
Rect rectFromString(string s);      //Get a rectangle from comma-separated values in a string
Point pointFromString(string s);    //Get a point from comma-separated values in a string
DWORD colorFromString(string s);    //Get a color from comma-separated values in a string


#endif
