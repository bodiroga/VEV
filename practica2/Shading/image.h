#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
	char *fileName; // The image path
	int width;      // (must be power of 2)
	int height;     // (must be power of 2)
	size_t size;
	unsigned char *data;
} Img;

void InitImageManager();
void DestroyImageManager();
Img *SceneRegisterImage(char *fname);
Img *SceneFindImage(char *fname);

Img *CreateImage(char *fname);
void DestroyImage(Img **theImg);
void PrintRegisteredImages();

#endif
