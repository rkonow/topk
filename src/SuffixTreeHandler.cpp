class SuffixTreeHandler
{

	public:
	SuffixTree *cst;
	size_t size;
	//tree_ff *tf;

	SuffixTreeHandler(SuffixTreeY *&cst,size_t n)
	{
		cout << "constructing handler " << endl;
		cout << "size = " << n << endl;
		this->cst = cst;
		this->size = n;
	}

	vector< pair<size_t,size_t> > getAllNodes(size_t pl,size_t pr)
	{
		vector <pair<size_t,size_t> > result;
		vector <pair<size_t,size_t> > n;
		n.push_back(make_pair(pl,pr));
		size_t next_vl,next_vr;
		size_t level = 0;
		while (n.size() != 0)
		{
			size_t pl_aux = n.front().first;
			size_t pr_aux = n.front().second;
			//size_t level_aux = n.front()->level;
			result.push_back(make_pair(pl_aux,pr_aux));
			n.erase(n.begin());
			cst->FChild(pl_aux, pr_aux, &next_vl, &next_vr);
			if (next_vl != (size_t)-1 && next_vr != (size_t) - 1)
			{
				n.insert(n.end(),make_pair(next_vl,next_vr));
				while(1)
				{
					cst->NSibling(next_vl, next_vr, &pl_aux, &pr_aux);
					if (pl_aux != (size_t) - 1 && pr_aux != (size_t) - 1)
					{
						next_vl = pl_aux;
						next_vr = pr_aux;
						n.insert(n.end(),make_pair(pl_aux,pr_aux));
					}
					else
					{
						break;
					}
				}			
			}
		}
		//generateBitmap();
		return result;
	}

	void generateBitmapAux(pair <size_t,size_t> nodes,vector<uint> &result,vector<pair<size_t,size_t> > &preorder_map,vector<pair<size_t,size_t> > &backtracing)
	{
		pair <size_t,size_t> r;
		//cout << "waza2" << endl;
		//cout << "size = " << nodes.size() << endl;
		//cout << "entering node = " << i << endl;
		size_t pl_aux = nodes.first;
		size_t pr_aux = nodes.second;
		// if (pl_aux == (size_t)-1 || pr_aux == (size_t)-1)
		// 	return;
	
		// cout << pl_aux << " , " << pr_aux << endl;
		preorder_map.push_back(make_pair(pl_aux,pr_aux));
		size_t next_vl,next_vr;
		size_t nsibling_l,nsibling_r;
		cst->FChild(pl_aux, pr_aux, &next_vl, &next_vr);
		cst->NSibling(pl_aux,pr_aux,&nsibling_l,&nsibling_r);
		r = make_pair(next_vl,next_vr);
		if (next_vl != (size_t)-1 && next_vr != (size_t)-1)
		{
			// cout << "(";
			result.push_back(0);
			generateBitmapAux(r,result,preorder_map,backtracing);
			// cout << ")";
			result.push_back(1);
			pl_aux = next_vl;
			pr_aux = next_vr;
		}
		if (nsibling_l != (size_t)-1 && nsibling_r != (size_t)-1)
		{
			r = make_pair(nsibling_l,nsibling_r);
//				backtracing.push_back(r);
			// cout << "(";
			result.push_back(0);
			generateBitmapAux(r,result,preorder_map,backtracing);
			result.push_back(1);
			// cout << ")";
		}
		
	//}
/*		if (backtracing.size() != 0)
		{
			pair<size_t,size_t> next_one = backtracing.back();
			cout << "(";
			r = make_pair(next_one.first,next_one.second);
			backtracing.erase(backtracing.end());
			generateBitmapAux(r,result,preorder_map,backtracing);
			cout << ")";
		}
*/
			//if (next_one.first == (size_t)-1 || next_one.second == (size_t)-1)
			//	return;
	//	cout << "(";
	//	cout << ")";
	}

	
		
		
	 #define W 32
	 #define bitget(e,p) ((((e)[(p)/W] >> ((p)%W))) & 1)
	pair<uint *,uint> generateBitmap(vector<pair<size_t,size_t> > &preorder_map)
	{
		cout << "waza" << endl;
		size_t a,b;
		cst->Root(&a,&b);
		vector<uint> result;
		result.push_back(0);
		cout << "(";
		vector<pair<size_t,size_t> > backtracing;
		generateBitmapAux(make_pair(a,b),result,preorder_map,backtracing);
		result.push_back(1);
		cout << ")";
		cout << "result.size = " << result.size() << endl;
		uint *bitmap = new uint[(int)(result.size()/32+1)];
		for (int i = 0 ; i < result.size()/32+1;i++)
		{
			bitmap[i] = 0u;
		}
		for (int i = 0 ; i < result.size();i++)
		{
			cout << result[i];
			if (result[i] == 1)
			{
				// cout << "setting bit " << i << " to 1" << endl;
				int array_pos = i/32;
				int pos = i%32;
				// cout << "array_pos = " << array_pos << endl;
				// cout << "pos = " << pos << endl;
		        bitmap[array_pos] = bitmap[array_pos] | (1u<<(pos));
			}
		}

		// for (int i=0;i<result.size();i++)
		// {
		// 	if (bitget(bitmap,i))
		// 		cout << "1";
		// 	else
		// 		cout << "0";
		// }
		return make_pair(bitmap,result.size());

	}

	vector <pair<size_t,size_t> > getChildsNodes(size_t pl,size_t pr)
	{
		vector <pair<size_t,size_t> > result;
		vector <pair<size_t,size_t> > n;
		n.push_back(make_pair(pl,pr));
		size_t next_vl,next_vr;
		size_t level = 0;
		while (n.size() != 0)
		{
			size_t pl_aux = n.front().first;
			size_t pr_aux = n.front().second;
			//size_t level_aux = n.front()->level;
			//result.push_back(make_pair(pl_aux,pr_aux));
			n.erase(n.begin());
			cst->FChild(pl_aux, pr_aux, &next_vl, &next_vr);
			if (next_vl != (size_t)-1 && next_vr != (size_t) - 1)
			{
				n.push_back(make_pair(next_vl,next_vr));
				while(1)
				{
					cst->NSibling(next_vl, next_vr, &pl_aux, &pr_aux);
					if (pl_aux != (size_t) - 1 && pr_aux != (size_t) - 1)
					{
						next_vl = pl_aux;
						next_vr = pr_aux;
						n.insert(n.begin(),make_pair(pl_aux,pr_aux));
					}
					else
					{
						break;
					}
				}			
			}
			else
			{
				result.push_back(make_pair(pl_aux,pr_aux));
			//	cout << "adding : " << pl_aux << " , " << pr_aux << endl;
			}
		}

		return result;
	}

	BitSequenceRG * buildBs(size_t *file_sizes,size_t n,bool random=false)
	{
		uint sum = 0;
		BitString *bs = new BitString(this->size+1);
		if (random)
		{
			srand ( time(NULL) );
			for (int i = 0 ; i < n;i++)
			{
				sum = rand() % (this->size);
				cout << "Setting random bit " << sum << " to 1" << endl;
				bs->setBit(sum);
			}
		}
		else
		{
			for (int i = 1;i<n;i++)
			{
				sum += file_sizes[i-1];
				cout << "setting bit " << sum << " to 1" << endl;
				bs->setBit(sum);
			}
		}
		BitSequenceRG *bsrg = new BitSequenceRG(*bs,20);
		delete bs;
		return bsrg;		
	}
};