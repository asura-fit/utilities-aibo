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

//
// Quick Hacked by Nazy 2003
//	for UNIX/Win32
//

#ifndef MEMFILE_H
#define MEMFILE_H

#ifdef  _BSD_SIZE_T_
typedef _BSD_SIZE_T_    size_t;
#undef  _BSD_SIZE_T_
#endif
//typedef unsigned int size_t;

class memfile{
	void *buf;
	int len;
public:
	memfile() :buf(0), len(0) {}
	~memfile() {mclose();}

	void *mopen(const char * const filename, int &length);
	void mclose();
};

#endif
