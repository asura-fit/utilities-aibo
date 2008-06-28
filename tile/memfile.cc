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

//
// Quick Hacked by Nazy 2003
//	for UNIX/Win32
//

#include "memfile.h"
#include "funcname.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <new>

/* memfile is a class to abstract away somewhat the complexity involved
in mmap'ing a file. */

void *memfile::mopen(const char * const filename, int &length)
{
	std::streamsize thelength;
	mclose();

	std::ifstream ifs(filename, std::ios::binary);

	if (!ifs) {
		return NULL;
	}

	ifs.seekg(0, std::ios::end);
	thelength = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	try {
		buf = reinterpret_cast<void*>(new char[thelength]);
	} catch(std::bad_alloc &e) {
		std::cerr << "bad_alloc : " << e.what() << " in " << __FILE__ << '(' << __LINE__ << ") of " << _G_FUNCTIONNAMEEX << std::endl;
		std::exit(0);
	}

	ifs.read(reinterpret_cast<char*>(buf), thelength);
	if (!ifs.good() || ifs.gcount() != thelength) {
		std::cerr << "I/O Error : " << " in " << __FILE__ << '(' << __LINE__ << ") of " << _G_FUNCTIONNAMEEX << std::endl;
		std::cerr << !ifs.good() << " " << (ifs.gcount() != thelength) << std::endl;
		std::cerr << ifs.gcount() << " " << thelength << std::endl;
		delete [] buf;
		buf = 0;
		return NULL;
	}
	length = len = thelength;

	return(buf);
}

void memfile::mclose()
{
	if(buf){
		delete []buf;
		buf = NULL;
		len = 0;
	}
}
