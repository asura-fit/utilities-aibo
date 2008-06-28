/*========================================================================
    Memfile.h : Simple C++ wrapper for memeory mapping whole files
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

#ifndef MEMFILE_H
#define MEMFILE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>

class memfile{
  void *buf;
  off_t len;
  int fd;
public:
  memfile() {buf=NULL; len=0;}
  ~memfile() {mclose();}

  void *mopen(char *filename,off_t &length);
  void mclose();
};

#ifndef MAP_FAILED
#define MAP_FAILED (-1)
#endif

#endif
