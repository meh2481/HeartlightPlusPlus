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
  bool bTimed;
  bool bMin;
  bool bMax;
  bool bKillMin;
  bool bKillMax;
  string sUserData;

public:
  ~Interpolate();
  Interpolate(float32* interpObj);
  
  bool update(float32 fTimestep); //Returns true if interpolation is over (object should be destroyed), false otherwise
  
  
  //Access methods
  void    setUserData(string s) {sUserData = s;};
  string  getUserData() {return sUserData;};
  
  void setMinVal(float32 val, bool bKill = true)  {min = val; bMin = true; bKillMin = bKill;};
  void setMaxVal(float32 val, bool bKill = true)  {max = val; bMax = true; bKillMax = bKill;};
  void setTime(float32 fTime)               {interpTime = fTime; bTimed = true;};
  void setIncrement(float32 incr)                 {increment = incr;};
  void calculateIncrement(float32 finalVal, float32 fTime); //Interpolate from current value to finalVal over fTime seconds
  
};



























#endif