#ifndef _WVTREE_RMQ_H
#define _WVTREE_RMQ_H

#include <WaveletTreeNoptrs.h>
#include <rmq.h>
using namespace rmq_space;
namespace cds_static
{
	class WaveletTreeRMQ : public Sequence
	{
		 public:

            /** Builds a Wavelet Tree for the string stored in a.
             * @param bmb builder for the bitmaps in each level.
             * @param am alphabet mapper (we need all symbols to be used).
             * */
            /** Builds a Wavelet Tree for the string
             * pointed by symbols assuming its length
             * equals n and uses bmb to build the bitsequence
             * @param bmb builder for the bitmaps in each level.
             * @param am alphabet mapper (we need all symbols to be used).
             * */
            WaveletTreeRMQ(const Array & a, BitSequenceBuilder * bmb, Mapper * am,uint *weight);
            WaveletTreeRMQ(uint * symbols, size_t n, BitSequenceBuilder * bmb, Mapper * am, uint *weight, bool deleteSymbols = false);
            // 
            /** Builds a Wavelet Tree for the string
             * pointed by symbols is an array of elements of "width" bits and length
             * n. 
             * @param bmb builder for the bitmaps in each level.
             * @param am alphabet mapper (we need all symbols to be used).
             * */
            /** Destroys the Wavelet Tree */
            virtual ~WaveletTreeRMQ();
            vector<uint>  range_report_aux(size_t x_start,size_t x_end,size_t value);
            void range_report(uint lev, size_t x_start, size_t x_end,uint sym, size_t start, size_t end,vector <uint> &result,size_t value);
            vector<uint>  range_report_2d_aux(size_t x_start,size_t x_end,size_t y_start,size_t y_end,RMQ *&rmq);
            void range_report_2d(uint lev, size_t x_start, size_t x_end,size_t y_start, size_t y_end,uint sym, size_t start, size_t end, vector<uint> &result,size_t freq,RMQ *&r);

            vector<uint>  rr2d_aux(size_t x_start,size_t x_end,size_t y_start,size_t y_end,size_t y_start_aux, size_t y_end_aux);
            void rr2d(uint lev, size_t x_start, size_t x_end,size_t y_start, size_t y_end,size_t y_start_aux, size_t y_end_aux, uint sym, size_t start, size_t end, vector<uint> &result);



            size_t getSizeRMQ();

            virtual size_t rank(uint symbol, size_t pos) const;
            virtual size_t select(uint symbol, size_t j) const;
            virtual uint access(size_t pos) const;
            virtual size_t getSize() const;

            virtual size_t count(uint symbol) const;

            virtual void save(ofstream & fp) const;
            static WaveletTreeRMQ * load(ifstream & fp);

        protected:
            WaveletTreeRMQ();

            Mapper * am;
            /** Only one bit-string for the Wavelet Tree. */
            BitSequence **bitstring, *occ;

            /** Length of the string. */
            size_t n;

            /** Height of the Wavelet Tree. */
            uint height, max_v;
            RMQ **rmq;
            /** Obtains the maximum value from the string
             * symbols of length n */
            uint max_value(uint * symbols, size_t n);
            uint max_value(uint * symbols, unsigned width, size_t n);

            /** How many bits are needed to represent val */
            uint bits(uint val);

            /** Returns true if val has its ind-th bit set
             * to one. */
            bool is_set(uint val, uint ind) const;

            /** Sets the ind-th bit in val */
            uint set(uint val, uint ind) const;

            /** Recursive function for building the Wavelet Tree. */
            void build_level(uint **bm, uint *symbols, uint level, uint length, uint offset);
            void build_level(uint **bm, uint *symbols, unsigned width, uint level, uint length, uint offset);
            void sortWeights(uint *weight);
    };
};
#endif