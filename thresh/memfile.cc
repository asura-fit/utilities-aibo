/*========================================================================
    Memfile.cc : Simple C++ wrapper for memeory mapping whole files
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

#include "memfile.h"

/* memfile is a class to abstract away somewhat the complexity involved
in mmap'ing a file. */

void *memfile::mopen(char *filename,off_t &length)
{
  struct stat st;
  off_t l;

  mclose();

  fd = open(filename,O_RDONLY);
  if(!fd) return(NULL);

  memset(&st,0,sizeof(st));
  fstat(fd,&st);
  l = st.st_size;
  if(l == 0) return(NULL);

  buf = mmap(NULL,l,PROT_READ,MAP_PRIVATE,fd,0);
  if(buf == (void*)MAP_FAILED) buf = NULL;

  if(buf) length = len = l;

  return(buf);
}

void memfile::mclose()
{
  if(buf){
    // munmap(buf,len);
    buf = NULL;
    len = 0;
  }
  if(fd){
    close(fd);
    fd = 0;
  }
}
