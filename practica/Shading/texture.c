#include <stdlib.h>
#include <stdio.h>
#include "texture.h"
#include "tools.h"

#include "jpeglib.h" //JPEG LIBRARY

// Forward declarations
void LoadJPGTexture(Texture *thisTexture, char *FileName);
void SetGLTexture(Texture *thisTexture);
void DecrBitmapRefCount(Texture *thisTexture);

// Create textures

Texture *CreateVoidTexture(char *name) {

	Texture *newTexture;

	newTexture = malloc(sizeof(*newTexture));

	newTexture->type = AGA_TEX_NORMAL;
	newTexture->fileName = CopyString(name);
	newTexture->height = 0;
	newTexture->width = 0;

	newTexture->mipmap = 0;
	newTexture->magFilter = GL_LINEAR;
	newTexture->minFilter = GL_LINEAR;

	newTexture->wrapS = GL_REPEAT;
	newTexture->wrapT = GL_REPEAT;

	// Allocates a texture name
	glGenTextures( 1, &newTexture->textureId );

	newTexture->components = 3;
	newTexture->format = GL_RGB;
	newTexture->size = 0;
	newTexture->data = NULL;

	SetGLTexture(newTexture);
	return newTexture;
}

void DestroyTexture (Texture **theTexture) {

	Texture *thisTexture = *theTexture;
	/* DecrBitmapRefCount(thisTexture); */
	if (!thisTexture) return;
	DestroyString(&thisTexture->fileName);
	glDeleteTextures(1, &thisTexture->textureId);
	if (thisTexture->data) free(thisTexture->data);
	free(thisTexture);
	*theTexture = NULL;
}

int GetTypeTexture(Texture *theTexture) {
	return theTexture->type;
}

void CopyTexture (Texture *to, const Texture *from) {

	DestroyString(&(to->fileName));
	/* DecrBitmapRefCount(to); */

	to->fileName = CopyString(from->fileName);
	to->height = from->height;
	to->width = from->width;
	to->mipmap = from->mipmap;

	to->magFilter = from->magFilter;
	to->minFilter = from->minFilter;

	to->wrapS = from->wrapS;
	to->wrapT = from->wrapT;

	to->components = from->components;
	to->format = from->format;
	to->size = from->size;
	to->data = from->data;

	to->textureId = from->textureId;

}

Texture *CreateCopyTexture(const Texture *from) {

	Texture *newTexture;

	newTexture = CreateVoidTexture(from->fileName);
	CopyTexture(newTexture, from);
	return newTexture;
}

/**
 * @@ TODO: Create a new texture given a file name
 *
 * 1. Create a void Texture ad assign a name (FileName)
 * 2. load image (use function LoadJPGTexture. See the comments of the function)
 * 3. fill the opengl texture with the appropiate values (use function SetGLTexture)
 *
 */

Texture *CreateTexture(char *FileName) {

	Texture *newTexture;

	newTexture = CreateVoidTexture(FileName);
	newTexture->fileName = CopyString(FileName);

	return newTexture;
}

/**
 * @@ TODO: Create a new mipmap texture given a file name
 *
 * 1. Create a void Texture ad assign a name (FileName)
 * 2. put proper value to mipmap field
 * 3. Set filters for mipmapping (see values in texture.h)
 * 4. load image (use function LoadJPGTexture. See the comments of the function)
 * 5. fill the opengl texture with the appropiate values (use function SetGLTexture)
 *
 */

Texture *CreateMipmapTexture(char *FileName) {

	Texture *newTexture;

	newTexture = CreateVoidTexture(FileName);
	newTexture->fileName = CopyString(FileName);

	return newTexture;
}

/**
 * @@ TODO: Copy structure internals to OpenGL
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

void SetGLTexture(Texture *thisTexture) {

	// bind this texture to texture unit index
	int old_text;

	// Save current texture id
	glGetIntegerv(GL_TEXTURE_BINDING_2D,  &old_text);
	// Set our texture as active
	BindGLTexture(thisTexture);

	// Restore old texture
	glBindTexture( GL_TEXTURE_2D, old_text );
}

// BindGLTexture
//
// Set texture as "active" in the 0 texture unit. Subsequent texture
// operations are done on this texture.

int BindGLTexture(Texture *thisTexture) {
	int old_text;

	glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D,  &old_text);
	glBindTexture( GL_TEXTURE_2D, thisTexture->textureId );
	return old_text;
}

void BindGLTextureTunit(Texture *thisTexture, int location) {
	// Set texture unit index
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture( GL_TEXTURE_2D, thisTexture->textureId );
}

/* Set functions */

// SetDepthTexture: Set texture so it can store depth values (for shadow maps, etc)

static void set_texture_map(Texture *thisTexture, int height, int width,
							int components, GLuint internalFormat, GLuint format) {
	int old_tex;

	thisTexture->mipmap = 0; // No mipmap for target textures
	// Save current texture id
	if (thisTexture->height) {
		// The texture has a previous image. Delete the texture.
		glDeleteTextures(1, &thisTexture->textureId);
		if (thisTexture->data) free(thisTexture->data);
		thisTexture->data = 0;
		// Allocates new texture
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
	old_tex = BindGLTexture(thisTexture);
	// GL_LINEAR does not make sense for depth texture.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, thisTexture->minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, thisTexture->magFilter);

	// Remove artefact on the edges of the shadowmap
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, thisTexture->wrapS );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, thisTexture->wrapT );

	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, thisTexture->width,
				  thisTexture->height, 0, format, GL_UNSIGNED_BYTE,
				  0);
	// Restore old texture
	glBindTexture( GL_TEXTURE_2D, old_tex );
}

void SetTextureDepthMap(Texture *thisTexture, int height, int width) {

	int old_tex;
	set_texture_map(thisTexture, height, width, 1, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
	thisTexture->type = AGA_TEX_DEPTH;
	old_tex = BindGLTexture(thisTexture);
	/* glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY); */
	/* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE); */
	/* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL); */
	// Restore old texture
	glBindTexture( GL_TEXTURE_2D, old_tex );
}

void SetTextureColorMap(Texture *thisTexture, int height, int width) {

	set_texture_map(thisTexture, height, width, 3, GL_RGB, GL_RGB);
	thisTexture->type = AGA_TEX_COLOR;
}

// Wrap

void SetTextureWrapST(Texture *thisTexture, GLenum wrapS, GLenum wrapT) {

	thisTexture->wrapS = wrapS;
	thisTexture->wrapT = wrapT;
	SetGLTexture(thisTexture);
}

void SetTextureWrapS(Texture *thisTexture, GLenum wrapS) {

	thisTexture->wrapS = wrapS;
	SetGLTexture(thisTexture);
}

void SetTextureWrapT(Texture *thisTexture, GLenum wrapT) {
	thisTexture->wrapT = wrapT;
	SetGLTexture(thisTexture);
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
	SetGLTexture(thisTexture);
}

void SetTextureMinFilter(Texture *thisTexture, GLenum minFilter) {
	thisTexture->minFilter = minFilter;
	SetGLTexture(thisTexture);
}


void CycleTextureMagFilter(Texture *thisTexture) {

	static GLenum mag_fil[] = { GL_NEAREST, GL_LINEAR };
	//static char *mag_fil_str[] = { "GL_NEAREST", "GL_LINEAR" };

	int idx = cycleTexEnum(mag_fil, 2, thisTexture->magFilter);
	SetTextureMagFilter(thisTexture, mag_fil[idx]);
	//printf("%s\n", mag_fil_str[idx]);
}

void CycleTextureMinFilter(Texture *thisTexture) {

	static GLenum min_fil[] = { GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST,
								GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR,
								GL_LINEAR_MIPMAP_LINEAR };

	/* static char *min_fil_str[] = { "GL_NEAREST", "GL_LINEAR", "GL_NEAREST_MIPMAP_NEAREST", */
	/*								 "GL_LINEAR_MIPMAP_NEAREST", "GL_NEAREST_MIPMAP_LINEAR", */
	/*								 "GL_LINEAR_MIPMAP_LINEAR"}; */

	int idx = cycleTexEnum(min_fil, 6, thisTexture->minFilter);
	SetTextureMinFilter(thisTexture, min_fil[idx]);
	//printf("%s\n", min_fil_str[idx]);
}

void SetTextureMagFilter(Texture *thisTexture, GLenum magFilter) {
	thisTexture->magFilter = magFilter;
	SetGLTexture(thisTexture);
}

/* Auxiliary functions */

// Decrement bitmap data refcount of a texture. Destroy data refcount == 0.

/* void DecrBitmapRefCount(Texture *thisTexture) { */
/*   (*thisTexture->bitmapRefCount)--; */
/*   // See if we were last texture sharing bitmap data */
/*   if (*thisTexture->bitmapRefCount == 0) { */
/*     free(thisTexture->bitmapRefCount); */
/*     if (thisTexture->data != NULL) { */
/*       free(thisTexture->data); */
/*     } */
/*     thisTexture->data = NULL; */
/*   } */
/* } */

// Load JPG and set to texture data
//
// After the function ends, the following fields of texture are updated:
//
//  - thisTexture->width  -> the width of the image
//  - thisTexture->height -> the width of the image
//  - thisTexture->size   -> the size (in bytes) of the image
//  - thisTexture->data   -> points to the image data

void LoadJPGTexture(Texture *thisTexture, char *FileName) {

	/* This struct contains the JPEG decompression parameters and pointers to
	 * working space (which is allocated as needed by the JPEG library).
	 */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * infile;		/* source file */
	/* More stuff */
	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */
	//int image_size;		/* physical size of image buffer */
	unsigned char *out_ptr;

	if ((infile = fopen(FileName, "rb")) == NULL) {
		fprintf(stderr, "[E] LoadJPGTexture: can't open %s\n", FileName);
		exit(1);
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */

	(void) jpeg_read_header(&cinfo, TRUE);

	/* Step 5: Start decompressor */

	(void) jpeg_start_decompress(&cinfo);

	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;
	//image_size = row_stride * cinfo.output_height;
	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	if(cinfo.quantize_colors) {
		fprintf(stderr, "[E] LoadGLTexture: %s is an invalid JPEG (color quantification)\n", FileName);
		exit(1);
	}

	// Init texture

	thisTexture->format = GL_RGB;
	thisTexture->components = 3;
	thisTexture->width = cinfo.output_width;
	thisTexture->height = cinfo.output_height;
	thisTexture->size = cinfo.output_width * cinfo.output_height * cinfo.output_components;

	if (cinfo.output_components != 3) {
		fprintf(stderr, "[E] LoadGLTexture: %s is an invalid JPEG (only RGB allowed)\n.\n", FileName);
		exit(1);
	}

	thisTexture->data = malloc(thisTexture->size);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	out_ptr = &(thisTexture->data[0]) + thisTexture->size - row_stride;

	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);

		/* put_scanline_into texture data */
		memcpy(out_ptr, buffer[0], row_stride);
		out_ptr -= row_stride;
	}

	/* Step 7: Finish decompression */

	(void) jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	/* Step 8: Release JPEG decompression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	 * Here we postpone it until after no more JPEG errors are possible,
	 * so as to simplify the setjmp error logic above.  (Actually, I don't
	 * think that jpeg_destroy can do an error exit, but why assume anything...)
	 */
	fclose(infile);
}

void SyncOpenGLTexture(Texture *thisTexture) {
	int old_text;
	GLuint format;

	if (!thisTexture->size) return;
	format = GL_RGB;
	if (thisTexture->type == AGA_TEX_DEPTH)
		format = GL_DEPTH_COMPONENT;
	// Save current texture id
	glGetIntegerv(GL_TEXTURE_BINDING_2D,  &old_text);

	glBindTexture(GL_TEXTURE_2D, thisTexture->textureId);
	if (thisTexture->data) {
		free (thisTexture->data);
	}
	thisTexture->data = malloc(thisTexture->size * sizeof(*thisTexture->data));
	// get image from openGL


	glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, thisTexture->data);
	glBindTexture(GL_TEXTURE_2D, old_text);
}

void SaveJPGTexture(Texture *thisTexture, char *filename) {

	int quality = 75; // 1...100 range

	int     image_width;
	int     image_height;
	int     image_components;
	GLuint  image_format;
	unsigned char * image_buffer;

	if (!thisTexture->data) SyncOpenGLTexture(thisTexture); // get image from openGL memory

	image_width = thisTexture->width;
	image_height = thisTexture->height;
	image_components = thisTexture->components;
	image_format = thisTexture->format;
	image_buffer = thisTexture->data;

	/* This struct contains the JPEG compression parameters and pointers to
	 * working space (which is allocated as needed by the JPEG library).
	 * It is possible to have several such structures, representing multiple
	 * compression/decompression processes, in existence at once.  We refer
	 * to any one struct (and its associated working data) as a "JPEG object".
	 */
	struct jpeg_compress_struct cinfo;
	/* This struct represents a JPEG error handler.  It is declared separately
	 * because applications often want to supply a specialized error handler
	 * (see the second half of this file for an example).  But here we just
	 * take the easy way out and use the standard error handler, which will
	 * print a message on stderr and call exit() if compression fails.
	 * Note that this struct must live as long as the main JPEG parameter
	 * struct, to avoid dangling-pointer problems.
	 */
	struct jpeg_error_mgr jerr;
	/* More stuff */
	FILE * outfile;		/* target file */
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */

	/* Step 1: allocate and initialize JPEG compression object */

	/* We have to set up the error handler first, in case the initialization
	 * step fails.  (Unlikely, but it could happen if you are out of memory.)
	 * This routine fills in the contents of struct jerr, and returns jerr's
	 * address which we place into the link field in cinfo.
	 */
	cinfo.err = jpeg_std_error(&jerr);
	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination (eg, a file) */
	/* Note: steps 2 and 3 can be done in either order. */

	/* Here we use the library-supplied code to send compressed data to a
	 * stdio stream.  You can also write your own code to do something else.
	 * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	 * requires it in order to write binary files.
	 */
	if ((outfile = fopen(filename, "wb")) == NULL) {
		fprintf(stderr, "[E] SaveJPGTexture: can't open %s\n", filename);
		exit(1);
	}
	jpeg_stdio_dest(&cinfo, outfile);

	/* Step 3: set parameters for compression */

	/* First we supply a description of the input image.
	 * Four fields of the cinfo struct must be filled in:
	 */
	cinfo.image_width = image_width;	/* image width and height, in pixels */
	cinfo.image_height = image_height;
	cinfo.input_components = image_components;	/* # of color components per pixel */
	cinfo.in_color_space = image_format == GL_DEPTH_COMPONENT ? JCS_GRAYSCALE : JCS_RGB;	/* colorspace of input image */
	/* Now use the library's routine to set default compression parameters.
	 * (You must set at least cinfo.in_color_space before calling this,
	 * since the defaults depend on the source color space.)
	 */
	jpeg_set_defaults(&cinfo);
	/* Now you can set any non-default parameters you wish to.
	 * Here we just illustrate the use of quality (quantization table) scaling:
	 */
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	/* Step 4: Start compressor */

	/* TRUE ensures that we will write a complete interchange-JPEG file.
	 * Pass TRUE unless you are very sure of what you're doing.
	 */
	jpeg_start_compress(&cinfo, TRUE);

	/* Step 5: while (scan lines remain to be written) */
	/*           jpeg_write_scanlines(...); */

	/* Here we use the library's state variable cinfo.next_scanline as the
	 * loop counter, so that we don't have to keep track ourselves.
	 * To keep things simple, we pass one scanline per call; you can pass
	 * more if you wish, though.
	 */
	//row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */
	row_stride = image_width * image_components;	/* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could pass
		 * more than one scanline at a time if that's more convenient.
		 */
		row_pointer[0] = & image_buffer[( image_height - 1 - cinfo.next_scanline ) * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	/* Step 6: Finish compression */

	jpeg_finish_compress(&cinfo);
	/* After finish_compress, we can close the output file. */
	fclose(outfile);

	/* Step 7: release JPEG compression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_compress(&cinfo);

	/* And we're done! */
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

	printf("File Name: %s\n", thisTexture->fileName);
	printf("id:%d \t Mipmap: %d\n", thisTexture->textureId, thisTexture->mipmap);
	printf("Resolution: %d x %d \t Size: %zu\tData: %p\n", thisTexture->width, thisTexture->height, thisTexture->size, thisTexture->data);
	printf("Format: %s\tComponents:%d\n", PT_string(thisTexture->format), thisTexture->components);
	printf("Filters (min,mag): %s, %s\t Wrap (s,t): %s,%s\n\n",
		   PT_string(thisTexture->minFilter), PT_string(thisTexture->magFilter),
		   PT_string(thisTexture->wrapS), PT_string(thisTexture->wrapT));
}
