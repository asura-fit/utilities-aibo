/*========================================================================
    Load.cc : File loading support for the Simple Image class
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

#include "load.h"
#include "memfile.h"

#define MAP_GRAY 0
#define MAP_RGB  1

int strloc(char *s,int n,char ch)
{
  int i;
  i = 0;
  while(s[i] && i<n && s[i]!=ch) i++;
  return(i);
}

/*
bool getstr(char *str,int num,char *input,int ofs,int max);
{
  int i;

  if(!str || num<=0) return(false);
  str[0] = 0;
  if(!input || ofs<0 || ofs>=max || max<=0) return(false);

  i = 0;
  input += ofs;
  while(input[i] && input[i]!='\n' && i<num-1){
    str[i] = input[i];
    i++;
  }
  str[i] = 0;

  return(true);
}
*/

bool getword(char *str,int len,int &ofs,char *buf,int num)
{
  int i,l;
  char ch;

  if(!buf || num<=0) return(false);
  buf[0] = 0;

  // skip whitespace
  i = ofs;
  while(i<len && (ch=str[i]) && ch<=' ') i++;

  // copy word
  l = 0;
  while(i<len && l<num-1 && (ch=str[i]) && ch>' '){
    buf[l++] = str[i++];
  }
  buf[l] = 0;

  return(l > 0);
}

bool getnum(char *str,int len,int &ofs,int &num)
{
  int i,n;
  char ch;

  ch = 0;
  i = ofs;

  do{
    while(i<len && (ch=str[i]) && ch<=' ') i++;
    if(ch == '#'){
      // Skip comment
      while((ch=str[i]) && ch!='\n' && i<len) i++;
    }else if(ch>='0' && ch<='9'){
      // Read number
      n = 0;
      while((ch=str[i]) && ch>='0' && ch<='9' && i<len){
        n = (n * 10) + (ch - '0');
        i++;
      }

      ofs = i;
      num = n;
      return(true);
    }else{
      ch = 0;
    }
  }while(ch);

  ofs = i;
  num = 0;
  return(false);
}

void *load_image(char *filename,int &width,int &height,int &format,int out)
{
  memfile mf;
  off_t len;
  int ofs;
  // char buf[256];

  char *p;

  rgb *img;
  uchar *gimg;
  int w,h,f,m,size;
  int c,t;
  int i;
  // char ch;

  width = height = 0;
  format = FMT_ERROR;

  p = (char*)mf.mopen(filename,len);

  if(len<9 || !p || p[0]!='P') return(NULL);

  ofs = 1;
  getnum(p,len,ofs,i);
  f = strloc("356",3,p[1]);
  if(f == 3) return(NULL);
  f += FMT_PPMA;

  getnum(p,len,ofs,w);
  getnum(p,len,ofs,h);
  size = w * h;

  if(w<=0 || h<=0 || size<=0) return(NULL);

  getnum(p,len,ofs,m);
  ofs++;

  // printf("Open: P%d w=%d h=%d m=%d\n",i,w,h,m);

  if(out == MAP_RGB){
    if(len-ofs < (int)sizeof(rgb)*size) return(NULL);
    img = new rgb[size];
    if(!img) return(NULL);

    switch(f){
      case FMT_PGMA:
        for(i=0; i<size; i++){
          getnum(p,len,ofs,c);
	  img[i].red = img[i].green = img[i].blue = (c * 255) / m;
        }
	break;
      case FMT_PPMA:
        for(i=0; i<size; i++){
          getnum(p,len,ofs,c);  img[i].red   = (c * 255) / m;
          getnum(p,len,ofs,c);  img[i].green = (c * 255) / m;
          getnum(p,len,ofs,c);  img[i].blue  = (c * 255) / m;
        }
	break;
      case FMT_PGM:
        p += ofs;
        for(i=0; i<size; i++){
          img[i].red = img[i].green = img[i].blue = p[i];
        }
        break;
      case FMT_PPM:
        memcpy(img,p + ofs,size*sizeof(rgb));
        break;
    }

    width = w;
    height = h;
    format = f;
    return(img);
  }else if(out == MAP_GRAY){
    if(len-ofs < (int)sizeof(rgb)*width*height) return(NULL);
    gimg = new uchar[size];
    if(!gimg) return(NULL);
    img = (rgb*)(p + ofs);

    switch(f){
      case FMT_PGMA:
        for(i=0; i<size; i++){
          getnum(p,len,ofs,c);
	  gimg[i] = (c * 255) / m;
        }
	break;
      case FMT_PPMA:
        for(i=0; i<size; i++){
          getnum(p,len,ofs,c);  t  = (c * 255) / m;
          getnum(p,len,ofs,c);  t += (c * 255) / m;
          getnum(p,len,ofs,c);  t += (c * 255) / m;
          gimg[i] = t / 3;
        }
	break;
      case FMT_PGM:
        memcpy(img,p + ofs,size*sizeof(uchar));
        break;
      case FMT_PPM:
        for(i=0; i<size; i++){
          gimg[i] = (img[i].red + img[i].green + img[i].blue) / 3;
        }
        break;
    }

    width = w;
    height = h;
    format = f;
    return(gimg);
  }else{
    return(NULL);
  }
}

rgb *load_rgb_image(char *filename,int &width,int &height,int &format)
{
  return((rgb*)load_image(filename,width,height,format,MAP_RGB));
}

uchar *load_gray_image(char *filename,int &width,int &height,int &format)
{
  return((uchar*)load_image(filename,width,height,format,MAP_GRAY));
}

bool save_image(char *filename,rgb *img,int width,int height,int format)
{
  FILE *out;
  bool ok;
  unsigned size;

  ok = false;

  out = fopen(filename,"wb");
  if(!out) return(false);

  size = width * height;
  switch(format){
    case FMT_RAW:
      ok = (fwrite(img,sizeof(rgb),size,out) == size);
      break;
    case FMT_PGMA:
      ok = false;
      break;
    case FMT_PPMA:
      ok = false;
      break;
    case FMT_PGM:
      ok = false;
      break;
    case FMT_PPM:
      fprintf(out,"P6\n%d %d\n%d\n",width,height,255);
      ok = (fwrite(img,sizeof(rgb),size,out) == size);
      break;
  }

  fclose(out);
  return(ok);
}

bool save_image(char *filename,uchar *img,int width,int height,int format)
{
  FILE *out;
  bool ok;
  unsigned size;

  out = fopen(filename,"wb");
  if(!out) return(false);

  size = width * height;

  fprintf(out,"P5\n%d %d\n%d\n",width,height,255);
  ok = (fwrite(img,sizeof(rgb),size,out) == size);
  fclose(out);
  return(ok);
}

/*
int main(int argc,char **argv)
{
  if(argc != 3) return(1);

  rgb *img;
  int w,h,f;

  img = load_rgb_image(argv[1],w,h,f);
  if(!img) return(2);

  if(!save_image(argv[2],img,w,h,FMT_PPM)) return(3);

  return(0);
}
*/
