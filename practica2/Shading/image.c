#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "hash.h"
#include "tools.h"
#include "jpeglib.h" //JPEG LIBRARY

// aux functions
static void load_jpg(char *FileName, Img *result);

static hash *img_hash = NULL;

void InitImageManager() {
	img_hash = CreateVoidHash();
}

void DestroyImageManager() {

	Img *oneImg;

	if (img_hash == NULL) return;
	oneImg = StartLoopHash( img_hash );
	while(oneImg) {
		DestroyImage(&oneImg);
		oneImg = GetNextHash( img_hash );
	}
	DestroyHash(&img_hash);
}

Img *SceneRegisterImage(char *fname) {

	Img *theImg;

	theImg = FindHashElement(img_hash, fname);
	if(!theImg) {
		theImg = CreateImage(fname);
		theImg = InsertHashElement(img_hash, fname, theImg);
	}
	return theImg;
}

Img *SceneFindImage(char *fname) {
	return FindHashElement(img_hash, fname);
}


Img *CreateImage(char *fname) {

	Img *newImg;

	newImg = malloc(sizeof(*newImg));

	newImg->fileName = CopyString(fname);
	newImg->width = 0;
	newImg->height = 0;
	newImg->size = 0;
	newImg->data = NULL;

	load_jpg(fname, newImg);
	return newImg;
}

void DestroyImage(Img **theImg) {

	Img *thisImg = *theImg;
	if (!thisImg) return;
	DestroyString(&thisImg->fileName);
	if (thisImg->data) free(thisImg->data);
	free(thisImg);
	*theImg = NULL;
}

void PrintRegisteredImages() {

	Img *item;

	for( item = StartLoopHash(img_hash); item; item = GetNextHash(img_hash) ) {
		printf("Img: %s (%d, %d, %lu, %p)\n", item->fileName, item->width, item->height, item->size, item->data);
	}



}

static void load_jpg(char *FileName, Img *result) {

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

	result->width = cinfo.output_width;
	result->height = cinfo.output_height;
	result->size = cinfo.output_width * cinfo.output_height * cinfo.output_components;

	if (cinfo.output_components != 3) {
		fprintf(stderr, "[E] LoadGLTexture: %s is an invalid JPEG (only RGB allowed)\n.\n", FileName);
		exit(1);
	}

	result->data = malloc(result->size);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	out_ptr = &(result->data[0]) + result->size - row_stride;

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

/* void SaveImage(Img *thisImg, char *filename) { */

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
