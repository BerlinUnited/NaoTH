/*
 *  Copyright (C) 2011 Justin Stoecker
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef RVDRAW_H
#define RVDRAW_H

#include <string>
#include <cstdio>
#include <cstring>

using namespace std;

inline int writeCharToBuf(unsigned char* buf, unsigned char value) {
  *buf = value;
  return 1;
}

inline int writeDoubleToBuf(unsigned char* buf, double value) {
  char temp[20];
  sprintf(temp, "%6f", value);
  memcpy(buf, temp, 6);
  return 6;
}

inline int writeColorToBuf(unsigned char* buf, const double* color, int channels) {
  int i;
  for (i = 0; i < channels; i++)
    writeCharToBuf(buf+i, (unsigned char)(color[i]*255)); 
  return i;
}

inline int writeStringToBuf(unsigned char* buf, const string* text) {
  long i = 0;
  if (text != NULL)
    i += text->copy((char*)buf+i, text->length(), 0); 
  i += writeCharToBuf(buf+i, 0);
  return i;
}

unsigned char* newBufferSwap(const string* name, int* bufSize) {
  *bufSize = 3 + ((name != NULL) ? name->length() : 0);
  unsigned char* buf = new unsigned char[*bufSize];

  long i = 0;
  i += writeCharToBuf(buf+i, 0);
  i += writeCharToBuf(buf+i, 0);
  i += writeStringToBuf(buf+i, name);

  return buf;
}

unsigned char* newCircle(const double* center, double radius, double thickness,
    const double* color, const string* setName, int* bufSize) {

  *bufSize = 30 + ((setName != NULL) ? setName->length() : 0);
  unsigned char* buf = new unsigned char[*bufSize];
  
  long i = 0;
  i += writeCharToBuf(buf+i, 1);
  i += writeCharToBuf(buf+i, 0);
  i += writeDoubleToBuf(buf+i, center[0]);
  i += writeDoubleToBuf(buf+i, center[1]);
  i += writeDoubleToBuf(buf+i, radius);
  i += writeDoubleToBuf(buf+i, thickness);
  i += writeColorToBuf(buf+i, color, 3);
  i += writeStringToBuf(buf+i, setName);

  return buf;
}

unsigned char* newLine(const double* a, const double* b, double thickness,
    const double* color, const string* setName, int* bufSize) {
  
  *bufSize = 48 + ((setName != NULL) ? setName->length() : 0);
  unsigned char* buf = new unsigned char[*bufSize];

  long i = 0;
  i += writeCharToBuf(buf+i, 1);
  i += writeCharToBuf(buf+i, 1);
  i += writeDoubleToBuf(buf+i, a[0]);
  i += writeDoubleToBuf(buf+i, a[1]);
  i += writeDoubleToBuf(buf+i, a[2]);
  i += writeDoubleToBuf(buf+i, b[0]);
  i += writeDoubleToBuf(buf+i, b[1]);
  i += writeDoubleToBuf(buf+i, b[2]);
  i += writeDoubleToBuf(buf+i, thickness);
  i += writeColorToBuf(buf+i, color, 3);
  i += writeStringToBuf(buf+i, setName);
  
  return buf;
}

unsigned char* newPoint(const double* p, double size, const double* color,
    const string* setName, int* bufSize) {
  
  *bufSize = 30 + ((setName != NULL) ? setName->length() : 0);
  unsigned char* buf = new unsigned char[*bufSize];

  long i = 0;
  i += writeCharToBuf(buf+i, 1);
  i += writeCharToBuf(buf+i, 2);
  i += writeDoubleToBuf(buf+i, p[0]);
  i += writeDoubleToBuf(buf+i, p[1]);
  i += writeDoubleToBuf(buf+i, p[2]);
  i += writeDoubleToBuf(buf+i, size);
  i += writeColorToBuf(buf+i, color, 3);
  i += writeStringToBuf(buf+i, setName);
  
  return buf;
}

unsigned char* newSphere(const double* p, double radius, const double* color,
    const string* setName, int* bufSize) {
  
  *bufSize = 30 + ((setName != NULL) ? setName->length() : 0);
  unsigned char* buf = new unsigned char[*bufSize];

  long i = 0;
  i += writeCharToBuf(buf+i, 1);
  i += writeCharToBuf(buf+i, 3);
  i += writeDoubleToBuf(buf+i, p[0]);
  i += writeDoubleToBuf(buf+i, p[1]);
  i += writeDoubleToBuf(buf+i, p[2]);
  i += writeDoubleToBuf(buf+i, radius);
  i += writeColorToBuf(buf+i, color, 3);
  i += writeStringToBuf(buf+i, setName);
  
  return buf;
}

unsigned char* newPolygon(const double* v, int numVerts, const double* color,
    const string* setName, int* bufSize) {

  *bufSize = 18 * numVerts + 8 + ((setName != NULL) ? setName->length() : 0);
  unsigned char* buf = new unsigned char[*bufSize];

  long i = 0;
  i += writeCharToBuf(buf+i, 1);
  i += writeCharToBuf(buf+i, 4);
  i += writeCharToBuf(buf+i, numVerts);
  i += writeColorToBuf(buf+i, color, 4);
  
  for (int j = 0; j < numVerts; j++) {
    i += writeDoubleToBuf(buf+i, v[j*3+0]);
    i += writeDoubleToBuf(buf+i, v[j*3+1]);
    i += writeDoubleToBuf(buf+i, v[j*3+2]);
  }

  i += writeStringToBuf(buf+i, setName);

  return buf;
}

unsigned char* newAnnotation(const string* text, const double* p,
    const double* color, const string* setName, int* bufSize) {
  
  *bufSize = 25 + text->length() + setName->length();
  unsigned char* buf = new unsigned char[*bufSize];

  long i = 0;
  i += writeCharToBuf(buf+i, 2);
  i += writeCharToBuf(buf+i, 0);
  i += writeDoubleToBuf(buf+i, p[0]);
  i += writeDoubleToBuf(buf+i, p[1]);
  i += writeDoubleToBuf(buf+i, p[2]);
  i += writeColorToBuf(buf+i, color, 3);
  i += writeStringToBuf(buf+i, text);
  i += writeStringToBuf(buf+i, setName);

  return buf;
}

unsigned char* newAgentAnnotation(const string* text, bool leftTeam,
    int agentNum, const double* color, int* bufSize) {

  *bufSize = (text == NULL) ? 3 : 7 + text->length();
  unsigned char* buf = new unsigned char[*bufSize];

  long i = 0;
  i += writeCharToBuf(buf+i, 2);
  
  if (text == NULL) {
    i += writeCharToBuf(buf+i, 2);
    i += writeCharToBuf(buf+i, (leftTeam ? agentNum - 1 : agentNum + 127));
  } else {
    i += writeCharToBuf(buf+i, 1);
    i += writeCharToBuf(buf+i, (leftTeam ? agentNum - 1 : agentNum + 127));
    i += writeColorToBuf(buf+i, color, 3);
    i += writeStringToBuf(buf+i, text);
  }

  return buf;
}

#endif
