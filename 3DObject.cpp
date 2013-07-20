#include "3DObject.h"
#include <sstream>
#include <set>
using namespace std;

Object3D::Object3D(string sOBJFile, string sImgFile)
{
    m_obj = m_tex = 0;
    setTexture(sImgFile);
    fromOBJFile(sOBJFile);
    pos.x = pos.y = pos.z = 0.0f;
    rot.x = rot.y = rot.z = 0.0f;
    scale.x = scale.y = scale.z = 1.0f;
    angle = 0.0f;
    m_sObjFilename = sOBJFile;
    m_sTexFilename = sImgFile;
    _add3DObjReload(this);
}

Object3D::Object3D()
{
  m_obj = m_tex = 0;
  _add3DObjReload(this);  
}

Object3D::~Object3D()
{
  _remove3DObjReload(this);
}

void Object3D::fromOBJFile(string sFilename)
{
    m_sObjFilename = sFilename;
    errlog << "Loading 3D object: " << sFilename << endl;
    vector<Vertex> vVerts;
    vector<Vertex> vNormals;
    vector<UV> vUVs;
    UV tmp;
    tmp.u = tmp.v = 0.0;
    vUVs.push_back(tmp);    //Push a 0,0 UV coordinate in case there's no UVs in this .obj file
    list<Face> lFaces;

    bool bUVs = false;
    bool bNorms = false;

    ifstream infile(sFilename.c_str());
    if(infile.fail())
    {
        errlog << "Error: Unable to open wavefront object file " << sFilename << endl;
        return;    //Abort
    }
    //Loop until we hit eof or fail
    while(!infile.eof() && !infile.fail())
    {
        string s;
        getline(infile, s);
        if(infile.eof() || infile.fail())
            break;
        istringstream iss(s);
        string c;
        if(!(iss >> c)) break;
        switch (c[0])
        {
            case 'v': //Vertex
                if(c[1] == 't') //"vt" denotes UV coordinate
                {
                    bUVs = true;
                    UV vt;
                    if(!(iss >> vt.u >> vt.v)) continue;
                    //Flip UV coordinates to match up right
                    vt.v = 1.0 - vt.v;
                    vUVs.push_back(vt);
                }
                else if(c[1] == 'n')    //"vn" denotes face normal
                {
                    bNorms = true;
                    Vertex vn;
                    if(!(iss >> vn.x >> vn.y >> vn.z)) continue; //Skip over malformed lines
                    vNormals.push_back(vn);
                }
                else    //"v" denotes vertex
                {
                    Vertex v;
                    if(!(iss >> v.x >> v.y >> v.z)) continue; //Skip over malformed lines
                    vVerts.push_back(v);
                }
                break;
            case 'f':
                Face f;
                char ctmp;
                iss.get(ctmp);
                if(iss.eof() || infile.eof() || iss.fail() || infile.fail())
                    break;
                for(int i = 0; i < 3; i++)
                {
                    uint32_t vertPos = 0;
                    uint32_t uvPos = 0;
                    uint32_t normPos = 0;
                    string sCoord;
                    getline(iss, sCoord, ' ');
                    istringstream issCord(sCoord);
                    issCord >> vertPos;
                    if(bNorms)
                    {
                        issCord.ignore();   //Ignore the '/' character
                        if(bUVs)
                            issCord >> uvPos;
                        issCord.ignore();
                        issCord >> normPos;
                    }
                    else if(bUVs)
                    {
                        issCord.ignore();
                        issCord >> uvPos;
                    }

                    switch(i)
                    {
                        case 0:
                            f.v1 = vertPos;
                            f.uv1 = uvPos;
                            f.norm1 = normPos;
                            break;
                        case 1:
                            f.v2 = vertPos;
                            f.uv2 = uvPos;
                            f.norm2 = normPos;
                            break;
                        case 2:
                            f.v3 = vertPos;
                            f.uv3 = uvPos;
                            f.norm3 = normPos;
                            break;

                    }
                }
                lFaces.push_back(f);
                break;
            default:    //Skip anything else we don't care about (Comment lines; mtl definitions, etc)
                continue;
        }
    }
    infile.close();

    //Done with file; create object
    m_obj = glGenLists(1);
    glNewList(m_obj,GL_COMPILE);
	
    //Loop through and add faces
    glBegin(GL_TRIANGLES);
    for(list<Face>::iterator i = lFaces.begin(); i != lFaces.end(); i++)
    {
        if(bNorms)
            glNormal3f(vNormals[i->norm1-1].x, vNormals[i->norm1-1].y, vNormals[i->norm1-1].z);  //TODO
        if(bUVs)
            glTexCoord2f(vUVs[i->uv1].u, vUVs[i->uv1].v);
        glVertex3f(vVerts[i->v1-1].x, vVerts[i->v1-1].y, vVerts[i->v1-1].z);
        if(bNorms)
            glNormal3f(vNormals[i->norm2-1].x, vNormals[i->norm2-1].y, vNormals[i->norm2-1].z);
        if(bUVs)
            glTexCoord2f(vUVs[i->uv2].u, vUVs[i->uv2].v);
        glVertex3f(vVerts[i->v2-1].x, vVerts[i->v2-1].y, vVerts[i->v2-1].z);
        if(bNorms)
            glNormal3f(vNormals[i->norm3-1].x, vNormals[i->norm3-1].y, vNormals[i->norm3-1].z);
        if(bUVs)
            glTexCoord2f(vUVs[i->uv3].u, vUVs[i->uv3].v);
        glVertex3f(vVerts[i->v3-1].x, vVerts[i->v3-1].y, vVerts[i->v3-1].z);
    }

    glEnd();

    glEndList();

}

void Object3D::setTexture(string sFilename)
{
    m_sTexFilename = sFilename;
    errlog << "Creating 3D object texture: " << sFilename << endl;
#ifndef __APPLE__
    SDL_Surface *surface;
    int mode;

    surface = IMG_Load(sFilename.c_str());	//Use SDL_image to load various formats

    // could not load filename
    if(!surface)
    {
        errlog << "No img " << sFilename << std::endl;
        m_tex = 0;
        return;
    }

    // work out what format to tell glTexImage2D to use...
    if(surface->format->BytesPerPixel == 3) // RGB 24bit
        mode = GL_RGB;
    else if(surface->format->BytesPerPixel == 4)  // RGBA 32bit
        mode = GL_RGBA;
    else //Unsupported format
    {
        SDL_FreeSurface(surface);
        m_tex = 0;
        return;
    }

    // create one texture name
    glGenTextures(1, &m_tex);

    // tell opengl to use the generated texture name
    glBindTexture(GL_TEXTURE_2D, m_tex);

    // this reads from the sdl surface and puts it into an opengl texture
    glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

    // these affect how this texture is drawn later on...
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);


    // clean up
    SDL_FreeSurface(surface);
#else
  //errlog << "Load " << sFilename << endl;
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
	glGenTextures(1, &m_tex);
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, m_tex);
	//store the texture data for OpenGL use
	glTexImage2D(GL_TEXTURE_2D, 0, mode, w, h, 0, mode, GL_UNSIGNED_BYTE, bits);
  
  // these affect how this texture is drawn later on...
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  
	//Free FreeImage's copy of the data
	FreeImage_Unload(bitmap2);
#endif
}

void Object3D::render()
{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //TODO For Wireframe Games logo. Woot woot!
	glEnable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(angle, rot.x, rot.y, rot.z);
    glScalef(scale.x, scale.y, scale.z);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glCallList(m_obj);
    glPopMatrix();
	glDisable(GL_LIGHTING);	//TODO: Enable/disable someplace else that makes more sense
}

void Object3D::_reload()
{
  setTexture(m_sTexFilename);
  fromOBJFile(m_sObjFilename);
}

static set<Object3D*> sg_objs;

void reload3DObjects()
{
  for(set<Object3D*>::iterator i = sg_objs.begin(); i != sg_objs.end(); i++)
  {
    (*i)->_reload();
  }
}

void _add3DObjReload(Object3D* obj)
{
  sg_objs.insert(obj);
}

void _remove3DObjReload(Object3D* obj)
{
  sg_objs.erase(obj);
}

