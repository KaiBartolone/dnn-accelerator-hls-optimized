#ifndef INPUT_DOUBLE_BUFFER_H
#define INPUT_DOUBLE_BUFFER_H


typedef ac_int<ac::nbits<INPUT_BUFFER_SIZE>::val, false> ibuf_t;

template <int size, int IC0, int OC0>
class InputDoubleBufferWriter{
public:
    InputDoubleBufferWriter(){}

    #pragma hls_design interface
    void CCS_BLOCK(run)(ac_channel<Params> &paramsIn,
                        ac_channel<PackedInt<INPUT_PRECISION, 4> > &din,
                        ac_channel<chanStruct<PackedInt<INPUT_PRECISION,IC0>,size> > &dout)
    {
        // -------------------------------
        // Your code starts here
        
        #ifndef __SYNTHESIS__
        while(paramsIn.available(1))
        #endif
        {
        Params params = paramsIn.read();

        ibuf_t IX0 = (params.OX0 - 1) * params.STRIDE + params.FX;
        ibuf_t IY0 = (params.OY0 - 1) * params.STRIDE + params.FY;
        
        ibuf_t ifmap_max_wadr_c = params.IC1 * IX0 * IY0;
        chanStruct<PackedInt<INPUT_PRECISION,IC0>,size> tmp;
            for (ibuf_t ifmap_wadr = 0; ifmap_wadr < ifmap_max_wadr_c; ifmap_wadr++) {
                for (ibuf_t i = 0; i < IC0; i += 4) {
                    PackedInt<INPUT_PRECISION,4> packedInt = din.read();
                    for (ibuf_t j = 0; j < 4; j++) {
                        tmp.data[ifmap_wadr].value[i+j] = packedInt.value[j];
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
class InputDoubleBufferReader{
public:
    InputDoubleBufferReader(){}

    #pragma hls_design interface
    void CCS_BLOCK(run)(ac_channel<Params> &paramsIn,
                        ac_channel<chanStruct<PackedInt<INPUT_PRECISION, IC0>,size> > &din, 
                        ac_channel<PackedInt<INPUT_PRECISION, IC0> > &dout)
    {
        // -------------------------------
        // Your code starts here
        
        #ifndef __SYNTHESIS__
        while(paramsIn.available(1))
        #endif
        {
        Params params = paramsIn.read();

        ibuf_t IX0 = (params.OX0 - 1) * params.STRIDE + params.FX;
        ibuf_t IY0 = (params.OY0 - 1) * params.STRIDE + params.FY;

        chanStruct<PackedInt<INPUT_PRECISION,IC0>,size> tmp;
            tmp = din.read();
            for (ibuf_t oc1 = 0; oc1 < params.OC1; oc1++) {
                for (ibuf_t ic1 = 0; ic1 < params.IC1; ic1 ++) {
                    for (ibuf_t fy = 0; fy < params.FY; fy++) {
                        for (ibuf_t fx = 0; fx < params.FX; fx++) {
                            for (ibuf_t oy0 = 0; oy0 < params.OY0; oy0++) {
                                for (ibuf_t ox0 = 0; ox0 < params.OX0; ox0++) {
                                    ibuf_t ix0 = ox0 * params.STRIDE + fx;
                                    ibuf_t iy0 = oy0 * params.STRIDE + fy;
                                    dout.write(tmp.data[ic1 * IX0 * IY0 + iy0 * IX0 + ix0]);
                                }
                            }
                        }
                    }
                }
            }
        }
        // Your code ends here
        // -------------------------------
    }
};

template <int size, int IC0, int OC0>
class InputDoubleBuffer{
public:
  InputDoubleBuffer(){}

  #pragma hls_design interface
  void CCS_BLOCK(run)(ac_channel<PackedInt<INPUT_PRECISION, 4> > &inputs_in, 
                      ac_channel<PackedInt<INPUT_PRECISION, IC0> > &inputs_out,
                      ac_channel<Params> &paramsIn)
    {

        Params params = paramsIn.read();

        for (ibuf_t oy1_ox1 = 0; oy1_ox1 < params.OY1 * params.OX1; oy1_ox1++) {
            inputDoubleBufferReaderParams.write(params);
            inputDoubleBufferWriterParams.write(params);
        }

        inputDoubleBufferWriter.run(inputDoubleBufferWriterParams, inputs_in, mem);

        inputDoubleBufferReader.run(inputDoubleBufferReaderParams, mem, inputs_out);
    }

private:
    ac_channel<chanStruct<PackedInt<INPUT_PRECISION, IC0>,size> > mem;
    
    InputDoubleBufferWriter<size, IC0, OC0> inputDoubleBufferWriter;
    ac_channel<Params> inputDoubleBufferWriterParams;
    
    InputDoubleBufferReader<size, IC0, OC0> inputDoubleBufferReader;
    ac_channel<Params> inputDoubleBufferReaderParams;
};

#endif
