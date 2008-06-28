# /* LICENSE:
#   =========================================================================
#     CMPack'02 Source Code Release for OPEN-R SDK v1.0
#     Copyright (C) 2002 Multirobot Lab [Project Head: Manuela Veloso]
#     School of Computer Science, Carnegie Mellon University
#   -------------------------------------------------------------------------
#     This software is distributed under the GNU General Public License,
#     version 2.  If you do not have a copy of this licence, visit
#     www.gnu.org, or write: Free Software Foundation, 59 Temple Place,
#     Suite 330 Boston, MA 02111-1307 USA.  This program is distributed
#     in the hope that it will be useful, but WITHOUT ANY WARRANTY,
#     including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   -------------------------------------------------------------------------
#     Additionally licensed to Sony Corporation under the following terms:
# 
#     This software is provided by the copyright holders AS IS and any
#     express or implied warranties, including, but not limited to, the
#     implied warranties of merchantability and fitness for a particular
#     purpose are disclaimed.  In no event shall authors be liable for
#     any direct, indirect, incidental, special, exemplary, or consequential
#     damages (including, but not limited to, procurement of substitute
#     goods or services; loss of use, data, or profits; or business
#     interruption) however caused and on any theory of liability, whether
#     in contract, strict liability, or tort (including negligence or
#     otherwise) arising in any way out of the use of this software, even if
#     advised of the possibility of such damage.
#   ========================================================================= */

CC = g++
CFLAGS = -O2 -g -fno-rtti -Wall
SRC = image2.cc image.cc learn.cc lprintf.cc load.cc memfile.cc
OBJ = $(SRC:%.cc=%.o)
LIB =
INC =
DEF =
INSTALLDIR = /usr/local/bin

.SUFFIXES: .cc
%.o: %.cc *.h
	$(CC) $(CFLAGS) $(DEF) $(INC) -c $< -o $@

all: thresh thresh2 thresh2thresh

install-cygwin: all
	@echo "Install thresh to $(INSTALLDIR) (y/n)"
	@( \
	read answer;  \
	case $$answer in \
	y) \
		cp -f $(TARGET).exe $(INSTALLDIR); \
		echo "install done." \
	;; \
	n) \
		echo "install canceled." \
	;; \
	*) \
		echo "please input \`y' or \`n'" \
		;; \
	esac \
	)


thresh: $(OBJ) thresh.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) thresh.o $(LIB)

thresh2: $(OBJ) thresh2.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) thresh2.o $(LIB)

thresh2thresh: thresh2thresh.o
	$(CC) $(CFLAGS) -o $@  thresh2thresh.o 

clean:
	rm -f *.o *~ $(TARGET1) $(TARGET2) core
