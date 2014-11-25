/**
 * @file TeamCommEncoder.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 * @brief encoder our team message (protobuf) to ANSCII, and decode from it
 * M essage may consist of 20 characters, which may be taken from the ASCII printing character
 * subset [0x20, 0x7E] except the white space character and the normal brackets ( and ).
 */

#ifndef TEAMCOMMENCODER_H
#define TEAMCOMMENCODER_H

#include <Tools/Communication/ASCIIEncoder.h>

class TeamCommEncoder
{
public:
    TeamCommEncoder();

    // from protobuf to ASCII
    std::string encode(const std::string& data);

    // from ASCII to protobuf
    std::string decode(const std::string& anscii);

private:
    ASCIIEncoder encoder;

    Vector2d maxSize;
    double anglePiece;
};

#endif // TEAMCOMMENCODER_H
