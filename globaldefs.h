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
#include "VFS.h"

#define Point b2Vec2    //Our point structure

class Rect {
public:
    float32 left,top,right,bottom;
    float32 width() {return right-left;};
    float32 height() {return bottom-top;};
    Point center() {Point pt; pt.x = (right-left)/2.0 + right; pt.y = (bottom-top)/2.0 + top; return pt;};
};

extern ofstream errlog;


#endif
