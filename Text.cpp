/*
    Heartlight++ source - Text.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "Text.h"

Text::Text(string sXMLFilename)
{
    m_iAlign = ALIGN_RIGHT | ALIGN_BOTTOM;
    m_iSCALE_FAC = 1;
    m_imgFont = NULL;

    //  Load font image and glyphs from xml
    //  File format:
    //
    //  <?xml version="1.0" encoding="UTF-8"?>
    //  <font name="green"
    //      <image path="res/gfx/orig/text_green.png" />
    //      <char ascii="a" rect="0,0,15,9" />
    //      ...
    //  </font>

    XMLDocument* doc = new XMLDocument();
    doc->LoadFile(sXMLFilename.c_str());

    XMLElement* elem = doc->FirstChildElement("font");
    if(elem == NULL) return;
    const char* cName = elem->Attribute("name");
    if(cName != NULL)
        m_sName = cName;
    errlog << "Creating font \"" << m_sName << "\"" << endl;
    for(elem = elem->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
    {
        cName = elem->Name();
        if(cName == NULL) return;
        string sName(cName);
        if(sName == "image")    //Image
        {
            const char* cPath = elem->Attribute("path");
            if(cPath == NULL) return;
            m_imgFont = new Image(cPath);   //Create image

        }
        else if(sName == "char")    //Character
        {
            const char* cChar = elem->Attribute("ascii");
            if(cChar == NULL) return;
            const char* cRect = elem->Attribute("rect");
            if(cRect == NULL) return;
            Rect rc = rectFromString(cRect);
            m_mRectangles[cChar[0]] = rc;   //Stick this into the list
        }
    }
    delete doc;
}

Text::~Text()
{
    errlog << "Destroying font \"" << m_sName << "\"" << endl;
    if(m_imgFont != NULL)
        delete m_imgFont;
}

void Text::render(string sText, Point pt)
{
    render(sText, pt.x, pt.y);
}

void Text::render(string sText, float32 x, float32 y)
{
    if(m_imgFont == NULL)
        return;

    if(m_iAlign & ALIGN_LEFT)
        x -= sizeString(sText).x;
    else if(m_iAlign & ALIGN_CENTER)
        x -= sizeString(sText).x / 2;
    if(m_iAlign & ALIGN_TOP)
        y -= sizeString(sText).y;
    else if(m_iAlign & ALIGN_MIDDLE)
        y -= sizeString(sText).y / 2;

    for(string::iterator i = sText.begin(); i != sText.end(); i++)
    {
        unsigned char c = *i;
        if(c == '\0')
            break;

        map<unsigned char, Rect>::iterator iRect = m_mRectangles.find(c);
        if(iRect == m_mRectangles.end())
            continue;   //Skip over chars we can't draw

        Rect rc = iRect->second;
        rc.left *= m_iSCALE_FAC;
        rc.top *= m_iSCALE_FAC;
        rc.right *= m_iSCALE_FAC;
        rc.bottom *= m_iSCALE_FAC;

        m_imgFont->draw(x, y, rc);
        x += rc.width();
    }
}

Point Text::sizeString(string sText)
{
    Point ptResult;
    ptResult.SetZero();
    if(!sText.size())
        return ptResult;
    for(string::iterator i = sText.begin(); i != sText.end(); i++)
    {
        unsigned char c = *i;
        if(c == '\0')
            break;

        map<unsigned char, Rect>::iterator iRect = m_mRectangles.find(c);
        if(iRect == m_mRectangles.end())
            continue;   //Skip over chars we can't draw

        //If this is a taller character than the others, make this the final height
        if(ptResult.y < iRect->second.height())
            ptResult.y = iRect->second.height();

        ptResult.x += iRect->second.width(); //Add the width of this char to the total width
    }
    ptResult *= m_iSCALE_FAC;
    return ptResult;
}

void Text::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if(m_imgFont != NULL)
        m_imgFont->setColor(r,g,b,a);
}

void Text::setColor(DWORD dwCol)
{
    if(m_imgFont != NULL)
        m_imgFont->setColor(dwCol);
}

//void Text::setScale(uint16_t iScaleFac)
//{
//    m_iSCALE_FAC = iScaleFac;
//    if(m_imgFont != NULL)
//        m_imgFont->scale(iScaleFac);
//}

void Text::setAlign(uint8_t iAlign)
{
    m_iAlign = iAlign;
}







