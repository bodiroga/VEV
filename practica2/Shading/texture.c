
#include <stdlib.h>
#include <stdio.h>
#include "texture.h"
#include "tools.h"
#include "image.h"

// Forward declarations
static void send_texparams_gl(Texture *thisTexture);
static void send_image_gl(Texture *thisTexture);

static const char *TT_string(int type);
static char *PT_string(GLenum e);
static void set_texture_RT(Texture *thisTexture, int height, int width,
						   int components, GLuint internalFormat, GLuint format);

// Create textures

Texture *CreateVoidTexture(char *img) {

	Texture *newTexture;

	newTexture = malloc(sizeof(*newTexture));

	newTexture->type = AGA_TEX_TEX;
	newTexture->target = GL_TEXTURE_2D;

	newTexture->mipmap = 0;
	newTexture->magFilter = GL_LINEAR;
	newTexture->minFilter = GL_LINEAR;

	newTexture->wrapS = GL_REPEAT;
	newTexture->wrapT = GL_REPEAT;

	//Allocates a texture name
	glGenTextures( 1, &newTexture->textureId );

	newTexture->height = 0;
	newTexture->width = 0;
	newTexture->size = 0;
	newTexture->components = 3;
	newTexture->format = GL_RGB;

	newTexture->img = 0;

	return newTexture;
}

void DestroyTexture (Texture **theTexture) {

	Texture *thisTexture = *theTexture;
	/* DecrBitmapRefCount(thisTexture); */
	if (!thisTexture) return;
	glDeleteTextures(1, &thisTexture->textureId);
	thisTexture->img = NULL;
	free(thisTexture);
	*theTexture = NULL;
}


void SetImageTexture(Texture *thisTexture,
					 char *FileName) {

	Img *img;

	if(thisTexture->img) {
		//remove image data from openGL
		glDeleteTextures(1, &thisTexture->textureId);
		//Allocates a texture name
		glGenTextures( 1, &thisTexture->textureId );
	}

	img = SceneRegisterImage(FileName);
	thisTexture->img = img;
	thisTexture->size = img->size;
	thisTexture->height = img->height;
	thisTexture->width = img->width;
	thisTexture->format = GL_RGB;
	thisTexture->components = 3;

	// Filters
	if (thisTexture->mipmap) {
		// Filters
		thisTexture->magFilter = GL_LINEAR;
		thisTexture->minFilter = GL_NEAREST_MIPMAP_NEAREST;
	} else {
		thisTexture->magFilter = GL_LINEAR;
		thisTexture->minFilter = GL_LINEAR;
	}
	// Send image into openGL texture
	send_image_gl(thisTexture);
	// Send texture data to OpenGL (initialize OpenGL texture)
	send_texparams_gl(thisTexture);
}

void SetBumpTexture(Texture *thisTexture,
					char *FileName) {

	Img *img;

	thisTexture->type = AGA_TEX_BUMPMAP;
	thisTexture->mipmap = 0; // no mipmap for bump textures
	SetImageTexture(thisTexture, FileName);
}

void SetProjTexture(Texture *thisTexture,
					char *FileName) {

	thisTexture->type = AGA_TEX_PROJ;
	thisTexture->mipmap = 0; // no mipmap for projective textures
	SetImageTexture(thisTexture, FileName);
	thisTexture->magFilter = GL_LINEAR;
	thisTexture->minFilter = GL_LINEAR;
	thisTexture->wrapS = GL_CLAMP_TO_BORDER;
	thisTexture->wrapT = GL_CLAMP_TO_BORDER;
	send_texparams_gl(thisTexture);
}

void SetCubeMapTexture(Texture *thisTexture,
					   char *xpos, char *xneg,
					   char *ypos, char *yneg,
					   char *zpos, char *zneg) {

	thisTexture->type = AGA_TEX_CUBEMAP;
	thisTexture->target = GL_TEXTURE_CUBE_MAP;

	Img *img;
	int i = 0;
	GLuint targets[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	char *names[6] = { xpos, xneg, ypos, yneg, zpos, zneg };


	/* // Typical cube map settings */
	thisTexture->magFilter = GL_LINEAR;
	thisTexture->minFilter = GL_LINEAR;
	thisTexture->wrapS = GL_CLAMP_TO_EDGE;
	thisTexture->wrapT = GL_CLAMP_TO_EDGE;
	thisTexture->wrapR = GL_CLAMP_TO_EDGE;

	BindGLTexture(thisTexture);
	for(i = 0; i < 6; i++) {
		img = SceneRegisterImage(names[i]);
		glTexImage2D(targets[i], 0, 3,
					 img->width, img->height,
					 0, GL_RGB, GL_UNSIGNED_BYTE, img->data);
	}

	/* // Set OpenGL texture wrap */
	/* glTexParameteri( thisTexture->target, GL_TEXTURE_WRAP_S, thisTexture->wrapS); */
	/* glTexParameteri( thisTexture->target, GL_TEXTURE_WRAP_T, thisTexture->wrapT); */
	/* glTexParameterf(thisTexture->target, GL_TEXTURE_WRAP_R, thisTexture->wrapR); */

	/* // Set OpenGL texture filters */
	/* glTexParameteri( thisTexture->target, GL_TEXTURE_MAG_FILTER, thisTexture->magFilter); */
	/* glTexParameteri( thisTexture->target, GL_TEXTURE_MIN_FILTER, thisTexture->minFilter); */

	//UnbindGLTexture(thisTexture);
	send_texparams_gl(thisTexture);
}

// SetDepthTexture: Set texture so it can store depth values (for shadow maps, etc)

void SetDepthMapTexture(Texture *thisTexture, int height, int width) {

	set_texture_RT(thisTexture, height, width, 1, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
	thisTexture->type = AGA_TEX_DEPTH;
	//BindGLTexture(thisTexture);
	/* glTexParameteri(thisTexture->target, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY); */
	/* glTexParameteri(thisTexture->target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE); */
	/* glTexParameteri(thisTexture->target, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL); */
	// Restore old texture
	//UnbindGLTexture(thisTexture);
}

void SetColorMapTexture(Texture *thisTexture, int height, int width) {

	set_texture_RT(thisTexture, height, width, 3, GL_RGB, GL_RGB);
	thisTexture->type = AGA_TEX_COLOR;
}


// BindGLTexture
//
// Set texture as "active" in the zero texture unit. Subsequent texture
// operations are done on this texture.

void BindGLTexture(Texture *thisTexture) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( thisTexture->target, thisTexture->textureId );
}

// Unbind texture
void UnbindGLTexture(Texture *thisTexture) {
	glBindTexture( thisTexture->target, 0 );
}

void BindGLTextureTunit(Texture *thisTexture, int location) {
	// Set texture unit index
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture( thisTexture->target, thisTexture->textureId );
}

void UnbindGLTextureTunit(Texture *thisTexture, int location) {
	// Set texture unit index
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture( thisTexture->target, 0 );
}

/* Set/get functions */


int SetMipmapTexture(Texture *thisTexture, int mipmap) {
	int old = thisTexture->mipmap;
	thisTexture->mipmap = mipmap;
	return old;
}

int GetMipmapTexture(Texture *thisTexture) {
	return thisTexture->mipmap;
}

int GetTypeTexture(Texture *theTexture) {
	return theTexture->type;
}

// Wrap

void SetTextureWrapST(Texture *thisTexture, GLenum wrapS, GLenum wrapT) {

	thisTexture->wrapS = wrapS;
	thisTexture->wrapT = wrapT;
	send_texparams_gl(thisTexture);
}

void SetTextureWrapS(Texture *thisTexture, GLenum wrapS) {

	thisTexture->wrapS = wrapS;
	send_texparams_gl(thisTexture);
}

void SetTextureWrapT(Texture *thisTexture, GLenum wrapT) {
	thisTexture->wrapT = wrapT;
	send_texparams_gl(thisTexture);
}

static int cycleTexEnum(GLenum *fil, int m,
						GLenum theFil) {
	int fil_i = 0;
	for(fil_i = 0; fil_i != m; ++fil_i) {
		if(fil[fil_i] == theFil)
			break;
	}
	fil_i++;
	return (fil_i % m);
}

void CycleTextureWrapS(Texture *thisTexture) {

	static GLenum wrap_fil[] = { GL_CLAMP, GL_REPEAT };
	//static char *wrap_fil_str[] = { "GL_CLAMP", "GL_REPEAT" };

	int idx = cycleTexEnum(wrap_fil, 2, thisTexture->wrapS);
	SetTextureWrapS(thisTexture, wrap_fil[idx]);
	//printf("%s\n", wrap_fil_str[idx]);
}

void CycleTextureWrapT(Texture *thisTexture) {

	static GLenum wrap_fil[] = { GL_CLAMP, GL_REPEAT };
	//static char *wrap_fil_str[] = { "GL_CLAMP", "GL_REPEAT" };

	int idx = cycleTexEnum(wrap_fil, 2, thisTexture->wrapT);
	SetTextureWrapT(thisTexture, wrap_fil[idx]);
	//printf("%s\n", wrap_fil_str[idx]);
}

// Filters

void SetTextureFilters(Texture *thisTexture, GLenum minFilter, GLenum magFilter) {
	thisTexture->magFilter = magFilter;
	thisTexture->minFilter = minFilter;
	send_texparams_gl(thisTexture);
}

void SetTextureMinFilter(Texture *thisTexture, GLenum minFilter) {
	thisTexture->minFilter = minFilter;
	send_texparams_gl(thisTexture);
}


void CycleTextureMagFilter(Texture *thisTexture) {

	static GLenum mag_fil[] = { GL_NEAREST, GL_LINEAR };

	int idx = cycleTexEnum(mag_fil, 2, thisTexture->magFilter);
	SetTextureMagFilter(thisTexture, mag_fil[idx]);
}

void CycleTextureMinFilter(Texture *thisTexture) {

	static GLenum min_fil[] = { GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST,
								GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR,
								GL_LINEAR_MIPMAP_LINEAR };

	int idx = cycleTexEnum(min_fil, 6, thisTexture->minFilter);
	SetTextureMinFilter(thisTexture, min_fil[idx]);
	//printf("%s\n", min_fil_str[idx]);
}

void SetTextureMagFilter(Texture *thisTexture, GLenum magFilter) {
	thisTexture->magFilter = magFilter;
	send_texparams_gl(thisTexture);
}

/* Auxiliary functions */

// TODO: change to SaveTexture(fname)
//
// if depth map, get texture from openGL


/* void SyncOpenGLTexture(Texture *thisTexture) { */
/* 	GLuint format; */

/* 	if (!thisTexture->size) return; */
/* 	format = GL_RGB; */
/* 	if (thisTexture->type == AGA_TEX_DEPTH) */
/* 		format = GL_DEPTH_COMPONENT; */
/* 	BindGLTexture(thisTexture); */
/* 	if (thisTexture->data) { */
/* 		free (thisTexture->data); */
/* 	} */
/* 	thisTexture->data = malloc(thisTexture->size * sizeof(*thisTexture->data)); */
/* 	// get image from openGL */

/* 	glGetTexImage(thisTexture->target, 0, format, GL_UNSIGNED_BYTE, thisTexture->data); */
/* 	UnbindGLTexture(thisTexture); */
/* } */

/* void SaveJPGTexture(Texture *thisTexture, char *filename) { */

/* 	int quality = 75; // 1...100 range */

/* 	int     image_width; */
/* 	int     image_height; */
/* 	int     image_components; */
/* 	GLuint  image_format; */
/* 	unsigned char * image_buffer; */

/* 	if (!thisTexture->data) SyncOpenGLTexture(thisTexture); // get image from openGL memory */

/* 	image_width = thisTexture->width; */
/* 	image_height = thisTexture->height; */
/* 	image_components = thisTexture->components; */
/* 	image_format = thisTexture->format; */
/* 	image_buffer = thisTexture->data; */

/* 	/\* This struct contains the JPEG compression parameters and pointers to */
/* 	 * working space (which is allocated as needed by the JPEG library). */
/* 	 * It is possible to have several such structures, representing multiple */
/* 	 * compression/decompression processes, in existence at once.  We refer */
/* 	 * to any one struct (and its associated working data) as a "JPEG object". */
/* 	 *\/ */
/* 	struct jpeg_compress_struct cinfo; */
/* 	/\* This struct represents a JPEG error handler.  It is declared separately */
/* 	 * because applications often want to supply a specialized error handler */
/* 	 * (see the second half of this file for an example).  But here we just */
/* 	 * take the easy way out and use the standard error handler, which will */
/* 	 * print a message on stderr and call exit() if compression fails. */
/* 	 * Note that this struct must live as long as the main JPEG parameter */
/* 	 * struct, to avoid dangling-pointer problems. */
/* 	 *\/ */
/* 	struct jpeg_error_mgr jerr; */
/* 	/\* More stuff *\/ */
/* 	FILE * outfile;		/\* target file *\/ */
/* 	JSAMPROW row_pointer[1];	/\* pointer to JSAMPLE row[s] *\/ */
/* 	int row_stride;		/\* physical row width in image buffer *\/ */

/* 	/\* Step 1: allocate and initialize JPEG compression object *\/ */

/* 	/\* We have to set up the error handler first, in case the initialization */
/* 	 * step fails.  (Unlikely, but it could happen if you are out of memory.) */
/* 	 * This routine fills in the contents of struct jerr, and returns jerr's */
/* 	 * address which we place into the link field in cinfo. */
/* 	 *\/ */
/* 	cinfo.err = jpeg_std_error(&jerr); */
/* 	/\* Now we can initialize the JPEG compression object. *\/ */
/* 	jpeg_create_compress(&cinfo); */

/* 	/\* Step 2: specify data destination (eg, a file) *\/ */
/* 	/\* Note: steps 2 and 3 can be done in either order. *\/ */

/* 	/\* Here we use the library-supplied code to send compressed data to a */
/* 	 * stdio stream.  You can also write your own code to do something else. */
/* 	 * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that */
/* 	 * requires it in order to write binary files. */
/* 	 *\/ */
/* 	if ((outfile = fopen(filename, "wb")) == NULL) { */
/* 		fprintf(stderr, "[E] SaveJPGTexture: can't open %s\n", filename); */
/* 		exit(1); */
/* 	} */
/* 	jpeg_stdio_dest(&cinfo, outfile); */

/* 	/\* Step 3: set parameters for compression *\/ */

/* 	/\* First we supply a description of the input image. */
/* 	 * Four fields of the cinfo struct must be filled in: */
/* 	 *\/ */
/* 	cinfo.image_width = image_width;	/\* image width and height, in pixels *\/ */
/* 	cinfo.image_height = image_height; */
/* 	cinfo.input_components = image_components;	/\* # of color components per pixel *\/ */
/* 	cinfo.in_color_space = image_format == GL_DEPTH_COMPONENT ? JCS_GRAYSCALE : JCS_RGB;	/\* colorspace of input image *\/ */
/* 	/\* Now use the library's routine to set default compression parameters. */
/* 	 * (You must set at least cinfo.in_color_space before calling this, */
/* 	 * since the defaults depend on the source color space.) */
/* 	 *\/ */
/* 	jpeg_set_defaults(&cinfo); */
/* 	/\* Now you can set any non-default parameters you wish to. */
/* 	 * Here we just illustrate the use of quality (quantization table) scaling: */
/* 	 *\/ */
/* 	jpeg_set_quality(&cinfo, quality, TRUE /\* limit to baseline-JPEG values *\/); */

/* 	/\* Step 4: Start compressor *\/ */

/* 	/\* TRUE ensures that we will write a complete interchange-JPEG file. */
/* 	 * Pass TRUE unless you are very sure of what you're doing. */
/* 	 *\/ */
/* 	jpeg_start_compress(&cinfo, TRUE); */

/* 	/\* Step 5: while (scan lines remain to be written) *\/ */
/* 	/\*           jpeg_write_scanlines(...); *\/ */

/* 	/\* Here we use the library's state variable cinfo.next_scanline as the */
/* 	 * loop counter, so that we don't have to keep track ourselves. */
/* 	 * To keep things simple, we pass one scanline per call; you can pass */
/* 	 * more if you wish, though. */
/* 	 *\/ */
/* 	//row_stride = image_width * 3;	/\* JSAMPLEs per row in image_buffer *\/ */
/* 	row_stride = image_width * image_components;	/\* JSAMPLEs per row in image_buffer *\/ */

/* 	while (cinfo.next_scanline < cinfo.image_height) { */
/* 		/\* jpeg_write_scanlines expects an array of pointers to scanlines. */
/* 		 * Here the array is only one element long, but you could pass */
/* 		 * more than one scanline at a time if that's more convenient. */
/* 		 *\/ */
/* 		row_pointer[0] = & image_buffer[( image_height - 1 - cinfo.next_scanline ) * row_stride]; */
/* 		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1); */
/* 	} */

/* 	/\* Step 6: Finish compression *\/ */

/* 	jpeg_finish_compress(&cinfo); */
/* 	/\* After finish_compress, we can close the output file. *\/ */
/* 	fclose(outfile); */

/* 	/\* Step 7: release JPEG compression object *\/ */

/* 	/\* This is an important step since it will release a good deal of memory. *\/ */
/* 	jpeg_destroy_compress(&cinfo); */

/* 	/\* And we're done! *\/ */
/* } */

/**
 * Send texture parameters to OpenGL
 *
 * hints:
 *        - use glTexParameteri for setting filters/wrap
 *        - use glTexImage2D or gluBuild2DMipmaps for loading image into graphic card
 *          -- internalFormat: 3
 *          -- format:         GL_RGB
 *          -- border:         0
 *          -- type:           GL_UNSIGNED_BYTE
 *
 */

static void send_texparams_gl(Texture *thisTexture) {

	// Set our texture as active
	BindGLTexture(thisTexture);

	// Set OpenGL texture wrap
	glTexParameteri( thisTexture->target, GL_TEXTURE_WRAP_S, thisTexture->wrapS);
	glTexParameteri( thisTexture->target, GL_TEXTURE_WRAP_T, thisTexture->wrapT);
	if (thisTexture->type == AGA_TEX_CUBEMAP) {
		glTexParameterf(thisTexture->target, GL_TEXTURE_WRAP_R,
						thisTexture->wrapR);
	}

	// Set OpenGL texture filters
	glTexParameteri( thisTexture->target, GL_TEXTURE_MAG_FILTER, thisTexture->magFilter);
	glTexParameteri( thisTexture->target, GL_TEXTURE_MIN_FILTER, thisTexture->minFilter);

	// Restore old texture
	UnbindGLTexture(thisTexture);
}

/**
 * Send image to OpenGL
 *
 * hints:
 *        - use glTexImage2D or gluBuild2DMipmaps for loading image into graphic card
 *          -- internalFormat: 3
 *          -- format:         GL_RGB
 *          -- border:         0
 *          -- type:           GL_UNSIGNED_BYTE
 *
 */


static void send_image_gl(Texture *thisTexture) {

	Img *img = thisTexture->img;

	if (!img) return;

	// Set our texture as active
	BindGLTexture(thisTexture);

	if (img->height && img->width) {
		// Load image to OpenGL texture
		if (thisTexture->mipmap) {
			gluBuild2DMipmaps(thisTexture->target, thisTexture->components, img->width, img->height,
							  thisTexture->format, GL_UNSIGNED_BYTE, img->data);
		} else {
			glTexImage2D( thisTexture->target, 0, thisTexture->components, img->width,
						  img->height, 0, thisTexture->format, GL_UNSIGNED_BYTE,
						  img->data);
		}
	}
	// Restore old texture
	UnbindGLTexture(thisTexture);
}

// set Render Target (RT) texture:
//  - properly set filters and anti-aliasing setings
//  - reclaim memory in OpenGL for storing the texture

static void set_texture_RT(Texture *thisTexture, int height, int width,
						   int components, GLuint internalFormat, GLuint format) {


	thisTexture->mipmap = 0; // No mipmap for target textures
	if (thisTexture->img) {
		// The texture has a previous image. Delete OpenGL's texture object.
		glDeleteTextures(1, &thisTexture->textureId);
		thisTexture->img = 0;
		// Allocates new texture object
		glGenTextures( 1, &thisTexture->textureId );
	}
	thisTexture->height = height;
	thisTexture->components = components;
	thisTexture->format = internalFormat;

	thisTexture->width = width;
	thisTexture->size = components * height * width;
	if (!thisTexture->size) {
		fprintf(stderr, "[E] texture with zero size\n");
		exit(1);
	}

	thisTexture->magFilter = GL_NEAREST;
	thisTexture->minFilter = GL_NEAREST;
	thisTexture->wrapS = GL_CLAMP;
	thisTexture->wrapT = GL_CLAMP;

	BindGLTexture(thisTexture);
	send_texparams_gl(thisTexture);
	// Reclaim OpenGL space for image data
	glTexImage2D( thisTexture->target, 0, internalFormat, thisTexture->width,
				  thisTexture->height, 0, format, GL_UNSIGNED_BYTE,
				  0);
	// Restore old texture
	UnbindGLTexture(thisTexture);
}

static const char *TT_string(int e) {

	static int N[] = {AGA_TEX_TEX, AGA_TEX_DEPTH, AGA_TEX_COLOR, AGA_TEX_CUBEMAP, AGA_TEX_BUMPMAP, AGA_TEX_PROJ};
	static char *T[]  = {"AGA_TEX_TEX", "AGA_TEX_DEPTH", "AGA_TEX_COLOR", "AGA_TEX_CUBEMAP", "AGA_TEX_BUMPMAP", "AGA_TEX_PROJ"};

	int i;
	int m;

	m = sizeof(T) / sizeof(char *);
	for(i=0; i < m; i++) {
		if (e == N[i]) return T[i];
	}
	return "<unknown>";
}


static char *PT_string(GLenum e) {

	static GLenum N[] = {GL_CLAMP, GL_CLAMP_TO_BORDER, GL_REPEAT, GL_LINEAR, GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_RGB, GL_DEPTH_COMPONENT};
	static char *T[]  = {"GL_CLAMP", "GL_CLAMP_TO_BORDER", "GL_REPEAT", "GL_LINEAR", "GL_NEAREST", "GL_NEAREST_MIPMAP_LINEAR", "GL_NEAREST_MIPMAP_NEAREST",
						 "GL_LINEAR_MIPMAP_LINEAR", "GL_LINEAR_MIPMAP_NEAREST", "GL_RGB", "GL_DEPTH_COMPONENT"};
	int i;
	int m;

	m = sizeof(T) / sizeof(char *);
	for(i=0; i < m; i++) {
		if (e == N[i]) return T[i];
	}
	return "<unknown>";
}

void PrintTexture (Texture *thisTexture) {

	Img *img;
	//printf("File Name: %s\n", thisTexture->fileName);
	printf("Texture id:%d\tType: %s\n", thisTexture->textureId, TT_string(thisTexture->type));
	img = thisTexture->img;
	if (!img) {
		printf ("No jpeg image\n");
	} else {
		printf("JPG image: %s\tResolution: %d x %d\tSize: %lu\tMipmap: %d\tData: %p\n", img->fileName, thisTexture->width, thisTexture->height,
			   thisTexture->size, thisTexture->mipmap, img->data);

	}
	if (!img && thisTexture->size) {
		printf("Generated image\tResolution: %d x %d \t Size: %lu\tMipmap: %d\n", thisTexture->width, thisTexture->height,
			   thisTexture->size, thisTexture->mipmap);
	}
	printf("Format: %s\tComponents:%d\n", PT_string(thisTexture->format), thisTexture->components);
	printf("Filters (min,mag): %s, %s\t Wrap (s,t): %s,%s\n\n",
			 PT_string(thisTexture->minFilter), PT_string(thisTexture->magFilter),
			 PT_string(thisTexture->wrapS), PT_string(thisTexture->wrapT));
}

