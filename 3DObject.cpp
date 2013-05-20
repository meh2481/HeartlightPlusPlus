#include "3DObject.h"
#include <sstream>

Object3D::Object3D(string sOBJFile, string sImgFile)
{
    m_obj = m_tex = 0;
    setTexture(sImgFile);
    fromOBJFile(sOBJFile);
}

Object3D::Object3D()
{
    m_obj = m_tex = 0;
}

Object3D::~Object3D()
{

}

void Object3D::fromOBJFile(string sFilename)
{
    vector<Vertex> vVerts;
    vector<UV> vUVs;
    UV tmp;
    tmp.u = tmp.v = 0.0;
    vUVs.push_back(tmp);    //Push a 0,0 UV coordinate in case there's no UVs in this .obj file
    list<Face> lFaces;

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
        //errlog << c << endl;
        switch (c[0])
        {
            case '#':
            case 's':
                continue;   //Skip over comment lines and "s off" lines
            case 'v': //Vertex
                if(c[1] == 't') //"vt" denotes UV coordinate
                {
                    UV v;
                    if(!(iss >> v.u >> v.v)) continue;
                    //v.u = 1.0 - v.u; //Flip UV coordinates to match up right
                    v.v = 1.0 - v.v;
                    vUVs.push_back(v);
                }
                else    //Normal vertex
                {
                    Vertex v;
                    if(!(iss >> v.x >> v.y >> v.z)) continue; //Skip over malformed lines
                    vVerts.push_back(v);
                }
                break;
            case 'f':
                //Gonna have to pull in faces by hand if UV vertices are specified as well
                Face f;
                char ctmp;
                iss.get(ctmp);
                //errlog << "peek: " << iss.peek() << endl;
                if(iss.eof() || infile.eof() || iss.fail() || infile.fail())
                    break;
                for(int i = 0; i < 3; i++)
                {
                    float32 vertPos = 0.0;
                    float32 uvPos = 0.0;
                    string sCoord;
                    getline(iss, sCoord, ' ');
                    istringstream issCord(sCoord);
                    issCord >> vertPos;
                    //errlog << "vert pos: " <<vertPos << endl;
                    if(issCord.peek() == '/') //Have a UV coordinate here, as well
                    {
                        issCord.ignore();
                        issCord >> uvPos;
                        //errlog << "UV pos: " << uvPos << endl;
                    }

                    switch(i)
                    {
                        case 0:
                            f.v1 = vertPos;
                            f.uv1 = uvPos;
                            break;
                        case 1:
                            f.v2 = vertPos;
                            f.uv2 = uvPos;
                            break;
                        case 2:
                            f.v3 = vertPos;
                            f.uv3 = uvPos;
                            break;

                    }
                }
                lFaces.push_back(f);
                break;
            default:
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
        //errlog << "faces" << endl;
        /*errlog << "Face: " << vVerts[i->v1-1].x << ", " << vVerts[i->v1-1].y << ", " << vVerts[i->v1-1].z << endl
               << vVerts[i->v2-1].x << ", " << vVerts[i->v2-1].y << ", " << vVerts[i->v2-1].z << endl
               << vVerts[i->v3-1].x << ", " << vVerts[i->v3-1].y << ", " << vVerts[i->v3-1].z << endl
               << vUVs[i->uv1-1].u << " " << vUVs[i->uv1-1].v << endl
               << vUVs[i->uv2-1].u << " " << vUVs[i->uv2-1].v << endl
               << vUVs[i->uv3-1].u << " " << vUVs[i->uv3-1].v << endl;*/
        glTexCoord2f(vUVs[i->uv1].u, vUVs[i->uv1].v);
        glVertex3f(vVerts[i->v1-1].x, vVerts[i->v1-1].y, vVerts[i->v1-1].z);
        glTexCoord2f(vUVs[i->uv2].u, vUVs[i->uv2].v);
        glVertex3f(vVerts[i->v2-1].x, vVerts[i->v2-1].y, vVerts[i->v2-1].z);
        glTexCoord2f(vUVs[i->uv3].u, vUVs[i->uv3].v);
        glVertex3f(vVerts[i->v3-1].x, vVerts[i->v3-1].y, vVerts[i->v3-1].z);
    }

    glEnd();
//    glDisable(GL_TEXTURE_2D);

    glEndList();

}

void Object3D::setTexture(string sFilename)
{
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
}

void Object3D::render()
{
//    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glCallList(m_obj);
}
