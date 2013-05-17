/*
    Heartlight++ source - myEngine_retro.cpp
    Copyright (c) 2012 Mark Hutcheson
*/

#include "myEngine.h"

bool myEngine::_moveToGridSquare_retro(int row, int col)
{
    if(row < 0 ||
       col < 0 ||
       row >= LEVEL_HEIGHT ||
       col >= LEVEL_WIDTH)
        return false; //We're at the edge of the map -- ignore
    retroObject* obj = m_levelGrid[col][row];
    if(obj == NULL || keyDown(HGEK_SPACE))
        return false; //Nothing to do here
    switch(obj->getNameChar())
    {
        case '$':   //Heart
            m_iCollectedHearts++;
            if(m_iCollectedHearts == m_iHeartsTotal)    //Collected all hearts; open the doors
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
            else    //Otherwise, play getting heart noise
                playSound("o_heart");
            m_levelGrid[col][row]->kill();
            m_levelGrid[col][row] = NULL;
            m_oldGrid[col][row] = NULL;
            break;

        case '.':   //Grass- just destroy
            m_levelGrid[col][row]->kill();
            m_levelGrid[col][row] = NULL;
            m_oldGrid[col][row] = NULL;
            break;

        case '!':
            if(m_levelGrid[col][row]->getFrame() == 3)  //If open
            {
                m_levelGrid[col][row]->kill();
                m_levelGrid[col][row] = NULL;
                m_oldGrid[col][row] = NULL;
                m_iWinningCount = WIN_COUNT+2;  //+2 here because we'll miss a couple frames
                playSound("o_applause");
            }
            break;

        //Tunnels
        case '>':   //Tunnel to the right
        {
            //See if tunnel is obstructed
            bool bObstructed = true;
            for(uint16_t coltest = col+1; coltest < LEVEL_WIDTH; coltest++)
            {
                if(coltest == LEVEL_WIDTH)
                    break;
                retroObject* tunnelTest = m_oldGrid[coltest][row];
                if(tunnelTest == NULL || tunnelTest->getNameChar() == '.')
                {
                    bObstructed = false;    //We can go this way if it's just grass or empty space
                    break;
                }
                else if(tunnelTest->getNameChar() != '>')
                    break;  //Obstructed
            }
            if(bObstructed)
                break;  //We can't go through this tunnel (also will prevent you from going the wrong way through tunnels)

            //Enter the tunnel
            obj->setImage(getImage("o_tunnelrup"));
            obj->setName(">*"); //Player is in here
            m_bTunnelMoved = true;  //Entered this tunnel
            playSound("o_subway");

            //Destroy the current dwarf object
            for(uint32_t i = 0; i < LEVEL_HEIGHT; i++)
            {
                for(uint32_t j = 0; j < LEVEL_WIDTH; j++)
                {
                    if(m_levelGrid[j][i] != NULL && m_levelGrid[j][i]->getNameChar() == '*')
                    {
                        m_levelGrid[j][i]->kill();
                        m_levelGrid[j][i] = NULL;
                        m_oldGrid[j][i] = NULL;
                    }
                }
            }
            return true;
        }

        case '<':   //Tunnel to the left
        {
            //See if tunnel is obstructed
            bool bObstructed = true;
            for(uint16_t coltest = col-1; coltest >= 0; coltest--)
            {
                retroObject* tunnelTest = m_oldGrid[coltest][row];
                if(tunnelTest == NULL || tunnelTest->getNameChar() == '.')
                {
                    bObstructed = false;    //We can go this way if it's just grass or empty space
                    break;
                }
                else if(tunnelTest->getNameChar() != '<')
                    break;  //Obstructed
            }
            if(bObstructed)
                break;  //We can't go through this tunnel (also will prevent you from going the wrong way through tunnels)

            //Enter the tunnel
            obj->setImage(getImage("o_tunnellup"));
            obj->setName("<*"); //Player is in here
            playSound("o_subway");

            //Destroy the current dwarf object
            for(uint32_t i = 0; i < LEVEL_HEIGHT; i++)
            {
                for(uint32_t j = 0; j < LEVEL_WIDTH; j++)
                {
                    if(m_levelGrid[j][i] != NULL && m_levelGrid[j][i]->getNameChar() == '*')
                    {
                        m_levelGrid[j][i]->kill();
                        m_levelGrid[j][i] = NULL;
                        m_oldGrid[j][i] = NULL;
                    }
                }
            }

            return true;
        }

        case '@':
        case '&':   //Push bombs or rocks
        case '0':   //or balloons
            if(col > 0 &&
               m_levelGrid[col-1][row] != NULL &&
               m_levelGrid[col-1][row]->getNameChar() == '*' &&
               m_levelGrid[col-1][row]->getFrame() < 2 &&
               keyDown(HGEK_RIGHT) && !keyDown(HGEK_SPACE))    //Pushing to the right
            {
                if(col < LEVEL_WIDTH-1 && m_levelGrid[col+1][row] == NULL)  //There's an empty space for us to push it
                {
                    //Push it over
                    m_levelGrid[col+1][row] = m_levelGrid[col][row];
                    m_levelGrid[col+1][row]->offset(GRID_WIDTH*SCALE_FAC,0);
                    m_levelGrid[col][row] = NULL;
                    m_oldGrid[col][row] = NULL;
                }
            }
            else if(col < LEVEL_WIDTH-1 &&
                    m_levelGrid[col+1][row] != NULL &&
                    m_levelGrid[col+1][row]->getNameChar() == '*' &&
                    m_levelGrid[col+1][row]->getFrame() < 2 &&
                    keyDown(HGEK_LEFT) && !keyDown(HGEK_SPACE))    //Pushing to the left
            {
                if(col > 0 && m_levelGrid[col-1][row] == NULL)
                {
                    m_levelGrid[col-1][row] = m_levelGrid[col][row];
                    m_levelGrid[col-1][row]->offset(-GRID_WIDTH*SCALE_FAC,0);
                    m_levelGrid[col][row] = NULL;
                    m_oldGrid[col][row] = NULL;
                }
            }
            break;

    }
    return false;
}

void myEngine::loadLevel_retro()
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
                    m_iHeartsTotal++;   //Increment the total number of hearts in this level
                    m_levelGrid[col][row] = new retroObject(getImage("o_heart"));
                    m_levelGrid[col][row]->setNumFrames(6);
                    m_levelGrid[col][row]->setFrame(randInt(0,5));    //randomize the frame it starts at
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '@':   //rock
                    m_levelGrid[col][row] = new retroObject(getImage("o_rock"));
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '.':   //grass
                    m_levelGrid[col][row] = new retroObject(getImage("o_grass"));
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '*':   //the dwarf
                    m_levelGrid[col][row] = new Dwarf(getImage("o_dwarf"));
                    m_levelGrid[col][row]->setNumFrames(8);
                    //Set to facing left if on right side of the screen
                    if(col >= LEVEL_WIDTH/2)
                        m_levelGrid[col][row]->setFrame(1);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    //m_cur->setTrack(m_levelGrid[col][row]);
                    break;

                case '!':   //exit door
                    m_levelGrid[col][row] = new Door(getImage("o_door"));
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '&':   //bomb
                    m_levelGrid[col][row] = new retroObject(getImage("o_bomb"));
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '0':   //balloon
                    m_levelGrid[col][row] = new retroObject(getImage("o_balloon"));
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setFrame(randInt(0,3));
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '=':   //plasma
                    m_levelGrid[col][row] = new retroObject(getImage("o_plasma"));
                    m_levelGrid[col][row]->setNumFrames(8);
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '#':   //brick wall
                    m_levelGrid[col][row] = new retroObject(getImage("o_brick"));
                    m_levelGrid[col][row]->setAnimate(false);   //Don't animate
                    m_levelGrid[col][row]->setNumFrames(4);
                    m_levelGrid[col][row]->setFrame(m_iCurrentLevel % 4);  //Color depends on level number, like original game
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;

                case '%':   //metal wall
                    m_levelGrid[col][row] = new retroObject(getImage("o_metalwall"));
                    m_levelGrid[col][row]->setPos(col * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                    m_levelGrid[col][row]->setName(cObj);
                    addObject(m_levelGrid[col][row]);
                    break;
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

void myEngine::updateGrid_retro() //Workhorse for updating the objects in the game
{
    m_bTunnelMoved = false;
    //Copy new grid back to old
    for(int row = 0; row < LEVEL_HEIGHT; row++)
    {
        for(int col = 0; col < LEVEL_WIDTH; col++)
            m_oldGrid[col][row] = m_levelGrid[col][row];
    }

    //Loop through and update
    for(int row = 0; row < LEVEL_HEIGHT; row++)
    {
        for(int col = 0; col < LEVEL_WIDTH; col++)
        {
            retroObject* obj = m_oldGrid[col][row];
            if(obj == NULL)
                continue;   //ignore empty spaces

            string s = obj->getName();
            switch(obj->getNameChar())
            {
                case 'x':   //exploding bomb
                    obj->kill();
                    m_levelGrid[col][row] = new retroObject(getImage("o_explode"));
                    m_oldGrid[col][row] = m_levelGrid[col][row];
                    m_levelGrid[col][row]->setName('X');
                    m_levelGrid[col][row]->setNumFrames(7);
                    m_levelGrid[col][row]->setFrame(1); //Skip first frame
                    m_levelGrid[col][row]->setPos(col*GRID_WIDTH*SCALE_FAC, row*GRID_HEIGHT*SCALE_FAC);
                    addObject(m_levelGrid[col][row]);
                    explode_retro(row+1, col, true);
                    explode_retro(row, col+1, true);
                    explode_retro(row-1, col);
                    explode_retro(row, col-1);
                case 'X':   //explosion
                    if(obj->getFrame() == 6)
                    {
                        obj->kill();
                        m_oldGrid[col][row] = NULL;
                        m_levelGrid[col][row] = NULL;
                    }
                    break;
                case '$':   //heart
                case '@':   //rock
                case '&':   //bomb
                    if(obj->getData() == BOMB_EXPLODEDELAY1)
                    {
                        //Explode
                        obj->setImage(getImage("o_bombexplode"));
                        obj->setName('x');
                        playSound("o_explode");
                        break;
                    }
                    else if(obj->getData() == BOMB_EXPLODEDELAY2)
                    {
                        obj->setData(BOMB_EXPLODEDELAY1);
                        break;
                    }

                    if((row == LEVEL_HEIGHT-1 || m_oldGrid[col][row+1] != NULL) && obj->isData(FALLING_1|FALLING_2))   //Hitting something
                    {
                        if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1]->getNameChar() == '*') //Hit player
                        {
                            //kill player
                            if(m_levelGrid[col][row+1]->getFrame() < 4) //But only if not dying or winning
                            {
                                m_levelGrid[col][row+1]->kill();
                                m_levelGrid[col][row+1] = new retroObject(getImage("o_explode"));
                                m_oldGrid[col][row+1] = m_levelGrid[col][row+1];
                                m_levelGrid[col][row+1]->setName('X');
                                m_levelGrid[col][row+1]->setNumFrames(7);
                                m_levelGrid[col][row+1]->setPos(col*GRID_WIDTH*SCALE_FAC, (row+1)*GRID_HEIGHT*SCALE_FAC);
                                addObject(m_levelGrid[col][row+1]);
                                playSound("o_explode");
                                if(!m_iDyingCount)
                                    m_iDyingCount = DIE_COUNT/2;    //Start death countdown timer
                            }
                            obj->removeData(FALLING_1);
                            if(obj->isData(FALLING_2))
                            {
                                obj->removeData(FALLING_2);
                                obj->addData(FALLING_1);
                            }


                        }
                        else  //Hit ground
                        {
                            if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1]->getNameChar() == '.' && obj->isData(FALLING_2)) //Hit grass
                            {
                                if(*(s.begin()) == '$') //Heart hit
                                    playSound("o_heartgrass");
                                else
                                    playSound("o_grass");
                            }
                            else if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1]->getNameChar() == '0')    //Hit a balloon
                            {
                                //do nothing for now
                            }
                            else if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1]->getNameChar() == '&' && obj->isData(FALLING_2))    //Hit a bomb
                            {
                                //Explode
                                m_oldGrid[col][row+1]->setData(BOMB_EXPLODEDELAY1);
                                if(*(s.begin()) == '&')    //bomb hit
                                {
                                    //Explode
                                    obj->setImage(getImage("o_bombexplode"));
                                    obj->setName('x');
                                    playSound("o_explode");
                                }
                            }
                            else if(obj->isData(FALLING_2))
                            {
                                if(*(s.begin()) == '$') //Heart hit
                                {
                                    char cName[256];
                                    sprintf(cName, "o_hearthit%d", randInt(1,4));
                                    playSound(string(cName));   //Random hit noise
                                }
                                else if(*(s.begin()) == '&')    //bomb hit
                                {
                                    //Explode
                                    obj->setImage(getImage("o_bombexplode"));
                                    obj->setName('x');
                                    playSound("o_explode");
                                }
                                else    //rock hit
                                    playSound("o_rock");
                            }
                            obj->removeData(FALLING_1);
                            if(obj->isData(FALLING_2))
                            {
                                obj->removeData(FALLING_2);
                                obj->addData(FALLING_1);
                            }
                        }
                    }
                    else if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1] == NULL)   //Fall down
                    {
                        obj->offset(0,GRID_HEIGHT*SCALE_FAC);
                        m_levelGrid[col][row+1] = obj;
                        m_levelGrid[col][row] = NULL;
                        obj->addData(FALLING_2);
                    }
                    if(row < LEVEL_HEIGHT-1 && m_oldGrid[col][row+1] != NULL)   //Check and see if we should move over to fall
                    {
                        //Check and see if we're on top of an object we can slide off of
                        char cObj = m_levelGrid[col][row+1]->getNameChar();
                        if(cObj == '$' ||
                           cObj == '@' ||
                           cObj == '&' ||
                           cObj == '=' ||
                           cObj == '#')
                        {
                            //Check left side first, as per game behavior
                            if(col > 0 && m_levelGrid[col-1][row] == NULL && m_levelGrid[col-1][row+1] == NULL && !obj->isData(FALLING_1|FALLING_2))
                            {
                                if(row < 1 || m_levelGrid[col-1][row-1] == NULL || (m_levelGrid[col-1][row-1]->getNameChar() != '$' &&
                                                                                    m_levelGrid[col-1][row-1]->getNameChar() != '&' &&
                                                                                    m_levelGrid[col-1][row-1]->getNameChar() != '@'))    //Also check above for falling objects
                                {
                                    m_levelGrid[col-1][row] = obj;
                                    obj->offset(-GRID_WIDTH*SCALE_FAC,0);
                                    m_levelGrid[col][row] = NULL;   //Move over
                                }
                            }
                            //Then check right side
                            else if(col < LEVEL_WIDTH-1 && m_levelGrid[col+1][row] == NULL && m_levelGrid[col+1][row+1] == NULL && !obj->isData(FALLING_1|FALLING_2))
                            {
                                if(row < 1 || m_levelGrid[col+1][row-1] == NULL || (m_levelGrid[col+1][row-1]->getNameChar() != '$' &&
                                                                                    m_levelGrid[col+1][row-1]->getNameChar() != '&' &&
                                                                                    m_levelGrid[col+1][row-1]->getNameChar() != '@'))    //Also check above for falling objects
                                {
                                    m_levelGrid[col+1][row] = obj;
                                    obj->offset(GRID_WIDTH*SCALE_FAC,0);
                                    m_levelGrid[col][row] = NULL;   //Move over
                                }
                            }
                        }
                    }
                    break;

                case '*':   //dwarf
                    if(obj->getFrame() > 3)
                        break;
                    //Move the player if pressing keys
                    if(keyDown(HGEK_RIGHT))
                    {
                        if(_moveToGridSquare_retro(row, col+1))
                            break;
                        if(!keyDown(HGEK_SPACE) && col+1 < LEVEL_WIDTH && m_levelGrid[col+1][row] == NULL)
                        {
                            m_levelGrid[col+1][row] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->offset(GRID_WIDTH*SCALE_FAC, 0);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->getFrame();
                            if(iFrame > 1)
                                iFrame = 0;
                            else
                                iFrame = 2;
                            if(col+1 >= LEVEL_WIDTH || m_oldGrid[col+1][row] != NULL)
                                iFrame = 0;
                            obj->setFrame(iFrame);
                        }
                    }
                    else if(keyDown(HGEK_LEFT))
                    {
                        if(_moveToGridSquare_retro(row, col-1))
                            break;
                        if(!keyDown(HGEK_SPACE) && col > 0 && m_levelGrid[col-1][row] == NULL)
                        {
                            m_levelGrid[col-1][row] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->offset(-GRID_WIDTH*SCALE_FAC, 0);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->getFrame();
                            if(iFrame > 1)
                                iFrame = 1;
                            else
                                iFrame = 3;
                            if(col <= 0 || m_oldGrid[col-1][row] != NULL)
                                iFrame = 1;
                            obj->setFrame(iFrame);
                        }
                    }
                    else if(keyDown(HGEK_DOWN))
                    {
                        _moveToGridSquare_retro(row+1, col);
                        if(!keyDown(HGEK_SPACE) && row+1 < LEVEL_HEIGHT && m_levelGrid[col][row+1] == NULL)
                        {
                            m_levelGrid[col][row+1] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->offset(0, GRID_HEIGHT*SCALE_FAC);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->getFrame();
                            if(iFrame > 1)
                                iFrame -= 2;
                            else
                                iFrame += 2;
                            if(row+1 >= LEVEL_HEIGHT || m_oldGrid[col][row+1] != NULL)
                                if(iFrame > 1)
                                iFrame -= 2;
                            obj->setFrame(iFrame);
                        }
                    }
                    else if(keyDown(HGEK_UP))
                    {
                        _moveToGridSquare_retro(row-1, col);
                        if(!keyDown(HGEK_SPACE) && row > 0 && m_levelGrid[col][row-1] == NULL)
                        {
                            m_levelGrid[col][row-1] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->offset(0, -GRID_HEIGHT*SCALE_FAC);
                        }
                        if(!keyDown(HGEK_SPACE))
                        {
                            //Set to right frame
                            int16_t iFrame = obj->getFrame();
                            if(iFrame > 1)
                                iFrame -= 2;
                            else
                                iFrame += 2;
                            if(row <= 0 || m_oldGrid[col][row-1] != NULL)
                                if(iFrame > 1)
                                iFrame -= 2;
                            obj->setFrame(iFrame);
                        }
                    }
                    else    //Set to stopped frame
                    {
                        int16_t iFrame = obj->getFrame();
                        if(iFrame >= 2)
                            obj->setFrame(iFrame-2);
                    }
                    break;

                case '0':   //balloon
                    //Move up one slot if we can
                    if(row > 0 && m_levelGrid[col][row-1] == NULL)
                    {
                        m_levelGrid[col][row-1] = m_levelGrid[col][row];
                        m_levelGrid[col][row] = NULL;
                        obj->offset(0, -GRID_HEIGHT*SCALE_FAC);
                        if(obj->isData(BALLOON_FLOATDELAY))
                            obj->removeData(BALLOON_FLOATDELAY);
                    }
                    //Move up this and the slot above it if we can
                    else if(row > 1 && m_levelGrid[col][row-1] != NULL && floatable_retro(m_levelGrid[col][row-1]) && m_levelGrid[col][row-2] == NULL)
                    {
                        if(!obj->isData(BALLOON_FLOATDELAY))
                        {
                            obj->addData(BALLOON_FLOATDELAY);
                            //Scoot both upwards one
                            m_levelGrid[col][row-2] = m_levelGrid[col][row-1];
                            m_levelGrid[col][row-1] = obj;
                            m_levelGrid[col][row] = NULL;
                            obj->offset(0, -GRID_HEIGHT*SCALE_FAC);
                            m_levelGrid[col][row-2]->offset(0, -GRID_HEIGHT*SCALE_FAC);
                        }
                        else
                            obj->removeData(BALLOON_FLOATDELAY);
                    }
                    //Get pushed down a slot if we can
                    else if(row > 1 &&
                            row < LEVEL_HEIGHT-1 &&
                            m_levelGrid[col][row-1] != NULL &&
                            m_levelGrid[col][row-2] != NULL &&
                            m_levelGrid[col][row+1] == NULL &&
                            floatable_retro(m_levelGrid[col][row-1]) &&
                            floatable_retro((m_levelGrid[col][row-2])) &&
                            m_levelGrid[col][row-2]->getNameChar() != '*')
                    {
                        if(!obj->isData(BALLOON_FLOATDELAY))
                        {
                            obj->addData(BALLOON_FLOATDELAY);
                            //Scoot both down one (the third will fall)
                            m_levelGrid[col][row]->offset(0, GRID_HEIGHT*SCALE_FAC);
                            m_levelGrid[col][row-1]->offset(0, GRID_HEIGHT*SCALE_FAC);
                            m_levelGrid[col][row+1] = obj;
                            m_levelGrid[col][row] = m_levelGrid[col][row-1];
                            m_levelGrid[col][row-1] = NULL;
                            m_oldGrid[col][row-1] = NULL;
                        }
                        else
                            obj->removeData(BALLOON_FLOATDELAY);
                    }
                    else if(obj->isData(BALLOON_FLOATDELAY))
                        obj->removeData(BALLOON_FLOATDELAY);
                    break;

                case '<':   //Left tunnel
                {
                    if(s == "<*")   //Has the player inside
                    {
                        //Move player left
                        retroObject* nextTunnel = m_levelGrid[col-1][row];  //Assume this is good, since we'll test for it before entering a tunnel
                        if(nextTunnel == NULL || nextTunnel->getNameChar() == '.')  //We can place player down in empty spaces or grass
                        {
                            if(nextTunnel != NULL)
                                nextTunnel->kill();
                            //Create dwarf
                            m_levelGrid[col-1][row] = new Dwarf(getImage("o_dwarf"));
                            m_oldGrid[col-1][row] = m_levelGrid[col-1][row];
                            m_levelGrid[col-1][row]->setNumFrames(8);
                            m_levelGrid[col-1][row]->setFrame(1);
                            m_levelGrid[col-1][row]->setPos((col-1) * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                            m_levelGrid[col-1][row]->setName('*');
                            addObject(m_levelGrid[col-1][row]);
                            obj->setName('<');
                            obj->setImage(getImage("o_tunnell"));

                        }
                        else if(nextTunnel->getNameChar() == '<')    //Next tunnel
                        {
                            nextTunnel->setName("<*");  //Put player inside this next tunnel
                            obj->setName('<');  //Move player out of this tunnel
                            //Swap the images of each
                            Image* img = obj->getImage();
                            obj->setImage(nextTunnel->getImage());
                            nextTunnel->setImage(img);
                        }
                    }
                    break;
                }

                case '>':   //Right tunnel
                {
                    if(s == ">*" && !m_bTunnelMoved)   //Has the player inside
                    {
                        m_bTunnelMoved = true;
                        //Move player right
                        retroObject* nextTunnel = m_oldGrid[col+1][row];  //Assume this is good, since we'll test for it before entering a tunnel
                        if(nextTunnel == NULL || nextTunnel->getNameChar() == '.')  //We can place player down in empty spaces or grass
                        {
                            if(nextTunnel != NULL)
                                nextTunnel->kill();
                            //Create dwarf
                            m_levelGrid[col+1][row] = new Dwarf(getImage("o_dwarf"));
                            m_oldGrid[col+1][row] = m_levelGrid[col+1][row];
                            m_levelGrid[col+1][row]->setNumFrames(8);
                            m_levelGrid[col+1][row]->setPos((col+1) * GRID_WIDTH*SCALE_FAC, row * GRID_HEIGHT*SCALE_FAC);
                            m_levelGrid[col+1][row]->setName('*');
                            addObject(m_levelGrid[col+1][row]);
                            obj->setName('>');
                            obj->setImage(getImage("o_tunnelr"));

                        }
                        else if(nextTunnel->getNameChar() == '>')    //Next tunnel
                        {
                            nextTunnel->setName(">*");  //Put player inside this next tunnel
                            obj->setName('>');  //Move player out of this tunnel
                            //Swap the images of each
                            Image* img = obj->getImage();
                            obj->setImage(nextTunnel->getImage());
                            nextTunnel->setImage(img);
                        }
                    }
                    break;
                }
            }
        }
    }
}

void myEngine::explode_retro(uint16_t row, uint16_t col, bool bStartFrame1)
{
    if(row < 0 ||
       row > LEVEL_HEIGHT-1 ||
       col < 0 ||
       col > LEVEL_WIDTH-1)
    {
        return;     //Off grid = nothing
    }
    retroObject* obj = m_levelGrid[col][row];
    if(obj == NULL) //If null, go ahead and create explosion
    {
        m_levelGrid[col][row] = new retroObject(getImage("o_explode"));
        m_levelGrid[col][row]->setName('X');
        m_levelGrid[col][row]->setNumFrames(7);
        if(bStartFrame1)
            m_levelGrid[col][row]->setFrame(1); //Skip first frame
        m_levelGrid[col][row]->setPos(col*GRID_WIDTH*SCALE_FAC, row*GRID_HEIGHT*SCALE_FAC);
        addObject(m_levelGrid[col][row]);
    }
    else
    {
        switch(obj->getNameChar())
        {
            case '.':
            case '@':
            case '=':
            case '#':
            case '$':
            case '!':
            case '*':
                if(obj->getNameChar() == '*' && m_iWinningCount)
                    break;  //Don't explode_retro if winning
                obj->kill();
                m_levelGrid[col][row] = new retroObject(getImage("o_explode"));
                m_oldGrid[col][row] = m_levelGrid[col][row];
                m_levelGrid[col][row]->setName('X');
                m_levelGrid[col][row]->setNumFrames(7);
                if(bStartFrame1)
                    m_levelGrid[col][row]->setFrame(1); //Skip first frame
                m_levelGrid[col][row]->setPos(col*GRID_WIDTH*SCALE_FAC, row*GRID_HEIGHT*SCALE_FAC);
                addObject(m_levelGrid[col][row]);
                if(obj->getNameChar() == '*' && !m_iDyingCount)
                    m_iDyingCount = DIE_COUNT/2;    //Start dying count
                break;
            case '&':
                if(bStartFrame1)
                    obj->setData(BOMB_EXPLODEDELAY2);
                else
                    obj->setData(BOMB_EXPLODEDELAY1);
                return;

        }
    }
}

bool myEngine::floatable_retro(retroObject* obj)
{
    switch(obj->getNameChar())
    {
        case '*':
        case '&':
        case '@':
        case '$':
            return true;
    }
    return false;
}












