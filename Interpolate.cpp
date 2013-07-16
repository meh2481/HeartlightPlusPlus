#include "Interpolate.h"


//template <class T>
Interpolate::~Interpolate()
{
  
}

//template <class T>
Interpolate::Interpolate(float32* interpObj)
{
  ptr = interpObj;
  bTimed = bMin = bMax = bKillMin = bKillMax = false;
  interpTime = 0.0f;
}

//template <class T>
bool Interpolate::update(float32 fTimestep)
{
  (*ptr) += increment * fTimestep;
  if(bTimed)
  {
    interpTime -= fTimestep;
    if(interpTime <= 0.0)
      return true;
  }
  if(bMin)
  {
    if((*ptr) < min)
    {
      (*ptr) = min;
      increment = -increment;
      if(bKillMin)
        return true;
    }
  }
  if(bMax)
  {
    if((*ptr) > max)
    {
      (*ptr) = max;
      increment = -increment;
      if(bKillMax)
        return true;
    }
  }
  
  return false;
}

//template <class T>
void Interpolate::calculateIncrement(float32 finalVal, float32 fTime)
{
  increment = (finalVal - *ptr) / fTime;
}





















