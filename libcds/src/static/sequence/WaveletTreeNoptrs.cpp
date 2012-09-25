/* WaveletTreeNoptrs.cpp
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * WaveletTreeNoptrs definition
 *
 * This library is free software; you can redistribute it and/or
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

#include <WaveletTreeNoptrs.h>
namespace cds_static
{
    WaveletTreeNoptrs::WaveletTreeNoptrs(const Array & a, BitSequenceBuilder * bmb, Mapper * am) : Sequence(0) {
        bmb->use();
        this->n=a.getLength();
        this->length = n;
        this->am=am;
        am->use();
        uint * symbols = new uint[n];
        for(size_t i=0;i<n;i++)
            symbols[i] = am->map(a[i]);
        max_v=am->map(a.getMax());
        height=bits(max_v);
        uint *occurrences=new uint[max_v+1];
        for(uint i=0;i<=max_v;i++) occurrences[i]=0;
        for(uint i=0;i<n;i++)
            occurrences[symbols[i]]++;
        uint to_add=0;
        for(uint i=0;i<max_v;i++)
            if(occurrences[i]==0) to_add++;
        uint * new_symb = new uint[n+to_add];
        for(uint i=0;i<n;i++)
            new_symb[i] = symbols[i];

        delete [] symbols;

        to_add = 0;
        for(uint i=0;i<max_v;i++)
        if(occurrences[i]==0) {
            occurrences[i]++;
            new_symb[n+to_add]=i;
            to_add++;
        }
        uint new_n = n+to_add;
        for(uint i=1;i<=max_v;i++)
            occurrences[i] += occurrences[i-1];
        uint *oc = new uint[(new_n+1)/W+1];
        for(uint i=0;i<(new_n+1)/W+1;i++)
            oc[i] = 0;
        for(uint i=0;i<=max_v;i++)
            bitset(oc,occurrences[i]-1);
        bitset(oc,new_n);
        occ = bmb->build(oc,new_n+1);
        delete [] occurrences;
        this->n = new_n;
        uint ** _bm=new uint*[height];
        for(uint i=0;i<height;i++) {
            _bm[i] = new uint[new_n/W+1];
            for(uint j=0;j<new_n/W+1;j++)
                _bm[i][j]=0;
        }
        build_level(_bm,new_symb,0,new_n,0);
        bitstring = new BitSequence*[height];
        for(uint i=0;i<height;i++) {
            bitstring[i] = bmb->build(_bm[i],new_n);
            delete [] _bm[i];
        }
        delete [] _bm;

        // delete [] new_symb; // already deleted in build_level()!
        delete [] oc;
        bmb->unuse();
    }

    WaveletTreeNoptrs::WaveletTreeNoptrs(uint * symbols, size_t n, BitSequenceBuilder * bmb, Mapper * am, bool deleteSymbols) : Sequence(n) {
        bmb->use();
        this->n=n;
        this->am=am;
        am->use();
        for(uint i=0;i<n;i++)
            symbols[i] = am->map(symbols[i]);
        max_v=max_value(symbols,n);
        height=bits(max_v);
        uint *occurrences=new uint[max_v+1];
        for(uint i=0;i<=max_v;i++) occurrences[i]=0;
        for(uint i=0;i<n;i++)
            occurrences[symbols[i]]++;
        uint to_add=0;
        for(uint i=0;i<max_v;i++)
            if(occurrences[i]==0) to_add++;
        uint * new_symb = new uint[n+to_add];
        for(uint i=0;i<n;i++)
            new_symb[i] = symbols[i];

        if (deleteSymbols) {
            delete [] symbols;
            symbols = 0;
        }

        to_add = 0;
        for(uint i=0;i<max_v;i++)
        if(occurrences[i]==0) {
            occurrences[i]++;
            new_symb[n+to_add]=i;
            to_add++;
        }
        uint new_n = n+to_add;
        for(uint i=1;i<=max_v;i++)
            occurrences[i] += occurrences[i-1];
        uint *oc = new uint[(new_n+1)/W+1];
        for(uint i=0;i<(new_n+1)/W+1;i++)
            oc[i] = 0;
        for(uint i=0;i<=max_v;i++)
            bitset(oc,occurrences[i]-1);
        bitset(oc,new_n);
        occ = bmb->build(oc,new_n+1);
        delete [] occurrences;
        this->n = new_n;
        uint ** _bm=new uint*[height];
        for(uint i=0;i<height;i++) {
            _bm[i] = new uint[new_n/W+1];
            for(uint j=0;j<new_n/W+1;j++)
                _bm[i][j]=0;
        }
        build_level(_bm,new_symb,0,new_n,0);
        bitstring = new BitSequence*[height];
        for(uint i=0;i<height;i++) {
            bitstring[i] = bmb->build(_bm[i],new_n);
            delete [] _bm[i];
        }
        delete [] _bm;

        if (!deleteSymbols)
            for(uint i=0;i<n;i++)
                symbols[i] = am->unmap(symbols[i]);

        // delete [] new_symb; // already deleted in build_level()!
        delete [] oc;
        bmb->unuse();
    }

    // symbols is an array of elements of "width" bits
    WaveletTreeNoptrs::WaveletTreeNoptrs(uint * symbols, size_t n, uint width, BitSequenceBuilder * bmb, Mapper * am, bool deleteSymbols) : Sequence(n) {
        bmb->use();
        this->n=n;
        this->am=am;
        am->use();
        for(uint i=0;i<n;i++)
            set_field(symbols, width, i, am->map(get_field(symbols, width, i)));
        max_v=max_value(symbols, width, n);
        height=bits(max_v);
        uint *occurrences=new uint[max_v+1];
        for(uint i=0;i<=max_v;i++) occurrences[i]=0;
        for(uint i=0;i<n;i++)
            occurrences[get_field(symbols, width, i)]++;
        uint to_add=0;
        for(uint i=0;i<max_v;i++)
            if(occurrences[i]==0) to_add++;
        uint * new_symb = new uint[((n+to_add)*width)/W + 1];
        for(uint i=0;i<n;i++)
            set_field(new_symb, width, i, get_field(symbols, width, i));

        if (deleteSymbols) {
            delete [] symbols;
            symbols = 0;
        }

        to_add = 0;
        for(uint i=0;i<max_v;i++)
        if(occurrences[i]==0) {
            occurrences[i]++;
            set_field(new_symb, width, n+to_add, i);
            to_add++;
        }
        uint new_n = n+to_add;
        for(uint i=1;i<=max_v;i++)
            occurrences[i] += occurrences[i-1];
        uint *oc = new uint[(new_n+1)/W+1];
        for(uint i=0;i<(new_n+1)/W+1;i++)
            oc[i] = 0;
        for(uint i=0;i<=max_v;i++)
            bitset(oc,occurrences[i]-1);
        bitset(oc,new_n);
        occ = bmb->build(oc,new_n+1);
        delete [] occurrences;
        this->n = new_n;
        uint ** _bm=new uint*[height];
        for(uint i=0;i<height;i++) {
            _bm[i] = new uint[new_n/W+1];
            for(uint j=0;j<new_n/W+1;j++)
                _bm[i][j]=0;
        }
        build_level(_bm,new_symb,width,0,new_n,0);
        bitstring = new BitSequence*[height];
        for(uint i=0;i<height;i++) {
            bitstring[i] = bmb->build(_bm[i],new_n);
            delete [] _bm[i];
        }
        delete [] _bm;

        if (!deleteSymbols)
            for(uint i=0;i<n;i++)
                set_field(symbols, width, i, am->unmap(get_field(symbols, width, i)));

        // delete [] new_symb; // already deleted in build_level()!
        delete [] oc;
        bmb->unuse();
    }

    WaveletTreeNoptrs::WaveletTreeNoptrs():Sequence(0) {
        bitstring = NULL;
        occ = NULL;
        am = NULL;
    }

    WaveletTreeNoptrs::~WaveletTreeNoptrs() {
        if(bitstring) {
            for(uint i=0;i<height;i++)
                if(bitstring[i])
                    delete bitstring[i];
            delete [] bitstring;
        }
        if(occ)
            delete occ;
        if(am)
            am->unuse();
    }

    void WaveletTreeNoptrs::save(ofstream & fp) const
    {
        uint wr = WVTREE_NOPTRS_HDR;
        saveValue(fp,wr);
        saveValue<size_t>(fp,n);
        saveValue(fp,max_v);
        saveValue(fp,height);
        am->save(fp);
        for(uint i=0;i<height;i++)
            bitstring[i]->save(fp);
        occ->save(fp);
    }

    WaveletTreeNoptrs * WaveletTreeNoptrs::load(ifstream & fp) {
        uint rd = loadValue<uint>(fp);
        if(rd!=WVTREE_NOPTRS_HDR) return NULL;
        WaveletTreeNoptrs * ret = new WaveletTreeNoptrs();
        ret->n = loadValue<size_t>(fp);
	ret->length = ret->n;
        ret->max_v = loadValue<uint>(fp);
        ret->height = loadValue<uint>(fp);
        ret->am = Mapper::load(fp);
        if(ret->am==NULL) {
            delete ret;
            return NULL;
        }
        ret->am->use();
        ret->bitstring = new BitSequence*[ret->height];
        for(uint i=0;i<ret->height;i++) 
            ret->bitstring[i] = NULL;
        for(uint i=0;i<ret->height;i++) {
            ret->bitstring[i] = BitSequence::load(fp);
            if(ret->bitstring[i]==NULL) {
                cout << "damn" << i << " " << ret->height << endl;
                delete ret;
                return NULL;
            }
        }
        ret->occ = BitSequence::load(fp);
        if(ret->occ==NULL) {
            delete ret;
            return NULL;
        }
        return ret;
    }

    inline uint get_start(uint symbol, uint mask) {
        return symbol&mask;
    }

    inline uint get_end(uint symbol, uint mask) {
        return get_start(symbol,mask)+!mask+1;
    }

    bool WaveletTreeNoptrs::is_set(uint val, uint ind) const
    {
        assert(ind<height);
        return (val & (1<<(height-ind-1)))!=0;
    }

    uint WaveletTreeNoptrs::set(uint val, uint ind) const
    {
        assert(ind<=height);
        return val | (1<<(height-ind-1));
    }

    vector<uint>  WaveletTreeNoptrs::range_report_aux(size_t x_start,size_t x_end,size_t value)
    {
        vector<uint> result;
        //      result.reserve(1500);
        range_report(0,x_start,x_end,0,0,n-1,result,value);
        return result;
    }

/*

    priority_queue<uint> topk_aux(uint x_start,uint x_end,priority_queue<uint> &q,int k)
    {
         uint max = 0;
         if (bits(k) <= 2)
             return q;
         //max = query(x_start,x_end);
       //  q.push(qeuery(x_start,x_end));
         topk_aux(x_start,max-1,q,--k);
         topk_aux(max+1,x_end,q,--k);
    }


    priority_queue<uint> topk(uint x_start,uint x_end,int k)
    {
        priority_queue<uint> q;
        return topk_aux(x_start,x_end,q,k);
    }
   */
    priority_queue<uint>  WaveletTreeNoptrs::range_report_2d_aux(size_t x_start,size_t x_end,size_t y_start,size_t y_end)
    {
        //RMQ *t = new RMQ();
        priority_queue<uint> result;
        //      result.reserve(1500);
        range_report_2d(0,x_start,x_end,y_start,y_end,0,0,n-1,result,0);
        return result;
    }

    void WaveletTreeNoptrs::range_report_2d(uint lev, size_t x_start, size_t x_end,size_t y_start, size_t y_end,uint sym, size_t start, size_t end,priority_queue<uint> &result,size_t freq)
    {
        size_t x_start_left,x_start_right,x_end_left,x_end_right;
        size_t before;
        size_t end_left;
        size_t end_right;
        size_t start_left;
        size_t start_right;
        // cout << "x_start = " << x_start << endl;
        // cout << "x_end = " << x_end << endl;
        // cout << "start = " << start << endl;
        // cout << "end = " << end << endl; 
        //  cout << "x_start= " <<  x_start << " x_end= " << x_end << endl;
        if (x_start > x_end || x_end > n-1)     
            return;

        if (am->unmap(sym) > y_end)
            return;
        if (lev < height)
        {
            BitSequence* bs = bitstring[lev];
            cout << "[" << y_start << ", " << y_end << "] " << endl;
            cout << "sym (map) = " << am->unmap(sym) << endl;
            cout << "sym = " << sym << endl;
            //cout << "LEVEl " << lev << endl;
            //cout << "start -> " << start << " end -> " << end << endl;
            if (start==0) before=0;
            else before = bs->rank1(start-1);

                size_t rank_left = bs->rank1(start+x_start-1);
                size_t rank_right = bs->rank1(start+x_end);
               
                freq = (end - start + 1) - (rank_right - rank_left);
                x_start_left = x_start - (rank_left - before);
                x_end_left = x_end - (rank_right - before);
                end_left = end - (bs->rank1(end) - before);
                start_left = start;
                //cout << "izquierda | x_start_left = " << x_start_left << " x_end_left = " << x_end_left << endl;
                range_report_2d(lev+1,x_start_left,x_end_left,y_start,y_end,sym,start_left,end_left,result,freq);


                sym=set(sym,lev);
                if (am->unmap(sym) > y_end)
                    return;                       
                freq = rank_right - rank_left;
                start_right = end - (bs->rank1(end)-before) + 1;
                end_right = end;
                x_start_right = rank_left - before ;
                x_end_right = rank_right - before-1;   
                //cout << "derecha | x_start_left = " << x_start_right << " x_end_left = " << x_end_right << endl;
                range_report_2d(lev+1,x_start_right,x_end_right,y_start,y_end,sym,start_right,end_right,result,freq);

        }
        else
        {   
                if (x_start > x_end)
                return;
                BitSequence* bs = bitstring[lev-1];
                if (am->unmap(sym) <= y_end && am->unmap(sym) >= y_start)
                {
                    // cout << "SYMBOL = " << sym << endl << endl;
            
                    // cout << "start = " << start << " , " << " end = " << end << endl;
                    // cout << "start_aux = " << x_start << " , " << " end_aux = " << x_end << endl;
                    // cout << "rank0 start" << bs->rank0(start) << endl;
                    // cout << "rank0 end" << bs->rank0(end) << endl;
                    // cout << "rank1 start" << bs->rank1(start) << endl;
                    // cout << "rank1 end" << bs->rank1(end) << endl;
                    // cout << "r0 = " << bs->rank0(end) - bs->rank0(start) << endl;
                    // cout << "r1 = " << bs->rank1(end) - bs->rank1(start) << endl;
                    // //cout << "position in " << bs->select1(bs->rank1(end) +  ) << endl;
                    uint q_start;
                    uint q_end;
                    if (x_start == x_end)
                    {
                        q_start = start+x_start;
                        q_end = q_start+1;
                    }
                    else
                    {
                        q_start = start+x_start;
                        q_end = start+x_end;
                        // for (int i = x_start;i <= x_end;i++)
                        // {
                        //     cout << "symbol " << sym << "occurence " << i << " on position " << start+i << endl;
                        // }
                    }

                    int k = 10;
                    vector<priority_queue<uint> > v_queue;
                   // v_queue.push_back(topk(q_start,q_end,10));
                    for (int i = 0; i< v_queue.size();i++)
                    {
                        while(!v_queue[i].empty())
                        {
                            result.push(v_queue[i].top());
                            v_queue[i].pop();
                        }
                    }

                    // cout << "Adding -> " << am->unmap(sym) << " -> freq = " << freq << endl;
                    // cout << "------------------------------------------" << endl << endl;
                }
                else
                    return;
               // }
        }   

    }
    void WaveletTreeNoptrs::range_report(uint lev, size_t x_start, size_t x_end,uint sym, size_t start, size_t end,vector <uint> &result,size_t value)
    {
        size_t x_start_left,x_start_right,x_end_left,x_end_right;
        size_t before;
        size_t end_left;
        size_t end_right;
        size_t start_left;
        size_t start_right;
        // cout << "x_start = " << x_start << endl;
        // cout << "x_end = " << x_end << endl;
        // cout << "start = " << start << endl;
        // cout << "end = " << end << endl; 
        //  cout << "x_start= " <<  x_start << " x_end= " << x_end << endl;
        if (x_start > x_end || x_end > n-1)     
            return;

        if (lev < height)
        {
            BitSequence* bs = bitstring[lev];
            //cout << "LEVEl " << lev << endl;
            //cout << "start -> " << start << " end -> " << end << endl;
            if (start==0) before=0;
            else before = bs->rank1(start-1);

                size_t rank_left = bs->rank1(start+x_start-1);
                size_t rank_right = bs->rank1(start+x_end);
                x_start_left = x_start - (rank_left - before);
                x_end_left = x_end - (rank_right - before);
                end_left = end - (bs->rank1(end) - before);
                start_left = start;
                //cout << "izquierda | x_start_left = " << x_start_left << " x_end_left = " << x_end_left << endl;
                range_report(lev+1,x_start_left,x_end_left,sym,start_left,end_left,result,value);

                sym=set(sym,lev);                       
                start_right = end - (bs->rank1(end)-before) + 1;
                end_right = end;
                x_start_right = rank_left - before ;
                x_end_right = rank_right - before-1;   
                //cout << "derecha | x_start_left = " << x_start_right << " x_end_left = " << x_end_right << endl;
                range_report(lev+1,x_start_right,x_end_right,sym,start_right,end_right,result,value);

        }
        else
        {   
                if (x_start > x_end)
                return;
                //if (am->unmap(sym) > value)
                //    return;

                // BitSequence* bs = bitstring[lev-1];
                // result.push_back(am->unmap(sym));
                // cout << "x_start = " << x_start << endl;
                // cout << "x_end = " << x_end << endl;
                // cout << "start = " << start << endl;
                // cout << "end = " << end << endl; 
                //cout << "length = " << bs->getLength() << endl;
               // cout << "rank1=" << bs->rank0(x_end) << endl;
               //                    cout << "position = " << this->select(am->unmap(sym),bs->rank0(x_end)) + (x_end - x_start) + i << endl;
                    cout << "Adding -> " << am->unmap(sym) << " , "  << endl;
               // }
        }   

    }

    uint WaveletTreeNoptrs::access(size_t pos) const
    {
        uint level=0;
        uint ret=0;
        size_t start=0;
        size_t end=n-1;
        while(level<height) {
            assert(pos>=start && pos<=end);
            if(bitstring[level]->access(pos)) {
                ret=set(ret,level);
                pos=bitstring[level]->rank1(pos-1)-bitstring[level]->rank1(start-1);
                start=(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
                start=end-start+1;
                pos+=start;
            }
            else {
                pos=pos-start-(bitstring[level]->rank1(pos)-bitstring[level]->rank1(start-1));
                end=end-start-(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
                end+=start;
                pos+=start;
            }
            level++;
        }
        return am->unmap(ret);
    }

    size_t WaveletTreeNoptrs::rank(uint symbol, size_t pos) const
    {
        symbol = am->map(symbol);
        uint level=0;
        size_t start=0;
        size_t end=n-1;
        size_t count=0;
        while(level<height) {
            if(is_set(symbol,level)) {
                pos=bitstring[level]->rank1(pos)-bitstring[level]->rank1(start-1)-1;
                count=pos+1;
                start=(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
                start=end-start+1;
                pos+=start;
            }
            else {
                pos=pos-start+bitstring[level]->rank1(start-1)-bitstring[level]->rank1(pos);
                count=pos+1;
                end=end-start-(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
                end+=start;
                pos+=start;
            }
            level++;
            if(count==0) return 0;
        }
        return count;
    }

    size_t WaveletTreeNoptrs::select(uint symbol, size_t j) const
    {
        symbol = am->map(symbol);
        uint mask = (1<<height)-2;
        uint sum=2;
        uint level = height-1;
        size_t pos=j;
        while(true) {
            size_t start = get_start(symbol,mask);
            size_t end = min((size_t)max_v+1,start+sum);
            start = (start==0)?0:(occ->select1(start)+1);
            end = occ->select1(end+1)-1;
            if(is_set(symbol,level)) {
                uint ones_start = bitstring[level]->rank1(start-1);
                pos = bitstring[level]->select1(ones_start+pos)-start+1;
            }
            else {
                uint ones_start = bitstring[level]->rank1(start-1);
                pos = bitstring[level]->select0(start-ones_start+pos)-start+1;
            }
            mask <<=1;
            sum <<=1;
            if(level==0) break;
            level--;
        }
        return pos-1;
    }

    size_t WaveletTreeNoptrs::getSize() const
    {
        size_t ptrs = sizeof(WaveletTreeNoptrs)+height*sizeof(Sequence*);
        size_t bytesBitstrings = 0;
        for(uint i=0;i<height;i++)
            bytesBitstrings += bitstring[i]->getSize();
        return bytesBitstrings+occ->getSize()+ptrs;
    }

    void WaveletTreeNoptrs::build_level(uint **bm, uint *symbols, uint level, uint length, uint offset) {
        if(level==height) {
            delete [] symbols;
            return;
        }
        uint cleft=0;
        for(size_t i=0;i<length;i++)
            if(!is_set(symbols[i],level))
                cleft++;
        uint cright=length-cleft;
        uint *left=new uint[cleft], *right=new uint[cright];
        cleft=cright=0;
        for(size_t i=0;i<length;i++)
        if(!is_set(symbols[i],level)) {
            left[cleft++]=symbols[i];
            bitclean(bm[level],offset+i);
        }
        else {
            right[cright++]=symbols[i];
            bitset(bm[level],offset+i);
        }

        delete [] symbols;
        symbols = 0;

        build_level(bm,left,level+1,cleft,offset);
        left = 0;                // Gets deleted in recursion.
        build_level(bm,right,level+1,cright,offset+cleft);
        right = 0;               // Gets deleted in recursion.
        //delete [] left;
        //delete [] right;
    }

    // symbols is an array of elements of "width" bits.
    void WaveletTreeNoptrs::build_level(uint **bm, uint *symbols, unsigned width, uint level, uint length, uint offset) {
        if(level==height) {
            delete [] symbols;
            return;
        }
        uint cleft=0;
        for(size_t i=0;i<length;i++)
            if(!is_set(get_field(symbols, width, i),level))
                cleft++;
        uint cright=length-cleft;
        uint *left=new uint[(cleft*width)/W + 1],
            *right=new uint[(cright*width)/W + 1];
        cleft=cright=0;
        for(size_t i=0;i<length;i++)
        if(!is_set(get_field(symbols,width,i),level)) {
            set_field(left,width,cleft++,get_field(symbols, width,i));
            bitclean(bm[level],offset+i);
        }
        else {
            set_field(right,width,cright++,get_field(symbols,width,i));
            bitset(bm[level],offset+i);
        }

        delete [] symbols;
        symbols = 0;

        build_level(bm,left,width,level+1,cleft,offset);
        left = 0;                // Gets deleted in recursion.
        build_level(bm,right,width,level+1,cright,offset+cleft);
        right = 0;               // Gets deleted in recursion.
        //delete [] left;
        //delete [] right;
    }

    uint WaveletTreeNoptrs::max_value(uint *symbols, size_t n) {
        uint max_v = 0;
        for(size_t i=0;i<n;i++)
            max_v = max(symbols[i],max_v);
        return max_v;
    }

    uint WaveletTreeNoptrs::max_value(uint *symbols, unsigned width, size_t n) {
        uint max_v = 0;
        for(size_t i=0;i<n;i++)
            max_v = max(get_field(symbols, width, i),max_v);
        return max_v;
    }

    uint WaveletTreeNoptrs::bits(uint val) {
        uint ret = 0;
        while(val!=0) {
            ret++;
            val >>= 1;
        }
        return ret ;
    }

    size_t WaveletTreeNoptrs::count(uint symbol) const
    {
        return occ->select1(am->map(symbol))-occ->select1(am->map(symbol)-1)+1;
    }

};
