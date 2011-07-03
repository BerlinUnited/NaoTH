#ifndef VIDEO2LOG_H
#define VIDEO2LOG_H

#include <string>


class Video2Log
{
public:
    Video2Log(std::string fileName, std::string outFile);
    virtual void generateLogFromVideoFile();

    virtual ~Video2Log();
private:
    std::string path;
    std::string outFile;
};

#endif // VIDEO2LOG_H
