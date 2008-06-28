/*========================================================================
    colors.h : Color definitions for CMVision2 and the Simple Image class
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

#ifndef __COLORS_H__
#define __COLORS_H__


#define RGB_COLOR_NAMES
#define RGB_OPERATORS


//==== Color Classes =================================================//

typedef unsigned char uchar;

#ifndef YUV_STRUCT
#define YUV_STRUCT
struct yuv{
  uchar y,u,v;
};

/* Depricated
struct yuv_int{
  int y,u,v;
};
*/
#endif

#ifndef YUVI_STRUCT
#define YUVI_STRUCT
struct yuvi{
  int y,u,v;
};
#endif

#ifndef YUVF_STRUCT
#define YUVF_STRUCT
struct yuvf{
  float y,u,v;
};
#endif

#ifndef YUYV_STRUCT
#define YUYV_STRUCT
struct yuyv{
  uchar y1,u,y2,v;
};
#endif

struct uyvy{
#ifndef UYVY_STRUCT
#define UYVY_STRUCT
  uchar u,y1,v,y2;
};
#endif

#ifndef RGB_STRUCT
#define RGB_STRUCT
struct rgb{
  uchar red,green,blue;
};
#endif

#ifndef RGBA_STRUCT
#define RGBA_STRUCT
struct rgba{
  uchar r,g,b,a;
};
#endif

#ifndef ARGB_STRUCT
#define ARGB_STRUCT
struct argb{
  uchar a,r,g,b;
};
#endif

#ifndef RGBF_STRUCT
#define RGBF_STRUCT
struct rgbf{
  float red,green,blue;

  rgbf()
    {}
  rgbf(float nred,float ngreen,float nblue)
    {red=nred; green=ngreen; blue=nblue;}
};
#endif

#ifndef RGBAF_STRUCT
#define RGBAF_STRUCT
struct rgbaf{
  float r,g,b,a;
};
#endif

#ifndef ARGBF_STRUCT
#define ARGBF_STRUCT
struct argbf{
  float a,r,g,b;
};
#endif


//==== Color Names ===================================================//

#ifdef RGB_COLOR_NAMES
namespace Rgb{
  const rgb Black = {  0,  0,  0};
  const rgb Blue  = {  0,  0,255};
  const rgb Green = {  0,255,  0};
  const rgb Red   = {255,  0,  0};
  const rgb White = {255,255,255};
  const rgb Gray  = {128,128,128};
  const rgb DarkGray = { 32, 32, 48};
};
#endif


//==== Color Arithmetic ==============================================//

#ifdef RGB_OPERATORS
rgb to_rgb(rgbf c);
rgbf to_rgbf(rgb c);
rgbf clip(rgbf c);
rgbf operator +(const rgbf a,const rgbf b);
rgbf operator *(const rgbf a,const rgbf b);
rgbf operator *(const rgbf a,const double m);
rgbf pow(const rgbf x,const double p);
double sse(rgbf a,rgbf b);
#endif

#endif
