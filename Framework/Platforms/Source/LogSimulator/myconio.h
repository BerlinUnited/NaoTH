/* 
 * File:   myconio.h
 * Author: Christian Seiler http://forum.de.selfhtml.org/archiv/2006/10/t138258/
 *
 * Created on 7. Dezember 2008, 14:50
 */

#ifndef WIN32
#ifndef _MYCONIO_H
#define	_MYCONIO_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

// Nachimplementierung der getch()-Funktion aus DOS-conio.h
// für POSIX-Betriebsysteme
// Lizenz: Public domain

int getch()
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


#ifdef	__cplusplus
}
#endif

#endif	/* _MYCONIO_H */
#endif  /* WIN32 */