#ifndef _3D_OBJ_H
#define _3D_OBJ_H

#include "Image.h"
#include "globaldefs.h"

class Vertex
{
public:
    float32 x, y, z;
};

class UV
{
public:
    float32 u, v;
};

struct Face
{
    uint32_t v1, v2, v3;
    uint32_t uv1, uv2, uv3;
    uint32_t norm1, norm2, norm3;
};

class Object3D
{
protected:
    GLuint m_obj;   //The object in 3D memory
    GLuint m_tex;   //The texture that'll be drawn to the object
    string m_sObjFilename;
    string m_sTexFilename;

public:
    Vertex pos;
    Vertex rot;
    Vertex scale;
    float32 angle;  //The angle we'll be rotating
    
    void _reload();  //Reload memory associated with this object

    Object3D(string sOBJFile, string sImgFile);
    Object3D();
    ~Object3D();

    void fromOBJFile(string sFilename);
    void setTexture(string sFilename);

    void render();


};

//Object3D reloading handler functions
void reload3DObjects();
void _add3DObjReload(Object3D* obj);
void _remove3DObjReload(Object3D* obj);










#endif
