#include "Cursor.h"

Cursor::Cursor(string sFilename)
{
    m_img = NULL;
    loadFromXML(sFilename);
}

Cursor::~Cursor()
{
    delete m_img;
}

void Cursor::draw(float32 x, float32 y)
{
    Point ptScale(1.0,1.0);
    float32 fAngle = 0.0;
    uint8_t r,g,b,a;
    r = g = b = a = 255;

    if(m_iCursorType & CURSOR_COLOR)
    {
        r = m_ir;
        g = m_ig;
        b = m_ib;
    }

    if(m_iCursorType &  CURSOR_SPIN)
        fAngle += m_fAngle;

    if(m_iCursorType &  CURSOR_FADE)
        a = m_ia;

    if(m_iCursorType &  CURSOR_BREATHE)
    {
        ptScale.x += m_ptScale.x - 1.0;
        ptScale.y += m_ptScale.y - 1.0;
    }

    if(m_iCursorType &  CURSOR_SWAY)
        fAngle += m_fAngle;

    if(m_iCursorType & CURSOR_TRACK)
    {
        if(m_objTrack != NULL)
        {
            b2Vec2 cur;
            cur.Set(x,y);
            b2Vec2 vAngle = m_objTrack->getCenter();
            vAngle -= cur;
            float32 fAngle2;
            if(vAngle.x == 0)
            {
                fAngle2 = PI/2.0;
                if(vAngle.y > 0)
                    fAngle2 = -fAngle2;
            }
            else if(vAngle.y == 0)
            {
                fAngle2 = 0.0;
                if(vAngle.x > 0)
                    fAngle2 = PI;
            }
            else
            {
                fAngle2 = atan(vAngle.y/vAngle.x);
                if(vAngle.x > 0)
                    fAngle2 += PI;
            }
            fAngle += fAngle2;
        }
    }
    m_img->setColor(r,g,b,a);
    m_img->drawCentered(x,y,fAngle,ptScale.x,ptScale.y);

}

void Cursor::update(float32 fTimestep)
{
    m_fCurTime += fTimestep;
    if(m_iCursorType & CURSOR_COLOR)
    {
        uint8_t r,g,b;
        r = m_ir;
        g = m_ig;
        b = m_ib;
        uint16_t iNum = fTimestep * m_color.fNumPerSec;
		//Run through hue values as per HSV color scheme. In the RGB color space, this looks like the following:
		//1. R starts at max
		//R ==========
		//G
		//B
		
		//2. Increase G until max
		//R ==========
		//G >>>>>>>>>>
		//B
		
		//3. Decrease R until 0
		//R <<<<<<<<<<
		//G ==========
		//B
		
		//4. Increase B until max
		//R
		//G ==========
		//B >>>>>>>>>>
		
		//5. Decrease G until 0
		//R
		//G <<<<<<<<<<
		//B ==========
		
		//6. Increase R until max
		//R >>>>>>>>>>
		//G
		//B ==========
		
		//7. Decrease B until 0
		//R ==========
		//G 
		//B <<<<<<<<<<
		
		//8. Loop back to beginning
		
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
        r = std::min(r,(uint8_t)255);
        g = std::min(g,(uint8_t)255);
        b = std::min(b,(uint8_t)255);
        r = std::max(r,(uint8_t)0);
        g = std::max(g,(uint8_t)0);
        b = std::max(b,(uint8_t)0);
        m_ir = r;
        m_ib = b;
        m_ig = g;
    }

    if(m_iCursorType & CURSOR_FADE)
    {
        int iAlpha = sin(m_fCurTime * m_fade.fFrequency) * m_fade.fAmplitude + m_fade.iBaseAlpha;
        iAlpha = std::max(iAlpha, 0);
        iAlpha = std::min(iAlpha, 255);  //Stay in range
        m_ia = iAlpha;
    }

    if(m_iCursorType & CURSOR_BREATHE)
    {
        m_ptScale.x = sin(m_fCurTime * m_breathe.ptFrequency.x + m_breathe.ptTimeOffset.x) * m_breathe.ptAmplitude.x + m_breathe.ptBaseScale.x;
        m_ptScale.y = sin(m_fCurTime * m_breathe.ptFrequency.y + m_breathe.ptTimeOffset.y) * m_breathe.ptAmplitude.y + m_breathe.ptBaseScale.y;
    }

    if(m_iCursorType & CURSOR_SPIN && !(m_iCursorType & CURSOR_SWAY))
        m_fAngle = m_fCurTime * m_spin.fSpinSpeed;

    if(m_iCursorType & CURSOR_SWAY)
        m_fAngle = sin(m_fCurTime * m_sway.fFrequency) * m_sway.fAmplitude + m_sway.fBaseRot;

    if((m_iCursorType & CURSOR_SPIN) && (m_iCursorType & CURSOR_SWAY))
        m_fAngle += m_fCurTime * m_spin.fSpinSpeed;

}

bool Cursor::loadFromXML(string sXMLFilename)
{
    if(m_img != NULL)
        delete m_img;
    m_iCursorType = CURSOR_NORMAL;
    m_ir = 255;
    m_ig = 0;
    m_ib = 0;
    m_ia = 255;
    m_ptScale.Set(1.0, 1.0);
    m_fAngle = 0.0;
    m_fCurTime = 0.0;

    //Default values for cursor alterer thingies
    m_color.fNumPerSec = 1530.0;   //Cycle through once per second
    m_spin.fSpinSpeed = 1.0;
    m_fade.fAmplitude = 128.0;
    m_fade.fFrequency = 1.0;
    m_fade.iBaseAlpha = 128;
    m_breathe.ptAmplitude.Set(0.5,0.5);
    m_breathe.ptBaseScale.Set(1.0,1.0);
    m_breathe.ptFrequency.Set(1.0,1.0);
    m_breathe.ptTimeOffset.Set(0.0,0.0);
    m_sway.fAmplitude = PI/4.0;
    m_sway.fBaseRot = 0.0;
    m_sway.fFrequency = 1.0;

    XMLDocument* doc = new XMLDocument();
    doc->LoadFile(sXMLFilename.c_str());

    XMLElement* root = doc->RootElement();
    if(root == NULL) return false;
    const char* cImageFilename = root->Attribute("filename");
    if(cImageFilename == NULL) return false;
    m_img = new Image(cImageFilename);
    for(XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) //Load all elements
    {
        const char* cName = elem->Name();
        if(cName == NULL) return false;
        string s = cName;
        if(s == "color")
        {
            m_iCursorType |= CURSOR_COLOR;
            float32 fVariance = 0.0;
            elem->QueryFloatAttribute("colorpersec", &m_color.fNumPerSec);
            elem->QueryFloatAttribute("colorvariance", &fVariance);
            m_color.fNumPerSec += m_color.fNumPerSec * randFloat(-fVariance, fVariance);
        }
        else if(s == "spin")
        {
            m_iCursorType |= CURSOR_SPIN;
            float32 fVariance = 0.0;
            elem->QueryFloatAttribute("speed", &m_spin.fSpinSpeed);
            elem->QueryFloatAttribute("speedvariance", &fVariance);
            m_spin.fSpinSpeed += m_spin.fSpinSpeed * randFloat(-fVariance, fVariance);
        }
        else if(s == "fade")
        {
            m_iCursorType |= CURSOR_FADE;
            //Query alpha value
            float32 fVariance = 0.0;
            int iBaseAlpha = m_fade.iBaseAlpha;
            elem->QueryIntAttribute("basealpha", &iBaseAlpha);
            m_fade.iBaseAlpha = iBaseAlpha;
            elem->QueryFloatAttribute("alphavariance", &fVariance);
            m_fade.iBaseAlpha += m_fade.iBaseAlpha * randFloat(-fVariance, fVariance);
            //Query frequency of fading animation
            fVariance = 0.0;
            elem->QueryFloatAttribute("frequency", &m_fade.fFrequency);
            elem->QueryFloatAttribute("frequencyvariance", &fVariance);
            m_fade.fFrequency += m_fade.fFrequency * randFloat(-fVariance, fVariance);
            //Query amplitude of alpha fading
            fVariance = 0.0;
            elem->QueryFloatAttribute("amplitude", &m_fade.fAmplitude);
            elem->QueryFloatAttribute("amplitudevariance", &fVariance);
            m_fade.fAmplitude += m_fade.fAmplitude * randFloat(-fVariance, fVariance);
        }
        else if(s == "scale")
        {
            m_iCursorType |= CURSOR_BREATHE;
            //Base scale value
            float32 fVariance = 0.0;
            elem->QueryFloatAttribute("basescalex", &m_breathe.ptBaseScale.x);
            elem->QueryFloatAttribute("basescalexvariance", &fVariance);
            m_breathe.ptBaseScale.x += m_breathe.ptBaseScale.x * randFloat(-fVariance, fVariance);
            fVariance = 0.0;
            elem->QueryFloatAttribute("basescaley", &m_breathe.ptBaseScale.y);
            elem->QueryFloatAttribute("basescaleyvariance", &fVariance);
            m_breathe.ptBaseScale.y += m_breathe.ptBaseScale.y * randFloat(-fVariance, fVariance);
            //Frequency
            fVariance = FLT_MIN;
            elem->QueryFloatAttribute("frequencyx", &m_breathe.ptFrequency.x);
            elem->QueryFloatAttribute("frequencyy", &m_breathe.ptFrequency.y);
            elem->QueryFloatAttribute("frequencyvariance", &fVariance); //Just "frequencyvariance" means both x and y have same freq variance
            if(fVariance == FLT_MIN)
            {
                fVariance = 0.0;
                elem->QueryFloatAttribute("frequencyxvariance", &fVariance);
                m_breathe.ptFrequency.x += m_breathe.ptFrequency.x * randFloat(-fVariance, fVariance);
                fVariance = 0.0;
                elem->QueryFloatAttribute("frequencyyvariance", &fVariance);
                m_breathe.ptFrequency.y += m_breathe.ptFrequency.y * randFloat(-fVariance, fVariance);
            }
            else
            {
                float32 fRand = randFloat(-fVariance, fVariance);
                m_breathe.ptFrequency.x += m_breathe.ptFrequency.x * fRand;
                m_breathe.ptFrequency.y += m_breathe.ptFrequency.y * fRand;
            }
            //Amplitude
            fVariance = 0.0;
            elem->QueryFloatAttribute("amplitudex", &m_breathe.ptAmplitude.x);
            elem->QueryFloatAttribute("amplitudexvariance", &fVariance);
            m_breathe.ptAmplitude.x += m_breathe.ptAmplitude.x * randFloat(-fVariance, fVariance);
            fVariance = 0.0;
            elem->QueryFloatAttribute("amplitudey", &m_breathe.ptAmplitude.y);
            elem->QueryFloatAttribute("amplitudeyvariance", &fVariance);
            m_breathe.ptAmplitude.y += m_breathe.ptAmplitude.y * randFloat(-fVariance, fVariance);
            //Time offset
            elem->QueryFloatAttribute("timeoffsetx", &m_breathe.ptTimeOffset.x);
            elem->QueryFloatAttribute("timeoffsety", &m_breathe.ptTimeOffset.y);
        }
        else if(s == "sway")
        {
            m_iCursorType |= CURSOR_SWAY;
            //Base rotation
            float32 fVariance = 0.0;
            elem->QueryFloatAttribute("baserotation", &m_sway.fBaseRot);
            elem->QueryFloatAttribute("baserotationvariance", &fVariance);
            m_sway.fBaseRot += m_sway.fBaseRot * randFloat(-fVariance, fVariance);
            //Frequency
            fVariance = 0.0;
            elem->QueryFloatAttribute("frequency", &m_sway.fFrequency);
            elem->QueryFloatAttribute("frequencyvariance", &fVariance);
            m_sway.fFrequency += m_sway.fFrequency * randFloat(-fVariance, fVariance);
            //Amplitude
            fVariance = 0.0;
            elem->QueryFloatAttribute("amplitude", &m_sway.fAmplitude);
            elem->QueryFloatAttribute("amplitudevariance", &fVariance);
            m_sway.fAmplitude += m_sway.fAmplitude * randFloat(-fVariance, fVariance);
        }
        else continue;
    }
    delete doc;


    return true;
}








