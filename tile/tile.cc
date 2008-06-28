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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "colors.h"
#include "image2.h"

rgb yuv2rgb(yuv c)
{
  rgb r;
  int y,u,v;

  y = c.y;
  u = c.u*2 - 255;
  v = c.v*2 - 255;

  r.red   = bound(y + u                     ,0,255);
  r.green = bound((int)(y - 0.51*u - 0.19*v),0,255);
  r.blue  = bound(y + v                     ,0,255);

  return(r);
}

void yuv2rgb(rgb *dest,yuv *src,int width,int height)
{
  int size = width*height;
  int i;

  for(i=0; i<size; i++){
    dest[i] = yuv2rgb(src[i]);
  }
}

void fix_image(simage &img)
// Gets rid of garbage bits at the bottom of all
// dog images by copying an adjacent row over it
{
  int i;
  rgb r;

  for(i=0; i<16; i++){
    r = img.getpixel_fast(i,img.h-2);
    img.setpixel(i,img.h-1,r);
  }
}

const rgb RGB_Black = {  0,  0,  0};
const rgb YUV_Black = {  0,128,128};


int main(int argc,char **argv)
{
  simage2 src, yuv_dest, rgb_dest;
  char *file;

  CameraInfo info1, info;
  bool ignore_flag;
  int width, height;
  int x, y, w, h, n;
  int a;

  if(argc < 3){
    puts("Usage: tile <width> [yuv image file(s)]");
    return(1);
  }

  // Read number of images and tiling dimensions
  n = argc - 2;
  w = atoi(argv[1]);
  if (0 == w) {
    puts("Error : <width> == 0");
    puts("Usage: tile <width> [yuv image file(s)]");
	return(1);
  }
  h = (n + w-1) / w;

  // grab first image to get dimensions
  a = 2;
  printf("Load First Image (%s)\n", argv[a]);
  src.load(argv[a]);
  width  = src.w;
  height = src.h;
  src.GetCameraInfo(info1);
  ignore_flag = (info1.version_str == "" || info1.settings_str == "");
  printf("Images=%d Width=%d Height=%d Tile=%dx%d\n",
         n, width, height, w, h);
  printf("CameraVersion='%s' CameraSettings='%s'\n",
		 info1.version_str.c_str(), info1.settings_str.c_str());

  // allocate total space for destination images and clear them
  yuv_dest.allocate(w*width,h*height);
  yuv_dest.clear(YUV_Black);
  yuv_dest.AddComment("Tiled Image");
  yuv_dest.AddComment("YUV 8:8:8");
  yuv_dest.SetCameraInfo(info1);
  rgb_dest.allocate(w*width,h*height);
  rgb_dest.clear(RGB_Black);
  rgb_dest.AddComment("Tiled Image");
  rgb_dest.AddComment("RGB 8:8:8");
  rgb_dest.SetCameraInfo(info1);

  printf("Adding images");
  for(y=0; y<h; y++){
    for(x=0; x<w; x++){
      if(a < argc){
        file = argv[a];
        if(src.load(file)){
		  src.GetCameraInfo(info);
          if(width==src.w && height==src.h &&
			 (ignore_flag || info1==info)){
			src.Fix();
            yuv_dest.blit(src,x*width,y*height);
            printf("."); fflush(stdout);
          }else{
            printf("\nERROR: Images are not all the same status ('%s')\n",file);
            exit(1);
          }
        }else{
          printf("\nERROR: Cannot load '%s'\n",file);
          exit(1);
        }
      }
      a++;
    }
  }
  printf("done.\n");

  printf("Converting to RGB..."); fflush(stdout);
  rgb_dest.copy(yuv_dest);
  rgb_dest.YUV2RGB();
  printf("done.\n");

  printf("Saving..."); fflush(stdout);
  yuv_dest.save("imageyuv.ppm");
  rgb_dest.save("imagergb.ppm");
  printf("done.\n");

  return(0);
}
