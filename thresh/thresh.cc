/* LICENSE:
  =========================================================================
    CMPack'02 Source Code Release for OPEN-R SDK v1.0
    Copyright (C) 2002 Multirobot Lab [Project Head: Manuela Veloso]
    School of Computer Science, Carnegie Mellon University
  -------------------------------------------------------------------------
    This software is distributed under the GNU General Public License,
    version 2.  If you do not have a copy of this licence, visit
    www.gnu.org, or write: Free Software Foundation, 59 Temple Place,
    Suite 330 Boston, MA 02111-1307 USA.  This program is distributed
    in the hope that it will be useful, but WITHOUT ANY WARRANTY,
    including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  -------------------------------------------------------------------------
    Additionally licensed to Sony Corporation under the following terms:

    This software is provided by the copyright holders AS IS and any
    express or implied warranties, including, but not limited to, the
    implied warranties of merchantability and fitness for a particular
    purpose are disclaimed.  In no event shall authors be liable for
    any direct, indirect, incidental, special, exemplary, or consequential
    damages (including, but not limited to, procurement of substitute
    goods or services; loss of use, data, or profits; or business
    interruption) however caused and on any theory of liability, whether
    in contract, strict liability, or tort (including negligence or
    otherwise) arising in any way out of the use of this software, even if
    advised of the possibility of such damage.
  ========================================================================= */

#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>
#include <string>

#include "util.h"

#include "image2.h"
#include "learn.h"
#include "lprintf.h"

/*
#define Y_VAR 8
#define U_VAR 8
#define V_VAR 8
#define OVERSAMP 16
*/

std::vector<std::string> comments;
const char* const VERSION_STRING = "version 2005.06.05";
const char* const FORMAT_IDENTIFIER = "TMAP";
const char* const FORMAT_VERSION = "3";

static bool isCreateCache = false;
static bool isCreateTMAP = false;
static bool isCreateResult = false;
static CameraInfo info1;

void randomize()
{
  timeval tv;
  gettimeofday(&tv,NULL);
  srand48(tv.tv_sec ^ tv.tv_usec);
}

//#define TEST_MISCLASSIFICATION_ERROR

#ifdef TEST_MISCLASSIFICATION_ERROR
static double random_class_prob=0.05;
#endif

example *make_list(simage2 &img,simage &label,color_info *colors,int num)
{
  static bool ignore, first=true;

  int i,c,size;
  example *p,*list;
  rgb r,r2;
  CameraInfo info;

  int num_added,num_ignored,num_unknown;

  if(!img.samesize(label)) return(NULL);

  img.GetCameraInfo(info);
  if (first) {
	info1 = info;
	ignore = (info1.version_str == "" || info1.settings_str == "");
	first = false;
  } else if (!ignore && info != info1) {
	lprintf("  Error: HeaderVersion or CameraSetting Setting is different to first one!!");
	return(NULL);
  }

  lprintf("  Adding %dx%d image (%s)",img.w,img.h,info1.settings_str.c_str());

  size = img.w * img.h;
  list = NULL;

  num_added = num_ignored = num_unknown = 0;
  c = 0;
  r2 = colors[0].rcolor;

  for(i=0; i<size; i++){
    // find color class
    r = label.img[i];
    if(r2 != r){
      c=0;
      while((c < num) && colors[c].rcolor!=r) c++;
      r2 = r;
    }

    // add if a valid class was found
    if(c < num){
#ifdef TEST_MISCLASSIFICATION_ERROR
      if(drand48() < random_class_prob)
        c = lrand48() % num;
#endif

      r = img.img[i];
      if(r.red!=r.green || r.green!=r.blue || r.red==255){
	p = new example;
	if(p){
	  /*
	  p->y = bound(r.red   + (lrand48() % (Y_VAR*2+1) - Y_VAR),0,255);
	  p->u = bound(r.green + (lrand48() % (U_VAR*2+1) - U_VAR),0,255);
	  p->v = bound(r.blue  + (lrand48() % (V_VAR*2+1) - V_VAR),0,255);
	  */
	  p->y = r.red;
	  p->u = r.green;
	  p->v = r.blue;
	  p->label = c;

	  p->next = list;
	  list = p;
	  num_added++;
	}
      }else{
	// ignore garbage bits at bottom of image
	num_ignored++;
      }
    }else{
      if(r.red==r.green && r.green==r.blue && r.red<=192 && r.red>=64){
	num_ignored++;
      }else{
	num_unknown++;
	if(num_unknown < 100){
	  if(num_unknown == 1) lprintf("  Unknown label(s) at ");
	  lprintf("(%d,%d) ",i%img.w,i/img.w);
	}
      }
    }
  }

  if(num_unknown > 0) printf("\n");

  lprintf("  %d added, %d ignored, %d errors\n",num_added,num_ignored,num_unknown);

  return(list);
}

example *make_list(char *image_file,char *label_file,color_info *colors,int num)
{
  simage2 img;
  simage label;

  lprintf("  Image='%s' Labels='%s'\n",image_file,label_file);

  if (!img.load(image_file)) {
    fprintf(stderr, "  Warning : Load YUV Image Failed!!! '%s' Skipped\n", image_file);
    return NULL;
  }
  if (!label.load(label_file)) {
    fprintf(stderr, "  Warning : Load Label Image Failed!!! '%s' Skipped\n", label_file);
    return NULL;
  }

  return(make_list(img,label,colors,num));
}

void classify(thresh &tlearn,
	      simage2 &image,simage2 &output,
	      color_info *colors,int num)
{
  int i,size;
  rgb r;

  if(!image.samesize(output)) return;

  size = image.w * image.h;

  for(i=0; i<size; i++){
    int symb_color;

    r = image.img[i];
    symb_color = tlearn.classify(r.red,r.green,r.blue);
    if(symb_color < COLORS)
      output.img[i] = colors[symb_color].rcolor;
    else
      output.img[i].red = output.img[i].green = output.img[i].blue = 0;
  }
}

void classify(thresh &tlearn,
	      char *image_file,char *output_file,
	      color_info *colors,int num)
{
  simage2 image,label,out;

  lprintf("  Test Image='%s' Output='%s'\n",image_file,output_file);

  image.load(image_file);
  out.copy(image);

  classify(tlearn,image,out,colors,num);
  out.save(output_file);
}

#define Y_LEV 16
#define U_LEV 64
#define V_LEV 64

rgb black    = {  0,  0,  0};
rgb darkgray = { 64, 64, 64};

bool find_weight_range(double &min_weight, double &max_weight, int color_idx, int size, double *wmap)
{
  bool found_one=false;

  min_weight=+HUGE_VAL;
  max_weight=0.0;

  for(int i=0; i<size; i++) {
    double weight=wmap[i*COLORS+color_idx];

    if(weight!=0.0)
      found_one=true;

    min_weight = min(min_weight, weight);
    max_weight = max(max_weight, weight);
  }

  return found_one;
}

// scale the weight for display
// returns a value in [0.0 - 1.0]
double scaleWeight(double weight, double min_weight, double max_weight) {
  if(weight < min_weight)
    return 0.0;

  double min_log = log(min_weight);
  double max_log = log(max_weight);

  double weight_log = log(weight);

  return (weight_log - min_log) / (max_log - min_log);
}

static const double black_frac=1e-200;

void dump_weightmaps(thresh &thr,char *out_prefix,double *wmap,int size, int num_colors)
{
  double max_weight=0.0,min_weight=+HUGE_VAL;
  double color_maxw,color_minw;

  for(int color_idx=0; color_idx<COLORS; color_idx++) {
    bool has_data;
    has_data = find_weight_range(color_minw,color_maxw,color_idx,size,wmap);

    if(has_data) {
      min_weight = min(min_weight, color_minw);
      max_weight = max(max_weight, color_maxw);
    }
  }

  min_weight = max(min_weight, black_frac * max_weight);

  for(int color_idx=0; color_idx<num_colors; color_idx++) {
    simage2 out,level;
    static char out_file[1024];
    
    sprintf(out_file,"%s%d.ppm",out_prefix,color_idx);

    level.allocate(U_LEV,V_LEV);
    out.allocate(U_LEV*4,V_LEV*Y_LEV/4);

    for(int y_idx=0; y_idx<Y_LEV; y_idx++) {
      for(int u_idx=0; u_idx<U_LEV; u_idx++) {
        for(int v_idx=0; v_idx<V_LEV; v_idx++) {
          int box_idx = thr.box_loc(y_idx,u_idx,v_idx);

          double val;

          val = scaleWeight(wmap[box_idx*COLORS+color_idx],min_weight,max_weight);
          rgb *out_pix=&level.img[u_idx*V_LEV + v_idx];
          out_pix->red = out_pix->green = out_pix->blue = (int)(val*255.999);

          //printf("color %d, idx %d, full idx %d, weight %g, scaled %g, val %d\n",
          //       color_idx,box_idx,box_idx*COLORS+color_idx,
          //       wmap[box_idx*COLORS+color_idx],val,(int)(val*255.999));
        }
      }

      out.blit(level,U_LEV*(y_idx%4),V_LEV*(y_idx/4));
    }
    
    out.save(out_file);
  }
}

void dump_color(thresh &thr,char *out_file,double *wmap,uchar *tmap,int size, int num_colors, color_info *colors)
{
  simage2 out,level;
  level.allocate(U_LEV,V_LEV);
  out.allocate(U_LEV*4,V_LEV*Y_LEV/4);

  for(int y_idx=0; y_idx<Y_LEV; y_idx++) {
    for(int u_idx=0; u_idx<U_LEV; u_idx++) {
      for(int v_idx=0; v_idx<V_LEV; v_idx++) {
        int box_idx = thr.box_loc(y_idx,u_idx,v_idx);

        double r,g,b;
        double total_weight;

        r = g = b = 0.0;
        total_weight = 0.0;
        
        for(int color_idx=0; color_idx<num_colors; color_idx++) {
          double val;
          
          val = wmap[box_idx*COLORS+color_idx];
          rgb src_color=colors[color_idx].rcolor;
          r += val * src_color.red;
          g += val * src_color.green;
          b += val * src_color.blue;

          total_weight += val;

          //printf("y %2d u %2d v %2d c %2d, w %10g, tw %10g, rgb'=(%10g,%10g,%10g)\n",
          //       y_idx,u_idx,v_idx,color_idx,val,total_weight,
          //       (total_weight!=0.0) ? (r/total_weight) : 0x80*1.0, 
          //       (total_weight!=0.0) ? (g/total_weight) : 0x00*1.0, 
          //       (total_weight!=0.0) ? (b/total_weight) : 0x80*1.0);
        }

        rgb *out_pix=&level.img[u_idx*V_LEV + v_idx];
        if(total_weight > 0.0) {
          out_pix->red   = (int)rint(r/total_weight);
          out_pix->green = (int)rint(g/total_weight);
          out_pix->blue  = (int)rint(b/total_weight);
        }
        else {
          out_pix->red   = 0x80;
          out_pix->green = 0x00;
          out_pix->blue  = 0x80;
        }
      }
    }

    out.blit(level,U_LEV*(y_idx%4),V_LEV*(y_idx/4));
  }
    
  out.save(out_file);
}

void dump_threshold(char *out_file,uchar *tmap,int size,color_info *colors)
{
  simage2 out,level;
  int l,i,c;

  level.allocate(U_LEV,V_LEV);
  out.allocate(U_LEV*4,V_LEV*Y_LEV/4);

  for(l=0; l<Y_LEV; l++){
    for(i=0; i<U_LEV*V_LEV; i++){
      c = tmap[l*U_LEV*V_LEV + i];
      
      level.img[i] = (c == 255)? darkgray : colors[c].rcolor;
    }

    out.blit(level,U_LEV*(l%4),V_LEV*(l/4));
  }

  out.save(out_file);
}

void save_threshold(char *out_file,uchar *tmap,int size_y,int size_u,int size_v)
{
  FILE *out;
  int size,wrote;

  out = fopen(out_file,"wb");
  if(!out)
    goto error;

  fprintf(out,"TMAP\n");

  fprintf(out, "## Generator    : thresh [%s]\n", VERSION_STRING);
  fprintf(out, "## Format       : %s [version %s]\n", FORMAT_IDENTIFIER, FORMAT_VERSION);
  {
    time_t t = time(NULL);
    tm* ptm = localtime(&t);    
    char buf[256];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%m:%S JST", ptm);
    fprintf(out, "## TimeStamp    : %s\n", buf);
  }
  fprintf(out, "## CameraVersion : %s\n", info1.version_str.c_str());
  fprintf(out, "## CameraSettings : %s\n", info1.settings_str.c_str());

  for (std::vector<std::string>::const_iterator citer = comments.begin(); citer != comments.end(); citer++) {
    fprintf(out,"# %s", (*citer).c_str());
  }
  
  fprintf(out,"YUV8\n%d %d %d\n",
          size_y,size_u,size_v);
  size = size_y * size_u * size_v;
  wrote = fwrite(tmap,sizeof(uchar),size,out);
  fclose(out);

  if(wrote != size)
    goto error;

  return;

 error:
  fprintf(stderr, "error saving tmap file\n");
}

#define MAXBUF 512

bool find_label(FILE *in,char *key)
// search forward in file until a line matching <key> is found
{
  char buf[MAXBUF];
  while(fgets(buf,MAXBUF,in)){
    if(buf[0] != '#'){
      buf[strlen(buf)-1] = '\0';
      printf("Scan: %s  \r",buf);
      if(strstr(buf,key)) {
	printf("\n");
	return(true);
      }
    }
  }
  printf("\n");
  return(false);
}

char *get_quote_string(char *dest,int len,char *src)
{
  char ch;
  int i,l;

  if(!src) return(NULL);

  i = l = 0;

  while((ch=src[i]) && ch!='"') i++;
  i++;

  while((ch=src[i]) && ch!='"' && l<len-1){
    dest[l] = src[i];
    i++;
    l++;
  }
  dest[l] = 0;

  while((ch=src[i]) && ch!='"') i++;
  if(ch=='"') i++;

  return(src + i);
}

color_info test_colors[COLORS] = {
  {"Background", {  0,  0,  0}, 0.5, 0.5,},
  {"Orange",     {255,128,  0}, 1.0, 0.9,},
  {"Green",      {  0,255,  0}, 1.0, 0.5,},
  {"Pink",       {255,  0,128}, 1.0, 0.5,},
  {"Cyan",       {  0,255,255}, 1.0, 0.5,},
  {"Yellow",     {255,255,  0}, 1.0, 0.5,},
  {"DarkBlue",   {  0,  0,255}, 2.0, 0.5,},
  {"Red",        {255,  0,  0}, 2.0, 0.5,},
  {"White",      {255,255,255}, 1.0, 0.5,},
  {"Error",      {255,  0,255}, 1.0, 0.5,}
};

void process(thresh &tlearn,char *file)
{
  int NUM;
  char buf[MAXBUF];
  char file1[MAXBUF];
  char file2[MAXBUF];
  timeval tv1,tv2;
  char *str;
  FILE *in;
  example *list;

  in = fopen(file,"r");
  if(!in) return;

  if (isCreateTMAP) {
    //************
    // HEADER
    //************
    lprintf("HEADER:\n");
    if (find_label(in,"header")) {
      while(fgets(buf,MAXBUF,in) && buf[0]!='}'){
	printf("# %s", buf);
	comments.push_back(std::string(buf));
	if (strncmp(buf, "Version", sizeof("Version")-1) == 0) {
		fprintf(stderr, " Error: Version header requires thresh2!!!!\n");
		exit(-1);
	}
      }
    } else {
      fprintf(stderr, "%s\n",   "  Warning : Can't find header label. Skipped");
      fseek(in, 0, SEEK_SET);
    }
  }

  //************
  // ColorTable
  //************
  lprintf("COLORS:\n");
  find_label(in,"colors");
  int color_idx=0;
  while(fgets(buf,MAXBUF,in) && buf[0]!='}'){
    if(buf[0] != '#'){
      char *loc=buf;

      char name[MAXBUF];
      int r,g,b;
      double weight,conf;

      loc = get_quote_string(name,MAXBUF,buf);

      if(sscanf(loc," , ( %d , %d , %d ) , %lg , %lg",&r,&g,&b,&weight,&conf)!=5) {
        fprintf(stderr,"  error reading end of line '%s'\n",buf);
      }
      
      test_colors[color_idx].name=strdup(name);
      test_colors[color_idx].rcolor.red  =r;
      test_colors[color_idx].rcolor.green=g;
      test_colors[color_idx].rcolor.blue =b;
      test_colors[color_idx].weight=weight;
      test_colors[color_idx].confidence=conf;
      color_idx++;
    }
  }

  NUM=color_idx;

  //************
  // LOAD
  //************
  lprintf("LOADING:\n");
  find_label(in,"train");
  while(fgets(buf,MAXBUF,in) && buf[0]!='}'){
	if(buf[0] != '#'){
	  str = buf;
	  file1[0] = file2[0] = 0;
	  str = get_quote_string(file1,MAXBUF,str);
	  str = get_quote_string(file2,MAXBUF,str);

	  list = make_list(file1,file2,test_colors,NUM);
	  if (list) {
		tlearn.addList(list);
	  }
	}
  }

  //************
  // LEARNING
  //************
  lprintf("LEARNING:\n");
  fflush(stdout);

  gettimeofday(&tv1,NULL);
  tlearn.learnMap(test_colors,NUM);
  gettimeofday(&tv2,NULL);
  printf("Elapsed time: %fsec\n",(tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec)/1E6);

  if (isCreateResult) {
    //************
    // TEST
    //************
    lprintf("TESTING:\n");
    find_label(in,"test");
    while(fgets(buf,MAXBUF,in) && buf[0]!='}'){
      if(buf[0] != '#'){
	str = buf;
	file1[0] = file2[0] = 0;
	str = get_quote_string(file1,MAXBUF,str);
	str = get_quote_string(file2,MAXBUF,str);
	if(file1[0] && file2[0]){
	  classify(tlearn,file1,file2,test_colors,NUM);
	}
      }
    }
  }

  //************
  // SAVE
  //************
  lprintf("SAVING:\n");
  fflush(stdout);
  dump_weightmaps(tlearn, "weight", tlearn.wmap, Y_LEV*U_LEV*V_LEV, NUM);
  dump_color(tlearn, "color.ppm", tlearn.wmap, tlearn.tmap, Y_LEV*U_LEV*V_LEV, NUM, test_colors);
  dump_threshold("out.ppm", tlearn.tmap, Y_LEV*U_LEV*V_LEV, test_colors);
  if (isCreateTMAP) {
    save_threshold("thresh.tm2", tlearn.tmap, Y_LEV, U_LEV, V_LEV);
  }

}

int main(int argc,char **argv)
{
  //      A234567890B234567890C234567890D234567890E234567890
  printf("--------------------------------------------------\n"
	 "     thresh                                       \n"
	 "%45s\n"
	 "--------------------------------------------------\n\n"
	 , VERSION_STRING);
  if(argc < 2) {
    printf("Usage: thresh colors.txt [-c] [-t] [-r]\n"
	   "-c : learn & create cache.dat\n"
	   "-t : create tmap from cache.dat\n"
	   "-r : test & create result files\n");
    return(1);
  }
  {  
    // Check File Exists
    FILE* fp;
    fp = fopen(argv[1], "rt");
    if (!fp) {
      fprintf(stderr, "Error: ``%s'' can't open. exit\n");
      return -1;
    } else {
      fclose(fp);
    }
  }

  for (int i = 2; i < argc; i++) {
    if (0 == strcmp(argv[i], "-c")) {
      isCreateCache = true;
    } else if (0 == strcmp(argv[i], "-t")) {
      isCreateTMAP = true;
    } else if (0 == strcmp(argv[i], "-r")) {
      isCreateResult = true;
    } else {
      printf("Unrecognized option ``%s''\n", argv[i]);
      return -1;
    }
  }
  if (!isCreateCache && !isCreateTMAP && !isCreateResult) {
    isCreateCache = isCreateTMAP = isCreateResult = true;
  }

  if (!isCreateCache) {
    FILE* fp;
    fp = fopen("cache.dat", "rb");
    if (!fp) {
      fprintf(stderr, "Warning: You need to create cache. Force create cache!!\n");
      isCreateCache = true;
    } else {
      fclose(fp);
    }
  }

  thresh tlearn;

  lprintf_open("thresh.log");

  tlearn.init(Y_LEV,U_LEV,V_LEV);
  process(tlearn,argv[1]);

  lprintf_close();

  return(0);
}
