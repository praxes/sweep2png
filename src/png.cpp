#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "crystallography.h"
#include "sweeper.h"
#include "data_io.h"

int write_pngs_color(SAMPLE *sample);
int write_pngs_confidence(SAMPLE *sample);

static int convrodtocolor(double *r, double *c);
static int convhkltocolor_cubic(double *U, double *c);
static int convhkltocolor_hexagonal(double *U, double *c);
static int convhkltocolor_orthorhombic(double *U, double *c);

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

  row_pointers = (png_byte **) png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
  for (y = 0; y < bitmap->height; ++y) {
    png_byte *row = (png_byte *)
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

        if (strcmp(sample->lattice, "cubic") == 0) {
          convhkltocolor_cubic(&(or->U[0]), &c[0]);
        } else if (strcmp(sample->lattice, "hexagonal") == 0) {
          convhkltocolor_hexagonal(&(or->U[0]), &c[0]);
        } else if (strcmp(sample->lattice, "orthorhombic") == 0) {
          convhkltocolor_orthorhombic(&(or->U[0]), &c[0]);
        } else {
          convrodtocolor(&(or->r[0]), &c[0]);
        }

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


static int convhkltocolor_cubic(double *U, double *c){
  // converts hkl (U31,U32,U33) to colour in cubic stereographic triangle

  double pi = 3.14159;
  double beta = 0;
  int i,j;

  double axis[3];
  axis[0] = fabs(U[6]);
  axis[1] = fabs(U[7]);
  axis[2] = fabs(U[8]);

  for(j=0;j<2;j++){
    for(i=j+1;i<3;i++){
      if (axis[j]>axis[i]){
        c[0]=axis[j];
        axis[j]=axis[i];
        axis[i]=c[0];
      }
    }
  }

  double r=sqrt(axis[0]*axis[0]/((axis[2]+1))/((axis[2]+1))+(axis[1]/(axis[2]+1)+1)*(axis[1]/(axis[2]+1)+1));
  if (axis[1]==0) {
    beta=0;
  }
  else {
    beta=atan(axis[0]/axis[1]);
  }

  //normalise colors
  double exponent = 0.5;

  c[0] = pow((sqrt(2.0)-r)/(sqrt(2.0)-1),exponent); // red
  c[1] = pow((1-4*beta/pi)*((r-1)/(sqrt(2.0)-1)),exponent); // green
  c[2] = pow(4*beta/pi*((r-1)/(sqrt(2.0)-1)),exponent); // blue

  return (SUCCESS);
}


static int convhkltocolor_hexagonal(double *U, double *c){
  // converts hkl (U31,U32,U33) to colour in hexagonal stereographic triangle

  int j;

  double rot[108];
  rot[0*9+0]= 1;  rot[0*9+1]= 0;  rot[0*9+2]= 0;
  rot[0*9+3]= 0;  rot[0*9+4]= 1;  rot[0*9+5]= 0;
  rot[0*9+6]= 0;  rot[0*9+7]= 0;  rot[0*9+8]= 1;

  rot[1*9+0]= -0.5;        rot[1*9+1]= -sqrt(0.75);  rot[1*9+2]= 0;
  rot[1*9+3]= sqrt(0.75);  rot[1*9+4]= -0.5;         rot[1*9+5]= 0;
  rot[1*9+6]= 0;           rot[1*9+7]= 0;            rot[1*9+8]= 1;

  rot[2*9+0]= -0.5;        rot[2*9+1]= sqrt(0.75);   rot[2*9+2]= 0;
  rot[2*9+3]= -sqrt(0.75); rot[2*9+4]= -0.5;         rot[2*9+5]= 0;
  rot[2*9+6]= 0;           rot[2*9+7]= 0;            rot[2*9+8]= 1;

  rot[3*9+0]= -1; rot[3*9+1]= 0;  rot[3*9+2]= 0;
  rot[3*9+3]= 0;  rot[3*9+4]= -1; rot[3*9+5]= 0;
  rot[3*9+6]= 0;  rot[3*9+7]= 0;  rot[3*9+8]= 1;

  rot[4*9+0]= 0.5;         rot[4*9+1]= sqrt(0.75);  rot[4*9+2]= 0;
  rot[4*9+3]= -sqrt(0.75); rot[4*9+4]= 0.5;         rot[4*9+5]= 0;
  rot[4*9+6]= 0;           rot[4*9+7]= 0;           rot[4*9+8]= 1;

  rot[5*9+0]= 0.5;         rot[5*9+1]= -sqrt(0.75);  rot[5*9+2]= 0;
  rot[5*9+3]= sqrt(0.75);  rot[5*9+4]= 0.5;          rot[5*9+5]= 0;
  rot[5*9+6]= 0;           rot[5*9+7]= 0;            rot[5*9+8]= 1;

  rot[6*9+0]= 0.5;         rot[6*9+1]= sqrt(0.75);  rot[6*9+2]= 0;
  rot[6*9+3]= sqrt(0.75);  rot[6*9+4]= -0.5;        rot[6*9+5]= 0;
  rot[6*9+6]= 0;           rot[6*9+7]= 0;           rot[6*9+8]= -1;

  rot[7*9+0]= 0.5;         rot[7*9+1]= -sqrt(0.75);  rot[7*9+2]= 0;
  rot[7*9+3]= -sqrt(0.75); rot[7*9+4]= -0.5;         rot[7*9+5]= 0;
  rot[7*9+6]= 0;           rot[7*9+7]= 0;            rot[7*9+8]= -1;

  rot[8*9+0]= -1; rot[8*9+1]= 0;  rot[8*9+2]= 0;
  rot[8*9+3]= 0;  rot[8*9+4]= 1;  rot[8*9+5]= 0;
  rot[8*9+6]= 0;  rot[8*9+7]= 0;  rot[8*9+8]= -1;

  rot[9*9+0]= -0.5;        rot[9*9+1]= -sqrt(0.75); rot[9*9+2]= 0;
  rot[9*9+3]= -sqrt(0.75); rot[9*9+4]= 0.5;         rot[9*9+5]= 0;
  rot[9*9+6]= 0;           rot[9*9+7]= 0;           rot[9*9+8]= -1;

  rot[10*9+0]= -0.5;        rot[10*9+1]= sqrt(0.75);  rot[10*9+2]= 0;
  rot[10*9+3]= sqrt(0.75);  rot[10*9+4]= 0.5;         rot[10*9+5]= 0;
  rot[10*9+6]= 0;           rot[10*9+7]= 0;           rot[10*9+8]= -1;

  rot[11*9+0]= 1;  rot[11*9+1]= 0;  rot[11*9+2]= 0;
  rot[11*9+3]= 0;  rot[11*9+4]= -1; rot[11*9+5]= 0;
  rot[11*9+6]= 0;  rot[11*9+7]= 0;  rot[11*9+8]= -1;

  double square=1.;
  double angle = 0;
  double frac = sqrt(1/3.);
  double g[9],uvw[3],uvw_final[3];
  double a,x,y,f,s;
  double Amat[9];
  Amat[0] = 0; Amat[1] = 1;         Amat[2] = 0;
  Amat[3] = 0; Amat[4] = -sqrt(3.); Amat[5] = 1;
  Amat[6] = 1; Amat[7] = 0;         Amat[8] = 0;
  double a0=1/sqrt(3.);
  double a1=1;
  double a2=1;

  for(j=0;j<12;j++){
    g[0] = U[0]*rot[j*9+0] + U[1]*rot[j*9+3] + U[2]*rot[j*9+6];
    g[1] = U[0]*rot[j*9+1] + U[1]*rot[j*9+4] + U[2]*rot[j*9+7];
    g[2] = U[0]*rot[j*9+2] + U[1]*rot[j*9+5] + U[2]*rot[j*9+8];
    g[3] = U[3]*rot[j*9+0] + U[4]*rot[j*9+3] + U[5]*rot[j*9+6];
    g[4] = U[3]*rot[j*9+1] + U[4]*rot[j*9+4] + U[5]*rot[j*9+7];
    g[5] = U[3]*rot[j*9+2] + U[4]*rot[j*9+5] + U[5]*rot[j*9+8];
    g[6] = U[6]*rot[j*9+0] + U[7]*rot[j*9+3] + U[8]*rot[j*9+6];
    g[7] = U[6]*rot[j*9+1] + U[7]*rot[j*9+4] + U[8]*rot[j*9+7];
    g[8] = U[6]*rot[j*9+2] + U[7]*rot[j*9+5] + U[8]*rot[j*9+8];

    a = acos((g[0]+g[4]+g[8]-1)*0.5);
    if (g[8]>0) {
      uvw[0] = g[6];  uvw[1] = g[7];  uvw[2] = g[8];
    }
    else {
      uvw[0] = -g[6];  uvw[1] = -g[7];  uvw[2] = -g[8];
    }
    x = uvw[0]/(1+uvw[2]);
    y = uvw[1]/(1+uvw[2]);
    f = y/x;
    s = x*x+y*y;
    if ((f<=frac)&&(s<=square)&&(x>=0)&&(y>=0)){
	angle = a;
	frac = f;
	square = s;
	uvw_final[0] = uvw[0];
	uvw_final[1] = uvw[1];
	uvw_final[2] = uvw[2];
    }
  }


  //normalise colors
  double exponent = 0.7;
  double axis[3];
  axis[0] = pow(Amat[0]*uvw_final[0]+Amat[3]*uvw_final[1]+Amat[6]*uvw_final[2],exponent);
  axis[1] = pow(Amat[1]*uvw_final[0]+Amat[4]*uvw_final[1]+Amat[7]*uvw_final[2],exponent);
  axis[2] = pow(Amat[2]*uvw_final[0]+Amat[5]*uvw_final[1]+Amat[8]*uvw_final[2],exponent);

  c[0] = axis[0]/(a2); // red
  c[1] = axis[1]/(a1); // green
  c[2] = axis[2]/(a0); // blue

  double mx=c[0];
  if (c[1] > mx) { mx = c[1]; }
  if (c[2] > mx) { mx = c[2]; }

  c[0] = c[0]/(mx); // red
  c[1] = c[1]/(mx); // green
  c[2] = c[2]/(mx); // blue

  return (SUCCESS);
}


static int convhkltocolor_orthorhombic(double *U, double *c){
  // converts hkl (U31,U32,U33) to colour in orthorhombic stereographic triangle

  double pi = 3.14159;

  double axis[3];
  axis[0] = fabs(U[6]);
  axis[1] = fabs(U[7]);
  axis[2] = fabs(U[8]);

  //normalise colors
  double exponent = 0.5;

  c[0] = pow(2*asin(axis[2])/pi,exponent); // red
  c[1] = pow(2*asin(axis[0])/pi,exponent); // green
  c[2] = pow(2*asin(axis[1])/pi,exponent); // blue

  return (SUCCESS);
}
