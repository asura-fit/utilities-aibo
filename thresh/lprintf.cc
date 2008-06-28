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

#include <stdarg.h>
#include <stdio.h>

static FILE *log = NULL;

bool lprintf_open(char *file)
{
  if(log) fclose(log);
  log = fopen(file,"wt");
  return(log != NULL);
}

bool lprintf_close()
{
  int ret;

  ret = fclose(log);
  log = NULL;

  return(ret == 0);
}

int lprintf(char *fmt, ...)
{
  va_list al;
  int ret;

  if(log){
    va_start(al,fmt);
    vfprintf(log, fmt, al);
    va_end(al);
  }

  va_start(al,fmt);
  ret = vprintf(fmt, al);
  va_end(al);

  return(ret);
}
