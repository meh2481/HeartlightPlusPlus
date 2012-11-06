/*
    Heartlight++ source - globaldefs.h
    Global definitions for data types
    Copyright (c) 2012 Mark Hutcheson
*/

#include <string>
using namespace std;
#include "hge.h"
#include "hgesprite.h"
#include "hgeparticle.h"
#include "Box2D.h"
#include "tinyxml2.h"
#include "VFS.h"

#define Point b2Vec2    //Our point structure

typedef struct RECT {
    float32 top, bottom, left, right;
}Rect;
