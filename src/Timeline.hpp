#pragma once
#ifndef TIMELINE_H_
#define TIMELINE_H_

#define GLEW_STATIC


template <typename T>
class Timeline{
public:
  Timeline(T (*timeSource)());
  Timeline();
  void setSource(T (*timeSource)());
  T getTime();
  T get() {return(getTime());}
  T elapsed();
  T reset();
  void pause();
  void unpause();
  void togglePause();

  T lastTime;
protected:
  T (*timeSource)()=nullptr;
  T start;
  T pausetime;
  T lastElapsed;
  
  bool paused = false;
};

#endif