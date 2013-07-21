#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include "globaldefs.h"

//template <class T>
class Interpolate
{
  Interpolate(){};
  
protected:
  float32* ptr;
  float32 increment;
  float32 interpTime;
  float32 min;
  float32 max;
  float32 delay;
  float32 delayLeft;
  float32 initial;
  bool bTimed;
  bool bMin;
  bool bMax;
  bool bKillMin;
  bool bKillMax;
  bool bInitial;
  string sUserData;

public:
  ~Interpolate();
  Interpolate(float32* interpObj);
  
  bool update(float32 fTimestep); //Returns true if interpolation is over (object should be destroyed), false otherwise
  
  
  //Access methods
  void    setUserData(string s) {sUserData = s;};
  string  getUserData() {return sUserData;};
  
  void setMinVal(float32 val, bool bKill = true)  {min = val; bMin = true; bKillMin = bKill;};
  float32 getMinVal()                             {if(!bMin) return 0.0f; return min;};
  void setMaxVal(float32 val, bool bKill = true)  {max = val; bMax = true; bKillMax = bKill;};
  float32 getMaxVal()                             {if(!bMax) return 0.0f; return max;};
  void setTime(float32 fTime)               	  {interpTime = fTime; bTimed = true;};
  float32 getTime()                               {if(!bTimed) return 0.0f; return interpTime;};
  void setIncrement(float32 incr)                 {increment = incr;};
  float32 getIncrement()                          {return increment;};
  void setDelay(float32 del)                      {delay = del;delayLeft = del;};
  float32 getDelay()                              {return delay;};
  void setInitial(float32 init)                   {initial = init; bInitial = true;};
  float32 getInitial()                            {if(!bInitial) return 0.0f; return initial;};
  void calculateIncrement(float32 finalVal, float32 fTime, bool bUseCurrent = false); //Interpolate from current value to finalVal over fTime seconds
  
};



























#endif