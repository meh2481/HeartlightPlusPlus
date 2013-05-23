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
//    HGE* h = hgeCreate(HGE_VERSION);
//    ret = h->Random_Int(min, max);
//    h->Release();
//    return ret;
}

float32 randFloat(float32 min, float32 max)
{
    if(min == max)
        return min;
    float32 scale = rand() % 1001;
    return((float32)scale/1000.0*(max-min) + min);
//    HGE* h = hgeCreate(HGE_VERSION);
//    ret = h->Random_Float(min, max);
//    h->Release();
//    return ret;
}

//void crossProduct(float v1[], float v2[], float vR[])
Vec3 crossProduct(Vec3 vec1, Vec3 vec2)
{
    Vec3 ret;
    ret.x = ((vec1.y * vec2.z) - (vec1.z * vec2.y));
    ret.y = -((vec1.z * vec2.z) - (vec1.z * vec2.x));
    ret.z = ((vec1.x * vec2.y) - (vec1.y * vec2.x));
    return ret;
}

//void normalize(float v1[], float vR[])
void Vec3::normalize()
{
    float32 fMag;
    fMag = sqrt(x*x + y*y + z*z);

    x = x / fMag;
    y = y / fMag;
    z = z / fMag;
}

//Rotate the point (x,y,z) around the vector (u,v,w)
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
    float32 x = vecToRot.x;
    float32 y = vecToRot.y;
    float32 z = vecToRot.z;
    result.x = rotVec.x*(ux+vy+wz)+(x*(rotVec.y*rotVec.y+rotVec.z*rotVec.z)-rotVec.x*(vy+wz))*ca+(-wy+vz)*sa;
    result.y = rotVec.y*(ux+vy+wz)+(y*(rotVec.x*rotVec.x+rotVec.z*rotVec.z)-rotVec.y*(ux+wz))*ca+(wx-uz)*sa;
    result.z = rotVec.z*(ux+vy+wz)+(z*(rotVec.x*rotVec.x+rotVec.y*rotVec.y)-rotVec.z*(ux+vy))*ca+(-vx+uy)*sa;
    return result;
}
/*Function RotatePointAroundVector(x#,y#,z#,u#,v#,w#,a#)
ux#=u*x
uy#=u*y
uz#=u*z
vx#=v*x
vy#=v*y
vz#=v*z
wx#=w*x
wy#=w*y
wz#=w*z
sa#=Sin(a)
ca#=Cos(a)
x#=u*(ux+vy+wz)+(x*(v*v+w*w)-u*(vy+wz))*ca+(-wy+vz)*sa
y#=v*(ux+vy+wz)+(y*(u*u+w*w)-v*(ux+wz))*ca+(wx-uz)*sa
z#=w*(ux+vy+wz)+(z*(u*u+v*v)-w*(ux+vy))*ca+(-vx+uy)*sa
End Function*/






