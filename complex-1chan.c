#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "core-fft.h"
#include "simple_fifo.h"
//thread stuff?

static inline short int endswp(short int x) {
	return((short signed int)( (((short unsigned int) x)>>8) | (((short unsigned int) x)<<8) ));
}

int complex_1chan(struct core_param o, struct core_return *retstr) {

    fftw_plan fftplan;

    unsigned long int s_bytes, i_bytes, o_bytes, ol_bytes, ol_shift, ret;
    unsigned long int i, ffts, iavg, iphs, nyq, seeksamp, bps, half;
    long int bl_first, bl_last;
    long int power, maxmag = 0, minmag = 0;
    off_t dataend, bskip_avg = 0, bskip_fft = 0;
    char dummy;
    int N_read = 0;
    double avg_mag[o.N], window[o.N], pow_adj;
    //	complex double avg_phs[o.N][N_PHASES];
    //	complex double nphs;
    complex double datai;
    struct timeval now,then;
    //	unsigned flags = FFTW_PATIENT|FFTW_DESTROY_INPUT;
    unsigned flags = FFTW_MEASURE|FFTW_DESTROY_INPUT;
    //	unsigned flags = FFTW_ESTIMATE|FFTW_DESTROY_INPUT;
    double time;
    struct timespec nsl;
    FILE *istream, *ostream, *tstream = NULL;//, *phstream;

    //FIFOness for Dartmouth RxDSP
    struct simple_fifo *fifo;
    long int fifo_loc,fifo_size;
    //  long int skip_loc;
    //  long int oldskip_loc;
    /* char *fifo_outbytes; */
    char fifo_srch[18];

    long int RxDSPAcqSize,RxDSPHeaderSize,has_DartmouthRxDSP_headers;
    unsigned int seekRxDSPHeader,endianSwap; //2017/09/06 Added by Spence

    long int BYTESBEFDARTMOUTH,BYTESINCDARTMOUTH;
    //OLD WAY 
    /* long int hPosInSamps,backShift,forwardShift,headerCount; */
    /* short int *hptr; */

    /* has_DartmouthRxDSP_headers = 0; */
    seekRxDSPHeader = 0;
    endianSwap = 1;
    BYTESBEFDARTMOUTH = 132;
    BYTESINCDARTMOUTH = 44;
    static char head_strings[2][2][33] =
	{
	    { // Normal byte order                                                                           
		"aDtromtu hoCllge eaMtsreR DxPS  \0",
		"aDtromtu hoCllge elSva eR DxPS  \0" 
	    },
	    { // Swapped byte order (used with -E)
		// (I guess short ints are read in with bytes swapped compared to disk order?)
		"Dartmouth College Master RxDSP  \0",
		"Dartmouth College Slave  RxDSP  \0"
	    }
	};

    signal(SIGINT,do_depart);

    /*
     * Complex/Real Input decisions
     */
    short int *samples, *sampofft = NULL, *sampfill = NULL;
    fftw_complex *input, *output;
    bps = 2*sizeof(short int);
    s_bytes = bps*o.N;
    i_bytes = sizeof(fftw_complex)*o.N;
    o_bytes = sizeof(fftw_complex)*o.N;
    nyq = o.N;
    ol_bytes = o.overlap*bps; // bytes overlapped
    ol_shift = s_bytes-ol_bytes; // bytes kept for overlap

    /*
     * Memory allocation.
     */
    printf("Memory allocation...");

    samples = (short int *) malloc(s_bytes);
    if (samples == NULL) fprintf(stderr,"Failed to allocate initial sample memory!\n");
    input = (fftw_complex *) fftw_malloc(i_bytes);
    if (input == NULL) fprintf(stderr,"Failed to allocate fft input array!\n");
    output = (fftw_complex *) fftw_malloc(o_bytes);
    if (output == NULL) fprintf(stderr,"Failed to allocate fft output array!\n");

    //fifo setup
    fifo = malloc( sizeof(*fifo) );
    if (fifo == NULL) fprintf(stderr,"Failed to allocate fifo!\n");
    /* fifo_size = 4*s_bytes; */
    RxDSPAcqSize = 131200;
    RxDSPHeaderSize = BYTESBEFDARTMOUTH + BYTESINCDARTMOUTH;
    fifo_size = 4*RxDSPAcqSize;
    fifo_init(fifo, fifo_size);  

    if (s_bytes > RxDSPAcqSize) {
	printf("Too big!\n");
	return -1;
    }
    /* fifo_outbytes = malloc(rtdbytes); */
    //in place of fifo_outbytes, we use "samples"
    strcpy(fifo_srch, head_strings[endianSwap][0]);

    if (o.overlap > 0) {
	N_read = o.overlap;
	sampofft = (short int *) (samples + 2*o.overlap); // pointer to end of overlap region
	sampfill = (short int *) (samples + 2*(o.N-o.overlap)); // pointer to new read-in region
	printf("ol_bytes: %lu, ol_shift: %lu, N_read: %i\n",ol_bytes,ol_shift,N_read);
	printf("samples: %p, sampofft: %p, sampfill: %p\n",samples,sampofft,sampfill);
    }

    printf("done.  (%liKB)\n",(s_bytes + i_bytes + o_bytes)/1024);

    /*
     * FFTW Setup.  Initialize threads, plan_many.
     */
    printf("FFTW initialization and planning...");

    gettimeofday(&then,NULL);
    if (o.threads > 1) {
	if (fftw_init_threads() == 0) {
	    fprintf(stderr,"Failed to initialize FFTW Threads!\n");
	}
	fftw_plan_with_nthreads(o.threads);
    }
    fftplan = fftw_plan_dft_1d(o.N, input, output, FFTW_FORWARD, flags);
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

    if (strcmp(o.timefile,"") != 0) {
	tstream = fopen(o.timefile,"r");
	if (tstream == NULL) {
	    fprintf(stderr,"Failed to open %s for reading.\n",o.timefile);
	    return(1);
	}
    }

    ostream = fopen(o.outfile,"w+");
    if (ostream == NULL) {
	fprintf(stderr,"Failed to open %s for writing.\n",o.outfile);
	return(1);
    }

    printf("fiddling...");

    fseeko(istream,0,SEEK_END);
    /* dataend = ftello(istream)-s_bytes; */
    dataend = ftello(istream)-(4*RxDSPAcqSize);
    rewind(istream);

    if (o.n_startsample > 0) {
	seeksamp = o.n_startsample*s_bytes;

	if (fseeko(istream,seeksamp,SEEK_SET)) {
	    fprintf(stderr,"Failed to seek to starting sample!\n");
	}

	for (i = 0; i < o.n_startsample; i++) {
	    fgets(NULL, 512, tstream);
	}
    }

    /*
     * Window selection, and setup to adjust for Coherent Gain,
     * 2x for real input data, and hz/bin multiplier.
     */
    window_sel(window,&o);
    pow_adj = 1/window_cog(&o);
    nsl.tv_sec = 0; nsl.tv_nsec = 1;

    half = (int) floor(o.N/2);

    if (o.bl_last == -1) bl_last = nyq; else bl_last = o.bl_last;
    if (o.bl_first == -1) bl_first = 1; else bl_first = o.bl_first;
    if (o.skip_avg > 0) bskip_avg = bps*o.skip_avg;
    if (o.skip_fft > 0) bskip_fft = bps*o.skip_fft;

    ffts = 0;
    iavg = 0;
    time = o.time_start;

    printf("done.\n"); fflush(stdout); fflush(stderr);

    running = true;

    while ((ftello(istream) <= dataend) && (running == true)) {
	nanosleep(&nsl,NULL);
	/*
	 * Main loop: read in data while data is still readable.
	 */
	//		if (o.verbose) printf("Loading %lu bytes...\n",s_bytes);
	if (o.overlap > 0) {
	    memmove(samples,sampofft,ol_shift);
	    ret = fread(sampfill,bps,N_read,istream);
	    if (ret != N_read) {
		printf("Failed to read from data file.\n");
	    }
	} else {


	    if (o.junkRxDSPHeader) {

		printf("Looking for RxDSP header ...\n");
		/* printf("Available: %d\n",(fifo_size-fifo_avail(fifo))/1024/1024);	     */
		/* if ( fifo_avail(fifo) < 2*RxDSPAcqSize ){ */
		if ( fifo_avail(fifo) < RxDSPAcqSize ){
		    seekRxDSPHeader = 1;

		    /* fifo_writefromstream(fifo,istream,fifo_size-fifo_avail(fifo));		 */
		    /* fifo_writefromstream(fifo,istream,fifo_size-fifo_avail(fifo));		 */
		    fifo_flush(fifo);
		    fifo_writefromstream(fifo,istream,RxDSPAcqSize);		

		}

		while (seekRxDSPHeader) {

		    //try to kill headers right away
		    fifo_loc = fifo_search(fifo, 2*RxDSPAcqSize, fifo_srch, 18);

		    /* if ( (fifo_loc != -1) && (fifo_headpos_rel_to_fifo_base(fifo) < (fifo_loc-BYTESBEFDARTMOUTH)) ) { */
		    if (fifo_loc != -1) {
		    
			//print skips array		
			/* printf("[%0.5d SAMPLES ARRAY]\n",ffts); */
			/* printf("[%0.5d BEFORE       ]\n",ffts); */
			/* printf("%0.4d      %0.4X, ",0,(long int) (*(fifo->head))); */
			/* /\* printf("%0.4X, ",); *\/ */
			/* for (i = 1; i < RxDSPHeaderSize/2; i++) { */
			/* 	if (i*2 % 16 == 0) { */
			/* 	    printf("\n%0.4d      %0.4X, ",i,(unsigned int) *((char *) ((long int)fifo->head+i*2))); */
			/* 	} else { */
			/* 	    printf("%0.4X, ",(unsigned int) *((char *)((long int)fifo->head+i*2))); */
			/* 	} */
			/* 	if (i*2 == (RxDSPHeaderSize - 2)) printf("\n"); */
			/* } */



			/* fifo_skip(fifo,fifo_srch,18,0,RxDSPHeaderSize,fifo_avail(fifo),(-1)*BYTESBEFDARTMOUTH); */

			//try killing instead of skip
			fifo_kill(fifo, fifo_loc);
			break;
		    } else break;

		}

	    //OLD WAY
	    /* ret = fread(samples,bps,o.N,istream); */
	    /* if (ret != o.N) { */
	    /* 	printf("Failed to read from data file.\n"); */
	    /* } */

	    // Added by Spence 2017/09/06
		////OLD WAY
	    /* if (seekRxDSPHeader) { */

		//hptr = memmem(samples,o.N,fifo_srch,2);
		//
		////If there is no sign of Dartmouth College, keep it moving
		//if (hptr != NULL) {
		//
		//    //Got "Da"! Do we need to read in more data?
		//    hPosInSamps = (long int) (hptr-samples);
		//
		//    if ( ( hPosInSamps ) >= ( o.N - 34 ) ) {
		//
		//	printf("Better 'djust dat!\n");
		//
		//    }
		//
		//    headerCount = 0;
		//    while (true) {
		//    
		//	//Now a full test
		//	/* hptr = memmem((short int *) (((long int) samples) + hPosInSamps),o.N,fifo_srch,18 * sizeof(*fifo_srch)); */
		//	/* if ( headerCount > 0) printf("Check %li\n",headerCount+1); */
		//	hptr = memmem(samples,o.N,fifo_srch,18 * sizeof(*fifo_srch));
		//
		//	//print sample array
		//	//
		//	/* if ((ffts == 0) || (ffts == (o.n_ffts - 1))) { */
		//
		//	if (hptr != NULL) {
		//
		//	    if ( (headerCount == 0) && (ffts < 1000))  {
		//		printf("[%0.5d SAMPLES ARRAY]\n",ffts);
		//		printf("[%0.5d BEFORE       ]\n",ffts);
		//		printf("%0.4d      %0.8X, ",0,samples[0]);
		//		/* printf("%0.4X, ",); */
		//		for (i = 1; i < o.N; i++) {
		//		    if (i % 8 == 0) {
		//			printf("\n%0.4d      %0.8X, ",i,samples[i]);
		//		    } else {
		//			printf("%.8X, ",samples[i]);
		//		    }
		//		    if (i == (o.N - 1)) printf("\n");
		//		}
		//	    }
		//
		//	    hPosInSamps = (long int) (hptr-samples);
		//	    /* printf("(%0.2li) Found it at %li!\n",ffts,hPosInSamps); */
		//    
		//	    if ( ( hPosInSamps + BYTESINCDARTMOUTH ) > o.N ) {
		//		//In this case some of the header will be in the next read
		//		printf("Whoa!\n");
		//	    } else {
		//		//In this case we need to move the data back BYTESINCDARTMOUTH bytes, then read in as many additional bytes as necessary
		//		/* printf("Header: %.*s\n",BYTESINCDARTMOUTH,(char *) hptr); */
		//		memmove(hptr,(short int *) ( ((long int) hptr) + BYTESINCDARTMOUTH),o.N-(hPosInSamps+BYTESINCDARTMOUTH));
		//		ret = fread((short int *) ( ((long int) samples) + o.N - BYTESINCDARTMOUTH),bps,BYTESINCDARTMOUTH,istream);
		//
		//		if (ret != BYTESINCDARTMOUTH) {
		//		    printf("Failed to read from data file.\n");
		//		}
		//	    }
		//	
		//	    /* forwardShift = ( BYTESINCDARTMOUTH < ( o.N - hPosInSamps ) ) ? BYTESINCDARTMOUTH : ( o.N - hPosInSamps ); */
		//
		//
		//	    //Any need to backshift over garbage data?
		//	    /* if ( ( hPosInSamps ) > 0 ) { */
		//	
		//	    /* 	backShift = ( BYTESBEFDARTMOUTH < hPosInSamps ) ? BYTESBEFDARTMOUTH : hPosInSamps; */
		//
		//	    /* 	/\* printf("Backshift: %li\n",backShift); *\/ */
		//
		//	    /* 	/\* memmove(samples,hptr,o.N-hPosInSamps); *\/ */
		//	    /* 	memmove((short int *) (((long int) hptr) - backShift),hptr,o.N-hPosInSamps); */
		//	
		//	    /* 	//Read in some more to account for shifting backwards, which erased junk data */
		//	    /* 	ret = fread((short int *) ( ((long int) samples) + (o.N - backShift)),bps,backShift,istream); */
		//
		//	    /* 	if (ret != backShift) { */
		//	    /* 	    printf("Failed to read from data file.\n"); */
		//	    /* 	} */
		//
		//	    /* 	hPosInSamps -= backShift; */
		//
		//	    /* } */
		//
		//	    /* printf("Headercount: %li\n",headerCount); */
		//	    
		//	    if ( (headerCount == 0) && (ffts < 1000))  {
		//		/* if (headerCount == 0) { */
		//		printf("[%0.5d SAMPLES ARRAY]\n",ffts);
		//		printf("[%0.5d AFTER        ]\n",ffts);
		//		printf("%0.4d      %0.8X, ",0,samples[0]);
		//		/* printf("%0.4X, ",); */
		//		for (i = 1; i < o.N; i++) {
		//		    if (i % 8 == 0) {
		//			printf("\n%0.4d      %0.8X, ",i,samples[i]);
		//		    } else {
		//			printf("%.8X, ",samples[i]);
		//		    }
		//		    if (i == (o.N - 1)) printf("\n");
		//		}
		//	    }
		//
		//	    headerCount++;
		//
		//	} else {
		//	    break;
		//	}
		//    }
		//}

		

		/* if( fifo_avail(fifo) > 2*s_bytes ) { */
		    //The lines below kill everything 
		    /* fifo_loc = fifo_search(fifo, 2*s_bytes, fifo_srch, 18); */
		    /* fifo_kill(fifo, fifo_loc); */
		/* } */
	    /* } */

	    /* fifo_read((char *) samples, fifo, s_bytes); */


	    } else {

		ret = fread(samples,bps,o.N,istream);
		if (ret != o.N) {
		    printf("Failed to read from data file.\n");
		}


	    }
	    
	}

	//THESE TWO LINES ARE NEW
	if (o.junkRxDSPHeader) {

	    while (fifo_avail(fifo) >= s_bytes) {
		fifo_read((char *) samples, fifo, s_bytes);

		/*
		 * Cast into fftw_complex input array, windowing too.
		 */
		//	if (tcd>>15 == 1) tcd -= 32768;
		//	if (tcd>>15 == 1) tcd -= 32768;
		for (i = 0; i < o.N; i++) {
		    if (endianSwap) {
			input[i] =  ((double) endswp(samples[2*i]));
			input[i] += ((double) endswp(samples[2*i+1]))*I;
		    }
		    else {
			input[i] =  ((double) samples[2*i]);
			input[i] += ((double) samples[2*i+1])*I;
		    }
		    /* input[i]  = ((double) samples[2*i]  ); */
		    /* input[i] += ((double) samples[2*i+1])*I; */
		    input[i] *= window[i];
		}

		fftw_execute(fftplan); // FOR GLORY!!!

		/*
		 * Get the squared magnitude, adjust powers
		 */
		for (i = 0; i < nyq; i++) {
		    iphs = 0;

		    /*
		     * Need the second half of output[] first,
		     * then first half second, because that's
		     * how DFTs output their frequencies.
		     */
		    if (i < half) {
			datai = output[i+half];
		    } else {
			datai = output[i-half];
		    }

		    if (iavg == 0) avg_mag[i] = 0; // First average, so initialize
		    avg_mag[i] += pow(cabs(datai),2);

		    /*			if (o.phases) {
					for (k = j+1; k < o.n_chan; k++) {
					if (iavg == 0) avg_phs[i][j][iphs] = 0;
					avg_phs[i][j][iphs] += datai[i][j]*conj(datai[i][k]);
					iphs++;
					}
					}*/
		}

		//		printf("Average #%i\n",iavg+1);

		iavg++;
		if (iavg == o.avg) {
		    /*
		     * We've averaged enough, output to files.
		     */
		    if (tstream != NULL) {
			fscanf(tstream,"%lf\n",&time);
		    }

		    if (o.verbose) printf("%f\n",time);

		    fprintf(ostream,"%8.8f\n",time);  // Timestamps

		    /*			if (o.phases) {
					fprintf(phstream,"%.8f\n",time);
					}
		    */
		    for (i = bl_first-1; i < bl_last; i++) {
			if (o.linear == true) {
			    return(1);  //not working
			    power = avg_mag[i]/o.avg;
			} else {
			    power = lround(10 * log10(avg_mag[i]/o.avg));	// log10(|x|^2)
			}

			//power &= 65535; // Trim to 16 bits.
			if (!o.binary) {
			    fprintf(ostream,"%li\n", power);
			} else {
			    // Blerg
			}

			/*
			 * Find global min & max.
			 */
			if (ffts < o.avg) {
			    minmag = maxmag = power;
			} else {
			    minmag = fmin(power,minmag);
			    maxmag = fmax(power,maxmag);
			}

			/*				if (o.phases) {
							for (k = 0; k < N_PHASES; k++) {
							nphs = avg_phs[i][j][k]/( \
							avg_mag[i][n_phsmap[k][0]] * \
							avg_mag[i][n_phsmap[k][1]] );
							if (k > 0) fprintf(phstream," ");
							fprintf(phstream,"%2.2f %2.2f",cabs(nphs),carg(nphs));
							}
							fprintf(phstream,"\n");
							}*/
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

	} else {

	    /*
	     * Cast into fftw_complex input array, windowing too.
	     */
	    //	if (tcd>>15 == 1) tcd -= 32768;
	    //	if (tcd>>15 == 1) tcd -= 32768;
	    for (i = 0; i < o.N; i++) {
		if (endianSwap) {
		    input[i] =  ((double) endswp(samples[2*i]));
		    input[i] += ((double) endswp(samples[2*i+1]))*I;
		}
		else {
		    input[i] =  ((double) samples[2*i]);
		    input[i] += ((double) samples[2*i+1])*I;
		}
		/* input[i]  = ((double) samples[2*i]  ); */
		/* input[i] += ((double) samples[2*i+1])*I; */
		input[i] *= window[i];
	    }

	    fftw_execute(fftplan); // FOR GLORY!!!

	    /*
	     * Get the squared magnitude, adjust powers
	     */
	    for (i = 0; i < nyq; i++) {
		iphs = 0;

		/*
		 * Need the second half of output[] first,
		 * then first half second, because that's
		 * how DFTs output their frequencies.
		 */
		if (i < half) {
		    datai = output[i+half];
		} else {
		    datai = output[i-half];
		}

		if (iavg == 0) avg_mag[i] = 0; // First average, so initialize
		avg_mag[i] += pow(cabs(datai),2);

		/*			if (o.phases) {
					for (k = j+1; k < o.n_chan; k++) {
					if (iavg == 0) avg_phs[i][j][iphs] = 0;
					avg_phs[i][j][iphs] += datai[i][j]*conj(datai[i][k]);
					iphs++;
					}
					}*/
	    }

	    //		printf("Average #%i\n",iavg+1);

	    iavg++;
	    if (iavg == o.avg) {
		/*
		 * We've averaged enough, output to files.
		 */
		if (tstream != NULL) {
		    fscanf(tstream,"%lf\n",&time);
		}

		if (o.verbose) printf("%f\n",time);

		fprintf(ostream,"%8.8f\n",time);  // Timestamps

		/*			if (o.phases) {
					fprintf(phstream,"%.8f\n",time);
					}
		*/
		for (i = bl_first-1; i < bl_last; i++) {
		    if (o.linear == true) {
			return(1);  //not working
			power = avg_mag[i]/o.avg;
		    } else {
			power = lround(10 * log10(avg_mag[i]/o.avg));	// log10(|x|^2)
		    }

		    //power &= 65535; // Trim to 16 bits.
		    if (!o.binary) {
			fprintf(ostream,"%li\n", power);
		    } else {
			// Blerg
		    }

		    /*
		     * Find global min & max.
		     */
		    if (ffts < o.avg) {
			minmag = maxmag = power;
		    } else {
			minmag = fmin(power,minmag);
			maxmag = fmax(power,maxmag);
		    }

		    /*				if (o.phases) {
						for (k = 0; k < N_PHASES; k++) {
						nphs = avg_phs[i][j][k]/( \
						avg_mag[i][n_phsmap[k][0]] * \
						avg_mag[i][n_phsmap[k][1]] );
						if (k > 0) fprintf(phstream," ");
						fprintf(phstream,"%2.2f %2.2f",cabs(nphs),carg(nphs));
						}
						fprintf(phstream,"\n");
						}*/
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
    }

    fclose(istream);
    fclose(ostream);
    if (tstream != NULL) {
	fclose(tstream);
    }

    fifo_destroy(fifo);

    printf("MADE IT!\n");

    //fclose(phstream);

    retstr->time_total = time-o.time_start;
    retstr->nfft = ffts;
    retstr->min[0] = minmag;
    retstr->max[0] = maxmag;

    return(0);
}

