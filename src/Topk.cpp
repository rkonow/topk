#include "Topk.h"

Topk::Topk(char * file,size_t *file_sizes, int num_files)
{
	// Loading the Text
	char *text=NULL;
	cout << "file = " << file << endl;
	if(loadText(file, &text, &this->length))
		return;
	// cout << "text = " << text << endl;

	// Constructing Suffix Tree
	// this->length--;
	cout << "constructing suffix tree";
	cout << "length =" << this->length << endl;
	this->cst = new SuffixTreeY(text, this->length, DAC, CN_NPR, 32); 
	cout << "Done!" << endl;

	delete[] text;
	// Constructing Suffix Array
	cout << "Constructing Suffix Array";
	if(loadText(file, &text, &this->length))
		return;
	//cout << "length: " << length  << endl;
	this->ticsa = new TextIndexCSA((uchar *)text, (ulong)length, NULL);
	cout << "Done!" << endl;

	// removing the text
	delete[] text;

	BitSequenceRG *bsrg;
	// Suffix Tree Handler provides several functions
	this->stp = new SuffixTreeHandler(cst,length);
	if (make_random_flag)
		bsrg = this->stp->buildBs(file_sizes,rand_files,true);
	else
		bsrg = this->stp->buildBs(file_sizes,num_files);
	
	this->d = bsrg;
	// Retrieve all Nodes in pre-Order
	// all_nodes[i] -> [x,y]
	vector<pair<size_t,size_t> >  preorder_vector;
	pair<uint *,uint> tree_stuff = this->stp->generateBitmap(preorder_vector);
	vector< pair<size_t,size_t> > all_nodes = preorder_vector;
    this->constructTree(tree_stuff.first,tree_stuff.second);
    delete []tree_stuff.first;

	//vector< pair<size_t,size_t> > child_nodes = stp->getChildsNodes(0,length);
	// Create a Map for each node.
	map<pair<uint,uint>,uint>  nodes;
	// create an inverse map, to index according to [x,y]
	for (int i = 0 ; i < preorder_vector.size();i++)
	{
		// cout << "setting ->" << preorder_vector[i].first << "," << preorder_vector[i].second << " to " << i << endl;
		nodes[make_pair(preorder_vector[i].first,preorder_vector[i].second)] = i;

	}
	this->nodes_preorder = nodes;

	map<pair<uint,uint>,uint>::iterator it;

	// create a Document Array
	this->da = new DocumentArray(cst,bsrg,all_nodes,ticsa);
	// Create the LinkList to handle documents and frequencies
	this->ll = new LinkList(2*length,2*length);
	size_t count = 0;

	for (int i = 1 ; i <= num_files;i++)
	{
//		cout << "document =  "  << i << endl;
		size_t pos = 0;
		pair<size_t, size_t> pairc = da->selectDocument(i-1,pos+1);
		// cout << "for document = " << i << " and pos = " << pos << " : " << endl;
		// cout << "pairc.first =" << pairc.first << " | pairc.second = " << pairc.second << endl;
		while(pairc.first < length && pairc.second < length)
		{
			size_t vl_lca,vr_lca;
			this->cst->LCA(pairc.first,pairc.first,pairc.second,pairc.second,&vl_lca,&vr_lca);
			// cout << "LCA = " << vl_lca << " , " << vr_lca << endl;
			if (vl_lca == (size_t)-1 || vr_lca == (size_t)-1)
				break;
			if (vl_lca == 0 && vr_lca == 0)
				break;

			this->ll->insert(nodes[make_pair(vl_lca,vr_lca)],i-1,da->countRange(i-1,vl_lca,vr_lca));
			// cout << "Inserting node : " <<  nodes[make_pair(vl_lca,vr_lca)]+1 << " document = " << i << " frequency = " << da->countRange(i-1,vl_lca,vr_lca+1) << endl;
			count++;
			
			pos++;
			// go for next document
			pairc = this->da->selectDocument(i-1,pos+1);
			// cout << "for document = " << i << " and pos = " << pos << " : " << endl;
			// cout << "pairc.first =" << pairc.first << " | pairc.second = " << pairc.second << endl;
			if (pos >= length)
				break;
		}
	}
	// cout << "preorder_vector = " << preorder_vector.size() << endl;
	for (int i = 0 ; i < preorder_vector.size();i++)
	{
		uint k = 0;
		pair<uint,uint> paird = ll->getValue(i,k);
		while (paird.first != (uint)-1 && paird.second != (uint)-1)
		{
			cout << i << " ( " << preorder_vector[i].first << "," << preorder_vector[i].second << ") " 
			<< " | " << paird.first << " | " << paird.second << endl;
			k++;
			paird = ll->getValue(i,k);

		}
	}
	//BitString *leafs = new BitString(all_nodes.size());
	//BitString *marks = new BitString(all_nodes.size());

	//vector<Pointer *> pointers;	
	vector<uint> frequencies;
	size_t count_docs = 0;
	size_t count_aux = 0;
	size_t pointersize = 0;

	uint *depth_sequence = new uint[preorder_vector.size()];
	vector<uint> vecmapbits;
	vector<uint> vecleafbits;

	cout << "Constructing Sequences" << endl;
	this->max_freq = 0;
	for (int i = 0; i < preorder_vector.size();i++)
	{
		size_t count_aux_prev = count_aux;
		size_t vl_p,vr_p;
		pair<size_t,size_t> aux_node = preorder_vector[i];
		//marks->setBit(count_docs);
		// cout << "NODE = " << i << endl;
		// cout << "(" << nodes_preorder[aux_node] << ")" << endl;

		// cout << "(" << aux_node.first << " , " << aux_node.second << ")" <<  endl;

		if (aux_node.first == aux_node.second)
		{
			vecleafbits.push_back(1);
			vecmapbits.push_back(1);
			// cout << "leaf" << endl;
			continue;
		}
		else
		{
			vecleafbits.push_back(0);
			vecmapbits.push_back(1);
		}
	//	cout << "node = " << i << "( " << aux_node.first << " , " << aux_node.second << " ) " << endl; 

		size_t k = 0;
		pair<uint,uint> aux = this->ll->getValue(i,k);
		pair<uint,uint> node_aux = preorder_vector[i];
		while(aux.second != (uint)-1)
		{
			uint level = 0;
			// cout << "NODE " << i << "HAS" << endl;
			// cout << "NODE' = " << nodes_preorder[preorder_vector[i]] << endl;
			// cout << "Document = " << ll->getValue(nodes_preorder[preorder_vector[i]],k).first  << endl;
			// cout << "FOR K = " << k << endl;
			size_t vl,vr;
			vl = node_aux.first;
			vr = node_aux.second;
			count_docs++;
			bool cont = true;
			while(cont)
			{

				cst->Parent(vl,vr,&vl_p,&vr_p);
				level++;
				 if (vl_p == (size_t)-1 || vr_p == (size_t)-1)
				 {
				 	this->documents.push_back(aux.first);
					//size_t tdepth = cst->TDepth(vl,vr);
					// cout << "WTF Adding a new pointer from:" << vl << "," << vr << " to: " << vl_p << "," << vr_p << " depth = " << 0 <<
					//  "frequency = " << aux.second << endl;
					depth_sequence[pointersize] = 0;
					// cout << "depth_sequence[" << pointersize << "] = " << 0 << endl;
					pointersize++;	
					vecmapbits.push_back(0);
					vecleafbits.push_back(0);						
					//pointers.push_back(new Pointer(tdepth,make_pair(vl_p,vr_p)));
					count_aux++;
					frequencies.push_back(aux.second);
					if (aux.second > max_freq)
						this->max_freq = aux.second;
					cont = false;
				 	break;
				}
					


				size_t l = 0;
				//  cout << "vl_p = " << vl_p << endl;
				//  cout << "vr_p = " << vr_p << endl;
				//  cout << "vl = " << vl << endl;
				//  cout << "vr = " << vr << endl;
				// cout << nodes_preorder[make_pair(vl,vr)] << " has NEW PARENT = " << nodes_preorder[make_pair(vl_p,vr_p)] << endl;
				pair<uint,uint> aux2 = ll->getValue(nodes_preorder[make_pair(vl_p,vr_p)],l);
				while(aux2.second != (uint)-1 && aux.first != (uint)-1)
				{
					// cout << "on level = " << level <<  " doc 1 = " << aux.first << ", doc 2 = " << aux2.first << " freq1 = " << 
					// aux.second << " freq2 = " << aux2.second << endl;
					if (aux2.first == aux.first)
					{
						if (aux2.second > aux.second)
						{
							
							// cout << "[" << aux_node.first << " ," << aux_node.second << " ] = " << "[" << count_aux_prev << " , " 
							// 	<< count_aux << " ] "<< endl;
						
							size_t tdepth = cst->TDepth(vl_p,vr_p);

							// cout << "Adding a new pointer from:" << vl << "," << vr << " to: " << vl_p << "," << vr_p << 
							// " depth = " << tdepth+1 << "frequency = " << aux2.second << endl;

							depth_sequence[pointersize] = tdepth+1;
							pointersize++;	
							this->documents.push_back(aux.first);
							frequencies.push_back(aux.second);
						
						
							vecmapbits.push_back(0);
							vecleafbits.push_back(0);	
						
							if (aux.second > max_freq)
								this->max_freq = aux.second;
						
							cont = false;
							break;
						}
					}
					l++;
					aux2 = ll->getValue(nodes[make_pair(vl_p,vr_p)],l);
				}
				vl = vl_p;
				vr = vr_p;
			}
		k++;
		aux = this->ll->getValue(i,k);
		}
	}

	// uint *mapbits = new uint[vecmapbits.size()];
	// uint *leafbits = new uint[vecleafbits.size()];
	for (int i = 0 ; i < pointersize;i++)
	{
		cout << i << " | " << depth_sequence[i] << " | "  << documents[i] << " | " << frequencies[i] << " | " << vecmapbits[i]  << endl;
	}
	cout << "mapbits = " << vecmapbits.size();
	cout << "depth sequence = " << pointersize;
	if (vecmapbits.size() != vecleafbits.size())
		cout << "WTF!!! (" << vecleafbits.size() << " != " << vecmapbits.size() << ")" << endl;
	BitString *bsmap = new BitString(vecmapbits.size());
	BitString *bsleaf = new BitString(vecmapbits.size());
	for (int i = 0 ; i < vecmapbits.size();i++)
	{
		if (vecmapbits[i] == 1)
			bsmap->setBit(i);
		if (vecleafbits[i] == 1)
			bsleaf->setBit(i);
	}

    this->bitsequence_map = new BitSequenceRG(*bsmap,20);
    this->bitsequence_leaf = new BitSequenceRG(*bsleaf,20);
    cout << "0s->" << this->bitsequence_map->countZeros() << endl;
    cout << "1s->" << this->bitsequence_map->countOnes() << endl;
    cout << "freeing space..." << endl;

    uint p = 4;
    cout << "select1(p)-p+1 = " << this->bitsequence_map->select1(p)-p+1  << endl;
    cout << "select1(p)-p+1 = " << this->bitsequence_map->select1(p+1) - p - 1 << endl;


    nodes.clear();
    all_nodes.clear();
    vecmapbits.clear();
    vecleafbits.clear();
   	
   	cout << "done! " << endl;
    this->pointer_size = pointersize;
    this->constructWT(pointersize,depth_sequence);
    this->constructRMQ(frequencies);
    
    delete da;
    delete ll;
   	delete stp;
   	delete bsmap;
   	delete bsleaf;
}

void Topk::constructTree(uint *bitmap,uint size)
{
	this->t = new tree_ff(bitmap,size,OPT_FAST_LCA | OPT_FAST_PREORDER_SELECT | OPT_FAST_LEAF_SELECT);

}
void Topk::constructWT(size_t pointersize,uint *&depth_sequence)
{
	cout << "Constructing Wavelet Tree" << endl;

	this->gd_sequence = depth_sequence;
	
	/*for (int i = 0 ; i < pointersize;i++ )
	{
		cout << this->frequencies[i];
	}
	cout << endl;*/
	//uint *sequence = new uint[pointers.size()];
	Array *A = new Array(depth_sequence,pointersize);
	MapperNone * map = new MapperNone();
	BitSequenceBuilder * bsb = new BitSequenceBuilderRG(30);
	this->d_sequence = new WaveletTreeRMQ(*A, bsb, map);		
	cout << "Done! " << endl;
	delete A;
	// delete []depth_sequence;
	/*for (int i = 0 ; i < pointers.size();i++)
	{
		delete pointers[i];
	}*/
	//pointers.clear();
	//delete[] depth_sequence;
}

void Topk::constructRMQ(vector<uint> &frequencies)
{
	cout << "Constructing RMQ..." << endl;
	cout << "pointer_size = " << this->pointer_size << endl;
	this->freq_array = new uint[this->pointer_size];
	uint *document_array = new uint[this->pointer_size];
	// for (uint i = 0 ; i < this->pointer_size;i++)
	// {
	// 	document_array[i] = this->documents[i];
	// }

	cout << "this->pointer_size = " << this->pointer_size << endl;
	int *wtest = new int[this->pointer_size];
	map<uint,uint> freq_map;
	for(int i = 0 ; i < this->pointer_size;i++)
	{
		map[frequencies[i]]++;
		this->freq_array[i] = frequencies[i];
		wtest[i] = this->max_freq - frequencies[i];
	}

	for (map<uint,uint>::const_iterator it = my_map.begin(); it != my_map.end(); ++it)
	{
		cout << it->first << "\t" << it->second << endl;
	}
	for (int i = 0 ; i <this->pointer_size;i++ )
	{
		cout << "| " << i << " | " << gd_sequence[i] << " | " <<  this->max_freq - this->freq_array[i] << " | " << document_array[i] << endl;
	}

	cout << "Sorting Freqs and documents..." << endl;
	sort(this->gd_sequence,this->freq_array,document_array,this->pointer_size);
	cout << "Done!" << endl;
	for (int i = 0 ; i < this->pointer_size;i++ )
	{
		cout << "| " << i << " | " << gd_sequence[i] << " | " <<  this->max_freq - this->freq_array[i] << " | " << document_array[i] << endl;
	}
	this->d_array = document_array;

	cout << "casted pointer size = " << (int)pointer_size << endl;
	this->rmq = new RMQ(wtest,(int)pointer_size);
	this->freq_dacs = new factorization(this->freq_array,this->pointer_size);
	frequencies.clear();
	cout << "Done! " << endl;
	delete []wtest;
}

double Topk::query(uchar *q,uint size_q)
{
	//start_timing();
	ulong *occ;
	ulong numocc;
	cout << "recevied query:" << q << " size = " << size_q << endl;
	ticsa->locate(q,size_q,&occ,&numocc);
	if (numocc == 0)
	{
		cout << "Nothing found..." << endl;
		return -1;
	}
	cout << "numocc = " << numocc - 1 << endl;
	size_t start_range = ticsa->getISA(occ[0]);
	size_t end_range = ticsa->getISA(occ[numocc-1]);
	// if is a leaf:
	if (start_range == end_range )
	{
		cout << "(LEAF) freq = 1 doc_id = " << this->d->rank1(start_range) << endl;
		double total_time = 1.0000000;
		return total_time;
	}
	
	cout << "start_range = " << start_range << endl;
	cout << "end_range = " << end_range << endl;
	
	uint l1 = this->nodes_preorder[make_pair(start_range,start_range)];
	uint l2 = this->nodes_preorder[make_pair(end_range,end_range)];
	cout << "l1 = " << l1 << endl;
	cout << "l2 = " << l2 << endl;
	uint lca = this->t->Lca(l1,l2);
	cout << "LCA -> " << lca << endl;
	
	uint tdepth;
	uint p;
	uint pp;

	#if 0
	// ALTERNATIVE BEGIN
	size_t r_lca;
	size_t l_lca
	cst->LCA(start_range,start_range,end_range,end_range,&l_lca,&r_lca);
	p = this->nodes_preorder[make_pair(l_lca,r_lca)];
	pp = p + (r_lca - l_lca);
	tdepth = this->cst->TDepth(l_lca,r_lca);
	cout << "p 2 = " << p << endl;
	cout << "pp 2 = " << pp << endl;
	cout << "tdepth2" << tdepth << endl;
	
	
	cout << "s bitmap range 2 = " << this->bitsequence_map->select1(p)- p+1 << endl;
	cout << "e bitmap range 2= " << this->bitsequence_map->select1(pp) - pp << endl;
	// ALTERNATIVE END
	#endif
	
	if (lca == 0)
	{
		p = 1;
		tdepth = 0;
	}
	else
	{
		tdepth = this->t->Depth(lca);
		p = this->t->Preorder_Rank(lca);
	}
	cout << "sub_tree size = " << this->t->Subtree_Size(lca) << endl;
	
	// p++;
	if (p == 0)
		p = 1;
	//pp = p + this->t->Subtree_Size(lca);
	pp = p + end_range - start_range ;
	// pp++;
	
	cout << "p = " << p << endl;
	cout << "pp = " << pp << endl;
	// size_t s_new_range = this->bitsequence_map->select1(p) - p + 2;
	// size_t e_new_range = this->bitsequence_map->select1(pp) -  pp + 1;
	size_t s_new_range = this->bitsequence_map->select1(p) - p + 1;
	size_t e_new_range = this->bitsequence_map->select1(pp) - pp - 1;
	
	// // cout << "tdepth =" << tdepth << endl;
	// cout << "s bitmap range = " << this->bitsequence_map->select1(p)- p+1 << endl;
	// cout << "e bitmap range = " << this->bitsequence_map->select1(pp) - pp << endl;
	if (tdepth != 0) {
		vector<uint> v = this->d_sequence->range_report_2d_aux(s_new_range, e_new_range, 0, tdepth-1, this->rmq);	
		
		cout << "vector size = " << v.size() << endl;
		typedef priority_queue<int,vector<int>,mycomparison> mypq_type;
	  	mypq_type pq (mycomparison(true));
		int heap_capacity = 10;
		uint min = (uint)-1;
		int count = 0;
		for (int i = 0;i< v.size();i++)
		{
			cout << "v[" << i << "] -> " << v[i] << endl;
			if (count == heap_capacity)
			{
				uint dacs_value = this->freq_dacs->access(v[i]+1);
				if (dacs_value > min)
				{
					min = dacs_value;
					pq.pop();
					pq.push(dacs_value);
				}
				else
					continue;
			}
			else
			{
				uint dacs_value = this->freq_dacs->access(v[i]+1);
				cout << " ADDING dacs_value = " << dacs_value << " v[i] = " << v[i] << " | " << this->d_array[v[i]] << endl;
				if (min > dacs_value)
					min = dacs_value;

					pq.push(dacs_value);
					count++;
			}
		}
		while (!pq.empty())
		{
			uint dacs_value = this->freq_dacs->access(pq.top()+1);
		//	cout << "dacs_value = " << this->max_freq - dacs_value << " v[i] = " << pq.top() << " document = " << this->d_array[pq.top()] << endl;
			pq.pop();
 		}
	} else {
		cout << "WTF!" << endl;
	}

	

	
	
	double total_time = 1.0000000;
	cout << "Time = " << total_time*1.0 << endl;
	free(occ);
	return total_time*1.0;
}

Topk::~Topk()
{
	delete this->cst;
	delete[] this->freq_array;
	delete this->rmq;
	delete this->ticsa;
	delete this->d_sequence;
	delete[] this->gd_sequence;
	delete this->t;
	delete []this->d_array;
	delete this->freq_dacs;
	delete this->bitsequence_leaf;
	delete this->bitsequence_map;
	delete this->d;
}
size_t Topk::getSize()
{
	cout << endl;
	cout << "----------------" << endl;
	size_t cst_size = this->cst->getSize();
	size_t csa_size = this->ticsa->getSize();
	size_t wt_size = this->d_sequence->getSize();
	size_t rmq_size = this->rmq->getSize();
	size_t freq_array_size = this->freq_dacs->getSize();
	//size_t map_size = (this->nodes_map.size())/2;
	size_t tree_size = this->t->size();
	size_t map_size = this->bitsequence_map->getSize();
	size_t documents_size = ((this->documents.size()*32)/8)/1.3;
	size_t total = csa_size + wt_size + rmq_size + freq_array_size + map_size + tree_size;
	cout << "CSA SIZE = " << csa_size << endl;
	cout << "WT SIZE = " << wt_size << endl;
	cout << "RMQ SIZE = " << rmq_size << endl;
	cout << "FREQ ARRAY = " << freq_array_size << endl;
	cout << "CST SIZE = " << cst_size<< endl;
	cout << "TREE SIZE = " << tree_size << endl;
	//cout << "FREQ ARRAY 2 = " << this->freq_dacs->getSize() << endl;

	cout << "MAP SIZE = " << map_size << endl;
	cout << "DOCUMENT ARRAY = " << documents_size << endl << endl;

	cout << "TOTAL = " << total << endl;
	cout << "TOTAL (MB) = " << total/(1024.00*1024.00) << endl;
	cout << "RATIO =" << (total*1.0)/(this->length/1.0) << endl;
	return total;

}