/**
* @author Heinrich Mellmann <mellmann@informatik.hu-berlin.de>
*/
#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <iostream>
#include <stdio.h>

#ifdef NAO
#include <unistd.h>
#endif

namespace FileUtils
{

static inline bool writeStringToFile(const std::string& content, const std::string& filename)
{
  FILE* file = NULL;
#ifdef WIN32
  fopen_s(&file, filename.c_str(), "wb");
#else
  file = fopen(filename.c_str(), "wb");
#endif
  if (file != NULL) 
  {
    size_t byteWrite = fwrite(content.c_str(), 1, content.size(), file);

    if ( byteWrite != content.size() )
    {
      printf("SynchronizedFileWriter write failed to %s", filename.c_str());
    }

    // synchronize with the filesystem, 
    // i.e., purge the fs buffer to the disk
#ifdef NAO
    int fd = fileno(file);
    fsync(fd);
#endif

    fclose(file);
    return true;
  }//end if

  return false;
}


static inline bool writeStreamToFile(const std::stringstream& content, const std::string& filename)
{
  std::string str = content.str();
  return writeStringToFile(str, filename);
}

};

#endif // FILEUTILS_H
