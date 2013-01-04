#include "Cursor.h"

Cursor::Cursor(string sFilename)
{
    m_img = new Image(sFilename);
    //m_ptHotSpot.SetZero();
    m_iCursorType = CURSOR_NORMAL;
    m_ir = 255;
    m_ig = 0;
    m_ib = 0;
    m_ia = 255;
    m_fScale = 1.0;
    m_fAngle = 0.0;
    m_fCurTime = 0.0;
}

Cursor::~Cursor()
{
    delete m_img;
}

void Cursor::draw(float32 x, float32 y)
{
    //switch(m_iCursorType)
    //{
    float fScale = 1.0;
    float fAngle = 0.0;
    uint8_t r,g,b,a;
    r = g = b = a = 255;
        //if(m_iCursorType == CURSOR_NORMAL)
        //{
        //    m_img->draw(x, y);
        //}
        if(m_iCursorType & CURSOR_COLOR)
        {
            //m_img->setColor(m_ir,m_ig,m_ib,255);
            //m_img->draw(x, y);
            r = m_ir;
            g = m_ig;
            b = m_ib;
        }
        if(m_iCursorType &  CURSOR_SPIN)
        {
            fAngle += m_fAngle;
            //m_img->drawCentered(x, y, m_fAngle);
        }
        if(m_iCursorType &  CURSOR_FADE)
        {
            a = m_ia;
            //m_img->setColor(255,255,255,m_ia);
            //m_img->draw(x, y);
        }
        if(m_iCursorType &  CURSOR_BREATHE)
        {
            fScale += m_fScale - 1.0;
            //m_img->drawCentered(x, y, 0.0, m_fScale);
        }
        if(m_iCursorType &  CURSOR_SWAY)
        {
            fAngle += m_fAngle;
            //m_img->drawCentered(x, y, m_fAngle);
        }
        if(m_iCursorType &  CURSOR_TRACK)
        {
            if(m_objTrack != NULL)
            {
                b2Vec2 cur;
                cur.Set(x,y);
                b2Vec2 vAngle = m_objTrack->getCenter();
                vAngle -= cur;
                float fAngle2;
                if(vAngle.x == 0)
                {
                    fAngle2 = pi/2.0;
                    if(vAngle.y > 0)
                        fAngle2 = -fAngle2;
                }
                else if(vAngle.y == 0)
                {
                    fAngle2 = 0.0;
                    if(vAngle.x > 0)
                        fAngle2 = pi;
                }
                else
                {
                    fAngle2 = atan(vAngle.y/vAngle.x);
                    if(vAngle.x > 0)
                        fAngle2 += pi;
                }
                fAngle += fAngle2;
            }
            //m_img->drawCentered(x,y,fAngle);
        }
        m_img->setColor(r,g,b,a);
        m_img->drawCentered(x,y,fAngle,fScale);
    //}
}

void Cursor::update(float32 fTimestep)
{
    //switch(m_iCursorType)
    //{
        //if(m_iCursorType & CURSOR_NORMAL: //Do nothing
        //    break;

        if(m_iCursorType & CURSOR_COLOR)
        {
            int16_t r,g,b;
            r = m_ir;
            g = m_ig;
            b = m_ib;
            uint16_t iNum = 60.0 * fTimestep * 5;
            if(r >= 255)
            {
                if(b > 0)
                    b -= iNum;
                else if(g < 255)
                    g += iNum;
            }
            if(g >= 255)
            {
                if(r > 0)
                    r -= iNum;
                else if(b < 255)
                    b += iNum;
            }
            if(b >= 255)
            {
                if(g > 0)
                    g -= iNum;
                else if(r < 255)
                    r += iNum;
            }
            r = min(r,(int16_t)255);
            g = min(g,(int16_t)255);
            b = min(b,(int16_t)255);
            r = max(r,(int16_t)0);
            g = max(g,(int16_t)0);
            b = max(b,(int16_t)0);
            m_ir = r;
            m_ib = b;
            m_ig = g;
        }

        if(m_iCursorType & CURSOR_SPIN)
            m_fAngle += fTimestep * 5.0;

        if(m_iCursorType & CURSOR_FADE)
        {
            m_fCurTime += fTimestep * 6.0;
            m_ia = sin(m_fCurTime) * 64.0 + 128.0 + 64.0;
        }

        if(m_iCursorType & CURSOR_BREATHE)
        {
            m_fCurTime += fTimestep * 6.0;
            m_fScale = sin(m_fCurTime) * 0.15 + 1.0;
        }

        if(m_iCursorType & CURSOR_SWAY)
        {
            m_fCurTime += fTimestep * 4.0;
            m_fAngle = sin(m_fCurTime) * pi/4.0;
        }
    //}
}
