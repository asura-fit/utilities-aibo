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

#ifndef __LEARN_H__
#define __LEARN_H__

#include <stdio.h>
#include <string.h>

#include "colors.h"

typedef unsigned char uchar;
typedef unsigned long ulong;

#define LEVELS 256
#define COLORS 16

struct example{
  uchar label;
  uchar y,u,v;
  example *next;
};

struct color_info{
  char *name;
  rgb rcolor;
  double weight,confidence;
};

class WeightProp {
private:
  double *wprop;

  int mapSize;
  int numColors;
  int numProps;

  int calcIdx(int map_idx, int color_idx, int prop_id) {
    return (map_idx * numColors + color_idx) * numProps + prop_id;
  }

public:
  WeightProp() {
    wprop=NULL;
  }

  void init(int map_size, int num_colors, int num_props) {
    mapSize   = map_size;
    numColors = num_colors;
    numProps  = num_props;
    wprop = new double[mapSize*numColors*numProps];
    memset(wprop,0,mapSize*numColors*numProps);
  }
  void release() {
    delete wprop;
    wprop=NULL;
  }

  double &idxProp(int map_idx, int color_idx, int prop_id) {
    return wprop[calcIdx(map_idx,color_idx,prop_id)];
  }
};

class thresh{
public:
  double *wmap; // weight map
  WeightProp wprop; // weight properties map
  ulong *count; // color example count

  example **ltable;
  int size;
  int size_y,size_u,size_v;
  int div_y,div_u,div_v;

  int loc(int y,int u,int v) {
    return(((y/div_y)*size_u + u/div_u)*size_v + v/div_v);
  }

  int box_loc(int y_box, int u_box, int v_box) {
    return ((y_box*size_u + u_box)*size_v + v_box);
  }

public:
  uchar *tmap;

  thresh() {ltable=NULL; tmap=NULL; count=NULL;}
  ~thresh() {clear();}

  bool init(int sy,int su,int sv);
  void clear();

  void addList(example *e);
  uchar classify(int y,int u,int v)
    {return(tmap[loc(y,u,v)]);}

  double calcWeightTwoSide(double gamma, int mind, int maxd);
  double calcWeightOneHere(uchar ymin, uchar umin, uchar vmin, example *ex, WeightProp &wprop, int map_idx, int color_idx);
  double calcWeightOneYP  (uchar ymin, uchar umin, uchar vmin, example *ex, double *prop);
  double calcWeightOneYM  (uchar ymin, uchar umin, uchar vmin, example *ex, double *prop);
  void calcWeightPropHere();
  void addWeightPropYP();
  void addWeightPropYM();
  void sweepWeightMapVP();
  void sweepWeightMapVM();
  // propagate weight to u +(plus) square
  void sweepWeightMapUP();
  // propagate weight to u -(minus) square
  void sweepWeightMapUM();
  void copyWeightProp();
  void fillInWeightMap(color_info *colors, int num);
  void learnMap(color_info *colors,int num);
};

#endif
// __LEARN_H__
