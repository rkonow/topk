#include <SuffixTree.h>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "DocumentArray.cpp"
#include "SuffixTreeHandler.cpp"
#include "utils.cpp"
#include <BitString.h>
#include "LinkList.cpp"
#include <cstdlib>
#include <libcdsTrees.h>
#include <TextIndex.h>

//#include <RMQ_succinct.h>
#include <factorization.h>
//#include <timing.h>



#include <WaveletTreeRMQ.h>
#include "rmq.cpp"
#include <tree_ff.h>

//#include "../dacs/src/dacs.h"
//#include "../dacs/src/dacs.c"

bool make_random_flag = false;
int rand_files = 3;
//class WaveletTreeRMQ;

class Topk
{
	public:
		char * file;
		size_t *file_sizes;
		int num_files;
		size_t  length;

		
		SuffixTreeY *cst;
		SuffixTreeHandler *stp;
		DocumentArray *da;
		LinkList * ll;
		TextIndex *ticsa;
		WaveletTreeRMQ *d_sequence;
		tree_ff *t;
		
		map< pair<uint,uint>,pair<uint,uint> > nodes_map;
		vector<uint> documents;
		map <pair<uint,uint>,uint> nodes_preorder;
		BitSequenceRG * d;
		factorization *freq_dacs;
		RMQ *rmq;
		BitSequenceRG *bitsequence_map;
    	BitSequenceRG *bitsequence_leaf;
    	uint* d_array;
		uint *freq_array;
		size_t pointer_size;
		uint max_freq;
		uint *gd_sequence;
		


		Topk(char * file,size_t *file_sizes, int num_files);
		void constructWT(size_t pointersize,uint *&depth_sequence);
		void constructTree(uint* bitmap, uint size);
		void constructRMQ(vector<uint> &freq_array);
		double query(uchar* q,uint size_q);
		size_t getSize();
		~Topk();
};