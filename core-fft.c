#define _GNU_SOURCE

#include <fftw3.h>
#include "core-fft.h"
//thread stuff?

void do_depart(int signum) {
	running = false;
	fprintf(stderr,"\nInterrupted.\n");
}

/*
int comedi_get_hardcal_converter(comedi_t *dev, unsigned subdevice, \
		unsigned channel, unsigned range, \
		enum comedi_conversion_direction direction, \
		comedi_polynomial_t *converter);

double comedi_to_physical(lsampl_t data, \
		const comedi_polynomial_t *conversion_polynomial);
*/

void window_sel(double *window, struct core_param *o) {
	double wvar, winpar = o->winpar;
	unsigned int N = o->N, i;

		if (Streq(o->window,"gaussian")) {
		        printf("Using Gaussian window...");
		} else if (Streq(o->window,"bohman")) {
		        printf("Using Bohman window...");
		} else if (Streq(o->window,"bartlett-hanning")) {
		        printf("Using Bartlett-Hanning window...");
		} else if (Streq(o->window,"hamming")) {
		        printf("Using Hamming window...");
		} else { 
		        printf("Using Hanning window...");
		}
	for (i = 0; i < N; i++) {
		if (Streq(o->window,"gaussian")) {
			window[i] = exp( -0.5 * pow(i-(N-1)/2.0,2) / pow(winpar*N,2) );
		} else if (Streq(o->window,"bohman")) {
			wvar = 2*abs(i-N/2.0)/N;
			window[i] = (1-wvar)*cos(PI*wvar)+sin(PI*wvar)/PI;
		} else if (Streq(o->window,"bartlett-hanning")) {
			wvar = i/N-0.5;
			window[i] = 0.62-0.48*abs(wvar)+0.38*cos(2*PI*wvar);
		} else if (Streq(o->window,"hamming")) {
			window[i] = 0.54-0.46*cos(2*PI*i/N);
		} else { // Default case = Hann window
			window[i] = 0.5-0.5*cos(2*PI*i/(N/o->windiv));
		}
	}
}

double window_cog(struct core_param *o) {
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

double apply_polynomial(const polynomial_t *pol, double input) {
    double value = 0.;
    double term = 1.;
    unsigned int i;
    for (i = 0; i <= pol->order; ++i) {
        value += pol->coefficients[i] * term;
        term *= input - pol->expansion_origin;
    }
    return value;
}

