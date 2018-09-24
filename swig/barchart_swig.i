/* -*- c++ -*- */

#define BARCHART_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "barchart_swig_doc.i"

%{
#include "barchart/DisplayPlot.h"
#include "barchart/displayform.h"
#include "barchart/vector_sink_f.h"
%}

%include "barchart/vector_sink_f.h"

GR_SWIG_BLOCK_MAGIC2(barchart, vector_sink_f);
