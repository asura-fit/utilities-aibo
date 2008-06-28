/*
  $Log: gengc2.cc,v $
  Revision 1.1  2002/06/19 12:30:39  ken
  new GC2 format introduced

  Revision 1.2  2002/03/27 18:41:05  ken
  The bug causes the screen fill with blue blob might be fixed. This was caused by threshold predicate bug. To reject a specified color, giving a reverse threshold which fulfills min > max condition for L or S will help you.
  Since tests not yet done, this must be done as soon as possible.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG 1
#define MAX_COLORS (100)


#define NUMOFPLANE 32

#define CUEBE_SIZE (NUMOFPLANE*256*256)


void LoadThresholds();
void LoadCuebe();
void SaveCuebe();

// For color detection
typedef struct ATSLThreshold {
  int R; // fill color R
  int G; // fill color G
  int B; // fill color B
  int T_max;
  int T_min;
  int S_max;
  int S_min;
  int L_max;
  int L_min;
} ATSLThreshold;

// global variables
int	   fNumOfColor;
ATSLThreshold fThreshs[MAX_COLORS];
unsigned char* fYUVCuebe;


void
LoadThresholds(const char* filename)
{
  fNumOfColor = 0;

  printf("LoadThresholds from %s\n",filename);
  FILE* fp = fopen( filename,"r" );
  if (fp == NULL) {
    printf(" fopen() error @ LoadThresholds()\n");
    exit(-1);
  }

  char linebuff[30];
  char* result;

  fscanf(fp,"%d",&fNumOfColor);
  if (fNumOfColor > MAX_COLORS) {
    printf("TSLVision::LoadThresholds() a number of colors is exceeded.\n");
    fNumOfColor = MAX_COLORS;
  }

  for(int i=0;i<fNumOfColor;i++) {
    fscanf(fp,"%d %d %d", &fThreshs[i].R, &fThreshs[i].G, &fThreshs[i].B);
    
    fscanf(fp,"%d %d", &fThreshs[i].T_min, &fThreshs[i].T_max);

    fscanf(fp,"%d %d", &fThreshs[i].S_min, &fThreshs[i].S_max);

    fscanf(fp,"%d %d", &fThreshs[i].L_min, &fThreshs[i].L_max);

    printf("COLOR %i ---------\n",i);
    printf("FILL (%3d,%3d,%3d)\n", fThreshs[i].R, fThreshs[i].G, fThreshs[i].B);
    printf("T    (%3d,%3d)\n",fThreshs[i].T_min, fThreshs[i].T_max);
    printf("S    (%3d,%3d)\n",fThreshs[i].S_min, fThreshs[i].S_max);
    printf("L    (%3d,%3d)\n",fThreshs[i].L_min, fThreshs[i].L_max);
    printf("------------------\n");

    if (fThreshs[i].S_min > fThreshs[i].S_max) {
      fprintf(stderr, "Illegal threshold S : max: %d < min: %d\n", fThreshs[i].S_max, fThreshs[i].S_min);
      fprintf(stderr, "COLOR %i will be rejected.\n");
    }

    if (fThreshs[i].L_min > fThreshs[i].L_max) {
      fprintf(stderr, "Illegal threshold L : max: %d < min: %d\n", fThreshs[i].L_max, fThreshs[i].L_min);
      fprintf(stderr, "COLOR %i will be rejected.\n");
    }
    
  }

  fclose(fp);
}



typedef unsigned char Ibyte;
typedef char byte;

/*
  This predicate for S and L.
  With giving a start > end threshold, you can reject its color.
*/
inline int IsWithin(int value,int start,int end) {
  return (start <= value) && (value <= end);
}

/*
  Since T is tint, start > end is valid whereas S and L is invalid.
  This predicate is for T.
*/
inline int IsWithinWrapped(int value, int start, int end) {
        return (start < end) ? (start <= value) && (value <= end) :
                               !((end <= value) && (value <= start));
}

inline int determinePlane(int val){
	int plane;
//	plane =  val*NUM_PLANES/256;
	if (val<40) plane = 0;
	else if (val<45) plane = 1;
	else if (val<50) plane = 2;
	else if (val<55) plane = 3;
	else if (val<60) plane = 4;
	else if (val<70) plane = 5;
	else if (val<80) plane = 6;
	else if (val<90) plane = 7;
	else if (val<100) plane = 8;
	else if (val<110) plane = 9;
	else if (val<130) plane = 10;
	else if (val<150) plane = 11;
	else if (val<180) plane = 12;
	else plane = 13;
	
	return plane;	
}


inline int determinePlane2(int val){
	return val / 8;	
}


void
generateGCI()
{
  double cons;
  double fdr,fdg;
  int t,s,l;
  int y,u,v;
  int n = 0;

  fYUVCuebe = (unsigned char*)malloc(CUEBE_SIZE);
  for(int i=0;i<NUMOFPLANE;i++) // YPLANES=14
    for(u=0;u<256;u++)
      for(v=0;v<256;v++)
	*(fYUVCuebe + i * 256 * 256 + u * 256  + v ) = 255;


  for(y=0;y<256;y++) {
    for(u=0;u<256;u++) {
      for(v=0;v<256;v++) {

 	cons = 4.3403*y+2*(u-128)+(v-128);
	fdr = ((-0.6697*(u-128))+(1.6959*(v-128)))/cons;
	fdg = ((-1.168*(u-128))-(1.3626*(v-128)))/cons;

	if (fdg > 0.0)
	  t = (int)((atan(fdr/fdg)/2.0/3.14159+1/4.0)*255);
	else if (fdg < 0.0)
	  t = (int)((atan(fdr/fdg)/2.0/3.14159+3/4.0)*255);
	else
	  t = 0;
	s = (int)(sqrt(9/5.0*((fdr*fdr)+(fdg*fdg)))*255.0);
	l = y;

	if (t <   0) t = 0;
	if (t > 255) t = 255;
	if (s <   0) s = 0;
	if (s > 255) s = 255;

	//if ( (y+u+v) % 60 == 0)
	  //  printf("(T,S,L) = (%3d,%3d,%3d)\n",t,s,l);

	int colorIndex = 0;

	for(int i=0;i<fNumOfColor;i++) {
	  // TODO: color should be classified with distance
	  if (IsWithinWrapped(t, fThreshs[i].T_min, fThreshs[i].T_max)
	      &&  IsWithin(s, fThreshs[i].S_min, fThreshs[i].S_max)
	      &&  IsWithin(l, fThreshs[i].L_min, fThreshs[i].L_max)
	      ) {

	    int yplane = determinePlane2(y);  // 

	    if (*(fYUVCuebe + yplane * 256*256 + u * 256 + v) != 255)
		break;
	    //	     *(fYUVCuebe + yplane * 256*256 + u * 256 + v) = 1 << i;
	    *(fYUVCuebe + yplane * 256*256 + u * 256 + v) = i;

	    // colorIndex = i;
	    //printf("%4d",colorIndex);  
	  }
	}
	//

      }
    }
  }
}



void
SaveGCI(const char* filename)
{
  FILE* fp;
  printf("Saving GC2 file:%s\n",filename);
  fp = fopen(filename,"w");
  if (fp == NULL) {
    fprintf(stderr,"Error file open: %s\n",filename);
    return;
  }

  if (fYUVCuebe != NULL) {
    int result = fwrite(fYUVCuebe,1,CUEBE_SIZE,fp);
    if (result < CUEBE_SIZE)
      fprintf(stderr,"Error file too short to write: %s\n",filename);
  } else
    fprintf(stderr,"No data to write\n");

  fclose(fp);
}


int
main(int argc, char* argv[])
{
  void* rgbData;
  int   width,height;

  printf("gengc2 version 2002/6/28 v.01\n");
  if (argc <= 2)
	  printf("gengc2 <color.lst> <MAUTO.GC2>\n");
 
  if (argc == 1) 
	LoadThresholds("color.lst");
  else
	LoadThresholds(argv[1]);

  printf("Calculating Generalized Color Index\n");
  generateGCI();

  if (argc <= 2)
        SaveGCI("MAUTO.GC2");
  else
	SaveGCI(argv[2]);
}

