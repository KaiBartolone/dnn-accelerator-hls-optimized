set blockname [file rootname [file tail [info script] ]]

source scripts/common.tcl

directive set -DESIGN_HIERARCHY { 
    {ProcessingElement<IDTYPE, WDTYPE, ODTYPE>} 
}

go compile

source scripts/set_libraries.tcl

go libraries

directive set -CLOCKS $clocks

go assembly

directive set /ProcessingElement<IDTYPE,WDTYPE,ODTYPE>/run -DESIGN_GOAL Latency
directive set /ProcessingElement<IDTYPE,WDTYPE,ODTYPE>/run -CLOCK_OVERHEAD 0.000000

go extract
