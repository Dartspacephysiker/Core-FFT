/*
 * simple_fifo.c
 *
 *  Created on: Mar 1, 2011
 *      Author: wibble
 *
 *  fifo_skip and fifo_search during Jul 2014 by Spencer Hatch, MPD
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simple_fifo.h"

int fifo_init(struct simple_fifo *fifo, long int size) {
	int ret = 0;

	fifo->size = size;
	fifo->base = malloc(size);
	if (fifo->base == NULL) {
		ret = EFIFO_MEM;
	}

	fifo->tail = fifo->base+size;
	fifo->head = fifo->tail;

	return(ret);
}

int fifo_write(struct simple_fifo *fifo, char *new_data, long int new_size) {
  long int span = fifo->size - new_size; // Amount of data to save
  long int shift = fifo->head - fifo->base; // Current read point

  //	printf("fifo_write(): have %li, adding %li.\n", fifo->tail-fifo->head, new_size);fflush(stdout);

  if (new_size >= shift) {
    memmove(fifo->base, fifo->base+new_size, span);
    memmove(fifo->base+span, new_data, new_size);
    fifo->head = fifo->base;
  } else {
    memmove(fifo->head-new_size, fifo->head, fifo->tail-fifo->head);
    memmove(fifo->tail-new_size, new_data, new_size);
    fifo->head = fifo->head - new_size;
  }

  return(0);
}

/* int fifo_write(struct simple_fifo *fifo, char *new_data, long int new_size) { */
int fifo_writefromstream(struct simple_fifo *fifo, FILE *istream, long int new_size) {
    unsigned long int ret;
    long int span = fifo->size - new_size; // Amount of data to save
    long int shift = fifo->head - fifo->base; // Current read point

  	/* printf("fifo_writefromstream(): have %li, adding %li.\n", fifo->tail-fifo->head, new_size);fflush(stdout); */

  if (new_size >= shift) {
    memmove(fifo->base, fifo->base+new_size, span);
    ret = fread(fifo->base+span,1,new_size,istream);
    fifo->head = fifo->base;
  } else {
    memmove(fifo->head-new_size, fifo->head, fifo->tail-fifo->head);
    ret = fread(fifo->tail-new_size,1,new_size,istream);
    fifo->head = fifo->head - new_size;
  }

  if (ret != new_size) {
      printf("FIFO crudded on write_to_fifo!\n");
  }

  return(0);
}


long int fifo_read(char *out, struct simple_fifo *fifo, long int bytes) {
  long int ret = 0;
  void *pret;

  	/* printf("fifo_read(): want %li, have %li.\n", bytes, fifo_avail(fifo));fflush(stdout); */

  if (bytes <= fifo_avail(fifo)) {
    if (out == NULL) out = malloc(bytes); // Malloc if out is new

    pret = memmove(out, fifo->head, bytes); // Move data
    if (pret != NULL) {
      fifo->head += bytes;
      ret = bytes;
    }
  }

  return(ret);
}

int fifo_kill(struct simple_fifo *fifo, long int bytes) {
  int ret = 0;

  if (bytes <= fifo_avail(fifo)) {
    fifo->head += bytes;
    ret = bytes;
  }

  return(ret);
}

// Kills "bytes" bytes at fifo_loc
/* int fifo_killbytes(struct simple_fifo *fifo, long int fifo_loc, long int bytes) { */
/*     long int fifo_begin_kill_address; */
/*     long int fifo_n_bytes_to_kill; */
/*     int ret = 0; */

/*     //fifo_n_bytes_to_kill = bytes; */
/*     //if ( fifo_loc < fifo_head ) { */
/*     // */
/*     //	fifo_begin_kill_address = fifo_head; */
/*     //	fifo_n_bytes_to_kill -= (fifo_head - fifo_loc) */
/*     // */
/*     //} else { */
/*     // */
/*     //} */
    
    

/*   /\* if (bytes <= fifo_avail(fifo)) { *\/ */
/*   /\*   fifo->head += bytes; *\/ */
/*   /\*   ret = bytes; *\/ */
/*   /\* } *\/ */

/*   return(ret); */
/* } */

void fifo_destroy(struct simple_fifo *fifo) {
  free(fifo->base);
}

long int fifo_avail(struct simple_fifo *fifo) {
  return(fifo->tail - fifo->head);
}

long int fifo_flush(struct simple_fifo *fifo) {
    fifo->head = fifo->tail;
    return 0;
}
    
long int fifo_headpos_rel_to_fifo_base(struct simple_fifo *fifo){

    return (void *)fifo->head - (void *)fifo->base;

}

/*Returns the distance of the beginning of the string relative to the head of the FIFO*/
long int fifo_search(struct simple_fifo *fifo,  size_t search_len, char *search_str, size_t search_strlen ) {
  
  void *ret = NULL;
  
  ret = memmem(fifo->head, search_len, search_str, search_strlen );

  if(ret == NULL) {
    return -1;
  } 
  return ret - (void *)fifo->head;
}
 

/* Returns the distance from fifo->head to the location where the beginning of the skipped byte string is found
 * AFTER resizing the FIFO.
 * This function moves all data between fifo->head and the first instance of skip_str forward by "skipbytes" bytes
 * So it effectively resizes the data available in the FIFO by overwriting all bytes to be skipped
 */
long int fifo_skip(struct simple_fifo *fifo, char *skip_str, size_t str_len, long int start_loc, long int skipbytes, size_t search_len, long int offset) {
/* long int fifo_skip(char *skip_str, size_t str_len, long int start_loc, long int skipbytes, size_t search_len) { */
  void *str_loc;
  long int span; // Amount of data between fifo->head and str_loc
  //  long int shift = fifo->head - fifo->base; // Current read point

  /* Calculate distance from head to skip_str */
  str_loc = memmem(fifo->head+start_loc, search_len, skip_str, str_len ); 
  if( str_loc != NULL ){

      str_loc += offset;

      span = (long int)str_loc - (long int)fifo->head; 

      /* Move all data between fifo->head and str_loc forward by skipbytes, overwriting all bytes to be skipped */
      memmove(fifo->head + skipbytes, fifo->head, span);
    
      fifo->head += skipbytes;
    
      return span; 
  }
  else {
    return EXIT_FAILURE;
  }
}
