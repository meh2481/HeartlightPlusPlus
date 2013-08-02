/*
    Heartlight++ source - Image.cpp
    Class for drawing images to the screen easily
    Copyright (c) 2012 Mark Hutcheson
*/

#include "Image.h"
#include <set>

int screenDrawWidth;
int screenDrawHeight;
static bool g_bBlurred = true;

Image::Image(string sFilename)
{
  m_ptHotSpot.SetZero();
  m_sFilename = sFilename;
  _load(sFilename);
  _addImgReload(this);
}

void Image::_load(string sFilename)
{
#ifdef __APPLE__  //For some reason, SDL_Image isn't working for me in PPC Mac. Hermph. Using FreeImage for now instead.
  errlog << "Load " << sFilename << endl;
  //image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);
	
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(sFilename.c_str(), 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(sFilename.c_str());
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
	{
    errlog << "Unknown image type for file " << sFilename << endl;
    return;
  }
  
	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, sFilename.c_str());
	//if the image failed to load, return failure
	if(!dib)
	{
    errlog << "Error loading image " << sFilename << endl;
    return;
  }  
	//retrieve the image data
  
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
  
  int w = power_of_two(width);
	int h = power_of_two(height);
  int mode;
  if(FreeImage_GetBPP(dib) == 24) // RGB 24bit
    mode = GL_RGB;
  else if(FreeImage_GetBPP(dib) == 32)  // RGBA 32bit
    mode = GL_RGBA;	
  FIBITMAP *bitmap2 = FreeImage_Allocate(w, h, FreeImage_GetBPP(dib));
	FreeImage_Paste(bitmap2, dib, 0, 0, 255);
  FreeImage_FlipVertical(bitmap2);  //Apparently, FreeImage handles this strangely. Flipping beforehand doesn't work right.
  FreeImage_Unload(dib);
  
	bits = FreeImage_GetBits(bitmap2);	//if this somehow one of these failed (they shouldn't), return failure
	if((bits == 0) || (width == 0) || (height == 0))
	{
    errlog << "Something went terribly horribly wrong with getting image bits; just sit and wait for the singularity" << endl;
    return;
  }
  
	//generate an OpenGL texture ID for this texture
  m_iWidth = width;
  m_iHeight = height;
  m_iRealWidth = w;
  m_iRealHeight = h;
	glGenTextures(1, &m_hTex);
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	//store the texture data for OpenGL use
	glTexImage2D(GL_TEXTURE_2D, 0, mode, w, h, 0, mode, GL_UNSIGNED_BYTE, bits);
  
	//Free FreeImage's copy of the data
	FreeImage_Unload(bitmap2);
  
#else
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
	
  unsigned char* data = (unsigned char*)surface->pixels;
  
  m_iWidth=surface->w;
  m_iHeight=surface->h;
  // create one texture name
  glGenTextures(1, &m_hTex);
  
  // tell opengl to use the generated texture name
  glBindTexture(GL_TEXTURE_2D, m_hTex);
  
  // this reads from the sdl surface and puts it into an opengl texture
  glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, data);
  
  // clean up
  SDL_FreeSurface(surface);
#endif //defined __APPLE__
}

Image::~Image()
{
    //image cleanup
    errlog << "Freeing image \"" << m_sFilename << "\"" << endl;
    glDeleteTextures(1, &m_hTex);
    _removeImgReload(this);
}

void Image::draw(Rect rcDrawPos)
{
    Rect rcImg = {0,0,m_iWidth,m_iHeight};
    draw(rcDrawPos, rcImg);
}

void Image::draw(Rect rcDrawPos, Rect rcImgPos)
{
    if(m_hTex == 0)
      return;

    // tell opengl to use the generated texture
    glBindTexture(GL_TEXTURE_2D, m_hTex);
  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	if(g_bBlurred)
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  
  
    int32_t w, h;
#ifdef __APPLE__
    w = m_iRealWidth;
    h = m_iRealHeight;
#else
    w = m_iWidth;
    h = m_iHeight;
#endif

    // make a rectangle
    glBegin(GL_QUADS);
    glColor4f(m_col.r,m_col.g,m_col.b,m_col.a);	//Colorize according to how we've colorized this image
    // top left
    glTexCoord2f((rcImgPos.left / (float32)w), (rcImgPos.top / (float32)h));
    glVertex3f((2.0*(float32)screenDrawWidth/(float32)screenDrawHeight)*((GLfloat)rcDrawPos.left/(GLfloat)screenDrawWidth-0.5), -2.0*(GLfloat)rcDrawPos.top/(GLfloat)screenDrawHeight + 1.0, 0.0);
    // bottom left
    glTexCoord2f((rcImgPos.left / (float32)w), (rcImgPos.bottom / (float32)h));
    glVertex3f((2.0*(float32)screenDrawWidth/(float32)screenDrawHeight)*((GLfloat)rcDrawPos.left/(GLfloat)screenDrawWidth-0.5), -2.0*(GLfloat)(rcDrawPos.bottom)/(GLfloat)screenDrawHeight+1.0, 0.0);
    // bottom right
    glTexCoord2f((rcImgPos.right / (float32)w), (rcImgPos.bottom / (float32)h));
    glVertex3f((2.0*(float32)screenDrawWidth/(float32)screenDrawHeight)*((GLfloat)(rcDrawPos.right)/(GLfloat)screenDrawWidth-0.5), -2.0*(GLfloat)(rcDrawPos.bottom)/(GLfloat)screenDrawHeight+1.0, 0.0);
    // top right
    glTexCoord2f((rcImgPos.right / (float32)w), (rcImgPos.top / (float32)h));
    glVertex3f((2.0*(float32)screenDrawWidth/(float32)screenDrawHeight)*((GLfloat)(rcDrawPos.right)/(GLfloat)screenDrawWidth-0.5), -2.0*(GLfloat)rcDrawPos.top/(GLfloat)screenDrawHeight+1.0, 0.0);

    //Reset color
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnd();
}

void Image::draw4V(Point ul, Point ur, Point bl, Point br)
{
  if(m_hTex == 0)
    return;
  
  // tell opengl to use the generated texture
  glBindTexture(GL_TEXTURE_2D, m_hTex);
  
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	if(g_bBlurred)
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  
  
  int32_t w, h;
#ifdef __APPLE__
  w = m_iRealWidth;
  h = m_iRealHeight;
#else
  w = m_iWidth;
  h = m_iHeight;
#endif
  // make a rectangle
  glBegin(GL_QUADS);
  glColor4f(m_col.r,m_col.g,m_col.b,m_col.a);	//Colorize according to how we've colorized this image
  // top left
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f((2.0*(float32)screenDrawWidth/(float32)screenDrawHeight)*((GLfloat)ul.x/(GLfloat)screenDrawWidth-0.5), -2.0*(GLfloat)ul.y/(GLfloat)screenDrawHeight + 1.0, 0.0);
  // bottom left
  glTexCoord2f(0.0f, 1.0f);// (rcImgPos.bottom / (float32)h));
  glVertex3f((2.0*(float32)screenDrawWidth/(float32)screenDrawHeight)*((GLfloat)bl.x/(GLfloat)screenDrawWidth-0.5), -2.0*(GLfloat)(bl.y)/(GLfloat)screenDrawHeight+1.0, 0.0);
  // bottom right
  glTexCoord2f(1.0f, 1.0f);//(rcImgPos.right / (float32)w), (rcImgPos.bottom / (float32)h));
  glVertex3f((2.0*(float32)screenDrawWidth/(float32)screenDrawHeight)*((GLfloat)(br.x)/(GLfloat)screenDrawWidth-0.5), -2.0*(GLfloat)(br.y)/(GLfloat)screenDrawHeight+1.0, 0.0);
  // top right
  glTexCoord2f(1.0f, 0.0f);//(rcImgPos.right / (float32)w), (rcImgPos.top / (float32)h));
  glVertex3f((2.0*(float32)screenDrawWidth/(float32)screenDrawHeight)*((GLfloat)(ur.x)/(GLfloat)screenDrawWidth-0.5), -2.0*(GLfloat)ur.y/(GLfloat)screenDrawHeight+1.0, 0.0);
  
  //Reset color
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glEnd();
  
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
    rcDrawPos.offset(-rcDrawPos.width()/2.0 + (float32)screenDrawWidth/2.0 - m_ptHotSpot.x, -rcDrawPos.height()/2.0 + (float32)screenDrawHeight/2.0 - m_ptHotSpot.y);
    //glLoadIdentity( );
    glPushMatrix();
    glTranslatef( (2.0*(float32)screenDrawWidth/(float32)screenDrawHeight)*((GLfloat)(x)/(GLfloat)screenDrawWidth-0.5), -2.0*(GLfloat)(y)/(GLfloat)screenDrawHeight + 1.0, 0.0);//MAGIC_ZOOM_NUMBER);
    glRotatef(-rotation*180.0/PI,0.0f,0.0f,1.0f);
    //glScalef(stretchFactorx, stretchFactory, 1.0f);
    draw(rcDrawPos, rcImgPos);
    glPopMatrix();  //Reset
    //Reset rotation
    //glLoadIdentity( );
    //glTranslatef( 0.0f, 0.0f, MAGIC_ZOOM_NUMBER);
    //glScalef(1.0/stretchFactorx, 1.0/stretchFactory, 1.0f);
    //glRotatef(rotation*180.0/PI,0.0f,0.0f,1.0f);
    //glTranslatef( -((2.0*(float32)screenDrawWidth/(float32)screenDrawHeight)*((GLfloat)(x)/(GLfloat)screenDrawWidth-0.5)), -(-2.0*(GLfloat)(y)/(GLfloat)screenDrawHeight + 1.0), 0.0);
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

void Image::_reload()
{
  _load(m_sFilename);
}

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

static set<Image*> sg_images;

void reloadImages()
{
  for(set<Image*>::iterator i = sg_images.begin(); i != sg_images.end(); i++)
  {
    (*i)->_reload();
  }
}

void _addImgReload(Image* img)
{
  sg_images.insert(img);
}

void _removeImgReload(Image* img)
{
  sg_images.erase(img);
}

void setImageBlurred()
{
	g_bBlurred = true;
}

void setImagePixellated()
{
	g_bBlurred = false;
}








