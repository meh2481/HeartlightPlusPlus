#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include "globaldefs.h"

template <class T>
class Interpolate
{
  Interpolate(){};
  
protected:
  T* ptr;
  T increment;
  float32 interpTime;
  T min;
  T max;
  bool bTimed;
  bool bMin;
  bool bMax;
  bool bKillMin;
  bool bKillMax;
  string sUserData;

public:
  ~Interpolate();
  Interpolate(T* interpObj);
  
  bool update(float32 fTimestep); //Returns true if interpolation is over (object should be destroyed), false otherwise
  
  
  //Access methods
  void    setUserData(string s) {sUserData = s;};
  string  getUserData() {return sUserData;};
  
  void setMinVal(T val, bool bKill = true)  {min = val; bMin = true; bKillMin = bKill;};
  void setMaxVal(T val, bool bKill = true)  {max = val; bMax = true; bKillMax = bKill;};
  void setTime(float32 fTime)               {interpTime = fTime; bTimed = true;};
  void setIncrement(T incr)                 {increment = incr;};
  void calculateIncrement(T finalVal, float32 fTime); //Interpolate from current value to finalVal over fTime seconds
  
};



























#endif