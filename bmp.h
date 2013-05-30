#ifndef BMP_H_INCLUDED
#define BMP_H_INCLUDED

int read_bmp(const char *filename, int *width, int *height, unsigned char *rgb);

int write_bmp(const char *filename, int width, int height, char *rgb);

#endif
