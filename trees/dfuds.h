/* Copyright (C) 2010, Rodrigo Cánovas, all rights reserved.
 *
 *This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#ifndef DFUDS
#define DFUDS

#include "bp_hb.h"

#define OPENING 0
#define CLOSING 1
#define NULLT -1
#define ROOT 1

typedef unsigned char symbol_T;
typedef unsigned long treeNode;


class dfuds {
   symbol_T *label;
   uint Nnodes;
   treeNode *boost;
   bool testing_times;
   double select_local_time;
   unsigned long long nselects;
public:
	 bp_hb *topology;
    dfuds(uint *par, uint n, symbol_T *_label, uint block_size, bool _testing_times);
    dfuds() {select_local_time=0.0; nselects=0;testing_times=false;};
    ~dfuds();
    uint getNnodes() {return Nnodes;};
    treeNode Root_Node();
    treeNode Parent_Sequential(treeNode x);
    treeNode Parent_Fast(treeNode x);
    treeNode Child(treeNode x, int i);
    treeNode Level_Ancestor(treeNode x, int delta);  // brute force
    uint     Depth(treeNode x); // brute force
    uint     Preorder_Rank(treeNode x);
    treeNode Preorder_Select(uint p);
    uint     Inspect(uint p);
    bool     Isleaf(treeNode x);
    uint     Subtree_Size(treeNode x);
    treeNode First_Child(treeNode x);
    treeNode Next_Sibling(treeNode x);
    treeNode Prev_Sibling(treeNode x);
    bool     Is_Ancestor(treeNode x, treeNode y);
    uint     Distance(treeNode x, treeNode y);  // brute force
    uint     Degree_Sequential(treeNode x);
    uint     Degree_Fast(treeNode x);
    uint     Child_Rank(treeNode x);
    symbol_T Get_Label(treeNode x);
    treeNode Labeled_Child(treeNode x, symbol_T s);
    uint size();
    void printStatistics();
};

#endif
