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

inline void stubSignal(string sSignal){errlog << "Generating signal: " << sSignal << endl;}; //For stubbing out HUD signal handling functions

// HUDItem class -- base class for HUD items
class HUDItem
{
private:
    HUDItem(){};

protected:
    string          m_sName;
    string          m_sSignal;  //What signal this generates
    Point           m_ptPos;
    list<HUDItem*>  m_lChildren;
    uint16_t        m_iSCALE_FAC;
    void (*m_signalHandler)(string);    //Function for handling signals this class creates

public:
    HUDItem(string sName);
    ~HUDItem();

    virtual void    event(hgeInputEvent event);                         //For handling input events as they come in
    virtual void    draw(float32 fCurTime, DWORD dwCol = 0xFFFFFFFF);   //For drawing to the screen
    void            addChild(HUDItem* hiChild);                         //Add this child to this HUDItem
    HUDItem*        getChild(string sName);                             //Get the first child that has this name (return NULL if none)

    //Accessor methods
    string          getName()                   {return m_sName;};
    void            setName(string sName)       {m_sName = sName;};
    Point           getPos()                    {return m_ptPos;};
    void            setPos(Point ptPos)         {m_ptPos = ptPos;};
    uint16_t        getScale()                  {return m_iSCALE_FAC;};
    virtual void    setScale(uint16_t iScale);
    void            setSignal(string sSignal)   {m_sSignal = sSignal;};
    string          getSignal()                 {return m_sSignal;};
    void            setSignalHandler(void (*signalHandler)(string));

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
    void setScale(uint16_t iScale);

    void    setFont(Text* txt)          {m_txtFont = txt;};     //Set the font used by this textbox
    Text*   getFont()                   {return m_txtFont;};
    void    setText(string s)           {m_sValue = s;};        //Set the text to display
    void    setText(uint64_t iNum);                             //Set the text from an integer
    string  getText()                   {return m_sValue;};
    void    setAlign(uint8_t iAlign)    {m_iAlign = iAlign;};   //Set the font alignment for this textbox
    uint8_t getAlign()                  {return m_iAlign;};
    void    setFill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void    setFill(DWORD dwFill);

};

//HUDToggle class -- Toggleable HUD items
class HUDToggle : public HUDItem
{
protected:
    int32_t     m_iKey;         //Key that we watch for to toggle value
    Image*      m_imgEnabled;   //Enabled/disabled images
    Image*      m_imgDisabled;
    bool        m_bValue;       //Enabled/Disabled

public:
    HUDToggle(string sName);
    ~HUDToggle();

    void event(hgeInputEvent event);                        //This will create a signal if the event matches our set key
    void draw(float32 fCurTime, DWORD dwCol = 0xFFFFFFFF);  //Draws the enabled or disabled image
    void setScale(uint16_t iScale);                         //Scales the child images

    void setEnabled(bool bValue)        {m_bValue = bValue;};
    bool getEnabled()                   {return m_bValue;};
    void setKey(int32_t iKey)           {m_iKey = iKey;};
    int32_t getKey()                    {return m_iKey;};
    void setEnabledImage(Image* img);
    void setDisabledImage(Image* img);

};

//HUDGroup class -- For clumping HUDToggle items together
//class HUDGroup : public HUDItem
//{

//};

//HUD class -- High-level handler of Heads-Up-Display and subclasses
class HUD : public HUDItem
{
protected:
    map<string, Image*> m_mImages;
    map<string, Text*>  m_mFonts;

public:
    HUD(string sName);
    ~HUD();

    void create(string sXMLFilename);   //Create this HUD from an XML file
};












#endif
