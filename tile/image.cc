/*========================================================================
    Image.cc : Simple Image class for loading and manipulating images
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

#include "image.h"
#include "load.h"

#include <stdlib.h>
#include <stdio.h>
#include <cstring>
//#include <Magick++.h>

using namespace std;
//using namespace Magick;


//==== RGB Utilities =================================================//

rgb blend(rgb a,rgb b,float t)
{
  rgb r;

  r.red   = (int)((a.red   * (1.0 - t)) + (b.red   * t));
  r.green = (int)((a.green * (1.0 - t)) + (b.green * t));
  r.blue  = (int)((a.blue  * (1.0 - t)) + (b.blue  * t));

  return(r);
}

bool operator ==(rgb a,rgb b)
{
  return(a.red==b.red && a.green==b.green && a.blue==b.blue);
}

bool operator !=(rgb a,rgb b)
{
  return(a.red!=b.red || a.green!=b.green || a.blue!=b.blue);
}


//==== SImage Class ==================================================//

bool simage::allocate(int nw,int nh)
{
  int size,nsize;

  size = w * h;
  nsize = nw * nh;

  // allocate new buffer if different size
  if(size != nsize){
    delete[] (img);
    img = new rgb[nsize];
  }

  if(img){
    w = nw;
    h = nh;
    return(true);
  }else{
    w = 0;
    h = 0;
    return(false);
  }
}

bool simage::load(const char *filename)
{
//  try{
//    Image image(filename);
//    allocate(image.columns(),image.rows());
//    image.write(0,0,w,h,"RGB",CharPixel,img);
//  }
//
//  catch(Exception &error_){
//    return(false);
//  }
//
//  return(true);
  int format;
  img=load_rgb_image((char *)filename,w,h,format);
  return img!=NULL;
}

bool simage::save(const char *filename)
{
//  try{
//    Image image(w,h,"RGB",CharPixel,img);
//    image.write(filename);
//  }
//
//  catch(Exception &error_){
//    return(false);
//  }
//
//  return(true);
  return save_image((char *)filename,img,w,h,FMT_PPM);
}

bool simage::copy(simage &a)
{
  if(!allocate(a.w,a.h)) return(false);

  memcpy(img,a.img,sizeof(rgb)*w*h);

  return(false);
}

void simage::clear(rgb c)
{
  int i,s;

  s = w * h;

  for(i=0; i<s; i++){
    img[i] = c;
  }
}

bool simage::blend(simage &a,simage &b,float t)
{
  int i,s;
  rgb r,r_a,r_b,*img_a,*img_b;

  if(a.w!=b.w || a.h!=b.h) return(false);
  if(!allocate(a.w,b.h)) return(false);

  s = w * h;
  img_a = a.img;
  img_b = b.img;

  for(i=0; i<s; i++){
    r_a = img_a[i];
    r_b = img_b[i];

    r.red   = (int)((r_a.red   * (1.0 - t)) + (r_b.red   * t));
    r.green = (int)((r_a.green * (1.0 - t)) + (r_b.green * t));
    r.blue  = (int)((r_a.blue  * (1.0 - t)) + (r_b.blue  * t));

    img[i] = r;
  }
  return true;
}

bool simage::blit(simage &src,int sx,int sy,int sw,int sh,int dx,int dy)
{
  int i,j;

  if(sx<0 || sy<0 || sx+sw>src.w || sy+sh>src.h) return(false);

  if(dx < 0){
    sw += dx;
    dx = 0;
  }

  if(dy < 0){
    sh += dy;
    dy = 0;
  }

  if(dx + sw > w){
    sw = w - dx;
  }

  if(dy + sh > h){
    sh = h - dy;
  }

  if(sw<=0 || sh<=0) return(true);

  for(i=0; i<sh; i++){
    for(j=0; j<sw; j++){
      img[dx+j + w*(dy+i)] = src.img[sx+j + src.w*(sy+i)];
    }
  }
  return true;
}

rgb simage::getpixel(int x,int y)
// get pixel in image (or neareast pixel if outside image)
{
  return(img[bound(x,0,w-1) + w*bound(y,0,h-1)]);
}

rgb simage::getpixel(float x,float y)
// get pixel using linear interpolation
{
  rgb tl,tr,bl,br;
  rgb t,b,r;
  int ix,iy;

  ix = (int)x;
  iy = (int)y;

  if(ix>0 && iy>0 && ix<w-1 && iy<y-1){
    tl = img[(iy + 0)*w + ix + 0];
    tr = img[(iy + 0)*w + ix + 1];
    bl = img[(iy + 1)*w + ix + 0];
    br = img[(iy + 1)*w + ix + 1];
  }else{
    tl = getpixel(ix + 0,iy + 0);
    tr = getpixel(ix + 1,iy + 0);
    bl = getpixel(ix + 0,iy + 1);
    br = getpixel(ix + 1,iy + 1);
  }

  t = ::blend(tl,tr,x-ix);
  b = ::blend(bl,br,x-ix);
  r = ::blend( t, b,y-iy);

  return(r);
}

bool simage::setpixel(int x,int y,rgb c)
{
  if(x<0 || y<0 || x>=w || y>=h) return(false);

  img[y*w + x] = c;
  return(true);
}
