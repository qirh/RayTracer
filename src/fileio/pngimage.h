#ifndef FILEIO_PNGIMAGE_H
#define FILEIO_PNGIMAGE_H

void png_version_info(void);

unsigned char *readPNG(const char *fname, int& width, int& height);
void writePNG(const char *iname, int width, int height, const void* data); 

#endif
