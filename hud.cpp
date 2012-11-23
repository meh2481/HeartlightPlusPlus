/*
    Heartlight++ source - hud.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "hud.h"

//-------------------------------------------------------------------------------------
// HUDItem class functions
//-------------------------------------------------------------------------------------
HUDItem::HUDItem(string sName)
{
    m_ptPos.SetZero();
    m_sName = sName;
    m_signalHandler = stubSignal;
    m_iSCALE_FAC = 1;
}

HUDItem::~HUDItem()
{
    for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
        delete (*i);    //Clean up all children also
}

void HUDItem::event(hgeInputEvent event)
{
    //Base class does nothing with this, except pass on
    for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
        (*i)->event(event);
}

void HUDItem::draw(float32 fCurTime, DWORD dwCol)
{
    //Base class does nothing with this, except pass on
    for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
        (*i)->draw(fCurTime, dwCol);
}

void HUDItem::setSignalHandler(void (*signalHandler)(string))
{
    m_signalHandler = signalHandler;

    //Make all children have this signal handler as well
    for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
        (*i)->setSignalHandler(signalHandler);
}

void HUDItem::addChild(HUDItem* hiChild)
{
    if(hiChild == NULL)
        return;
    //hiChild->setSignalHandler(m_signalHandler);
    hiChild->setScale(m_iSCALE_FAC);
    m_lChildren.push_back(hiChild);
}

HUDItem* HUDItem::getChild(string sName)
{
    if(m_sName == sName)    //Base case 1: We're this child they're looking for
        return this;

    for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
    {
        HUDItem* hi = (*i)->getChild(sName);   //Recursive call to children
        if(hi != NULL)
            return hi;
    }
    return NULL;    //Base case 2: No child of this with that name
}

void HUDItem::setScale(uint16_t iScale)
{
    m_iSCALE_FAC = iScale;
    for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
        (*i)->setScale(iScale);
}

//-------------------------------------------------------------------------------------
// HUDImage class functions
//-------------------------------------------------------------------------------------
HUDImage::HUDImage(string sName) : HUDItem(sName)
{
    m_img = NULL;
}

HUDImage::~HUDImage()
{

}

void HUDImage::draw(float32 fCurTime, DWORD dwCol)
{
    HUDItem::draw(fCurTime, dwCol);
    if(m_img != NULL)
    {
        m_img->setColor(dwCol);
        m_img->draw(m_ptPos.x * m_iSCALE_FAC, m_ptPos.y * m_iSCALE_FAC);
    }
}

void HUDImage::setScale(uint16_t iScale)
{
    HUDItem::setScale(iScale);
    m_img->scale(m_iSCALE_FAC);
}

void HUDImage::setImage(Image* img)
{
    m_img = img;
    img->scale(m_iSCALE_FAC);
}

//-------------------------------------------------------------------------------------
// HUDTextbox class functions
//-------------------------------------------------------------------------------------
HUDTextbox::HUDTextbox(string sName) : HUDItem(sName)
{
    m_iAlign = ALIGN_RIGHT | ALIGN_BOTTOM;
    m_txtFont = NULL;
    m_spFill = new hgeSprite(0, 0, 0, 32, 32);
    m_spFill->SetColor(0x00000000);
}

HUDTextbox::~HUDTextbox()
{
    delete m_spFill;
}

void HUDTextbox::draw(float32 fCurTime, DWORD dwCol)
{
    HUDItem::draw(fCurTime, dwCol);
    if(m_txtFont == NULL) return;

    m_txtFont->setAlign(m_iAlign);

    //Render a box around where this text will be
    Point ptSize = m_txtFont->sizeString(m_sValue);

    Rect rcText = {m_ptPos.x*m_iSCALE_FAC, m_ptPos.y*m_iSCALE_FAC, m_ptPos.x*m_iSCALE_FAC + ptSize.x, m_ptPos.y*m_iSCALE_FAC + ptSize.y};

    //Deal with alignment issues
    if(m_iAlign & ALIGN_CENTER)
        rcText.offset(-rcText.width()/2,0);
    if(m_iAlign & ALIGN_LEFT)
        rcText.offset(-rcText.width(),0);
    if(m_iAlign & ALIGN_MIDDLE)
        rcText.offset(0,-rcText.height()/2);
    if(m_iAlign & ALIGN_TOP)
        rcText.offset(0,-rcText.height());

    //Fill in bg
    m_spFill->Render4V(rcText.left, rcText.top,
                       rcText.left+rcText.width()+m_iSCALE_FAC, rcText.top,
                       rcText.left+rcText.width()+m_iSCALE_FAC, rcText.top+rcText.height()+m_iSCALE_FAC,
                       rcText.left, rcText.top+rcText.height()+m_iSCALE_FAC);

    //Render the text
    m_txtFont->render(m_sValue, m_ptPos.x*m_iSCALE_FAC, m_ptPos.y*m_iSCALE_FAC);
}

void HUDTextbox::setFill(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    setFill(ARGB(a,r,g,b));
}

void HUDTextbox::setFill(DWORD dwFill)
{
    m_spFill->SetColor(dwFill);
}

void HUDTextbox::setScale(uint16_t iScale)
{
    HUDItem::setScale(iScale);
    m_txtFont->setScale(iScale);
}

void HUDTextbox::setText(uint64_t iNum)
{
    char c[256];
    sprintf(c, "%ld", iNum);
    setText(c);
}

//-------------------------------------------------------------------------------------
// HUD class functions
//-------------------------------------------------------------------------------------
HUDToggle::HUDToggle(string sName) : HUDItem(sName)
{
    m_iKey = 0; //No HGEK_ code has this value. Convenient, wot wot?
    m_imgEnabled = NULL;
    m_imgDisabled = NULL;
    m_bValue = false;   //Default is disabled
}

HUDToggle::~HUDToggle()
{

}

void HUDToggle::event(hgeInputEvent event)
{
    HUDItem::event(event);
    if(event.type == INPUT_KEYDOWN && event.key == m_iKey)
    {
        m_signalHandler(m_sSignal); //Generate signal
        m_bValue = !m_bValue;   //Toggle
    }
}

void HUDToggle::draw(float32 fCurTime, DWORD dwCol)
{
    HUDItem::draw(fCurTime, dwCol);

    if(m_bValue)    //Draw enabled image
    {
        if(m_imgEnabled != NULL)
        {
            m_imgEnabled->setColor(dwCol);
            m_imgEnabled->draw(m_ptPos.x*m_iSCALE_FAC, m_ptPos.y*m_iSCALE_FAC);
        }
    }
    else    //Draw disabled image
    {
        if(m_imgDisabled != NULL)
        {
            m_imgDisabled->setColor(dwCol);
            m_imgDisabled->draw(m_ptPos.x*m_iSCALE_FAC, m_ptPos.y*m_iSCALE_FAC);
        }
    }
}

void HUDToggle::setScale(uint16_t iScale)
{
    HUDItem::setScale(iScale);
    if(m_imgEnabled != NULL)
        m_imgEnabled->scale(iScale);
    if(m_imgDisabled != NULL)
        m_imgDisabled->scale(iScale);
}

void HUDToggle::setEnabledImage(Image* img)
{
    m_imgEnabled = img;
    if(m_imgEnabled != NULL)
        m_imgEnabled->scale(m_iSCALE_FAC);
}

void HUDToggle::setDisabledImage(Image* img)
{
    m_imgDisabled = img;
    if(m_imgDisabled != NULL)
        m_imgDisabled->scale(m_iSCALE_FAC);
}


//-------------------------------------------------------------------------------------
// HUD class functions
//-------------------------------------------------------------------------------------
HUD::HUD(string sName) : HUDItem(sName)
{
}

HUD::~HUD()
{
    errlog << "Destroying HUD " << m_sName << endl;

    //Delete all images
    for(map<string, Image*>::iterator i = m_mImages.begin(); i != m_mImages.end(); i++)
        delete i->second;

    //And all fonts
    for(map<string, Text*>::iterator i = m_mFonts.begin(); i != m_mFonts.end(); i++)
        delete i->second;
}

void HUD::create(string sXMLFilename)
{
    //Load in the XML document

    XMLDocument doc;
    doc.LoadFile(sXMLFilename.c_str());

    XMLElement* elem = doc.FirstChildElement("hud");
    if(elem == NULL) return;
    const char* cName = elem->Attribute("name");
    if(cName != NULL)
        m_sName = cName;    //Grab the name
    errlog << "Creating HUD " << m_sName << endl;
    elem = elem->FirstChildElement();
    //Load all elements
    XMLNode* temp = elem;
    for(; elem != NULL; temp = elem->NextSibling())
    {
        //If there are non-element items, skip over them safely
        while(temp != NULL && temp->ToElement() == NULL)
            temp = temp->NextSibling();
        if(temp == NULL) break;
        elem = temp->ToElement();

        if(elem == NULL) break;
        cName = elem->Name();
        if(cName == NULL) return;
        string sName(cName);
        if(sName == "images")    //Image list
        {
            //Load all images
            XMLElement* elemImage = elem->FirstChildElement("image");
            XMLNode* temp2 = elemImage;
            for(; elemImage != NULL; temp2 = elemImage->NextSibling())
            {
                while(temp2 != NULL && temp2->ToElement() == NULL)
                    temp2 = temp2->NextSibling();
                if(temp2 == NULL) break;
                elemImage = temp2->ToElement();

                if(elemImage == NULL) break;    //Done
                const char* cImgName = elemImage->Attribute("name");
                if(cImgName == NULL) continue;
                string sImgName(cImgName);
                const char* cImgPath = elemImage->Attribute("path");
                if(cImgPath == NULL) continue;
                Image* img = new Image(cImgPath);   //Load image
                m_mImages[sImgName] = img;          //Stick it into our list
            }
        }
        else if(sName == "fonts")    //Font list
        {
            //Load all fonts
            XMLElement* elemFont = elem->FirstChildElement("font");
            XMLNode* temp2 = elemFont;
            for(; elemFont != NULL; temp2 = elemFont->NextSibling())
            {
                while(temp2 != NULL && temp2->ToElement() == NULL)
                    temp2 = temp2->NextSibling();
                if(temp2 == NULL) break;
                elemFont = temp2->ToElement();

                const char* cFontName = elemFont->Attribute("name");
                if(cFontName == NULL) continue;
                string sFontName(cFontName);
                const char* cFontPath = elemFont->Attribute("path");
                if(cFontPath == NULL) continue;
                Text* fon = new Text(cFontPath);   //Load font
                m_mFonts[sFontName] = fon;          //Stick it into our list
            }
        }
        else if(sName == "group")
        {

        }
        else if(sName == "toggleitem")
        {
            const char* cToggleName = elem->Attribute("name");
            if(cToggleName == NULL) continue;
            HUDToggle* tog = new HUDToggle(cToggleName);
            const char* cToggleImgOn = elem->Attribute("img_on");
            if(cToggleImgOn != NULL && cToggleImgOn[0] != '\0')
                tog->setEnabledImage(m_mImages[cToggleImgOn]);
            const char* cToggleImgOff = elem->Attribute("img_off");
            if(cToggleImgOff != NULL && cToggleImgOff[0] != '\0')
                tog->setDisabledImage(m_mImages[cToggleImgOff]);
            const char* cPosi = elem->Attribute("pos");
            if(cPosi != NULL)
            {
                Point ptPos = pointFromString(cPosi);
                tog->setPos(ptPos);
            }
            const char* cSig = elem->Attribute("signal");
            if(cSig != NULL)
                tog->setSignal(cSig);
            int32_t iKey = 0;
            elem->QueryIntAttribute("key", &iKey);
            tog->setKey(iKey);
            bool bEnabled = false;
            elem->QueryBoolAttribute("default", &bEnabled);
            tog->setEnabled(bEnabled);
            addChild(tog);
        }
        else if(sName == "bgimage")
        {
            const char* cHudImageName = elem->Attribute("name");
            if(cHudImageName == NULL) continue;
            const char* cImg = elem->Attribute("img");
            if(cImg == NULL) continue;
            //Create HUDImage
            HUDImage* hImg = new HUDImage(cHudImageName);
            hImg->setImage(m_mImages[cImg]);
            const char* cPos = elem->Attribute("pos");
            if(cPos != NULL)
            {
                Point ptPos = pointFromString(cPos);
                hImg->setPos(ptPos);
            }
            addChild(hImg); //Add this as a child of our HUD
        }
        else if(sName == "textbox")
        {
            const char* cTextName = elem->Attribute("name");
            if(cTextName == NULL) continue;
            const char* cTextFont = elem->Attribute("font");
            if(cTextFont == NULL) continue;
            HUDTextbox* tb = new HUDTextbox(cTextName);
            tb->setFont(m_mFonts[cTextFont]);
            const char* cDefaultText = elem->Attribute("default");
            if(cDefaultText != NULL)
                tb->setText(cDefaultText);
            const char* cPos = elem->Attribute("pos");
            if(cPos != NULL)
            {
                Point ptTextPos = pointFromString(cPos);
                tb->setPos(ptTextPos);
            }
            const char* cAlign = elem->Attribute("align");
            if(cAlign != NULL)
            {
                //Set font alignment
                string sAlign(cAlign);
                uint8_t iFontAlign = 0;
                if(sAlign.find("right") != sAlign.npos)
                    iFontAlign |= ALIGN_RIGHT;
                if(sAlign.find("left") != sAlign.npos)
                    iFontAlign |= ALIGN_LEFT;
                if(sAlign.find("center") != sAlign.npos)
                    iFontAlign |= ALIGN_CENTER;
                if(sAlign.find("top") != sAlign.npos)
                    iFontAlign |= ALIGN_TOP;
                if(sAlign.find("middle") != sAlign.npos)
                    iFontAlign |= ALIGN_MIDDLE;
                if(sAlign.find("bottom") != sAlign.npos)
                    iFontAlign |= ALIGN_BOTTOM;
                tb->setAlign(iFontAlign);
            }
            const char* cFill = elem->Attribute("fill");
            if(cFill != NULL)
            {
                //Get color to fill in background of textbox
                Rect rcCol = rectFromString(cFill); //Yes, I'm using a rect to get a color
                uint8_t r = rcCol.left;
                uint8_t g = rcCol.top;
                uint8_t b = rcCol.right;
                uint8_t a = rcCol.bottom;
                tb->setFill(r,g,b,a);
            }

            addChild(tb);
        }
        else
            errlog << "Unknown HUD item \"" << sName << "\". Ignoring..." << endl;
    }
}

























