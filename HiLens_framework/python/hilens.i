%module hilens_internal

%{
#define SWIG_FILE_WITH_INIT

#include "../include/errors.h"
#include "../include/ei_services.h"
#include "../include/hilens.h"
#include "../include/log.h"
#include "../include/media_process.h"
#include "../include/model.h"
#include "../include/output.h"
#include "../include/resource.h"
#include "../include/video_capture.h"
#include "../include/audio_capture.h"
#include "../include/audio_output.h"
#include "./wrapper.h"

using namespace hilens;
%}

%include "std_string.i"
%include "std_vector.i"
%include "std_shared_ptr.i"
%include "numpy.i"
%init %{
    import_array();
%}

// For Upload
%apply (unsigned char* IN_ARRAY1, int DIM1) {(const unsigned char *buffer, size_t bufferSize)}

// For InferData
%apply (unsigned char* IN_ARRAY1, int DIM1) {(const unsigned char* data, unsigned int size)}
%apply (float* IN_ARRAY1, int DIM1) {(const float *data, unsigned int size)}

// For PreprocessorWrapper
%apply (unsigned char* IN_ARRAY1, int DIM1) {(const unsigned char *srcData, unsigned int srcSize)}
%apply (unsigned char* INPLACE_ARRAY1, int DIM1) {(unsigned char* dstData, unsigned int dstSize)}

// For InferDataWrapper
%apply (unsigned char* ARGOUT_ARRAY1, int DIM1){(unsigned char* data, unsigned int size)}
%apply (float* ARGOUT_ARRAY1, int DIM1){(float* data, unsigned int size)}

%include "../include/errors.h"
%include "../include/ei_services.h"
%include "../include/hilens.h"
%include "../include/log.h"
%include "../include/media_process.h"
%include "../include/model.h"
%include "../include/output.h"
%include "../include/resource.h"
%include "../include/video_capture.h"
%include "../include/audio_capture.h"
%include "../include/audio_output.h"
%include "./wrapper.h"

%template(InferDataVec) std::vector<hilens::InferDataWrapper>;
%template(EIHeaders) std::vector<std::string>;
