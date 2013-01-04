#include "myEngine.h"

void myEngine::loadLevel_new()
{
    //Clear image cache
    clearImages();
    if(m_iCurrentLevel >= m_vLevels.size()) //At the end, when we shouldn't be
    {
        errlog << "No levels loaded! Abort. " << endl;
        exit(1);
    }
    clearObjects(); //If there's any memory hanging around with objects, clear it out
    m_iHeartsTotal = 0; //No hearts in this level yet
    m_iCollectedHearts = 0;     //And we've collected none
    m_iDyingCount = m_iWinningCount = 0;    //Reset dying and winning counts
    //And clear out our map
    for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
    {
        for(uint16_t col = 0; col < LEVEL_WIDTH; col++)
            m_levelGrid[col][row] = NULL;
    }

    string s = m_vLevels[m_iCurrentLevel];
    string::iterator it = s.begin();

    //Loop through, creating all objects
    for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
    {
        for(uint16_t col = 0; col < LEVEL_WIDTH+2; col++)   //+2 here for newlines. Rgh newlines
        {
            char cObj = *it++;
            bool bSkip = false;

            //Create object
            switch(cObj)
            {
                case ' ':
                    break;  //Ignore spaces

                case '$':   //Heart
                {
                    physicsObject* obj = new physicsObject(getImage("o_heart"));
                    obj->setNumFrames(6);
                    b2BodyDef def;
                    def.type = b2_dynamicBody;
                    def.position.Set((GRID_WIDTH / 2.0 + col * GRID_WIDTH) * SCALE_DOWN_FACTOR * SCALE_FAC,
                                     (GRID_HEIGHT / 2.0 + row * GRID_HEIGHT) * SCALE_DOWN_FACTOR * SCALE_FAC);
                    obj->addBody(createBody(&def));
                    b2FixtureDef fixtureDef;
                    b2PolygonShape poly;
                    b2Vec2 vertices[6];
                    vertices[5].Set(0, 6*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[4].Set(7 *SCALE_FAC*SCALE_DOWN_FACTOR, -1*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[3].Set(7 *SCALE_FAC*SCALE_DOWN_FACTOR, -5*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[2].Set(4 *SCALE_FAC*SCALE_DOWN_FACTOR, -7*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[1].Set(3 *SCALE_FAC*SCALE_DOWN_FACTOR, -7*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[0].Set(0, -5*SCALE_FAC*SCALE_DOWN_FACTOR);
                    poly.Set(vertices,6);
                    fixtureDef.shape = &poly;
                    fixtureDef.density = 0.5f;
                    fixtureDef.friction = 0.3f;
                    obj->addFixture(&fixtureDef);
                    vertices[5].Set(0, 6*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[4].Set(0, -5*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[3].Set(-3 *SCALE_FAC*SCALE_DOWN_FACTOR, -7*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[2].Set(-5 *SCALE_FAC*SCALE_DOWN_FACTOR, -7*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[1].Set(-7 *SCALE_FAC*SCALE_DOWN_FACTOR, -5*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[0].Set(-7 *SCALE_FAC*SCALE_DOWN_FACTOR, -1*SCALE_FAC*SCALE_DOWN_FACTOR);
                    poly.Set(vertices,6);
                    fixtureDef.shape = &poly;
                    fixtureDef.density = 0.5f;
                    fixtureDef.friction = 0.3f;
                    obj->addFixture(&fixtureDef);
                    addObject(obj);
                    break;
                }
                case '@':   //rock
                {
                    physicsObject* obj = new physicsObject(getImage("o_rock"));
                    b2BodyDef def;
                    def.type = b2_dynamicBody;
                    def.position.Set((GRID_WIDTH / 2.0 + col * GRID_WIDTH) * SCALE_DOWN_FACTOR * SCALE_FAC,
                                     (GRID_HEIGHT / 2.0 + row * GRID_HEIGHT) * SCALE_DOWN_FACTOR * SCALE_FAC);
                    obj->addBody(createBody(&def));
                    b2CircleShape circ;
                    circ.m_radius = (GRID_WIDTH - 0.5*SCALE_FAC) / 2.0 * SCALE_FAC * SCALE_DOWN_FACTOR;
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &circ;
                    fixtureDef.density = 1.0f;
                    fixtureDef.friction = 0.3f;
                    obj->addFixture(&fixtureDef);
                    addObject(obj);
                    break;
                }
                case '.':   //grass
                {
                    physicsObject* obj = new physicsObject(getImage("o_grass"));
                    b2BodyDef def;
                    def.type = b2_staticBody;
                    def.position.Set((GRID_WIDTH / 2.0 + col * GRID_WIDTH) * SCALE_DOWN_FACTOR * SCALE_FAC,
                                     (GRID_HEIGHT / 2.0 + row * GRID_HEIGHT) * SCALE_DOWN_FACTOR * SCALE_FAC);
                    obj->addBody(createBody(&def));
                    b2PolygonShape dynamicBox;
                    dynamicBox.SetAsBox(obj->getWidth() * SCALE_DOWN_FACTOR/2.0, obj->getHeight() * SCALE_DOWN_FACTOR/2.0);
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &dynamicBox;
                    //fixtureDef.density = 1.0f;
                    fixtureDef.friction = 0.3f;
                    obj->addFixture(&fixtureDef);
                    addObject(obj);
                    break;
                }
                case '*':   //the dwarf
                {
                    m_objTest = new physicsObject(getImage("o_dwarf"));
                    m_objTest->setNumFrames(8);
                    m_objTest->setAnimate(false);
                    b2BodyDef def;
                    def.type = b2_dynamicBody;
                    def.position.Set((GRID_WIDTH / 2.0 + col * GRID_WIDTH) * SCALE_DOWN_FACTOR * SCALE_FAC,
                                     (GRID_HEIGHT / 2.0 + row * GRID_HEIGHT) * SCALE_DOWN_FACTOR * SCALE_FAC);
                    b2Body* bod = createBody(&def);
                    bod->SetFixedRotation(true);
                    m_objTest->addBody(bod);
                    b2PolygonShape dynamicBox;
                    dynamicBox.SetAsBox((m_objTest->getWidth()-1) * SCALE_DOWN_FACTOR/2.0, (m_objTest->getHeight()-1) * SCALE_DOWN_FACTOR/2.0);
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &dynamicBox;
                    fixtureDef.density = 1.0f;
                    fixtureDef.friction = 0.3f;
                    m_objTest->addFixture(&fixtureDef);
                    addObject(m_objTest);
                    m_cur->setTrack(m_objTest);
                    break;
                }

                case '!':   //exit door
                    m_levelGrid[col][row] = new Door(getImage("o_door"));
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '&':   //bomb
                {
                    physicsObject* obj = new physicsObject(getImage("o_bomb"));
                    b2BodyDef def;
                    def.type = b2_dynamicBody;
                    def.position.Set((GRID_WIDTH / 2.0 + col * GRID_WIDTH) * SCALE_DOWN_FACTOR * SCALE_FAC,
                                     (GRID_HEIGHT / 2.0 + row * GRID_HEIGHT) * SCALE_DOWN_FACTOR * SCALE_FAC);
                    obj->addBody(createBody(&def));
                    b2CircleShape circ;
                    circ.m_p.Set(-1*SCALE_FAC*SCALE_DOWN_FACTOR,
                                 0);
                    circ.m_radius = 7 * SCALE_FAC * SCALE_DOWN_FACTOR;
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &circ;
                    fixtureDef.density = 1.0f;
                    fixtureDef.friction = 0.3f;
                    obj->addFixture(&fixtureDef);
                    b2PolygonShape poly;
                    b2Vec2 vertices[6];
                    vertices[0].Set(-4 *SCALE_FAC*SCALE_DOWN_FACTOR, -8*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[1].Set(3 *SCALE_FAC*SCALE_DOWN_FACTOR, -8*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[2].Set(5 *SCALE_FAC*SCALE_DOWN_FACTOR, -6*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[3].Set(3 *SCALE_FAC*SCALE_DOWN_FACTOR, -6*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[4].Set(2 *SCALE_FAC*SCALE_DOWN_FACTOR, -7*SCALE_FAC*SCALE_DOWN_FACTOR);
                    vertices[5].Set(-4 *SCALE_FAC*SCALE_DOWN_FACTOR, -7*SCALE_FAC*SCALE_DOWN_FACTOR);
                    poly.Set(vertices,6);
                    fixtureDef.shape = &poly;
                    fixtureDef.density = 0.4f;
                    obj->addFixture(&fixtureDef);
                    addObject(obj);
                    break;
                }
                case '0':   //balloon
                    m_levelGrid[col][row] = new retroObject(getImage("o_balloon"));
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setFrame(randInt(0,3));
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '=':   //plasma
                {
                    physicsObject* obj = new physicsObject(getImage("o_plasma"));
                    obj->setNumFrames(8);
                    b2BodyDef def;
                    def.type = b2_staticBody;
                    def.position.Set((GRID_WIDTH / 2.0 + col * GRID_WIDTH) * SCALE_DOWN_FACTOR * SCALE_FAC,
                                     (GRID_HEIGHT / 2.0 + row * GRID_HEIGHT) * SCALE_DOWN_FACTOR * SCALE_FAC);
                    obj->addBody(createBody(&def));
                    b2PolygonShape dynamicBox;
                    dynamicBox.SetAsBox(obj->getWidth() * SCALE_DOWN_FACTOR/2.0, obj->getHeight() * SCALE_DOWN_FACTOR/2.0);
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &dynamicBox;
                    //fixtureDef.density = 1.0f;
                    fixtureDef.friction = 0.3f;
                    obj->addFixture(&fixtureDef);
                    addObject(obj);
                    break;
                }
                case '#':   //brick wall
                {
                    physicsObject* obj = new physicsObject(getImage("o_brick"));
                    obj->setNumFrames(4);
                    obj->setFrame(m_iCurrentLevel % 4);
                    obj->setAnimate(false);
                    b2BodyDef def;
                    def.type = b2_staticBody;
                    def.position.Set((GRID_WIDTH / 2.0 + col * GRID_WIDTH) * SCALE_DOWN_FACTOR * SCALE_FAC,
                                     (GRID_HEIGHT / 2.0 + row * GRID_HEIGHT) * SCALE_DOWN_FACTOR * SCALE_FAC);
                    obj->addBody(createBody(&def));
                    b2PolygonShape dynamicBox;
                    dynamicBox.SetAsBox(obj->getWidth() * SCALE_DOWN_FACTOR/2.0, obj->getHeight() * SCALE_DOWN_FACTOR/2.0);
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &dynamicBox;
                    //fixtureDef.density = 1.0f;
                    fixtureDef.friction = 0.3f;
                    obj->addFixture(&fixtureDef);
                    addObject(obj);
                    break;
                }
                case '%':   //metal wall
                {
                    physicsObject* obj = new physicsObject(getImage("o_metalwall"));
                    b2BodyDef def;
                    def.type = b2_staticBody;
                    def.position.Set((GRID_WIDTH / 2.0 + col * GRID_WIDTH) * SCALE_DOWN_FACTOR * SCALE_FAC,
                                     (GRID_HEIGHT / 2.0 + row * GRID_HEIGHT) * SCALE_DOWN_FACTOR * SCALE_FAC);
                    obj->addBody(createBody(&def));
                    b2PolygonShape dynamicBox;
                    dynamicBox.SetAsBox(obj->getWidth() * SCALE_DOWN_FACTOR/2.0, obj->getHeight() * SCALE_DOWN_FACTOR/2.0);
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &dynamicBox;
                    //fixtureDef.density = 1.0f;
                    fixtureDef.friction = 0.3f;
                    obj->addFixture(&fixtureDef);
                    addObject(obj);
                    break;
                }
                case '<':   //left tunnel
                    m_levelGrid[col][row] = new retroObject(getImage("o_tunnell"));
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '>':   //right tunnel
                    m_levelGrid[col][row] = new retroObject(getImage("o_tunnelr"));
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '\n':
                    bSkip = true;
                    break;

                case '\r':
                    break;

                default:
                    errlog << "Warning: Invalid char \'" << cObj << "\'. Ignoring..." << endl;
                    break;  //Ignore
            }
            if(bSkip)
            {
                bSkip = false;
                break;  //Skip to next line
            }
        }
    }

    //Check and see if there are no hearts in this level
    if(m_iCollectedHearts == m_iHeartsTotal)
    {
        //Open all doors
        for(uint16_t row = 0; row < LEVEL_HEIGHT; row++)
        {
            for(uint16_t col = 0; col < LEVEL_WIDTH; col++)
            {
                if(m_levelGrid[col][row] == NULL)
                    continue;
                if(m_levelGrid[col][row]->getNameChar() == '!')
                {
                    m_levelGrid[col][row]->setFrame(1);
                    playSound("o_door");
                }
            }
        }
    }
}
