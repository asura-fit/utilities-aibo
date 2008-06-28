/*========================================================================
    Image.h : Simple Image class for loading and manipulating images
  ------------------------------------------------------------------------
    Copyright (C) 1999-2002  James R. Bruce
    School of Computer Science, Carnegie Mellon University
  ------------------------------------------------------------------------
    This software is distributed under the GNU General Public License,
    version 2.  If you do not have a copy of this licence, visit
    www.gnu.org, or write: Free Software Foundation, 59 Temple Place,
    Suite 330 Boston, MA 02111-1307 USA.  This program is distributed
    in the hope that it will be useful, but WITHOUT ANY WARRANTY,
    including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  ========================================================================*/

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdlib.h>

#include "colors.h"
#include "util.h"


//==== RGB Utilities =================================================//

rgb blend(rgb a,rgb b,float t);
bool operator ==(rgb a,rgb b);
bool operator !=(rgb a,rgb b);

//==== SImage Class ==================================================//

class simage{
public:
  rgb *img;
  int w,h;

  simage()
    {img=NULL; w=h=0;}
  ~simage() 
    {delete[] img;}
  bool allocate(int nw,int nh);
  bool load(const char *filename);
  bool save(const char *filename);
  bool copy(simage &a);

  void clear(rgb c);
  bool blend(simage &a,simage &b,float t);
  bool samesize(simage &a)
    {return(w==a.w && h==a.h);}

  bool blit(simage &src,int sx,int sy,int sw,int sh,int dx,int dy);
  bool blit(simage &src,int dx,int dy)
    {return(blit(src,0,0,src.w,src.h,dx,dy));}

  rgb getpixel(int x,int y);
  rgb getpixel(float x,float y);
  rgb getpixel_fast(int x,int y)
    {return(img[y*w + x]);}

  bool setpixel(int x,int y,rgb c);
};

#endif
