%module coreFFT
%{
#include "core-fft.h"
%}

extern struct core_param;
extern struct core_return;
extern int complex_1chan(struct core_param, struct core_return *);
extern int uint16_1chan(struct core_param, struct core_return *);
extern int float4_1chan(struct core_param, struct core_return *);
extern int float8_1chan(struct core_param, struct core_return *);
extern int real_nchan(struct core_param, struct core_return *);
extern int uint8_1chan(struct core_param, struct core_return *);

%typemap(out) int [ANY] {
	int i;
	$result = PyList_New($1_dim0);
	for (i = 0; i < $1_dim0; i++) {
		PyObject *o = PyInt_FromLong((long int) $1[i]);
		PyList_SetItem($result,i,o);
	}
}
%include "core-fft.h"
