/* 
 * File:   myconio.h
 * Author: Christian Seiler http://forum.de.selfhtml.org/archiv/2006/10/t138258/
 *
 * Created on 7. Dezember 2008, 14:50
 */

#ifdef  WIN32
  #include <conio.h>
  #include <windows.h>
  #include <stdio.h>
  #include <stdlib.h>

// HACK: test if I'am in the windows CMD by trying to request the console mode
bool testCMD() 
{
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
  if (hStdin == INVALID_HANDLE_VALUE) {
    return false;
  }

  DWORD fdwSaveOldMode;
  if ( !GetConsoleMode(hStdin, &fdwSaveOldMode) ) {
    return false;
  }

  return true;
}

// HACK: test if it's a linux like console by trying to run stty
bool testTTY() {
  FILE *fp = _popen("stty -isig ","r");

  if(fp == NULL) {
    false;
  }

  // skip the output
  char buffer[128];
  while(fgets(buffer, 128, fp)) {
    //printf(buffer);
  }

  // try to reset the console in any case 
  system("stty sane");

  // there was no problem running stty
  return _pclose(fp) == 0;
}

const bool imInCMD = testCMD();
const bool imInTTY = testTTY();

int mygetch()
{
  if(imInCMD) {
    return _getch();
  }

  if(imInTTY) {
    system("stty cbreak -echo");
    int r = getchar();
    system("stty -cbreak echo");
    return r;
  }

  // just use the standard getchar in line mode
  return getchar();
}

#else
#ifndef _MYCONIO_H
#define _MYCONIO_H

#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

// Nachimplementierung der getch()-Funktion aus DOS-conio.h
// fuer POSIX-Betriebsysteme
// Lizenz: Public domain

int mygetch()
{
  int ch;
  struct termios old_t, tmp_t;

  // alte Attribute holen
  if (tcgetattr(STDIN_FILENO, &old_t))
  {
    // moeglicherweise kein Terminal
    // moeglicherweise anderer Fehler
    return -1;
  }
  // struktur kopieren
  memcpy(&tmp_t, &old_t, sizeof (old_t));
  // Zeilenmodus sowie Spezialzeichen deaktivieren,
  // Bildschirmausgabe von eingegebenen Zeichen
  // deaktivieren
  tmp_t.c_lflag &= ~ICANON & ~ECHO;
  // Neue Flags setzen
  if (tcsetattr(STDIN_FILENO, TCSANOW, (const struct termios *) & tmp_t))
  {
    // moeglicherweise irgend ein Fehler
    return -1;
  }
  // zeichen einlesen (darauf verlassen,
  // STDIN_FILENO blockierend ist)
  ch = getchar();
  // Alte Flags zuruecksetzen
  // Rueckgabewert ignorieren, da das Terminal bei Fehler sowieso
  // nicht korrekt reagieren wird, wir dagegen aber nichts tun
  // koennen
  tcsetattr(STDIN_FILENO, TCSANOW, (const struct termios *) & old_t);
  // Zeichen zurueckgeben
  return ch;
}


#ifdef  __cplusplus
}
#endif

#endif  /* _MYCONIO_H */
#endif  /* WIN32 */