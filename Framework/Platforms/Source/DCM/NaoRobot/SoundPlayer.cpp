/**
* @file  Platform/linux/SoundPlayer.cpp
* Implementation of class SoundPlayer.
* @attention this is the Linux implementation
* @author Colin Graf
*/

#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstdio>
#include <glib/gstdio.h>

#include <iostream>

#include "SoundPlayer.h"

SoundPlayer::SoundPlayer()
  :
  started(false), 
  closing(false) 
{
  queue = g_async_queue_new();
}

SoundPlayer::~SoundPlayer()
{
  if(started)
  {
    closing = true;
    sem.post();
  }

  g_async_queue_unref(queue);
}


void* soundThreadCallback(void* ref)
{
  SoundPlayer* theSoundPlayer = static_cast<SoundPlayer*> (ref);
  theSoundPlayer->run();
  return NULL;
}//end soundThreadCallback


void SoundPlayer::start()
{
  GError* err = NULL;
  g_thread_create(soundThreadCallback, (void*)this, true, &err);
  if(err)
  {
    g_warning("Could not create sound player thread: %s", err->message);
  }
}//end start

void SoundPlayer::run()
{
  while(!closing)
  {
    flush();
    sem.wait();
  }
}//end run

void SoundPlayer::playDirect(const std::string& basename)
{
  static std::string command("gst-launch-0.10 playbin -q uri=file://");
  static std::string extension(".wav");
  
  std::string file_path(filePrefix);
  file_path += basename;
  file_path += extension;

  // check whether the file exists
  if(!g_file_test(file_path.c_str(), G_FILE_TEST_EXISTS))
    return;
  
  std::string full_command(command);
  full_command += file_path;

  int retVal = system( full_command.c_str() );
  if(retVal != 0)
  {
    std::cout << "SoundPlayer:playDirect error " << retVal << std::endl;
  }
}//end playDirect


void SoundPlayer::flush()
{
  while(true)
  {
    if(0 == g_async_queue_length(queue))
      break;

    GString* first = (GString*) g_async_queue_pop(queue);

    //std::string first = queue.front();
    //queue.pop_front();

    playDirect(std::string(first->str));
    g_string_free(first, true);
  }
}//end flush


int SoundPlayer::play(const std::string& name)
{
  if(g_async_queue_length(queue) == 0)
  {
    g_async_queue_push(queue, g_string_new(name.c_str()));
    //soundPlayer.queue.push_back(name.c_str()); // avoid copy-on-write

    if(!started)
    {
      started = true;
      filePrefix = "";
      filePrefix += "/home/nao/naoqi/Media/";
      start();
    }
    else
    {
      sem.post();
    }
  }//end if

  return g_async_queue_length(queue);
}//end play
