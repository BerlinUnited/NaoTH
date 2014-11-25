/**
* @file  Platform/linux/SoundPlayer.h
*
* Declaration of class SoundPlayer.
*/

#pragma once

#include <deque>
#include <string>
#include <glib.h>
#include "Tools/Semaphore.h"

class SoundPlayer
{
public:
  /**
   * Put a filename into play sound queue.
   * If you want to play GT_DIR\Sounds\bla.wav use play("bla.wav");
   * @param name The filename of the sound file.
   * @return The amound of files in play sound queue.
   */
  int play(const std::string& name);


  /**
   * main function
   */
  void run();

  SoundPlayer();
  ~SoundPlayer();

private:
  /**
   * start thread
   */
  void start();

  /**
   * play all sounds in queue and block until finished.
   */
  void flush();

  GAsyncQueue* queue;
  Semaphore sem;

  std::string filePrefix;

  bool started;
  volatile bool closing;

  void playDirect(const std::string& basename);
};
