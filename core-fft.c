#define _GNU_SOURCE

#include <fftw3.h>
#include "core-fft.h"
//thread stuff?

void do_depart(int signum) {
	/*
	 * Attempt to stop on Ctrl-C.  Not working right now.
	 */
	running = false;
	fprintf(stderr,"\nInterrupted.\n");
}

int coreAlloc(void *samples, void *input, struct core_param *o) {
	unsigned int bps; unsigned long int n_samp;

	n_samp = o->N*o->avg*o->n_chan;
//	if (o->complex) n_samp *= 2;

	switch (o->in_type) {
		case 'int8': bps = 1; break;
		case 'int16': bps = 2;	break;
		case 'float32': bps = 4; break;
		case 'float64':	bps = 8; break;
	}

	if (o->complex) {
		samples = malloc(2 * n_samp * bps);
		input = fftw_malloc(n_samp * sizeof(fftw_complex));
	}
	else {
		samples = malloc(n_samp * bps);
		input = fftw_malloc(n_samp * sizeof(double));
	}
	assert(samples != NULL);
	assert(input != NULL);

	o->bps = bps;
	o->n_samp = n_samp;
}

int coreRead(FILE *infile, void *samples, void *input, const struct core_param *o) {
	long int ret;
	long unsigned int i,j,k, nx;
	static bool startup = true;

	if (startup) {
		switch (o->in_type) {
			case 'uint8': static unsigned char *sarray; dsize = sizeof(unsigned char); break;
			case 'uint16': static unsigned short int *sarray; dsize = sizeof(unsigned short int); break;
			case 'uint32': static unsigned short int *sarray; dsize = sizeof(unsigned long int); break;
			case 'uint64': static unsigned short int *sarray; dsize = sizeof(unsigned long long); break;
			case 'int8': static char *sarray; dsize = sizeof(char); break;
			case 'int16': static short int *sarray; dsize = sizeof(short int); break;
			case 'int32': static short int *sarray; dsize = sizeof(long int); break;
			case 'int64': static short int *sarray; dsize = sizeof(long long); break;
			case 'float32': static float *sarray;+ dsize = sizeof(float); break;
			case 'float64': static double *sarray; dsize = sizeof(double); break;
		}

		if (!o.complex) {
			double dataz;
			double *iarray = (double *) input;
			nx = o->N;
		} else {
			complex double dataz;
			complex double *iarray = (complex double *) input;
			nx = o->N*2;
		}

		sarray = malloc(nx * dsize);

		startup = false;
	}

	ret = fread(samples,o->bps,o->n_samp,infile);
	if (ret != o->n_samp) printf("Failed to read from data file %s.\n",o->infile);

	for (k = 0; k < o->avg; k++) {
		for (j = 0; j < o->n_chan; j++) {
			cbase = k*j;
			for (i = 0; i < o->nx; i++) {
				dataz = (double) sarray[cbase+i];
				if (o->complex) dataz += I*((double) sarray[cbase+(i++)]);


			}
		}
	}
}

double core_Window(int N, struct core_param *o) {
	/*
	 * Returns the windowing coefficient for a given N.
	 */
	double wvar;
	static *warr = NULL;

	if (warr = NULL) {
		warr = malloc(o->N*sizeof(double));

		for (int i = 0; i < o->N; i++) {
			if (Streq(o->window,"gaussian")) {
				warr[i] = exp( -0.5 * pow(i-(N-1)/2.0,2) / pow(o->winpar*N,2) );
			} else if (Streq(o->window,"bohman")) {
				wvar = 2*abs(i-N/2.0)/N;
				warr[i] = (1-wvar)*cos(PI*wvar)+sin(PI*wvar)/PI;
			} else if (Streq(o->window,"bartlett-hanning")) {
				wvar = i/N-0.5;
				warr[i] = 0.62-0.48*abs(wvar)+0.38*cos(2*PI*wvar);
			} else if (Streq(o->window,"hamming")) {
				warr[i] = 0.54-0.46*cos(2*PI*i/N);
			} else { // Default case = Hann window
				warr[i] = 0.5-0.5*cos(2*PI*i/(N/o->windiv));
			}
		}
	}

	return warr[N];
}

double core_Window_cog(struct core_param *o) {
	/*
	 * Returns the Coherent Gain for a given window function.
	 * Divide by this number to adjust for the power loss due
	 * to the window.
	 */
	if (Streq(o->window,"gaussian")) {
		return(0.43);
	} else if (Streq(o->window,"bohman")) {
		return(0.41);
	} else if (Streq(o->window,"bartlett-hanning")) {
		return(0.50);
	} else if (Streq(o->window,"hamming")) {
		return(0.54);
	} else { // Default case = Hann window
		return(0.50);
	}
}

double core_ApplyPolynomial(const polynomial_t *pol, double input) {
	/*
	 * Recreation of comedi's apply_polynomial, for converting 'counts'
	 * to real voltages, using a comedi polynomial_t.  Not used yet.
	 */
    double value = 0.;
    double term = 1.;
    unsigned int i;
    for (i = 0; i <= pol->order; ++i) {
        value += pol->coefficients[i] * term;
        term *= input - pol->expansion_origin;
    }
    return value;
}

/*
int comedi_get_hardcal_converter(comedi_t *dev, unsigned subdevice, \
		unsigned channel, unsigned range, \
		enum comedi_conversion_direction direction, \
		comedi_polynomial_t *converter);

double comedi_to_physical(lsampl_t data, \
		const comedi_polynomial_t *conversion_polynomial);
*/


