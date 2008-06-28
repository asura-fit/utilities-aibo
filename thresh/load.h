/*========================================================================
    Load.h : File loading support for the Simple Image class
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

#include "colors.h"

rgb mkrgb(uchar red,uchar green,uchar blue);
//inline bool operator ==(rgb a,rgb b);
//inline bool operator !=(rgb a,rgb b);

struct pixel{
  uchar y,u,v,t;
};

//==== Image File Handling ============//

#define FMT_ERROR 0
#define FMT_RAW   1 /* not yet supported for read */
#define FMT_PGMA  2 /* P2 */
#define FMT_PPMA  3 /* P3 */
#define FMT_PGM   4 /* P5 */
#define FMT_PPM   5 /* P6 */

bool getword(char *str,int len,int &ofs,char *buf,int num);
bool getnum (char *str,int len,int &ofs,int &num);

rgb *load_rgb_image(char *filename,int &width,int &height,int &format);
uchar *load_gray_image(char *filename,int &width,int &height,int &format);
bool save_image(char *filename,rgb   *img,int width,int height,int format);
bool save_image(char *filename,uchar *img,int width,int height,int format);
