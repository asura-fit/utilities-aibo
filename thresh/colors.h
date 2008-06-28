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

typedef unsigned char uchar;


#ifndef YUV_STRUCT
#define YUV_STRUCT
struct yuv{
  uchar y,u,v;
};

struct yuv_int{
  int y,u,v;
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

#endif
