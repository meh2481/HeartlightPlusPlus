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
#include "include/Box2D.h"
#include "include/tinyxml2.h"
using namespace tinyxml2;
#include "include/VFS.h"
#include "include/VFSTools.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "FreeImage.h"
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL_image.h>
#endif


#define Point b2Vec2    //Our point structure
#define Vec2 Point
#define PI 3.1415926535 //Close enough for my calculations
#define DEG2RAD 3.141593f / 180.0f  //Convert degrees to radians
#define RAD2DEG 180.0f / 3.141593f  //Convert radians to degrees
#define DIFF_EPSILON 0.0000001      //HACK: How much different two vectors must be to register as a difference


//TODO Remove
#ifndef _WIN32
typedef uint32_t DWORD;
#endif
#define ARGB(a,r,g,b)	((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b))
#define GETA(col)		((col)>>24)
#define GETR(col)		(((col)>>16) & 0xFF)
#define GETG(col)		(((col)>>8) & 0xFF)
#define GETB(col)		((col) & 0xFF)
#define SETA(col,a)		(((col) & 0x00FFFFFF) + (DWORD(a)<<24))

class Rect {
public:
    float32 left,top,right,bottom;
    float32 width() {return right-left;};
    float32 height() {return bottom-top;};
    float32 area()  {return width()*height();};
    void    offset(float32 x, float32 y)    {left+=x;right+=x;top+=y;bottom+=y;};
    void    offset(Point pt)                {offset(pt.x,pt.y);};
    Point   center() {Point pt; pt.x = (right-left)/2.0 + right; pt.y = (bottom-top)/2.0 + top; return pt;};
    void    center(float32* x, float32* y)    {Point pt = center(); *x = pt.x; *y = pt.y;};
    void    scale(float32 fScale) {left*=fScale;right*=fScale;top*=fScale;bottom*=fScale;};
    void    scale(float32 fScalex, float32 fScaley) {left*=fScalex;right*=fScalex;top*=fScaley;bottom*=fScaley;};
    void    set(float32 fleft,float32 ftop,float32 fright,float32 fbottom)  {left=fleft;top=ftop;right=fright;bottom=fbottom;};
};

class Vec3
{
public:
    float32 x, y, z;

    void set(float32 fx, float32 fy, float32 fz)    {x=fx;y=fy;z=fz;};
    void setZero()  {set(0,0,0);};

    //Helpful math functions
    void normalize();
    Vec3 normalized();  //Doesn't modify original vector, returns normalized version

    //Operators for easy use
    bool operator!=(const Vec3& v);
};

extern ofstream errlog;

//Helper functions
Vec3 crossProduct(Vec3 vec1, Vec3 vec2);    //Cross product of two vectors
float32 dotProduct(Vec3 vec1, Vec3 vec2);   //Dot product of two vectors
Vec3 rotateAroundVector(Vec3 vecToRot, Vec3 rotVec, float32 fAngle);    //Rotate one vector around another
string stripCommas(string s);       //Strip all the commas from s, leaving spaces in their place
Rect rectFromString(string s);      //Get a rectangle from comma-separated values in a string
Point pointFromString(string s);    //Get a point from comma-separated values in a string
DWORD colorFromString(string s);    //Get a color from comma-separated values in a string
int32_t randInt(int32_t min, int32_t max);  //Get a random integer
float32 randFloat(float32 min, float32 max);        //Get a random float32
#ifdef __APPLE__
//returns the closest power of two value
int power_of_two(int input);
#endif

#endif
