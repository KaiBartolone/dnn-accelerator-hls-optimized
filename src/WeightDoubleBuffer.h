#ifndef WEIGHT_DOUBLE_BUFFER_H
#define WEIGHT_DOUBLE_BUFFER_H


template <int size, int IC0, int OC0>
class WeightDoubleBufferWriter{
public:
    WeightDoubleBufferWriter(){}

    #pragma hls_design interface
    void CCS_BLOCK(run)(ac_channel<Params> &paramsIn,
                        ac_channel<PackedInt<WEIGHT_PRECISION, 4> > &din,
                        ac_channel<chanStruct<PackedInt<WEIGHT_PRECISION, OC0>, size> > &dout)
    {
        // -------------------------------
        // Your code starts here
        Params params = paramsIn.read();
        chanStruct<PackedInt<WEIGHT_PRECISION, OC0>, size> tmp;
        for (int oy1_ox1_oc1 = 0; oy1_ox1_oc1 < params.OY1 * params.OX1 * params.OC1; oy1_ox1_oc1++) {
            for (int weight_wadr = 0; weight_wadr < params.IC1 * params.FY * params.FX * IC0; weight_wadr++) {
                for (int i = 0; i < OC0; i += 4) {
                    PackedInt<WEIGHT_PRECISION,4> packedInt = din.read();
                    for (int j = 0; j < 4; j++) {
                        tmp.data[weight_wadr].value[i+j] = packedInt.value[j];
                    }
                }
            }
            dout.write(tmp);
        }
        // Your code ends here
        // -------------------------------
    }
};

template <int size, int IC0, int OC0>
class WeightDoubleBufferReader{
public:
    WeightDoubleBufferReader(){}

    #pragma hls_design interface
    void CCS_BLOCK(run)(ac_channel<Params> &paramsIn,
                        ac_channel<chanStruct<PackedInt<WEIGHT_PRECISION, OC0>,size> > &din, 
                        ac_channel<PackedInt<WEIGHT_PRECISION, OC0> > &dout)
    {
        // -------------------------------
        // Your code starts here
        Params params = paramsIn.read();
        chanStruct<PackedInt<WEIGHT_PRECISION, OC0>,size> tmp;
        for (int oy1_ox1_oc1 = 0; oy1_ox1_oc1 < params.OY1 * params.OX1 * params.OC1; oy1_ox1_oc1++) {
            tmp = din.read();
            for (int weight_radr = 0; weight_radr < params.IC1 * params.FY * params.FX * IC0; weight_radr++) {
                dout.write(tmp.data[weight_radr]);
            }
        }
        // Your code ends here
        // -------------------------------
    }
};

template <int size, int IC0, int OC0>
class WeightDoubleBuffer{
public:
  WeightDoubleBuffer(){}

  #pragma hls_design interface
  void CCS_BLOCK(run)(ac_channel<PackedInt<WEIGHT_PRECISION, 4> > &weights_in, 
                      ac_channel<PackedInt<WEIGHT_PRECISION, OC0> > &weights_out,
                      ac_channel<Params> &paramsIn)
    {
        Params params = paramsIn.read();

        // #ifndef __SYNTHESIS__
        // ac_int<ac::log2_ceil<size>::val, false> block_size = IC0*params.IC1*params.FX*params.FY;
        // assert(block_size <= size);
        // #endif

        weightDoubleBufferReaderParams.write(params);
        weightDoubleBufferWriterParams.write(params);

        weightDoubleBufferWriter.run(weightDoubleBufferWriterParams, weights_in, mem);
        weightDoubleBufferReader.run(weightDoubleBufferReaderParams, mem, weights_out);
    }

private:
    ac_channel<chanStruct<PackedInt<WEIGHT_PRECISION, OC0>,size> > mem;
    
    WeightDoubleBufferWriter<size, IC0, OC0> weightDoubleBufferWriter;
    ac_channel<Params> weightDoubleBufferWriterParams;
    
    WeightDoubleBufferReader<size, IC0, OC0> weightDoubleBufferReader;
    ac_channel<Params> weightDoubleBufferReaderParams;
};


#endif
