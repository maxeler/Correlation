#include <stdio.h>
#include <stdint.h>


/* Generate random data */
void random_data (
	double** data,			/* Array of Timeseries */ 
	uint64_t numTimeseries, 	/* Number of Timeseries */ 
	uint64_t sizeTimeseries		/* Size of each Timeseries */
);

/* Calculate cross correlations among all numTimeseries. */
void correlate (
	double** data, 			/* Input data */
	uint64_t sizeTimeseries, 	/* Size of each Timeseries */
	uint64_t numTimeseries, 	/* Number of Timeseries */
	double* correlations		/* Output correlations */
);

/* Calculate index of correlation between (i,j) in correlations array */
uint64_t calc_index (
	uint64_t i,	/* ith Timeseries */ 
	uint64_t j	/* jth Timeseries */
);

/* Calculate number of correlations in correlations array */
uint64_t calc_num_correlations(
	uint64_t numTimeseries	/* Number of Timeseries */
); 


