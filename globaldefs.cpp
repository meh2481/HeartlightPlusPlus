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
    uint8_t r,g,b,a;
    istringstream iss(s);
    iss >> r >> g >> b >> a;
    return ARGB(a,r,g,b);
}











