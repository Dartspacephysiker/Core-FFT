#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "core-fft.h"

int real_nchan(struct core_param o, struct core_return *retstr) {

	fftw_plan plan_uber;

	unsigned long int s_bytes, i_bytes, o_bytes, ret;
	unsigned long int i, j, k, ffts, iavg, iphs, nyq, bps, chanoffset, binoffset;
	char outfull[1024];
	long int power, maxmag[o.n_chan], minmag[o.n_chan];
	off_t dataend, seeksamp;
	complex double datai[o.N][o.n_chan];
	double cur_mag[o.N], avg_mag[o.N][o.n_chan], window[o.N], pow_adj;
	complex double avg_phs[o.N][o.n_chan][N_PHASES];
	complex double nphs;
	struct timeval now,then;
	unsigned flags = FFTW_PATIENT|FFTW_DESTROY_INPUT;
//	unsigned flags = FFTW_ESTIMATE|FFTW_DESTROY_INPUT;
	double time, ratio;
	struct timespec nsl;
	FILE *istream, *ostream[o.n_chan], *phstream = NULL;

	/* For timestamp conversion */
	double tConv; 
	struct tm tm_initial;
	struct tm tm_info;
	int haveTimeStr;
	double seconds;
	char tBuf[255];

	/*
	 * Complex/Real Input
	 */
	short int *samples;
	double *input;
	fftw_complex *output;
	bps = sizeof(short int); // real, short int (2-byte) data
	s_bytes = bps*o.N*o.n_chan;
	i_bytes = sizeof(double)*o.N*o.n_chan;
	o_bytes = sizeof(fftw_complex)*o.N*o.n_chan;
	nyq = o.N/2;

	/* if there's a start string, use it! */
	haveTimeStr = 0;
	if( strncmp(o.tStartString,"",19) > 0) {

	    setenv("TZ", "UTC0", 1); //Set timezone to UTC
	    printf("startString: %s\n",o.tStartString);

	    /* One to keep track of changing time, one to keep start time  */
	    memset(&tm_initial, 0, sizeof(struct tm));
	    memset(&tm_info, 0, sizeof(struct tm));

	    strptime(o.tStartString,"%Y-%m-%d/%H:%M:%S",&tm_initial);
	    strptime(o.tStartString,"%Y-%m-%d/%H:%M:%S",&tm_info);

	    /* tm_initial.tm_sec = 0; */
	    /* tm_info.tm_sec = 0; */

	    /* strftime(tBuf,sizeof(tBuf), "%d %b %Y %H:%M",&tm_info); */
	    puts(tBuf);

	    haveTimeStr = 1;
	} else {
	
	    /* What about time units for x axis? */
	    tConv = 1;
	    if( strncmp(o.time_units,"m",1) == 0) {
		tConv = 60;
	    } else if( strncmp(o.time_units,"h",1) == 0) {
		tConv = 3600;
	    } else if( strncmp(o.time_units,"d",1) == 0) {
		tConv = 86400;
	    }

	    printf("tConv: %.0f (units: %s)\n",tConv,o.time_units);
	}

	/*
	 * Memory allocation.
	 */
	printf("Memory allocation...");
	samples = (short int *) malloc(s_bytes);
	if (samples == NULL) fprintf(stderr,"Failed to allocate initial sample memory!\n");
	input = (double *) fftw_malloc(i_bytes);
	if (input == NULL) fprintf(stderr,"Failed to allocate fft input array!\n");
	output = (fftw_complex *) fftw_malloc(o_bytes);
	if (output == NULL) fprintf(stderr,"Failed to allocate fft output array!\n");
	printf("done.  (%liKB)\n",(s_bytes + i_bytes + o_bytes)/1024);

	/*
	 * FFTW Setup.  Initialize threads, plan_many.
	 */
	printf("FFTW initialization and planning...");
	gettimeofday(&then,NULL);
	if (fftw_init_threads() == 0) {
		fprintf(stderr,"Failed to initialize FFTW Threads!\n");
	}
	fftw_plan_with_nthreads(o.threads);
	plan_uber = fftw_plan_many_dft_r2c(1, &o.N, o.n_chan, \
		input, NULL, 1, o.N, \
		output, NULL, 1, o.N, flags);
	gettimeofday(&now,NULL);
	printf("wewt.  (%fs elapsed)\n",(now.tv_sec-then.tv_sec)+(now.tv_usec-then.tv_usec)/1e6);

	/*
	 * File setup
	 */
	printf("Opening files...");
	istream = fopen(o.infile,"r");
	if (istream == NULL) {
		fprintf(stderr,"Failed to open %s for reading.\n",o.infile);
		return(1);
	}

	for (i = 0; i < o.n_chan; i++) {
		sprintf(outfull,"%s-ch%02lu.fft",o.outfile,i+1);
		ostream[i] = fopen(outfull,"w+");
		if (ostream[i] == NULL) {
			fprintf(stderr,"Failed to open %s for writing.\n",outfull);
			return(1);
		}
	}

	if (o.phases) {
		sprintf(outfull,"%s.cohphs",o.outfile);
		phstream = fopen(outfull,"w+");
		if (phstream == NULL) {
			fprintf(stderr,"Failed to open %s for writing.\n",outfull);
			return(1);
		}
	}

	printf("fiddling...");
	fseek(istream,0,SEEK_END);
	dataend = ftell(istream);
	if (o.newheader == COREH_TAIL) {
		dataend -= 8192;
	}
	rewind(istream);

	seeksamp = o.n_startsample*s_bytes;
	if (o.oldheader) seeksamp += 8;
	if (o.newheader == COREH_HEAD) seeksamp += 8192;
	if (fseeko(istream,seeksamp,SEEK_SET)){
		fprintf(stderr,"Failed to seek to starting sample!\n");
	}

	/*
	 * Window selection, and setup to adjust for Coherent Gain,
	 * 2x for real input data, and hz/bin multiplier.
	 */
	window_sel(window,&o);
	pow_adj = window_cog(&o)*2*(o.frequency/o.N);
	nsl.tv_sec = 0; nsl.tv_nsec = CORE_RFP;

	ffts = 0;
	iavg = 0;
	time = o.time_start;

	printf("done.\n"); fflush(stdout); fflush(stderr);

	running = true;

	while ((ftell(istream) < dataend) && (running == true)) {
		nanosleep(&nsl,NULL);
		/*
		 * Main loop: read in data while data is still readable.
		 */
		ret = fread(samples,bps,o.N,istream);
		if (ret != o.N) {
			printf("Failed to read from data file.\n");
		}

		/*
		 * Copy channels to input array, switching from row-major
		 * to column-major ordering to satisfy the (hopefully
		 * faster) plan_many data format, and casting to double as we go.
		 */
		for (i = 0; i < o.N; i++) {
			binoffset = i*o.n_chan;
			for (j = 0; j < o.n_chan; j++) {
				input[i+j*o.N] = (double) samples[binoffset+j]*window[i];
			}
		}

		fftw_execute(plan_uber); // FOR GLORY!!!

		/*
		 * Convert to C99 complex, and multi-dim array.
		 */
		for (i = 0; i < o.n_chan; i++) {
			chanoffset = i*(o.N);

			for (j = 0; j < nyq; j++) {
				datai[j][i] = output[j + chanoffset];
			}
		}

		for (i = 0; i < o.n_chan; i++) {
			for (j = 0; j < nyq; j++) {
				cur_mag[j] = pow(cabs(datai[i][j]),2);
			}

			if (o.agc_level != 0) {
				ratio = o.agc_level/cur_mag[o.agc_bin-1];

				for (j = 0; j < nyq; j++) {
					if (iavg == 0) avg_mag[j][i] = 0; // First average, so initialize
					avg_mag[j][i] += cur_mag[j]*ratio;
				}
			}
			else {
				for (j = 0; j < nyq; j++) {
					if (iavg == 0) avg_mag[j][i] = 0; // First average, so initialize
					avg_mag[j][i] += cur_mag[j];
				}
			}

		}

		if (o.phases) {
			for (i = 0; i < nyq; i++) {
				iphs = 0;
				for (j = 0; j < o.n_chan; j++) {
					for (k = j+1; k < o.n_chan; k++) {
						if (iavg == 0) avg_phs[i][j][iphs] = 0;
						avg_phs[i][j][iphs] += datai[i][j]*conj(datai[i][k]);
						iphs++;
					}
				}
			}
		}

		printf("Average #%i\n",iavg+1);

		iavg++;
		if (iavg == o.avg) {
			/*
			 * We've averaged enough, output to files.
			 */
			if (o.verbose) printf("%f\n",time);

			if(haveTimeStr >= 1){

			    /* Need to update seconds? */
			    printf("time: %.5f, %8.5f\n",time,time*1000);
			    if(seconds > 1){
				tm_info.tm_sec += floor(seconds);
				seconds -= floor(seconds);
				mktime( &tm_info);      // normalize tm_info
			    }

			    if(haveTimeStr == 1){
				strftime(tBuf,sizeof(tBuf),"%m-%d/%H:%M:%S",&tm_info);
				haveTimeStr++;
			    } else{
				strftime(tBuf,sizeof(tBuf),"%H:%M:%S",&tm_info);
			    }

			    for (i = 0; i < o.n_chan; i++) {
				fprintf(ostream[i],"%*s.%03.0f\n",strlen(tBuf),tBuf,seconds*1000);
			    }
			    if (o.phases) {
				fprintf(phstream,"%*s.%03.0f\n",strlen(tBuf),tBuf,seconds*1000);
			    }

			    /* printf("%s. %3.0f\n",tBuf,roundl(seconds*((double) 1000))); */
			    printf("%*s.%03.0f\n",strlen(tBuf),tBuf,seconds*1000);
			    /* printf("%s\n",tBuf); */
			} else{
			    /* fprintf(ostream,"%8.03f\n",time/tConv);  // Timestamps */
			    for (i = 0; i < o.n_chan; i++) {
				fprintf(ostream[i],"%8.03f\n",time/tConv);  // Timestamps
			    }
			    if (o.phases) {
				fprintf(phstream,"%8.03f\n",time/tConv);
			    }

			}

			for (i = 0; i < nyq; i++) {
				for (j = 0; j < o.n_chan; j++) {
					if (o.linear == true) {
						return(1);  //not working
						power = avg_mag[i][j]/o.avg;
					} else {
						power = lround(10 * log10(avg_mag[i][j]/o.avg));	// log10(|x|^2)
					}

					//power &= 4095; // Trim to 12 bits.
					if (!o.binary) {
						fprintf(ostream[j],"%li\n", power);
					} else {
						// Blerg
					}

					/*
					 * Find global min & max.
					 */
					if (ffts < o.avg) {
						minmag[j] = maxmag[j] = power;
					} else {
						minmag[j] = fmin(power,minmag[j]);
						maxmag[j] = fmax(power,maxmag[j]);
					}
				}
				if (o.phases) {
					for (k = 0; k < N_PHASES; k++) {
						nphs = avg_phs[i][j][k]/( \
								avg_mag[i][n_phsmap[k][0]] * \
								avg_mag[i][n_phsmap[k][1]] );
						if (k > 0) fprintf(phstream," ");
						fprintf(phstream,"%2.2f %2.2f",cabs(nphs),carg(nphs));
					}
					fprintf(phstream,"\n");
				}
			}

			/* Add time per avg. */
			if (o.time_avg > 0) {
				time += o.time_avg;
			}

			iavg = 0;
		}
		ffts++;

		if (ffts == o.n_ffts) break;
		if ((o.time_stop != 0) && (time > o.time_stop)) break;

		/*
		 * This will add o.time_nffts to the timestamp every o.time_fftmod ffts.
		 * Interaction with o.time_avg must be carefully considered to get
		 * the correct timestamp advancement.  Also note that some programs that
		 * may end up processing this data (e.g. for ps making) do not like it
		 * when timestamps increase inconsistently.
		 */
		if ((o.time_nfft > 0) && (ffts % o.time_fftmod == 0)) {
			time += o.time_nfft;
		}
	}

	fclose(istream);
	for (i = 0; i < o.n_chan; i++) {
		fclose(ostream[i]);
	}

	if( o.phases ) {
	  fclose(phstream);
	}

	retstr->time_total = time-o.time_start;
	retstr->nfft = ffts;
	for (i = 0; i < o.n_chan; i++) {
		retstr->min[i] = minmag[i];
		retstr->max[i] = maxmag[i];
	}

	return(0);
}
