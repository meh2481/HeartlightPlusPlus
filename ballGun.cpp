#include "ballGun.h"

ballGun::ballGun()
{
    //icon = gun = bullet = NULL;
    obj = NULL;
    num = MAX_CREATED_SPHERES;
    m_ptCursorPos.SetZero();
    delay = 0.0;
    m_bAutomatic = false;
    m_fLastFired = m_fCurTime = 0.0f;
    m_bMouseDown = false;
}

ballGun::~ballGun()
{

}

void ballGun::update(float32 fCurTime)
{
    m_fCurTime = fCurTime;
    if(m_bAutomatic && m_bMouseDown)
        fire(); //BLAMBLAMBLAMBLAMBLAM
}

void ballGun::mouseDown(float32 x, float32 y)
{
    m_ptCursorPos.Set(x,y);
    m_bMouseDown = true;
    fire();
}

void ballGun::mouseUp(float32 x, float32 y)
{
    m_bMouseDown = false;
}

void ballGun::fire()//mouseDown(float32 x, float32 y)
{
    if(m_fCurTime < delay + m_fLastFired)
        return;
    m_fLastFired = m_fCurTime;
    pew();
    float32 x = m_ptCursorPos.x;
    float32 y = m_ptCursorPos.y;
    if(obj == NULL || bullet == "") return;
    Point pos = obj->getCenter();
    pos.x = x - pos.x;
    pos.y = y - pos.y;
    pos.Normalize();
    Point vel = pos;
    vel *= SHOOT_VEL;
    if(gun == "")
        pos *= DEFAULT_GUN_LENGTH;
    else
        pos *= getImage(gun)->getWidth();
    pos += obj->getCenter();
    physicsObject* newObj = new physicsObject(getImage(bullet));
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x*SCALE_DOWN_FACTOR,pos.y*SCALE_DOWN_FACTOR);
    def.linearVelocity.Set(vel.x*SCALE_DOWN_FACTOR, vel.y*SCALE_DOWN_FACTOR);
    b2Body* sphere = createBody(&def);
    newObj->addBody(sphere);
    b2CircleShape circ;
    float fRad = randFloat(0.5f,0.7f);
    circ.m_radius = (16*fRad) / 2.0 * SCALE_DOWN_FACTOR;
    newObj->layer->scale.Set(fRad,fRad);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circ;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    newObj->addFixture(&fixtureDef);
    addObject(newObj);
    if(m_lFired.size() >= num)
    {
        physicsObject* old = m_lFired.front();
        old->kill();
        m_lFired.pop_front();
    }
    m_lFired.push_back(newObj);
}

void ballGun::draw(Rect rcScreen)
{
    if(icon != "")
        getImage(icon)->draw(rcScreen.left + GUN_ICON_DRAW_OFFSET, rcScreen.top + GUN_ICON_DRAW_OFFSET);
    if(gun != "" && obj != NULL)
    {
        Image* gunImg = getImage(gun);
        Point pos = obj->getCenter();
        pos.x -= rcScreen.left;
        pos.y -= rcScreen.top;
        Point dist = m_ptCursorPos;
        dist -= pos;
        float32 angle = 0.0;
        if(dist.x != 0.0 || dist.y != 0.0)
            angle = atan2(dist.y, dist.x);
        float32 fScale = 1.0;
        if(angle > pi/2.0 || angle < -pi/2.0)
            fScale = -1.0;
        gunImg->setHotSpot(0,gunImg->getHeight()/2.0);
        gunImg->drawCentered(pos, angle, 1.0, fScale);  //Flip on Y axis if we're pointing behind us
    }
}

void ballGun::clear()
{
    for(list<physicsObject*>::iterator i = m_lFired.begin(); i != m_lFired.end(); i++)
        (*i)->kill();
    m_lFired.clear();
}

//----------------------------------------------------------------------------------

placeGun::placeGun() : ballGun()
{
    dist = FLT_MAX;
    num = 1;
    m_bAutomatic = false;
}

placeGun::~placeGun()
{

}

void placeGun::fire()//mouseDown(float32 x, float32 y)
{
    if(m_fCurTime < delay + m_fLastFired)
        return;
    m_fLastFired = m_fCurTime;
    pew();
    float32 x = m_ptCursorPos.x;
    float32 y = m_ptCursorPos.y;
    if(obj == NULL || bullet == "") return;
    Point pos(x,y);
    Point myPos = obj->getCenter();
    myPos -= pos;
    if(myPos.LengthSquared() > dist*dist) return;  //Make sure not too far away
    physicsObject* newObj = new physicsObject(getImage(bullet));
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x*SCALE_DOWN_FACTOR,pos.y*SCALE_DOWN_FACTOR);
    newObj->addBody(createBody(&def));
    b2CircleShape circ;
    circ.m_radius = (16 - 0.5) / 2.0 * SCALE_DOWN_FACTOR;
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circ;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    newObj->addFixture(&fixtureDef);
    if(m_lFired.size() >= num)
    {
        physicsObject* old = m_lFired.front();
        old->kill();
        m_lFired.pop_front();
    }
    m_lFired.push_back(newObj);
    addObject(newObj);
}

//----------------------------------------------------------------------------------

blastGun::blastGun() : ballGun()
{
    num = 5;
    delay = 0.25;
    m_bAutomatic = true;
}

blastGun::~blastGun()
{

}

void blastGun::fire()//mouseDown(float32 x, float32 y)
{
    if(m_fCurTime < delay + m_fLastFired)
        return;
    m_fLastFired = m_fCurTime;
    pew();
    float32 x = m_ptCursorPos.x;
    float32 y = m_ptCursorPos.y;
    if(obj == NULL || bullet == "") return;
    Point pos = obj->getCenter();
    pos.x = x - pos.x;
    pos.y = y - pos.y;
    pos.Normalize();
    Point vel = pos;
    vel *= SHOOT_VEL;
    if(gun == "")
        pos *= DEFAULT_GUN_LENGTH;
    else
        pos *= getImage(gun)->getWidth();
    pos += obj->getCenter();
    physicsObject* newObj = new physicsObject(getImage(bullet));
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x*SCALE_DOWN_FACTOR,pos.y*SCALE_DOWN_FACTOR);
    def.linearVelocity.Set(vel.x*SCALE_DOWN_FACTOR, vel.y*SCALE_DOWN_FACTOR);
    b2Body* sphere = createBody(&def);
    newObj->addBody(sphere);
    b2CircleShape circ;
    float fRad = randFloat(0.9f,1.1f);
    circ.m_radius = (16*fRad) / 2.0 * SCALE_DOWN_FACTOR;
    newObj->layer->scale.Set(fRad,fRad);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circ;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    newObj->addFixture(&fixtureDef);
    addObject(newObj);
    if(m_lFired.size() >= num)
    {
        physicsObject* old = m_lFired.front();
        old->kill();
        m_lFired.pop_front();
    }
    m_lFired.push_back(newObj);

    //Make player be pushed back by firing the object
    b2Body* bod = obj->getBody();
    vel.Normalize();
    vel = -vel;
    vel *= sphere->GetMass() * MASS_FAC;    //Knockback galore
    bod->ApplyForceToCenter(vel);
}

//----------------------------------------------------------------------------------

shotgun::shotgun() : ballGun()
{
    num = 100;
    delay = 0.5;
    m_bAutomatic = true;
}

shotgun::~shotgun()
{

}

void shotgun::fire()//mouseDown(float32 x, float32 y)
{
    if(m_fCurTime < delay + m_fLastFired)
        return;
    m_fLastFired = m_fCurTime;
    pew();
    float32 x = m_ptCursorPos.x;
    float32 y = m_ptCursorPos.y;
    if(obj == NULL || bullet == "") return;
    for(int i = 0; i < 10; i++)  //Fire a spread of bullets
    {
        Point pos = obj->getCenter();
        pos.x = x - pos.x;
        pos.y = y - pos.y;
        pos.Normalize();
        Point vel = pos;
        vel *= SHOOT_VEL;
        if(gun == "")
            pos *= DEFAULT_GUN_LENGTH;
        else
            pos *= getImage(gun)->getWidth();
        pos += obj->getCenter();
        physicsObject* newObj = new physicsObject(getImage(bullet));
        b2BodyDef def;
        def.type = b2_dynamicBody;
        def.position.Set(pos.x*SCALE_DOWN_FACTOR,pos.y*SCALE_DOWN_FACTOR);
        def.linearVelocity.Set(vel.x*SCALE_DOWN_FACTOR+randFloat(-5,5), vel.y*SCALE_DOWN_FACTOR+randFloat(-5,5));
        b2Body* sphere = createBody(&def);
        newObj->addBody(sphere);
        b2CircleShape circ;
        float fRad = randFloat(0.15f,0.25f);
        circ.m_radius = (16*fRad) / 2.0 * SCALE_DOWN_FACTOR;
        newObj->layer->scale.Set(fRad,fRad);
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circ;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;
        newObj->addFixture(&fixtureDef);
        addObject(newObj);
        if(m_lFired.size() >= num)
        {
            physicsObject* old = m_lFired.front();
            old->kill();
            m_lFired.pop_front();
        }
        m_lFired.push_back(newObj);

        //Make player be pushed back by firing the object
        b2Body* bod = obj->getBody();
        vel.Normalize();
        vel = -vel;
        vel *= sphere->GetMass() * MASS_FAC;    //Knockback galore
        bod->ApplyForceToCenter(vel);
    }
}

//----------------------------------------------------------------------------------

machineGun::machineGun() : ballGun()
{
    num = 50;
    delay = 0.05;
    m_bAutomatic = true;
}

machineGun::~machineGun()
{

}

void machineGun::fire()//mouseDown(float32 x, float32 y)
{
    if(m_fCurTime < delay + m_fLastFired)
        return;
    m_fLastFired = m_fCurTime;
    pew();
    float32 x = m_ptCursorPos.x;
    float32 y = m_ptCursorPos.y;
    if(obj == NULL || bullet == "") return;
    Point pos = obj->getCenter();
    pos.x = x - pos.x;
    pos.y = y - pos.y;
    pos.Normalize();
    Point vel = pos;
    vel *= SHOOT_VEL;
    if(gun == "")
        pos *= DEFAULT_GUN_LENGTH;
    else
        pos *= getImage(gun)->getWidth();
    pos += obj->getCenter();
    physicsObject* newObj = new physicsObject(getImage(bullet));
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x*SCALE_DOWN_FACTOR,pos.y*SCALE_DOWN_FACTOR);
    def.linearVelocity.Set(vel.x*SCALE_DOWN_FACTOR+randFloat(-5,5), vel.y*SCALE_DOWN_FACTOR+randFloat(-5,5));
    b2Body* sphere = createBody(&def);
    newObj->addBody(sphere);
    b2CircleShape circ;
    float fRad = randFloat(0.15f,0.25f);
    circ.m_radius = (16*fRad) / 2.0 * SCALE_DOWN_FACTOR;
    newObj->layer->scale.Set(fRad,fRad);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circ;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    newObj->addFixture(&fixtureDef);
    addObject(newObj);
    if(m_lFired.size() >= num)
    {
        physicsObject* old = m_lFired.front();
        old->kill();
        m_lFired.pop_front();
    }
    m_lFired.push_back(newObj);

    //Make player be pushed back by firing the object
    b2Body* bod = obj->getBody();
    vel.Normalize();
    vel = -vel;
    vel *= sphere->GetMass() * MASS_FAC;// * 3.0;    //Knockback galore
    bod->ApplyForceToCenter(vel);
}

//----------------------------------------------------------------------------------

teleGun::teleGun() : ballGun()
{
    m_bAutomatic = false;
}

teleGun::~teleGun()
{

}

void teleGun::mouseDown(float32 x, float32 y)
{

}

void teleGun::mouseUp(float32 x, float32 y)
{

}

void teleGun::mouseMove(float32 x, float32 y)
{
    ballGun::mouseMove(x,y);
}

void teleGun::fire()
{

}

//----------------------------------------------------------------------------------

superGun::superGun() : ballGun()
{
    num = 400;
    delay = 0.05;
    m_bAutomatic = true;
}

superGun::~superGun()
{

}

void superGun::fire()//mouseDown(float32 x, float32 y)
{
    if(m_fCurTime < delay + m_fLastFired)
        return;
    m_fLastFired = m_fCurTime;
    pew();
    float32 x = m_ptCursorPos.x;
    float32 y = m_ptCursorPos.y;
    if(obj == NULL || bullet == "") return;
    for(int i = 0; i < 10; i++)  //Fire a spread of bullets
    {
        Point pos = obj->getCenter();
        pos.x = x - pos.x;
        pos.y = y - pos.y;
        pos.Normalize();
        Point vel = pos;
        vel *= SHOOT_VEL;
        if(gun == "")
            pos *= DEFAULT_GUN_LENGTH;
        else
            pos *= getImage(gun)->getWidth();
        pos += obj->getCenter();
        physicsObject* newObj = new physicsObject(getImage(bullet));
        b2BodyDef def;
        def.type = b2_dynamicBody;
        def.position.Set(pos.x*SCALE_DOWN_FACTOR,pos.y*SCALE_DOWN_FACTOR);
        def.linearVelocity.Set(vel.x*SCALE_DOWN_FACTOR+randFloat(-5,5), vel.y*SCALE_DOWN_FACTOR+randFloat(-5,5));
        b2Body* sphere = createBody(&def);
        newObj->addBody(sphere);
        b2CircleShape circ;
        float fRad = randFloat(0.15f,0.25f);
        circ.m_radius = (16*fRad) / 2.0 * SCALE_DOWN_FACTOR;
        newObj->layer->scale.Set(fRad,fRad);
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circ;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;
        newObj->addFixture(&fixtureDef);
        addObject(newObj);
        if(m_lFired.size() >= num)
        {
            physicsObject* old = m_lFired.front();
            old->kill();
            m_lFired.pop_front();
        }
        m_lFired.push_back(newObj);

        //Make player be pushed back by firing the object
        b2Body* bod = obj->getBody();
        vel.Normalize();
        vel = -vel;
        vel *= sphere->GetMass() * MASS_FAC;    //Knockback galore
        bod->ApplyForceToCenter(vel);
    }
}

void pew()
{
    switch(randInt(0,3))
    {
        case 0:
            playSound("n_pew", 50, 0, randFloat(0.5,2.0));
            break;
        case 1:
            playSound("n_pewpew", 50, 0, randFloat(0.5,2.0));
            break;
        case 2:
            playSound("n_pewpewpew", 50, 0, randFloat(0.5,2.0));
            break;
        case 3:
            playSound("n_pewpewpewpew", 50, 0, randFloat(0.5,2.0));
            break;
    }
}


















