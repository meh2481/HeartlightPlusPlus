/*
    Heartlight++ source - globaldefs.cpp
    Helper functions for various uses
    Copyright (c) 2012 Mark Hutcheson
*/

#include "globaldefs.h"
#include <sstream>

string stripCommas(string s)
{
    //Replace all ',' characters with ' '
    for(int i = 0; ; i++)
    {
        size_t iPos = s.find(',', i);
        if(iPos == s.npos)
            break;  //Done

        s.replace(iPos, 1, " ");
    }
    return s;
}

Rect rectFromString(string s)
{
    s = stripCommas(s);

    //Now, parse
    istringstream iss(s);
    Rect rc;
    iss >> rc.left >> rc.top >> rc.right >> rc.bottom;
    return rc;
}

Point pointFromString(string s)
{
    s = stripCommas(s);

    //Now, parse
    istringstream iss(s);
    Point pt;
    iss >> pt.x >> pt.y;
    return pt;
}

DWORD colorFromString(string s)
{
    s = stripCommas(s);

    //Now, parse
    uint16_t r,g,b,a;
    istringstream iss(s);
    iss >> r >> g >> b >> a;
    return ARGB(a,r,g,b);
}

//TODO Use actual random number generator (Mersenne Twister or such)
int32_t randInt(int32_t min, int32_t max)
{
    if(min == max)
        return min;
    int32_t diff = max-min+1;
    return(rand()%diff + min);
}

float32 randFloat(float32 min, float32 max)
{
    if(min == max)
        return min;
    float32 scale = rand() % 1001;
    return((float32)scale/1000.0*(max-min) + min);
}

void Vec3::normalize()
{
    float32 fMag;
    fMag = sqrt(x*x + y*y + z*z);

    x = x / fMag;
    y = y / fMag;
    z = z / fMag;
}

Vec3 Vec3::normalized()
{
    Vec3 ret;
    float32 fMag = sqrt(x*x + y*y + z*z);

    ret.x = x / fMag;
    ret.y = y / fMag;
    ret.z = z / fMag;
    return ret;
}

//Test for inequality between vectors
bool Vec3::operator!=(const Vec3& v)
{
    float32 xdiff = abs(v.x - x);
    float32 ydiff = abs(v.y - y);
    float32 zdiff = abs(v.z - z);

    if(xdiff > DIFF_EPSILON || ydiff > DIFF_EPSILON || zdiff > DIFF_EPSILON)    //I call hacks. But it works.
        return true;
    return false;
}

Vec3 crossProduct(Vec3 vec1, Vec3 vec2)
{
    Vec3 ret;
    ret.x = ((vec1.y * vec2.z) - (vec1.z * vec2.y));
    ret.y = -((vec1.z * vec2.z) - (vec1.z * vec2.x));
    ret.z = ((vec1.x * vec2.y) - (vec1.y * vec2.x));
    return ret;
}

float32 dotProduct(Vec3 vec1, Vec3 vec2)
{
    return (vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z);
}

//Rotate the vector vecToRot around the vector rotVec
Vec3 rotateAroundVector(Vec3 vecToRot, Vec3 rotVec, float32 fAngle)
{
    Vec3 result;
    float32 ux = rotVec.x * vecToRot.x;
    float32 uy = rotVec.x * vecToRot.y;
    float32 uz = rotVec.x * vecToRot.z;
    float32 vx = rotVec.y * vecToRot.x;
    float32 vy = rotVec.y * vecToRot.y;
    float32 vz = rotVec.y * vecToRot.z;
    float32 wx = rotVec.z * vecToRot.x;
    float32 wy = rotVec.z * vecToRot.y;
    float32 wz = rotVec.z * vecToRot.z;
    float32 sa = sin(DEG2RAD*fAngle);
    float32 ca = cos(DEG2RAD*fAngle);
    //Matrix math without actual matrices woo
    result.x = rotVec.x*(ux+vy+wz)+(vecToRot.x*(rotVec.y*rotVec.y+rotVec.z*rotVec.z)-rotVec.x*(vy+wz))*ca+(-wy+vz)*sa;
    result.y = rotVec.y*(ux+vy+wz)+(vecToRot.y*(rotVec.x*rotVec.x+rotVec.z*rotVec.z)-rotVec.y*(ux+wz))*ca+(wx-uz)*sa;
    result.z = rotVec.z*(ux+vy+wz)+(vecToRot.z*(rotVec.x*rotVec.x+rotVec.y*rotVec.y)-rotVec.z*(ux+vy))*ca+(-vx+uy)*sa;
    return result;
}


#ifdef __APPLE__
//returns the closest power of two value
int power_of_two(int input)
{
	int value = 1;
	while ( value < input ) {
		value <<= 1;
	}
	return value;
}
#endif


