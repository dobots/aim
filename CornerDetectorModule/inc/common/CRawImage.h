#ifndef CIMAGE_H
#define CIMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#define THICK_CROSS

/**
@author Tom Krajnik
*/
//template <typename T>
class CRawImage {
public:

  CRawImage(int wi,int he, int bpp=3);
  ~CRawImage();
  void saveBmp(const char* name);
  void saveBmp();
  bool loadBmp(const char* name);
  void swap();

  void updateHeader();

  void plotLine(int x,int y);
  void plotCenter();

  int  getSaveNumber();

  inline bool isMonochrome() { return (bpp == 1); }

  /**
   * Be careful, this really throws information away. The grayscale values are created
   * according to human sensitivity values (30, 59, 11).
   */
  void makeMonochrome();

  double getOverallBrightness(bool upperHalf);
  
	  
  int width;
  int height;
  int palette;
  int size;
  int bpp;

  int numSaved;

//  unsigned char* getData() { return data; }
//private:
  unsigned char* data;

};

#endif
