#ifndef _SWEEPER_
#define _SWEEPER_

#define FAILURE -1
#define SUCCESS  1

enum sweep_modes { SINGLEPOINT, MAPPING };
enum subarea_modes { BOX, CIRCLE };

typedef struct {
  int n_exp;
  int n_meas;
  double comp;
  double x[3];
  double phi1;
  double phi;
  double phi2;
  double r[3];
  double U[9];
  double meanIA;
} RGRAIN;

typedef struct {
  int active;
  unsigned short *pixellist;
  int bitsperpixel;
  int imin;

  unsigned int ny; /* number of pixels in y */
  unsigned int nz; /* number of pixels in z */
  double psy;      /* pixel size in y */
  double psz;      /* pixel size in z */
  double min_omega;
  double max_omega;
  double ymin;     /* detector AOI */
  double ymax;
  double zmin;
  double zmax;
  double x;
  double y;
  double z;
  double R[9];
  double invR[9];

  int z_dead_min; /* defines inactive areas on detector, e.g. gab on quantix */
  int z_dead_max;
} IMAGE;

typedef struct {
  char filenpath[500];
  int n_images;
  int ny;
  int nz;
  double psy;
  double psz;
  double x;
  double y;
  double z;
  double phix;
  double phiy;
  double phiz;
  double R[9];
  double invR[9];
  IMAGE *image;
  int bitsperpixel;
  int imin;
  int z_dead_min;
  int z_dead_max;
  double y0;
  double z0;
  double Rmin;
  double Rmax;
  double ymin;     /* detector AOI */
  double ymax;
  double zmin;
  double zmax;

  char stem[500];

  int o[4];

  int use_bgimage;
  char bg_filename[500];
  char ftype[500];

} SCREEN;

typedef struct {
  double x;
  double y;
  double z;
  double phix;
  double phiy;
  double phiz;
  double R[9];
  double invR[9];
} DETECTORPOS;

typedef struct {
  long grain_id;  // grain id
  int method; // which indexing method
  double U[9];
  double r[3];
  double min_phi1;
  double min_phi;
  double min_phi2;
  double max_phi1;
  double max_phi;
  double max_phi2;
  double phi1;
  double phi;
  double phi2;
  int n_measurements;
  int ntested;
  int nmeasured;
  int nnotmeasured;
  int naccepted;
  int nfailed;
  int *tslist;
  int ntheovec;
  double completeness;
} ORIENTATION;

typedef struct {
  long id; // voxel id
  int ix;
  int iy;
  int iz;
  double x;
  double y;
  double z;
  int nU;
  ORIENTATION *orientation;
  int neigbour_id[4];
  int n_neigbours;
  double delta;
  int processed;
  long nexts[27]; // closest voxels
  int nnexts;
  long label; // grain search id
} VOXEL;

typedef struct {
  int align;
  double xmin;
  double xmax;
  int nstepx;
  double ymin;
  double ymax;
  int nstepy;
  double zmin;
  double zmax;
  int nstepz;
} ALIGNMENT;

typedef struct {
  long id;
  long nvoxels;
  long *voxel_id;
  double *density;
  int ntested;
  int nmeasured;
  int nfailed;
  double r[3];
  double phi1;
  double phi;
  double phi2;
  double xcm;
  double ycm;
  double dcmsx;
  double dcmsy;
  int n_good_ref;
  double D_good_ref;
  double median_volume;
  double ar; /* area ratio */
  double density_cut;
  double lowest_density_cut;
  int best_solution;
} GRAIN;


typedef struct {
  long nvoxels;
  VOXEL *voxel;
  int nvx;
  int nvy;
  int nvz;
  double vsx;
  double vsy;
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  double zmin;
  double zmax;
} RMESH;

typedef struct {
  /* voxel settings, sample planes */
  long nvoxels;
  VOXEL *voxels;
  RMESH rmesh;
  int nvx;
  int nvy;
  int nvz;
  double vsx;
  double vsy;
  double vsz;
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  double zmin;
  double zmax;
  double z; /* temporary */
  /* crystallography */
  int nhklfam;
  /* eta settings */
  double min_eta;
  double max_eta;
  double deta;
  /* cuts in eta */
  double pole_angle;
  /* omega settings */
  int omega_nintervals;
  double min_omega[2];
  double max_omega[2];
  int filenr_beg[2];
  int filenr_end[2];
  double domega;
  double lowest_omega[2];
  double highest_omega[2];
  int nstepomega;
  int *activestate;
  int *imagenumbers;
  /* detector settings */
  DETECTORPOS *detectorpos;
  /* image settings */
  int startimg;
  int endimg;
  int stepimg;
  /* filenames */
  char inputdir[300];
  char outputdir[300];
  char outputfilestem[300];
  char inputmapname[300];
  char rgrainsfile[300];
  char gfffile[300];
  char input_2Dmeshfile[300];
  char input_meshfile[300];
  /* modes */
  int singledetectorpos;
  int imagecor;
  int img_flip_in_y;
  int img_flip_in_z;
  int findmedian;
  int dobacksub;
  char backgimg[300];
  /* completeness cuts */
  int maxmissing;
  int min_measured;
  /* sample grid */
  long n_voxels_in_list;
  long *voxel_id_list;
  long n_voxels_in_old_list;
  long *old_voxel_id_list;
  /* misc */
  double maxmisori;
  /* final map */
  int finalizemap;
  double final_xmin;
  double final_xmax;
  long final_nvx;
  double final_ymin;
  double final_ymax;
  long final_nvy;

  int ngrains;
  GRAIN *grain;

  int mode; /* Single point or mapping */

  double max_r;
  double x0;
  double y0;
  double z0;

  double density_cut;
  double lowest_density_cut;
  double max_Ldist;

  double dx_fit_connect;
  double dy_fit_connect;

  int n_solid_refs;

  char sp_filename[200];

  double rel_volume_dev;

  double completeness_cut;
  double refined_completeness_cut;

  int min_maxR;
  double R_frac;

  double single_point_x;
  double single_point_y;

  int z_dead_min; /* defines inactive areas on detector, e.g. gab on quantix */
  int z_dead_max;

  int subarea_mode;

  int Rodrigues_N;
  double R_length;

  double max_pixel_dev_y;
  double max_pixel_dev_z;

  int omega_right_hand;

  int use_spline;
  int *dist_slow;
  int *dist_fast;
  int n_rows;
  int n_cols;

  int nscreens;
  SCREEN *screen;

  int bitsperpixel;
  int imin;

  int nthreads;

  int n_grains_read;
  RGRAIN *rgrain;
  int runmode; //flag

  int Mode_2D;
  int Mode_3D;
  int jump_n_voxels;

  int n_images_stacked;

} SAMPLE;

typedef struct {
  double omega;
  double domega;
  double min_omega;
  double max_omega;
  double eta;
  double deta;
  double min_eta;
  double max_eta;
  double next_eta;
  double tth;
  double tantth;
  double sintth;
  double costth;
  double v[3];
  double length;
  double cosalpha;
  int hklfam_index;
  int theo_scatvec_index_i;
  int theo_scatvec_index_j;
  double C;
} SCATVEC;

typedef struct {
  double min_omega;
  double max_omega;
  int image_index;
} OMEGABIN;

typedef struct {
  double min_eta;
  double max_eta;
} ETARANGE;

typedef struct {
  double eta;
  double omega;
  double sineta;
  double coseta;
  double sinomega;
  double cosomega;

  IMAGE *img;
  int maxvalue;
  long maxvalue_voxel_id;

  /* for use in connectivety measurements */
  int y0;
  int z0;
  int img0;
  int I0;

  /* from connectivity measurements */
  double ycm; /* in the detector frame */
  double zcm;
  double imgcm;
  double omegacm;
  int min_img_nr;
  int max_img_nr;
  long Itot;
  long npixels;

  double xd; /* in units mm */
  double yd;
  double zd;
  /* end connectivity */

  THEOSCATVEC *th;
  double *BGhkl;
  double LBGhkl;
  int *hkl;

  double x; /* cms in sample */
  double y;

  /* predicted position in detector system */
  double y_ccd_predicted;
  double z_ccd_predicted;

  double slope_x;
  double slope_y;

  double dist;

  int outlier;
  int tmp_use;

  int useit;

  int imin;
  double volume;
  double App; /* projected pixel area in sample plane */
  double Ldist; /* area of quandrant with sides of length cord */

  int in_median_range;
} SIGPOS;


typedef struct {
  VOXEL *voxel;
  SCATVEC scatvec[3];
  int active_scatvec;
  int image_index;
  int image_index_sfs;
  int n_omega_bins;
  int omega_bin_index;
  OMEGABIN *omega_bin;
  int n_eta_ranges;
  int eta_range_index;
  ETARANGE eta_range[2];
  int cosalpha_index;
  HKLCLASS *hklclass;
  int pair_index;
  PAIR *pair;
  int generator_index;
  int sweepstate;

  int *Rodrigues;
  long Rodrigues_N;
  unsigned int label;
  double R_length;

  int ntested;
  int nmeasured;
  int nfailed;

  int max_ntested;
  int max_nmeasured;
  int max_nfailed;
  double max_completeness;
  double max_r[3];

  double max_xcm;
  double max_ycm;
  double max_zcm;
  int maxrri[3];

  long maxRi;

  int n_sigpos;
  SIGPOS *sigpos;

  double chisq;

  int tmp_n_sigpos;
  SIGPOS *tmp_sigpos;

  int store_tsigpos;

  int n_voxels_assigned;

  double xcm;
  double ycm;
  int n_in_cm;

  int assign_orientation;

  int writetomap;

  int voxel_accepted;
  double value;

  double density_cut;
  double lowest_density_cut;

  int n_good_ref;
  double D_good_ref;
  double median_volume;

  double maxdy; /* max deviation in y (pixels) */
  double maxdz; /* max deviation in z (pixels) */

  int tottrials;
} ACTIVEVOXEL;

typedef struct {
  long nvoxels;
  long *voxel_id;
} VLIST;


typedef struct {

  int imageid;

  SCATVEC scatvec;

  int *Rodrigues_tmp;
  long label;

} PSTRUCT;



#endif







