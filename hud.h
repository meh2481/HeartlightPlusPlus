/*
    Heartlight++ source - hud.h
    Base class for HUD (Heads Up Display) items
    Copyright (c) 2012 Mark Hutcheson
*/

#ifndef HUDITEM_H
#define HUDITEM_H

#include "globaldefs.h"
#include "Image.h"
#include "Text.h"

inline void stubSignal(string sSignal){}; //For stubbing out HUD signal handling functions

// HUDItem class -- base class for HUD items
class HUDItem
{
private:
    HUDItem(){};

protected:
    string          m_sName;
    string          m_sSlot;    //What signal we respond to
    Point           m_ptPos;
    list<HUDItem*>  m_lChildren;
    uint16_t        m_iSCALE_FAC;
    void (*m_signalHandler)(string);    //Function for handling signals this class creates

public:
    HUDItem(string sName);
    ~HUDItem();

    virtual void    slot(string sSignal);                               //For handing signals as they come in
    virtual void    event(hgeInputEvent event);                         //For handling input events as they come in
    virtual void    draw(float32 fCurTime, DWORD dwCol = 0xFFFFFFFF);   //For drawing to the screen
    void            addChild(HUDItem* hiChild);                         //Add this child to this HUDItem
    HUDItem*        getChild(string sName);                             //Get the first child that has this name (return NULL if none)

    //Accessor methods
    string          getName()               {return m_sName;};
    void            setName(string sName)   {m_sName = sName;};
    Point           getPos()                {return m_ptPos;};
    void            setPos(Point ptPos)     {m_ptPos = ptPos;};
    uint16_t        getScale()              {return m_iSCALE_FAC;};
    virtual void    setScale(uint16_t iScale);
    void            setSignalHandler(void (*signalHandler)(string));
    void            setSlot(string sSlot)   {m_sSlot = sSlot;};
    string          getSlot()               {return m_sSlot;};

};

// HUDImage class -- for drawing a bg image behind HUD items
class HUDImage : public HUDItem
{
protected:
    Image* m_img;

public:
    HUDImage(string sName);
    ~HUDImage();

    void draw(float32 fCurTime, DWORD dwCol = 0xFFFFFFFF);
    void setScale(uint16_t iScale);

    void    setImage(Image* img);
    Image*  getImage()              {return m_img;};
};

class HUDTextbox : public HUDItem
{
protected:
    Text* m_txtFont;
    string m_sValue;
    uint8_t m_iAlign;
    hgeSprite* m_spFill;    //for filling the background

public:
    HUDTextbox(string sName);
    ~HUDTextbox();

    void draw(float32 fCurTime, DWORD dwCol = 0xFFFFFFFF);
    void slot(string sSignal);                                  //This item responds to a signal to change the text
    void setScale(uint16_t iScale);

    void    setFont(Text* txt)          {m_txtFont = txt;};     //Set the font used by this textbox
    Text*   getFont()                   {return m_txtFont;};
    void    setText(string s)           {m_sValue = s;};        //Set the text to display (alternative to the signal/slot method)
    void    setText(uint64_t iNum);                             //Set the text from an integer
    string  getText()                   {return m_sValue;};
    void    setAlign(uint8_t iAlign)    {m_iAlign = iAlign;};   //Set the font alignment for this textbox
    uint8_t getAlign()                  {return m_iAlign;};
    void    setFill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void    setFill(DWORD dwFill);

};

//HUD class -- High-level handler of Heads-Up-Display and subclasses
class HUD : public HUDItem
{
protected:
    map<string, Image*> m_mImages;
    map<string, Text*>  m_mFonts;

public:
    HUD(string sName);
    ~HUD();

    void create(string sXMLFilename);   //Create this HUD from the XML file
};












#endif
