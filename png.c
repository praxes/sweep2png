#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "crystallography.h"
#include "sweeper.h"
#include "data_io.h"

int write_pngs_color(SAMPLE *sample);
int write_pngs_confidence(SAMPLE *sample);

static int convrodtocolor(double *r, double *c);

//---------------------- PNG ----------------------------------------

/* A coloured pixel. */

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_t;

/* A picture. */

typedef struct  {
    pixel_t *pixels;
    size_t width;
    size_t height;
} bitmap_t;

/* Given "bitmap", this returns the pixel of bitmap at the point
   ("x", "y"). */

pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}

/* Write "bitmap" to a PNG file specified by "path"; returns 0 on
   success, non-zero on error. */

int save_png_to_file (bitmap_t *bitmap, const char *path)
{
  FILE * fp;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  size_t x, y;
  png_byte ** row_pointers = NULL;
  /* "status" contains the return value of this function. At first
     it is set to a value which means 'failure'. When the routine
     has finished its work, it is set to a value which means
     'success'. */
  int status = -1;
  /* The following number is set by trial and error only. I cannot
     see where it it is documented in the libpng manual.
  */
  int pixel_size = 3;
  int depth = 8;

  fp = fopen (path, "wb");
  if (! fp) {
    goto fopen_failed;
  }

  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    goto png_create_write_struct_failed;
  }

  info_ptr = png_create_info_struct (png_ptr);
  if (info_ptr == NULL) {
    goto png_create_info_struct_failed;
  }

  /* Set up error handling. */

  if (setjmp (png_jmpbuf (png_ptr))) {
    goto png_failure;
  }

  /* Set image attributes. */

  png_set_IHDR (png_ptr,
		info_ptr,
		bitmap->width,
		bitmap->height,
		depth,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);

  /* Initialize rows of PNG. */

  row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
  for (y = 0; y < bitmap->height; ++y) {
    png_byte *row =
      png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
    row_pointers[y] = row;
    for (x = 0; x < bitmap->width; ++x) {
      //      pixel_t * pixel = pixel_at (bitmap, x, y);
      pixel_t *pixel = &(bitmap->pixels[bitmap->width*y + x]);
      *row++ = pixel->red;
      *row++ = pixel->green;
      *row++ = pixel->blue;
    }
  }

  /* Write the image data to "fp". */

  png_init_io (png_ptr, fp);
  png_set_rows (png_ptr, info_ptr, row_pointers);
  png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  /* The routine has successfully written the file, so we set
     "status" to a value which indicates success. */

  status = 0;

  for (y = 0; y < bitmap->height; y++) {
    png_free (png_ptr, row_pointers[y]);
  }
  png_free (png_ptr, row_pointers);

 png_failure:
 png_create_info_struct_failed:
  png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
  fclose (fp);
 fopen_failed:

  return status;
}

//--------------------------------------------------------------------

int write_pngs_confidence(SAMPLE *sample) {
  RMESH *rm=&(sample->rmesh);
  bitmap_t layer;
  int i,k;
  long N=0;
  char filename[300];

  layer.width  = rm->nvx;
  layer.height = rm->nvy;

  if ( (layer.pixels = (pixel_t *)calloc(sizeof(pixel_t), (long)layer.width*(long)layer.height))==NULL){
    printf("Not enough memory to allocate layer.pixels\n");
    exit(1);
  }

  for(k=0;k<rm->nvz;k++){

    while( (N<rm->nvoxels) && (rm->voxel[N].iz==k) ){

      VOXEL *v=&(rm->voxel[N]);

      pixel_t *pixel = &(layer.pixels[layer.width*v->iy + v->ix]);

      double maxcomp=0.;
      int maxi=0;

      if (v->nU>0){
        maxcomp=v->orientation[0].completeness;
	for(i=1;i<v->nU;i++){
	  if (v->orientation[0].completeness > maxcomp) {
	    maxi=i;
	    maxcomp=v->orientation[i].completeness;
	  }
	}
      }

      if (maxcomp>=sample->completeness_cut && maxcomp>0.) {

	pixel->red   = maxcomp*255;
	pixel->green = maxcomp*255;
	pixel->blue  = maxcomp*255;

      } else {
	pixel->red   = 0.;
	pixel->green = 0.;
	pixel->blue  = 0.;
      }

      ++N;
    }
      /* Write the image to a file 'fruit.png'. */
    sprintf(filename,"%s_conf_%03i.png",sample->outputfilestem,k);
    save_png_to_file (&layer, filename);
  }

  free(layer.pixels);

  return (SUCCESS);
}

int write_pngs_color(SAMPLE *sample) {
  RMESH *rm=&(sample->rmesh);
  bitmap_t layer;
  int i,k;
  long N=0;
  char filename[300];

  layer.width  = rm->nvx;
  layer.height = rm->nvy;

  if ( (layer.pixels = (pixel_t *)calloc(sizeof(pixel_t), (long)layer.width*(long)layer.height))==NULL){
    printf("Not enough memory to allocate layer.pixels\n");
    exit(1);
  }

  for(k=0;k<rm->nvz;k++){
    printf("k %i, N %ld, nvoxels %ld\n",k,N,rm->nvoxels);

    while( (N<rm->nvoxels) && (rm->voxel[N].iz==k) ){

      VOXEL *v=&(rm->voxel[N]);

      pixel_t *pixel = &(layer.pixels[layer.width*v->iy + v->ix]);

      double maxcomp=0.;
      int maxi=0;

      if (v->nU>0){
        maxcomp=v->orientation[0].completeness;
	for(i=1;i<v->nU;i++){
	  if (v->orientation[0].completeness > maxcomp) {
	    maxi=i;
	    maxcomp=v->orientation[i].completeness;
	  }
	}
      }

      if (maxcomp>=sample->completeness_cut && maxcomp>0.) {
	ORIENTATION *or=&(v->orientation[maxi]);
	double c[3];

	convrodtocolor(&(or->r[0]), &c[0]);

	pixel->red   = c[0]*255;
	pixel->green = c[1]*255;
	pixel->blue  = c[2]*255;

      } else {
	pixel->red   = 0.;
	pixel->green = 0.;
	pixel->blue  = 0.;
      }

      ++N;
    }
      /* Write the image to a file 'fruit.png'. */
    sprintf(filename,"%s_%03i.png",sample->outputfilestem,k);
    save_png_to_file (&layer, filename);
  }

  free(layer.pixels);

  return (SUCCESS);
}



static int convrodtocolor(double *r, double *c){
  // converts Rodriguez vector r1,r2,r3 to colour
  // set colour so that all orientations in Cubic space are allowed
  // from matlab script by Grethe Winther.

  double maxhx=62.8*3.14/180; double minhx=-62.8*3.14/180;

  double maxhy=62.8*3.14/180; double minhy=-62.8*3.14/180;

  double maxhz=62.8*3.14/180; double minhz=-62.8*3.14/180;

  double rr = sqrt(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]);

  double theta=2*atan(rr);

  double r1=r[0]/(rr); double r2=r[1]/(rr); double r3=r[2]/(rr);

  //normalise colors

  c[0] = (r1*theta-minhx)/(maxhx-minhx); // red
  c[1] = (r2*theta-minhy)/(maxhy-minhy); // green
  c[2] = (r3*theta-minhz)/(maxhz-minhz); // blue

  return (SUCCESS);
}

