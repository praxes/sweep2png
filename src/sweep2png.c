#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "crystallography.h"
#include "sweeper.h"
#include "data_io.h"

static int read_mesh(SAMPLE *sample);
int write_pngs_color(SAMPLE *sample);
int write_pngs_confidence(SAMPLE *sample);

char pngfilestem[300];

int main(int argc, char *argv[]) {

  SAMPLE sample;
  float f1;

  if(argc!=5) {
    printf("Usage: sweep2png meshfile png_filestem min_completeness lattice\n");
    exit(1);
  }

  sprintf(sample.input_meshfile,"%s",argv[1]);
  sprintf(sample.outputfilestem,"%s",argv[2]);

  {
    sscanf(argv[3],"%g",&f1);
    sample.completeness_cut=(double)f1;
  }

  sprintf(sample.lattice,"%s",argv[4]);

  printf("mesh file: %s, png filestem: %s, min completeness: %f, lattice: %s\n",
	 sample.input_meshfile,
         sample.outputfilestem,
         sample.completeness_cut,
         sample.lattice);

  read_mesh(&sample);

  write_pngs_color(&sample);
  write_pngs_confidence(&sample);

  return (EXIT_SUCCESS);
}

static int read_mesh(SAMPLE *sample){

  int LINE_LEN=300;
  FILE *fp;
  char line[300];
  int i1,i2,i3,i4,i5;
  float f[16];
  long i,j,k;

  RMESH *rm=&(sample->rmesh);

  fp=fopen(sample->input_meshfile,"r");

  if(fp==NULL) {
    printf("Cannot open %s\n",sample->input_meshfile);
    exit(1);
  }

  for(k=0;k<3;k++) fgets (line, LINE_LEN, fp);

  sscanf(line,"%i %i %i %f %f %f %f %f %f",&i1,&i2,&i3,&f[0],&f[1],&f[2],&f[3],&f[4],&f[5]);

  rm->nvoxels=(long)i1*(long)i2*(long)i3;
  rm->nvx = i1;
  rm->nvy = i2;
  rm->nvz = i3;
  rm->xmin=(double)f[0];
  rm->xmax=(double)f[1];
  rm->ymin=(double)f[2];
  rm->ymax=(double)f[3];
  rm->zmin=(double)f[4];
  rm->zmax=(double)f[5];
  if(rm->nvz==0) {
    rm->nvoxels=(long)i1*(long)i2;
    rm->nvz = 1;
  }

  printf("nv %ld x=[%f,%f] y=[%f,%f] z=[%f,%f]\n",rm->nvoxels,rm->xmin,rm->xmax,rm->ymin,rm->ymax,rm->zmin,rm->zmax);

  if( (rm->voxel=(VOXEL *)calloc((size_t)(rm->nvoxels),(size_t)(sizeof(VOXEL))))==NULL ) {
    printf("Not enough memory to allocate rm->nvoxels\n");
    return (FAILURE);
  }

  for(k=0;k<6;k++) fgets (line, LINE_LEN, fp);

  for(i=0;i<rm->nvoxels;i++){
    VOXEL *v=&(rm->voxel[i]);

    fgets (line, LINE_LEN, fp);
    sscanf(line,"%i %i %i %i %f %f %f %i",&i1,&i2,&i3,&i4,&f[0],&f[1],&f[2],&i5);
    if(rm->zmin==rm->zmax) {
      sscanf(line,"%i %i %i %f %f %i",&i1,&i2,&i3,&f[0],&f[1],&i5);
      i4=0;
      f[2]=0;
    }

    v->id=(long)i1;
    v->ix=i2;
    v->iy=i3;
    v->iz=i4;
    v->nU=i5;

    v->x=(double)f[0];
    v->y=(double)f[1];
    v->z=(double)f[2];

    //    printf("xyz %i %i %i   %i   %f %f %f\n",v->ix,v->iy,v->iz,v->nU,v->x,v->y,v->z);

    if(v->nU>0){
      if( (v->orientation=(ORIENTATION *)calloc((size_t)(v->nU),(size_t)(sizeof(ORIENTATION))))==NULL ) {
	printf("Not enough memory to allocate v->orientation\n");
	return (FAILURE);
      }

      fgets (line, LINE_LEN, fp);

      for(j=0;j<v->nU;j++){
	ORIENTATION *or=&(v->orientation[j]);

	fgets (line, LINE_LEN, fp);
	sscanf(line,"%i %i %i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %i %i %i %f",
	       &i1,&i2,&i3,
	       &f[0],&f[1],&f[2],
	       &f[3],&f[4],&f[5],
	       &f[6],&f[7],&f[8],&f[9],&f[10],&f[11],&f[12],&f[13],&f[14],
	       &i3,&i4,&i5,
	       &f[15]);

	or->grain_id=j;

	for(k=0;k<3;k++) or->r[k]=f[k];
	for(k=0;k<9;k++) or->U[k]=f[6+k];

	or->ntested = i3;
	or->nmeasured = i4;
	or->nfailed = i5;
	or->completeness = f[15];

	//	printf("  %02ld: r %f %f %f, ntested %i nmeasured %i nfailed %i completeness %f\n",j,or->r[0],or->r[1],or->r[2],or->ntested,or->nmeasured,or->nfailed,or->completeness);

      }
      for(k=0;k<2;k++) fgets (line, LINE_LEN, fp);
    } else {
      for(k=0;k<3;k++) fgets (line, LINE_LEN, fp);
    }
  }

  fclose(fp);

  return (EXIT_SUCCESS);
}
