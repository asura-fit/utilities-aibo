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

#define __KDTREE_H__

#ifndef __KDTREE_H__
#define __KDTREE_H__

#define KD_TREE KDTree<data,num,dim>
#define KD_TREE_TEM template <class data,class num,int dim>

KD_TREE_TEM
class KDTree{
  class node{
    int start,num;
    node *left,*right;
  };

  int vmax[dim];
  int vmin[dim];
  example *data;
  int num;
  node *root;

  int split(int s,int n,int d,int b);
  node *build_tree(int s,int n,int d,int b);

public:
  kdtree() {root=NULL;}
  ~kdtree() {clear();}

  

  void clear(); 
};

KD_TREE_TEM
int KD_TREE::split(int l,int r,int d,int v)
{
  int i,j,mask;

  mask = 1 << b;
  i = l;
  j = r;

  while(true){
    // look for out of position elements
    while(i<j && (data[i][d] & mask)==   0) i++;
    while(i<j && (data[j][d] & mask)==mask) j--;

    if(i >= j) break;

    // swap elements
    tmp = data[i];
    data[i] = data[j];
    data[j] = tmp;
  }

  // make i indicate last left side element, and j the first right side
  if((data[i][d] & mask) == mask) i--;

  return(i - l + 1);
}

KD_TREE_TEM
KD_TREE::node *KD_TREE::build_tree(int s,int n,int d,int b)
{
  example *p;
  int l;

  if(n <= 0) return(NULL);

  // split data into two half spaces
  l = split(s,s+n-1,d,b);

  // switch to dimensions for next level
  d = (d + 1) % dim;
  b >>= (d == 0);

  // create new node
  p = new node;
  p->start = s;
  p->num   = n;
  p->left  = build_tree(s,l,d,b);
  p->right = build_tree(s+l,n-l,d,b);

  return(p);
}

#endif
// __KDTREE_H__
