/*
    Heartlight++ source - Image.cpp
    Class for drawing images to the screen easily
    Copyright (c) 2012 Mark Hutcheson
*/

#include "Image.h"

Image::Image(string sFilename)
{
//    m_iScaleFac = 1;
  m_ptHotSpot.SetZero();
  m_sFilename = sFilename;
  SDL_Surface *surface;
  int mode;

  surface = IMG_Load(sFilename.c_str());	//Use SDL_image to load various formats

  // could not load filename
  if(!surface)
  {
	std::cout << "No img " << sFilename << std::endl;
	m_hTex = 0;
    return;
  }

  // work out what format to tell glTexImage2D to use...
  if(surface->format->BytesPerPixel == 3) // RGB 24bit
  {
	mode = GL_RGB;
  }
  else if(surface->format->BytesPerPixel == 4)  // RGBA 32bit
  {
	mode = GL_RGBA;
  }
  else //Unsupported format
  {
	SDL_FreeSurface(surface);
	m_hTex = 0;
    return;
  }

  m_iWidth=surface->w;
  m_iHeight=surface->h;
  // create one texture name
  glGenTextures(1, &m_hTex);

  // tell opengl to use the generated texture name
  glBindTexture(GL_TEXTURE_2D, m_hTex);

  // this reads from the sdl surface and puts it into an opengl texture
  glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

  // these affect how this texture is drawn later on...
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  // clean up
  SDL_FreeSurface(surface);

//    HGE* hge = hgeCreate(HGE_VERSION);
//    m_hTex = hge->Texture_Load(sFilename.c_str());
//    m_hscaledTex = 0;
/*
    if(!m_hTex) //Failed to load texture
    {
        errlog << "Error loading image " << sFilename << endl;
        m_hSprite = NULL;
        m_iWidth = m_iHeight = 32;
        //Keep going, it'll just create a white image instead
    }
    else
    {
        m_iWidth = hge->Texture_GetWidth(m_hTex, true);
        m_iHeight = hge->Texture_GetHeight(m_hTex, true);
        errlog << "Loading image \"" << sFilename << "\"" << endl;
    }

    m_hSprite = new hgeSprite(m_hTex, 0, 0, m_iWidth, m_iHeight);

    hge->Release();*/
}

Image::~Image()
{
    //image cleanup
    errlog << "Freeing image \"" << m_sFilename << "\"" << endl;
    glDeleteTextures(1, &m_hTex);
}

void Image::draw(Rect rcScreenPos)
{
    Rect rcImg = {0,0,m_iWidth,m_iHeight};
    draw(rcScreenPos, rcImg);
}

void Image::draw(Rect rcScreenPos, Rect rcImgPos)
{
    if(m_hTex == 0)
		return;

    // tell opengl to use the generated texture
    glBindTexture(GL_TEXTURE_2D, m_hTex);
//    glEnable(GL_TEXTURE_2D);

    // make a rectangle
    glBegin(GL_QUADS);
    glColor4f(m_col.r,m_col.g,m_col.b,m_col.a);	//Colorize according to how we've colorized this image
    // top left
    glTexCoord2f((rcImgPos.left / (float32)m_iWidth), (rcImgPos.top / (float32)m_iHeight));
    glVertex3f((2.0*(float32)SCREEN_WIDTH/(float32)SCREEN_HEIGHT)*((GLfloat)rcScreenPos.left/(GLfloat)SCREEN_WIDTH-0.5), -2.0*(GLfloat)rcScreenPos.top/(GLfloat)SCREEN_HEIGHT + 1.0, 0.0);
    // bottom left
    glTexCoord2f((rcImgPos.left / (float32)m_iWidth), (rcImgPos.bottom / (float32)m_iHeight));
    glVertex3f((2.0*(float32)SCREEN_WIDTH/(float32)SCREEN_HEIGHT)*((GLfloat)rcScreenPos.left/(GLfloat)SCREEN_WIDTH-0.5), -2.0*(GLfloat)(rcScreenPos.bottom)/(GLfloat)SCREEN_HEIGHT+1.0, 0.0);
    // bottom right
    glTexCoord2f((rcImgPos.right / (float32)m_iWidth), (rcImgPos.bottom / (float32)m_iHeight));
    glVertex3f((2.0*(float32)SCREEN_WIDTH/(float32)SCREEN_HEIGHT)*((GLfloat)(rcScreenPos.right)/(GLfloat)SCREEN_WIDTH-0.5), -2.0*(GLfloat)(rcScreenPos.bottom)/(GLfloat)SCREEN_HEIGHT+1.0, 0.0);
    // top right
    glTexCoord2f((rcImgPos.right / (float32)m_iWidth), (rcImgPos.top / (float32)m_iHeight));
    glVertex3f((2.0*(float32)SCREEN_WIDTH/(float32)SCREEN_HEIGHT)*((GLfloat)(rcScreenPos.right)/(GLfloat)SCREEN_WIDTH-0.5), -2.0*(GLfloat)rcScreenPos.top/(GLfloat)SCREEN_HEIGHT+1.0, 0.0);

    glEnd();
//    glDisable(GL_TEXTURE_2D);
}

void Image::draw(float32 x, float32 y)
{
    Rect rcScr = {x, y, m_iWidth+x, m_iHeight+y};
    draw(rcScr);
}

void Image::draw(Point pt)
{
    draw(pt.x, pt.y);
}

void Image::draw(float32 x, float32 y, Rect rcImgPos)
{
    Rect rcScr = {x, y, rcImgPos.width()+x, rcImgPos.height()+y};
    draw(rcScr, rcImgPos);
}

void Image::draw(Point pt, Rect rcImgPos)
{
    draw(pt.x, pt.y, rcImgPos);
}

void Image::drawCentered(float32 x, float32 y, float32 rotation, float32 stretchFactorx, float32 stretchFactory)
{
    Rect rcImg = {0,0,m_iWidth,m_iHeight};
    drawCentered(x, y, rcImg, rotation, stretchFactorx, stretchFactory);
}

void Image::drawCentered(Point pt, float32 rotation, float32 stretchFactorx, float32 stretchFactory)
{
    drawCentered(pt.x, pt.y, rotation, stretchFactorx, stretchFactory);
}

void Image::drawCentered(float32 x, float32 y, Rect rcImgPos, float32 rotation, float32 stretchFactorx, float32 stretchFactory)
{
    Rect rcDrawPos;
    rcDrawPos.set(0, 0, rcImgPos.width(), rcImgPos.height());
    rcDrawPos.scale(stretchFactorx,stretchFactory);
    rcDrawPos.offset(-rcDrawPos.width()/2.0 + (float32)SCREEN_WIDTH/2.0 - m_ptHotSpot.x, -rcDrawPos.height()/2.0 + (float32)SCREEN_HEIGHT/2.0 - m_ptHotSpot.y);
    glLoadIdentity( );
    glTranslatef( (2.0*(float32)SCREEN_WIDTH/(float32)SCREEN_HEIGHT)*((GLfloat)(x)/(GLfloat)SCREEN_WIDTH-0.5), -2.0*(GLfloat)(y)/(GLfloat)SCREEN_HEIGHT + 1.0, MAGIC_ZOOM_NUMBER);
    glRotatef(-rotation*180.0/PI,0.0f,0.0f,1.0f);
    draw(rcDrawPos,rcImgPos);
    //Reset rotation
    glLoadIdentity( );
    glTranslatef( 0.0f, 0.0f, MAGIC_ZOOM_NUMBER);
}

void Image::drawCentered(Point pt, Rect rcImgPos, float32 rotation, float32 stretchFactorx, float32 stretchFactory)
{
    drawCentered(pt.x, pt.y, rcImgPos, rotation, stretchFactorx, stretchFactory);
}

//Set the color of this image
void Image::setColor(DWORD dwCol)
{
    m_col.from256((dwCol & 0xFF0000) >> 16, (dwCol & 0xFF00) >> 8, dwCol & 0xFF, (dwCol & 0xFF000000) >> 24);
}

//Stretch this image onto a larger one, without interpolation
/*void Image::scale(uint16_t iScaleFac)
{
    if(iScaleFac < 1 || iScaleFac == m_iScaleFac)   //Don't scale up any if 0
        return;

    m_iScaleFac = iScaleFac;
    HGE* hge = hgeCreate(HGE_VERSION);
    if(m_hscaledTex)
    {
        hge->Texture_Free(m_hscaledTex);
        m_hscaledTex = 0;
        m_iWidth = hge->Texture_GetWidth(m_hTex, true);
        m_iHeight = hge->Texture_GetHeight(m_hTex, true);
        if(iScaleFac == 1)  //Rescale down to the original image
        {
            m_hSprite->SetTexture(m_hTex);
            m_hSprite->SetTextureRect(0,0,m_iWidth,m_iHeight);
            hge->Release();
            return; //Done
        }
    }
    else if(iScaleFac == 1)
    {
        //Nothing to do if there's no previous texture and we're just reverting
        hge->Release();
        return;
    }
    uint32_t m_iTexWidth = hge->Texture_GetWidth(m_hTex, false);    //Since the original texture width may not be a power of two, hang
                                                                    // on to this.

    m_hscaledTex = hge->Texture_Create(m_iWidth * iScaleFac, m_iHeight * iScaleFac);  //Create a new texture of the right size
    if(!m_hscaledTex)
    {
        errlog << "could not create scaled texture in Image::scale" << endl;
        hge->Release();
        return;
    }
    uint32_t m_iScaledWidth = hge->Texture_GetWidth(m_hscaledTex, false);   //Also hang onto scaled texture width
    DWORD* src = hge->Texture_Lock(m_hTex); //Grab our original texture data
    DWORD* dest = hge->Texture_Lock(m_hscaledTex, false);   //And our new data
    //Now loop through and copy over, scaling up
    for(uint32_t x = 0; x < m_iWidth; x++)
    {
        for(uint32_t y = 0; y < m_iHeight; y++)
        {
            DWORD srcPixel = src[y*m_iTexWidth+x]; //Grab this pixel
            //loop and make all the neighboring pixels this color too (Yeah, I don't understand this code either)
            for(uint16_t i = 0; i < iScaleFac; i++)
            {
                for(uint16_t j = 0; j < iScaleFac; j++)
                    dest[(iScaleFac*y+j)*m_iScaledWidth+(x*iScaleFac)+i] = srcPixel;
            }
        }
    }
    hge->Texture_Unlock(m_hTex);    //Unlock both textures
    hge->Texture_Unlock(m_hscaledTex);
    m_hSprite->SetTexture(m_hscaledTex);   //Set to the new texture
    m_iWidth *= iScaleFac;              //Set to new dimensions
    m_iHeight *= iScaleFac;
    m_hSprite->SetTextureRect(0,0,m_iWidth,m_iHeight);  //Set the sprite to this new texture rectangle
    hge->Release();
}

*/

Color::Color()
{
    r = g = b = a = 1.0;
}

void Color::from256(int ir, int ig, int ib, int ia)
{
	r = (float32)ir/255.0;
	g = (float32)ig/255.0;
	b = (float32)ib/255.0;
	a = (float32)ia/255.0;
}















