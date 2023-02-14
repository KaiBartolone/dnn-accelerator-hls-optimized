set blockname [file rootname [file tail [info script] ]]

source scripts/common.tcl

directive set -DESIGN_HIERARCHY " 
    {InputDoubleBuffer<4096, ${ARRAY_DIMENSION}, ${ARRAY_DIMENSION}>} 
"

go compile

source scripts/set_libraries.tcl

go libraries
directive set -CLOCKS $clocks

go assembly

# -------------------------------
# Set the correct word widths and the stage replication
# Your code starts here
directive set /InputDoubleBuffer<4096,16,16>/InputDoubleBufferReader<4096,16,16>/din -WORD_WIDTH 128
directive set /InputDoubleBuffer<4096,16,16>/InputDoubleBufferWriter<4096,16,16>/dout -WORD_WIDTH 128
directive set /InputDoubleBuffer<4096,16,16>/mem -WORD_WIDTH 128
directive set /InputDoubleBuffer<4096,16,16>/mem:cns -STAGE_REPLICATION 2
directive set /InputDoubleBuffer<4096,16,16>/InputDoubleBufferReader<4096,16,16>/run/tmp.data.value -WORD_WIDTH 128
directive set /InputDoubleBuffer<4096,16,16>/InputDoubleBufferWriter<4096,16,16>/run/tmp.data.value -WORD_WIDTH 128
# Your code ends here
# -------------------------------

go architect

go allocate
go extract
