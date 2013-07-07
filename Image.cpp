/*
    Heartlight++ source - Image.cpp
    Class for drawing images to the screen easily
    Copyright (c) 2012 Mark Hutcheson
*/

#include "Image.h"

#ifdef __APPLE__
//returns the closest power of two value
int power_of_two(int input)
{
	int value = 1;
	while ( value < input ) {
		value <<= 1;
	}
	return value;
}
#endif

Image::Image(string sFilename)
{
//    m_iScaleFac = 1;
  m_ptHotSpot.SetZero();
  m_sFilename = sFilename;
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
  FreeImage_FlipVertical(dib);
  
  int w = power_of_two(FreeImage_GetWidth(dib));
	int h = power_of_two(FreeImage_GetHeight(dib));
	FIBITMAP *bitmap2 = FreeImage_Allocate(w, h, 32);
	FreeImage_Paste(bitmap2, dib, 0, 0, 255);
  
	bits = FreeImage_GetBits(bitmap2);
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if((bits == 0) || (width == 0) || (height == 0))
	{
    errlog << "Something went terribly horribly wrong with getting image bits; just sit and wait for the singularity" << endl;
    return;
  }
  
  int mode;
  if(FreeImage_GetBPP(dib) == 24) // RGB 24bit
    mode = GL_RGB;
  else if(FreeImage_GetBPP(dib) == 32)  // RGBA 32bit
    mode = GL_RGBA;
  FreeImage_Unload(dib);
  
  //errlog << "Width: " << width << ", Height: " << height << endl;
  //for(int i = 0; i < FreeImage_GetBPP(dib) * width * height / 8; i++)
	//{
  //  errlog << (unsigned int)(bits[i]) << " ";
	//}
  //errlog << endl;
	
	//if this texture ID is in use, unload the current texture
	//if(m_texID.find(texID) != m_texID.end())
	//	glDeleteTextures(1, &(m_texID[texID]));
  
	//generate an OpenGL texture ID for this texture
  m_iWidth = width;
  m_iHeight = height;
	glGenTextures(1, &m_hTex);
  //errlog << "Generated texture: " << m_hTex << endl;
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, m_hTex);
	//store the texture data for OpenGL use
	glTexImage2D(GL_TEXTURE_2D, 0, mode, w, h, 0, mode, GL_UNSIGNED_BYTE, bits);
  
  // these affect how this texture is drawn later on...
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  
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

  // these affect how this texture is drawn later on...
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  // clean up
  SDL_FreeSurface(surface);
#endif //defined __APPLE__
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
    glEnable(GL_TEXTURE_2D);

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















