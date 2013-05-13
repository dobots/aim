#ifndef CIMAGE_H
#define CIMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#define THICK_CROSS

struct Pixel {
	char r;
	char g;
	char b;
};

struct Patch {
	unsigned char *data;
	int width;
	int height;
};

/**
@author Tom Krajnik
*/
//template <typename T>
class CRawImage {
public:

  CRawImage(int wi, int he, int bpp); //=3);
  ~CRawImage();

  //! Copy constructor
  CRawImage(const CRawImage & other);

  Pixel getPixel(int x, int y);

  Pixel getPixel(int x, int y, Patch & patch);

  void setPixel(int x, int y, Patch & patch, Pixel pixel);

  void getPatch(int x, int y, Patch & patch);

  CRawImage* patch2Img(Patch &patch);

  void refresh();

  void saveBmp(const char* name);

  //! With multiple instances of class CRawImage, they will overwrite each other
  void saveNumberedBmp(const char* name, bool increment = true);

  bool loadBmp(const char* name);
  void swap();

  void updateHeader();

  void plotLine(int x,int y);
  void plotCenter();

  void plotCross(int i,int j,int size);

  int  getSaveNumber();

  inline bool isMonochrome() { return (bpp == 1); }

  /**
   * Be careful, this really throws information away. The grayscale values are created
   * according to human sensitivity values (30, 59, 11).
   */
  void makeMonochrome();

  /**
   * Some as makeMonochrome, but keeps image intact, and stores result in parameter "result"
   * @param result   Previously constructed image
   */
  void makeMonochrome(CRawImage* result);

  double getOverallBrightness(bool upperHalf);

  static int numSaved;

//  int palette;

//  unsigned char* getData() { return data; }
//private:

  unsigned char* data;

  //! Make sure the data array is adjusted, by adding setters
  void setbpp(int bpp) { this->bpp = bpp; refresh(); }
  void setdimensions(int width, int height) { this->width = width; this->height = height; refresh(); }

  const int getwidth() { return width; };
  const int getheight() { return height; };
  const int getsize() { return size; };
private:
  int width;
  int height;
  int bpp;
  int size;
};

#endif
