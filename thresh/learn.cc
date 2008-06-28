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

#include <math.h>
#include <stdio.h>

#include "learn.h"

#define NUM_PROP 9

static const int PROPUMVM=0;
static const int PROPUMVS=1;
static const int PROPUMVP=2;
static const int PROPUSVM=3;
static const int PROPUSVS=4;
static const int PROPUSVP=5;
static const int PROPUPVM=6;
static const int PROPUPVS=7;
static const int PROPUPVP=8;

bool thresh::init(int sy,int su,int sv)
{
  int i;

  delete(ltable);
  delete(tmap);

  size = sy * su * sv;

  ltable = new example*[size];
  tmap   = new uchar[size];
  wmap   = new double[size*COLORS];

  if(!ltable || !tmap){
    delete(ltable);
    delete(tmap);
    delete[] wmap;
    return(false);
  }

  size_y = sy;
  size_u = su;
  size_v = sv;

  div_y = LEVELS / sy;
  div_u = LEVELS / su;
  div_v = LEVELS / sv;

  for(i=0; i<size; i++){
    ltable[i] = NULL;
    tmap[i] = 0;
  }

  delete[] count;
  
  count = new ulong[COLORS];

  for(int color_idx=0; color_idx<COLORS; color_idx++) {
    count[color_idx]=0UL;
  }
  return true;
}

void thresh::clear()
{
  delete(ltable);
  delete(tmap);
  delete[] wmap;

  ltable = NULL;
  tmap = NULL;

  size = 0;
  size_y = size_u = size_v = 0;
  div_y = div_u = div_v = 0;

  for(int color_idx=0; color_idx<COLORS; color_idx++)
    count[color_idx]=0UL;
}

void thresh::addList(example *e)
{
  example *p,*pn;
  int i;

  p = e;
  while(p){
    pn = p->next;

    count[p->label]++;

    i = loc(p->y,p->u,p->v);
    p->next = ltable[i];
    ltable[i] = p;

    // if(p->label >= 2) printf("%d %d %d %d->%d\n",p->y,p->u,p->v,p->label,i);

    p = pn;
  }
}

#define GAMMA_Y 0.5
#define GAMMA_U 0.5
#define GAMMA_V 0.5

double thresh::calcWeightTwoSide(double gamma, int mind, int maxd) {
  double weight=1.0;
  
  weight += gamma * (1 - pow(gamma,-mind)) / (1 - gamma);
  weight += gamma * (1 - pow(gamma, maxd)) / (1 - gamma);

  return weight;
}

// the weight for this cube due to an example in this cube
double thresh::calcWeightOneHere(uchar ymin, uchar umin, uchar vmin, example *ex, 
                                 WeightProp &wprop, int map_idx, int color_idx) {
  uchar ymax = ymin + div_y - 1;
  uchar umax = umin + div_u - 1;
  uchar vmax = vmin + div_v - 1;

  double weight=1.0;
  double wum,wus,wup; // weight u minus/sum/plus
  double wvm,wvs,wvp; // weight u minus/sum/plus
  weight *= calcWeightTwoSide(GAMMA_Y, ymin - ex->y, ymax - ex->y);

  wum = pow(GAMMA_U , ex->u - umin + 1);
  wus = calcWeightTwoSide(GAMMA_U, umin - ex->u, umax - ex->u);
  wup = pow(GAMMA_U , umax+1 - ex->u);

  wvm = pow(GAMMA_V , ex->v - vmin + 1);
  wvs = calcWeightTwoSide(GAMMA_V, vmin - ex->v, vmax - ex->v);
  wvp = pow(GAMMA_V , vmax+1 - ex->v);

  wprop.idxProp(map_idx,color_idx,PROPUMVM) += weight * wum * wvm;
  wprop.idxProp(map_idx,color_idx,PROPUMVS) += weight * wum * wvs;
  wprop.idxProp(map_idx,color_idx,PROPUMVP) += weight * wum * wvp;
  wprop.idxProp(map_idx,color_idx,PROPUSVM) += weight * wus * wvm;
  wprop.idxProp(map_idx,color_idx,PROPUSVS) += weight * wus * wvs;
  wprop.idxProp(map_idx,color_idx,PROPUSVP) += weight * wus * wvp;
  wprop.idxProp(map_idx,color_idx,PROPUPVM) += weight * wup * wvm;
  wprop.idxProp(map_idx,color_idx,PROPUPVS) += weight * wup * wvs;
  wprop.idxProp(map_idx,color_idx,PROPUPVP) += weight * wup * wvp;

  weight *= wus * wvs;

  return weight;
}

// the weight for the next more positive cube due to an example in this cube
double thresh::calcWeightOneYP(uchar ymin, uchar umin, uchar vmin, example *ex, double *prop) {
  uchar ymax = ymin + div_y - 1;
  uchar umax = umin + div_u - 1;
  uchar vmax = vmin + div_v - 1;

  double weight=1.0;
  double wum,wus,wup; // weight u minus/sum/plus
  double wvm,wvs,wvp; // weight u minus/sum/plus
  weight *= pow(GAMMA_Y, ymax+1 - ex->y);

  wum = pow(GAMMA_U , ex->u - umin + 1);
  wus = calcWeightTwoSide(GAMMA_U, umin - ex->u, umax - ex->u);
  wup = pow(GAMMA_U , umax+1 - ex->u);

  wvm = pow(GAMMA_V , ex->v - vmin + 1);
  wvs = calcWeightTwoSide(GAMMA_V, vmin - ex->v, vmax - ex->v);
  wvp = pow(GAMMA_V , vmax+1 - ex->v);

  prop[PROPUMVM] += weight * wum * wvm;
  prop[PROPUMVS] += weight * wum * wvs;
  prop[PROPUMVP] += weight * wum * wvp;
  prop[PROPUSVM] += weight * wus * wvm;
  prop[PROPUSVS] += weight * wus * wvs;
  prop[PROPUSVP] += weight * wus * wvp;
  prop[PROPUPVM] += weight * wup * wvm;
  prop[PROPUPVS] += weight * wup * wvs;
  prop[PROPUPVP] += weight * wup * wvp;

  weight *= wus * wvs;

  return weight;
}

// the weight for the next more negative cube due to an example in this cube
double thresh::calcWeightOneYM(uchar ymin, uchar umin, uchar vmin, example *ex, double *prop) {
  //  uchar ymax = ymin + div_y - 1;
  uchar umax = umin + div_u - 1;
  uchar vmax = vmin + div_v - 1;

  double weight=1.0;
  double wum,wus,wup; // weight u minus/sum/plus
  double wvm,wvs,wvp; // weight u minus/sum/plus
  weight *= pow(GAMMA_Y, ex->y - ymin + 1);

  wum = pow(GAMMA_U , ex->u - umin + 1);
  wus = calcWeightTwoSide(GAMMA_U, umin - ex->u, umax - ex->u);
  wup = pow(GAMMA_U , umax+1 - ex->u);

  wvm = pow(GAMMA_V , ex->v - vmin + 1);
  wvs = calcWeightTwoSide(GAMMA_V, vmin - ex->v, vmax - ex->v);
  wvp = pow(GAMMA_V , vmax+1 - ex->v);

  prop[PROPUMVM] += weight * wum * wvm;
  prop[PROPUMVS] += weight * wum * wvs;
  prop[PROPUMVP] += weight * wum * wvp;
  prop[PROPUSVM] += weight * wus * wvm;
  prop[PROPUSVS] += weight * wus * wvs;
  prop[PROPUSVP] += weight * wus * wvp;
  prop[PROPUPVM] += weight * wup * wvm;
  prop[PROPUPVS] += weight * wup * wvs;
  prop[PROPUPVP] += weight * wup * wvp;

  weight *= wus * wvs;

  return weight;
}

void thresh::calcWeightPropHere() {
  printf("filling in weight map here\n");

  for(int y_box=0; y_box<size_y; y_box++) {
    printf("here box:y=%3d/%3d\r",y_box,size_y);
    fflush(stdout);

    for(int u_box=0; u_box<size_u; u_box++) {
      for(int v_box=0; v_box<size_v; v_box++) {
        int i=box_loc(y_box,u_box,v_box);

        for(int color_idx=0; color_idx<COLORS; color_idx++)
          wmap[i*COLORS+color_idx]=0.0;
        
        // clear out counter array for used color classes
        example *p;
        p = ltable[i];
        while(p){
          calcWeightOneHere(y_box*div_y,u_box*div_u,v_box*div_v,p,wprop,i,p->label);
          
          p = p->next;
        }
      }
    }
  }

  printf("\n");
}

void thresh::addWeightPropYP() {
  printf("filling in weight prop yp\n");

  int cnt=0;

  for(int u_box=0; u_box<size_u; u_box++) {
    printf("yp box:u=%3d/%3d\r",u_box,size_u);
    fflush(stdout);

    for(int v_box=0; v_box<size_v; v_box++) {
      double prop[NUM_PROP*COLORS];
      
      for(int prop_idx=0; prop_idx<NUM_PROP; prop_idx++)
        for(int color_idx=0; color_idx<COLORS; color_idx++)
          prop[color_idx*NUM_PROP + prop_idx] = 0.0;

      for(int y_box=0; y_box<size_y-1; y_box++) {
        int box_idx       =box_loc(y_box  ,u_box,v_box);
        int box_idx_update=box_loc(y_box+1,u_box,v_box);
        
        example *p;
        p = ltable[box_idx];
        while(p){
          calcWeightOneYP(y_box*div_y,u_box*div_u,v_box*div_v,p,&prop[p->label*NUM_PROP]);
          
          p = p->next;
        }

        for(int prop_idx=0; prop_idx<NUM_PROP; prop_idx++) {
          for(int color_idx=0; color_idx<COLORS; color_idx++) {
            double new_prop;
            new_prop = prop[color_idx*NUM_PROP + prop_idx] * pow(GAMMA_Y, div_y);
            wprop.idxProp(box_idx_update,color_idx,prop_idx) += (prop[color_idx*NUM_PROP + prop_idx] - new_prop) / (1 - GAMMA_Y);
            prop[color_idx*NUM_PROP + prop_idx] = new_prop;
          }
        }

        cnt++;
      }
    }
  }

  printf("\n");
}

void thresh::addWeightPropYM() {
  printf("filling in weight prop ym\n");

  int cnt=0;

  for(int u_box=0; u_box<size_u; u_box++) {
    printf("ym box:u=%3d/%3d\r",u_box,size_u);
    fflush(stdout);

    for(int v_box=0; v_box<size_v; v_box++) {
      double prop[NUM_PROP*COLORS];
      
      for(int prop_idx=0; prop_idx<NUM_PROP; prop_idx++)
        for(int color_idx=0; color_idx<COLORS; color_idx++)
          prop[color_idx*NUM_PROP + prop_idx] = 0.0;

      for(int y_box=size_y-1; y_box>=1; y_box--) {
        int box_idx       =box_loc(y_box  ,u_box,v_box);
        int box_idx_update=box_loc(y_box-1,u_box,v_box);
        
        example *p;
        p = ltable[box_idx];
        while(p){
          calcWeightOneYM(y_box*div_y,u_box*div_u,v_box*div_v,p,&prop[p->label*NUM_PROP]);
          
          p = p->next;
        }

        for(int prop_idx=0; prop_idx<NUM_PROP; prop_idx++) {
          for(int color_idx=0; color_idx<COLORS; color_idx++) {
            double new_prop;
            new_prop = prop[color_idx*NUM_PROP + prop_idx] * pow(GAMMA_Y, div_y);
            wprop.idxProp(box_idx_update,color_idx,prop_idx) += (prop[color_idx*NUM_PROP + prop_idx] - new_prop) / (1 - GAMMA_Y);
            prop[color_idx*NUM_PROP + prop_idx] = new_prop;
          }
        }

        cnt++;
      }
    }
  }

  printf("\n");
}

void thresh::sweepWeightMapVP() {
  printf("filling in weight map vp\n");

  for(int y_box=0; y_box<size_y; y_box++) {
    printf("vp box:y=%3d/%3d\r",y_box,size_y);
    fflush(stdout);

    for(int u_box=0; u_box<size_u; u_box++) {
      double up[COLORS];
      double us[COLORS];
      double um[COLORS];
      for(int i=0; i<COLORS; i++) {
        up[i]=0.0;
        us[i]=0.0;
        um[i]=0.0;
      }
      for(int v_box=0; v_box<size_v; v_box++) {
        int i=box_loc(y_box,u_box,v_box);

        for(int color_idx=0; color_idx<COLORS; color_idx++) {
          double new_um;
          new_um = um[color_idx] * pow(GAMMA_V, div_v);
          wprop.idxProp(i,color_idx,PROPUMVS) += (um[color_idx] - new_um) / (1 - GAMMA_V);
          um[color_idx] *= pow(GAMMA_V, div_v);

          double new_us;
          new_us = us[color_idx] * pow(GAMMA_V, div_v);
          wprop.idxProp(i,color_idx,PROPUSVS) += (us[color_idx] - new_us) / (1 - GAMMA_V);
          us[color_idx] *= pow(GAMMA_V, div_v);

          double new_up;
          new_up = up[color_idx] * pow(GAMMA_V, div_v);
          wprop.idxProp(i,color_idx,PROPUPVS) += (up[color_idx] - new_up) / (1 - GAMMA_V);
          up[color_idx] *= pow(GAMMA_V, div_v);
        }

        for(int color_idx=0; color_idx<COLORS; color_idx++) {
          um[color_idx] += wprop.idxProp(i,color_idx,PROPUMVP);
          us[color_idx] += wprop.idxProp(i,color_idx,PROPUSVP);
          up[color_idx] += wprop.idxProp(i,color_idx,PROPUPVP);
        }
      }
    }
  }

  printf("\n");
}

void thresh::sweepWeightMapVM() {
  printf("filling in weight map vm\n");

  int cnt=0;

  for(int y_box=0; y_box<size_y; y_box++) {
    printf("vm box:y=%3d/%3d\r",y_box,size_y);
    fflush(stdout);

    for(int u_box=0; u_box<size_u; u_box++) {
      double us[COLORS];
      double up[COLORS];
      double um[COLORS];
      for(int i=0; i<COLORS; i++) {
        up[i]=0.0;
        us[i]=0.0;
        um[i]=0.0;
      }
      for(int v_box=size_v-1; v_box>=0; v_box--) {
        int i=box_loc(y_box,u_box,v_box);

        for(int color_idx=0; color_idx<COLORS; color_idx++) {
          double new_um;
          new_um = um[color_idx] * pow(GAMMA_V, div_v);
          wprop.idxProp(i,color_idx,PROPUMVS) += (um[color_idx] - new_um) / (1 - GAMMA_V);
          um[color_idx] *= pow(GAMMA_V, div_v);

          double new_us;
          new_us = us[color_idx] * pow(GAMMA_V, div_v);
          wprop.idxProp(i,color_idx,PROPUSVS) += (us[color_idx] - new_us) / (1 - GAMMA_V);
          us[color_idx] *= pow(GAMMA_V, div_v);

          double new_up;
          new_up = up[color_idx] * pow(GAMMA_V, div_v);
          wprop.idxProp(i,color_idx,PROPUPVS) += (up[color_idx] - new_up) / (1 - GAMMA_V);
          up[color_idx] *= pow(GAMMA_V, div_v);
        }

        for(int color_idx=0; color_idx<COLORS; color_idx++) {
          um[color_idx] += wprop.idxProp(i,color_idx,PROPUMVM);
          us[color_idx] += wprop.idxProp(i,color_idx,PROPUSVM);
          up[color_idx] += wprop.idxProp(i,color_idx,PROPUPVM);
        }

        cnt++;
      }
    }
  }

  printf("\n");
}

void thresh::sweepWeightMapUP() {
  printf("filling in weight map up\n");

  int cnt=0;

  for(int y_box=0; y_box<size_y; y_box++) {
    printf("up box:y=%3d/%3d\r",y_box,size_y);
    fflush(stdout);

    for(int v_box=0; v_box<size_v; v_box++) {
      double weight[COLORS];
      for(int i=0; i<COLORS; i++)
        weight[i]=0.0;
      for(int u_box=0; u_box<size_u; u_box++) {
        int i=box_loc(y_box,u_box,v_box);

        for(int color_idx=0; color_idx<COLORS; color_idx++) {
          double new_weight;
          new_weight = weight[color_idx] * pow(GAMMA_U, div_u);
          wprop.idxProp(i,color_idx,PROPUSVS) += (weight[color_idx] - new_weight) / (1 - GAMMA_U);
          weight[color_idx] *= pow(GAMMA_U, div_u);
        }

        for(int color_idx=0; color_idx<COLORS; color_idx++) {
          weight[color_idx] += wprop.idxProp(i,color_idx,PROPUPVS);
        }

        cnt++;
      }
    }
  }

  printf("\n");
}

void thresh::sweepWeightMapUM() {
  printf("filling in weight map um\n");

  int cnt=0;

  for(int y_box=0; y_box<size_y; y_box++) {
    printf("um box:y=%3d/%3d\r",y_box,size_y);
    fflush(stdout);

    for(int v_box=0; v_box<size_v; v_box++) {
      double weight[COLORS];
      for(int i=0; i<COLORS; i++)
        weight[i]=0.0;
      for(int u_box=size_u-1; u_box>=0; u_box--) {
        int i=box_loc(y_box,u_box,v_box);

        for(int color_idx=0; color_idx<COLORS; color_idx++) {
          double new_weight;
          new_weight = weight[color_idx] * pow(GAMMA_U, div_u);
          wprop.idxProp(i,color_idx,PROPUSVS) += (weight[color_idx] - new_weight) / (1 - GAMMA_U);
          weight[color_idx] *= pow(GAMMA_U, div_u);
        }

        // clear out counter array for used color classes
        for(int color_idx=0; color_idx<COLORS; color_idx++) {
          weight[color_idx] += wprop.idxProp(i,color_idx,PROPUMVS);
        }

        cnt++;
      }
    }
  }

  printf("\n");
}

void thresh::copyWeightProp() {
  printf("copying weight prop to weight map\n");

  for(int i=0; i<size; i++) {
    for(int color_idx=0; color_idx<COLORS; color_idx++) {
      wmap[i*COLORS+color_idx] = wprop.idxProp(i,color_idx,PROPUSVS);
    }
  }
}

void thresh::fillInWeightMap(color_info *colors, int num) {
  printf("filling in weight map\n");
  calcWeightPropHere();
  addWeightPropYP();
  addWeightPropYM();
  sweepWeightMapVP();
  sweepWeightMapVM();
  sweepWeightMapUP();
  sweepWeightMapUM();
  copyWeightProp();
}

void thresh::learnMap(color_info *colors,int num)
{
  //  example *p;
  double weight,max_weight,total_weight;
  double conf;
  int i,max_color;

  wprop.init(size,COLORS,NUM_PROP);
  
  const char *cache_fname="cache.dat";
  FILE *cache_file;

  cache_file=fopen(cache_fname,"rb");
  if(cache_file) {
    printf("using cache file\n");
    fread(wmap,sizeof(*wmap),COLORS*size,cache_file);
    fclose(cache_file);
  }
  else {
    printf("learning weight file\n");

    fillInWeightMap(colors,num);

    // write cache
    FILE *cache_file;
    cache_file=fopen(cache_fname,"wb");
    fwrite(wmap,sizeof(*wmap),COLORS*size,cache_file);
    fclose(cache_file);
  }

  printf("filling in tmap\n");
  for(i=0; i<size; i++){
    if((i & 0xFF)==0) {
      printf("i=%d/%d\r",i,size);
      fflush(stdout);
    }
    max_color = 0;
    weight = max_weight = total_weight = 0.0;

    for(int color_idx=0; color_idx<COLORS; color_idx++) {
      if(count[color_idx] > 0) {
        weight = (colors[color_idx].weight * wmap[i*COLORS+color_idx]);///count[color_idx];
        //printf("c%d w%g ",color_idx,weight);
        total_weight += weight;
        if(weight > max_weight) {
          max_weight = weight;
          max_color = color_idx;
        }
      }
    }

    conf = max_weight / total_weight;

    if(conf > colors[max_color].confidence) {
      tmap[i] = max_color;
    } else {
      tmap[i] = 0xFF;
    }

    //printf("i=%5d tmap=%2d\n",i,tmap[i]);
  }
  printf("\n");

  printf("counts\n");
  for(int color_idx=0; color_idx<COLORS; color_idx++)
    printf("color=%2d weight=%12g count=%lu\n",color_idx,colors[color_idx].weight,count[color_idx]);

  wprop.release();
}
