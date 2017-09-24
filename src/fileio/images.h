#ifndef FILEIO_IMAGES_H
#define FILEIO_IMAGES_H

/*
 * Improved readBMP/writeBMP.
 * Automatically detects extensions and read/write the data.
 * Currently supports: bmp, png
 * 
 */
extern unsigned char *readImage(const char *fname, int& width, int& height);
extern void writeImage(const char *iname, int width, int height, const void *data); 

#endif
