#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define _FILE_OFFSET_BITS 64

// Maximum channels to allocate memory for.
#define CORE_MAXCHAN 4
// Number of threads to use (for threaded engines)
#define N_THREADS 4
#define N_PHASES 6
// Refractory period: nanoseconds to sleep between FFTs
#define CORE_RFP 15

// The mapping of channels for crossed coherences
static int n_phsmap[6][2] = { \
		{0,1}, {0,2}, {0,3}, \
		{1,2}, {1,3}, {2,3}  \
		};

#define COREH_NONE 0
#define COREH_HEAD 1
#define COREH_TAIL 4

#include <complex.h>
#include <math.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <fftw3.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>

#define PI 3.1415926535
#define PITT 6.283185307
#define Streq(s1, s2) (strcmp((s1), (s2)) == 0)

typedef struct {
    double coefficients[CORE_MAXCHAN];
    double expansion_origin;
    unsigned order;
} polynomial_t;

struct core_param {
        char *infile;
        char *outfile;
        char *timefile;
        int N, windiv, overlap;
        long long unsigned int skip_fft;
        long long unsigned int skip_avg;
        long unsigned int avg;
        unsigned int n_chan;
        unsigned int n_ffts;
        long unsigned int n_startsample;
        long int bl_last;
        long int bl_first;
        long int agc_bin;
        double agc_level;

        polynomial_t poly[CORE_MAXCHAN];
        double frequency;
        double time_start;
        double time_stop;
        double time_avg;
        double time_nfft;
        int time_fftmod;

        char *window;
        double winpar;

        bool verbose;
        unsigned int threads;
        bool convert;
        bool continuous;
        bool phases;
        bool binary;
        bool linear;
        bool oldheader;
        int newheader;
};

struct core_return {
	int nfft;
	double time_total;
	int min[CORE_MAXCHAN];
	int max[CORE_MAXCHAN];
};

volatile bool running;

extern void do_depart(int signum);
extern void window_sel(double *, struct core_param *);
extern double window_cog(struct core_param *);
extern double apply_polynomial(const polynomial_t *, double);
extern int complex_1chan(struct core_param, struct core_return *);
extern int uint16_1chan(struct core_param, struct core_return *);
extern int float4_1chan(struct core_param, struct core_return *);
extern int float8_1chan(struct core_param, struct core_return *);
extern int real_nchan(struct core_param, struct core_return *);
extern int uint8_1chan(struct core_param, struct core_return *);
