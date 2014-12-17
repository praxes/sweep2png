#ifndef _CRYSTALLOGRAPHY_
#define _CRYSTALLOGRAPHY_

typedef struct {
  int included;
  double LBGhkl;
  unsigned int nBGhkl;
  double *BGhkl;
  int *hkl;
  double C;   /* constant for use in findomega */
  double TTh;
  double tantth;
  double sintth;
  double costth;
  double length;
  double Fhkl2;
  double sintth_div_Fhkl2;
} THEOSCATVEC;

typedef struct { /* one for each {hkl} */
  int hklfam_index2;
  int n_generators;
  double *invgenerator;
} PAIR;

typedef struct {
  double alpha;
  double cosalpha;
  int hklfam_index1;
  int n_pairs;
  PAIR *pair;
} HKLCLASS;

typedef struct {
  HKLCLASS *hklclass; /* array: cosalpha pairs for the i'th {hkl} family */
} COSALPHA;

typedef struct {
  double alpha;
  double beta;
  double gamma;
  double a;
  double b;
  double c;
  double lambda;
  double ThetaMin;
  double ThetaMax;
  int SpGrpId;
  long maxhkl;
  long nhklfam;
  THEOSCATVEC *theoscatvec;
  COSALPHA *cosalpha; /* array: pointer to the first cosalpha pair for i'th {hkl} family */
  int *ncosalpha;  /* array: number of cosalpha pairs for i'th {hkl} family */
  int nEqMat;
  double *EqMat;
  int nrefl_total;

  int nFhkl2;
  double *Fhkl2;

} CRYSTSTRUCT;

#endif



