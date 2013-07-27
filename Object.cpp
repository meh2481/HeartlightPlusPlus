/*
    Heartlight++ source - Object.cpp
    Class for objects within the game
    Copyright (c) 2012 Mark Hutcheson
*/

#include "Object.h"

Object::Object(Image* img)
{
    if(img == NULL)
    {
        errlog << "Error: NULL image encountered in Object::Object()" << endl;
        exit(1);
    }
    layer = new parallaxLayer(img);
    m_ptPos.SetZero();
    m_ptVel.SetZero();
    //m_Img = img;
    m_iNumFrames = 1;
    m_iCurFrame = 0;
    m_iWidth = img->getWidth();
    m_iHeight = img->getHeight();
    m_ptVel.SetZero();
    m_bDying = false;
    m_bAnimateOnce = false;
    m_bAnimate = true;
}

Object::~Object()
{
    delete layer;
}

void Object::updateFrame()
{
    if(!m_bAnimate)
        return;
    m_iCurFrame++;
    if(m_iCurFrame >= m_iNumFrames)
    {
        if(m_bAnimateOnce)  //If we're only supposed to animate once, die
            kill();
        m_iCurFrame = 0;
    }
}

bool Object::update()
{
    return !m_bDying;
}

void Object::draw()
{
    Rect rcImgPos = {0,m_iHeight*m_iCurFrame,m_iWidth,m_iHeight*(m_iCurFrame+1)};
    layer->pos = m_ptPos;//.Set(m_ptPos.x + getWidth()/2.0, m_ptPos.y + getHeight()/2.0);
    layer->draw(rcImgPos);
}

void Object::setNumFrames(uint16_t iNumFrames, bool bAnimateOnce)
{
    m_iNumFrames = iNumFrames;
    m_iHeight = layer->image->getHeight()/m_iNumFrames;
    m_bAnimateOnce = bAnimateOnce;
}

//-----------------------------------------------------------------------------------------------------------------------
retroObject::retroObject(Image* img) : Object(img)
{
    m_sName = "";
    m_iData = 0;
}

//-----------------------------------------------------------------------------------------------------------------------
//Brick::Brick(Image* img) : retroObject(img)
//{
//}

Door::Door(Image* img) : retroObject(img)
{
}

Dwarf::Dwarf(Image* img) : retroObject(img)
{
}

//void Brick::updateFrame()
//{
    //Don't update frame on bricks
//}

void Door::updateFrame()
{
    if(m_iCurFrame > 0)
        m_iCurFrame++;
    if(m_iCurFrame >= m_iNumFrames)
        m_iCurFrame = m_iNumFrames-1;
}

void Dwarf::updateFrame()
{
    //Don't update frame on dwarf
}

//-----------------------------------------------------------------------------------------------------------------------
physicsObject::physicsObject(Image* img) : Object(img)
{
    m_physicsBody = NULL;
}

physicsObject::~physicsObject()
{
}

void physicsObject::draw()
{
    Rect rcImgPos = {0,m_iHeight*m_iCurFrame,m_iWidth,m_iHeight*(m_iCurFrame+1)};
    layer->pos = m_physicsBody->GetPosition();
    layer->pos *= SCALE_UP_FACTOR;
    layer->rot = m_physicsBody->GetAngle();
    layer->draw(rcImgPos);
}

//-----------------------------------------------------------------------------------------------------------------------
// obj class

obj::obj()
{
  pos.x = pos.y = 0.0f;
  rot = 0.0f;
}

obj::~obj()
{
    for(list<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
        delete (*i);
}

void obj::draw()
{
    for(list<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
    {
        //Point ptPos(0,0);
        //float32 rot = 0.0;
        if((*i)->body != NULL)
        {
            pos = (*i)->body->GetPosition();
			pos *= SCALE_UP_FACTOR;
            rot = (*i)->body->GetAngle();
			//if((*i)->layer != NULL && (*i)->layer->image != NULL)
			//	(*i)->layer->image->drawCentered(ptPos.x + (*i)->layer->pos.x*cos(rot) - (*i)->layer->pos.y*sin(rot),
      //                                ptPos.y + (*i)->layer->pos.y*cos(rot) + (*i)->layer->pos.x*sin(rot),
      //                                (*i)->layer->rot + rot, (*i)->layer->scale.x, (*i)->layer->scale.y);
        }
        glPushMatrix();
        glTranslatef(pos.x, pos.y, 0.0f);
        glRotatef(rot*RAD2DEG, 0.0f, 0.0f, 1.0f);
        if((*i)->obj3D != NULL)
        {
          (*i)->obj3D->render();
        }
        if((*i)->layer != NULL)
        {
          (*i)->layer->draw();
        }
        glPopMatrix();
        
    }
}

void obj::addSegment(physSegment* seg)
{
    segments.push_back(seg);
}


//----------------------------------------------------------------------------------------------------
// physSegment class
physSegment::physSegment()
{
    body = NULL;
    layer = NULL;
    obj3D = NULL;
}

physSegment::~physSegment()
{
  if(body != NULL)
    ; //TODO: Free Box2D body
  if(layer != NULL)
    delete layer;
  if(obj3D != NULL)
    delete obj3D;
}


















