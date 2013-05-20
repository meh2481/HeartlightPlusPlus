#ifndef CURSOR_H
#define CURSOR_H

#include "globaldefs.h"
#include "Image.h"
#include "Object.h"

#define CURSOR_NORMAL   0
#define CURSOR_COLOR    1
#define CURSOR_SPIN     2
#define CURSOR_FADE     4
#define CURSOR_BREATHE  8
#define CURSOR_SWAY     16
#define CURSOR_TRACK    32

typedef struct
{
    float32 fNumPerSec;
} cursorColorValues;

typedef struct
{
    float32 fSpinSpeed;
} cursorSpinValues;

typedef struct
{
    uint8_t iBaseAlpha;
    float32 fFrequency;
    float32 fAmplitude;
} cursorFadeValues;

typedef struct
{
    Point ptBaseScale;  //Points here for separate x and y scaling
    Point ptFrequency;
    Point ptAmplitude;
    Point ptTimeOffset; //Offset for sin curves
} cursorBreatheValues;

typedef struct
{
    float32 fBaseRot;
    float32 fFrequency;
    float32 fAmplitude;
} cursorSwayValues;

class Cursor
{
private:
    Cursor(){}; //Default constructor is uncallable

protected:
    Image*      m_img;
    //Point       m_ptHotSpot;
    uint16_t    m_iCursorType;
    uint8_t     m_ir, m_ig, m_ib, m_ia;
    Point       m_ptScale;
    float32     m_fAngle;
    float32     m_fCurTime;
    Object*     m_objTrack;
    cursorColorValues   m_color;
    cursorSpinValues    m_spin;
    cursorFadeValues    m_fade;
    cursorBreatheValues m_breathe;
    cursorSwayValues    m_sway;

public:
    Cursor(string sXMLFilename);
    ~Cursor();

    //Accessor methods
    void setHotSpot(Point pt)               {;};//m_img->setHotSpot(pt);};
    void setHotSpot(float32 x, float32 y)   {;};//m_img->setHotSpot(x,y);};
    Point getHotSpot()                      {return m_img->getHotSpot();};
    uint32_t getWidth()                     {return m_img->getWidth();};
    uint32_t getHeight()                    {return m_img->getHeight();};
    uint16_t getType()                      {return m_iCursorType;};
    void setType(uint16_t type)             {m_iCursorType = type;};
    void setTrack(Object* obj)              {m_objTrack = obj; m_iCursorType |= CURSOR_TRACK;};

    //General methods
    void draw(float32 x, float32 y);
    void draw(Point pt)                     {draw(pt.x, pt.y);};
    void update(float32 fTimestep);
    bool loadFromXML(string sXMLFilename);

};


















#endif
