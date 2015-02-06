/**
 * File: correlationCPUCode.c
 * Purpose: calling correlationSAPI.h for correlation.max
 *
 * Correlation formula:
 *	scalar r(x,y) = (n*SUM(x,y) 	- SUM(x)*SUM(y))*SQRT_INVERSE(x)*SQRT_INVERSE(y)
 *	where:
 *		x,y,... 		- Time series data to be correlated
 *		n 			- window for correlation (minimum size of 2)
 *		SUM(x) 			- sum of all elements inside a window
 *		SQRT_INVERSE(x)		- 1/sqrt(n*SUM(x^2)- (SUM(x)^2))
 *	
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#define correlation_maxNumTimeseries (6000)
#define correlation_numTopScores (10)

// Function is from correlation_data.c
void correlation_data_flow (uint64_t numTimesteps, uint64_t numTimeSeries, uint64_t windowSize,
				double* precalculations, double*data_pairs,
				double* correlations, uint32_t* indices);


//Time measuring
double gettime(void) {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return tv.tv_sec + 1e-6 * tv.tv_usec;
}


void correlation_control_flow (double** data, uint64_t sizeTimeseries, uint64_t numTimeseries, uint64_t numTimesteps, double windowSize, double* precalculations, double* data_pairs) {

	if (numTimeseries > correlation_maxNumTimeseries) {
		fprintf(stderr, "Number of Time series should be less or equal to %d. Terminating!\n", correlation_maxNumTimeseries);
		fflush(stderr);
		exit(-1);
	}
	
	if (windowSize <2) {
		fprintf(stderr, "Window size must be equal or greater than 2. Terminating!\n");
		fflush(stderr);
		exit(-1);
	}

	if (numTimesteps > sizeTimeseries) {
		fprintf(stderr, "Number of Time steps should be less or equal to size of Time series. Terminating!\n");
		fflush(stderr);
		exit(-1);
	}
	
	int64_t old_index;
	double old, new;
	
	double** sums = (double**) malloc (numTimesteps*sizeof (double*));
	double** sums_sq = (double**) malloc (numTimesteps*sizeof (double*));
	double** inv = (double**) malloc (numTimesteps*sizeof (double*));

	for (uint64_t i=0; i < numTimesteps; i++) {
		sums[i] = (double*) malloc (numTimeseries*sizeof(double));
		sums_sq[i] = (double*) malloc (numTimeseries*sizeof(double));
		inv[i] = (double*) malloc (numTimeseries*sizeof(double));
	}

	// 2 DFE input streams: precalculations and data pairs 
	for (uint64_t i=0; i<numTimesteps; i++) {

		old_index = i - (uint64_t)windowSize;

		for (uint64_t j=0; j<numTimeseries; j++) {
			if (old_index<0)
				old = 0;
			else
				old = data [j][old_index];

			new = data [j][i];

			if (i==0) {
				sums [i][j] = new;
				sums_sq [i][j] = new*new;
			}
			else {
				sums [i][j] = sums [i-1][j] + new - old;
				sums_sq [i][j] = sums_sq [i-1][j] + new*new - old*old;
			}
			
			inv [i][j] = 1/sqrt((uint64_t)windowSize*sums_sq[i][j] - sums[i][j]*sums[i][j]);
			
			//Precalculations REORDERED in DFE ORDER
			precalculations [2*i*numTimeseries + 2*j] = sums[i][j];
			precalculations [2*i*numTimeseries + 2*j + 1] = inv [i][j];

			//Data pairs REORDERED in DFE ORDER
			data_pairs[2*i*numTimeseries + 2*j] = new;
			data_pairs[2*i*numTimeseries + 2*j + 1] = old;
		}
	}

	for (uint64_t i=0; i<numTimesteps; i++) {
		free (sums[i]);
		free (sums_sq[i]);
		free (inv[i]);
	}
	
	free (sums);
	free (sums_sq);
	free (inv);
			
}

void random_data (double** data, uint64_t numTimeseries, uint64_t sizeTimeseries) {

	srand(0);
	
	for (uint64_t i=0; i<numTimeseries; i++) {
		for (uint64_t j=0; j<sizeTimeseries; j++) {
			data[i][j] = ((double)rand()/(double)RAND_MAX);
		}
	}
}


int main () {

	uint64_t numTimesteps = 12;		
	uint64_t numTimeseries = 200;	
	uint64_t windowSize = 9;

	uint64_t sizeTimeseries = 100;	// number of elements in the timeseries

	double start_time, reorder_time, dataflow_time, total_time;

	
	/*===================== INITIALIZING =====================*/
	
	double** data = (double**) malloc (numTimeseries*sizeof (double*));
	for (uint64_t i=0; i < numTimeseries; i++) 
		data[i] = (double*) malloc (sizeTimeseries*sizeof(double));

	printf("Generating data!\n");
	random_data (data, numTimeseries, sizeTimeseries);		

	double* precalculations = (double*) malloc (2 * numTimeseries * numTimesteps * sizeof(double));
	double* data_pairs = (double*) malloc (2 * numTimeseries * numTimesteps * sizeof(double));
	
	// Correlations_data_flow outputs	
	double* correlations = (double*) malloc (correlation_numTopScores*numTimesteps*sizeof(double));
	uint32_t* indices = (uint32_t*) malloc (2 * correlation_numTopScores*numTimesteps*sizeof(double));
	
	
	/*==================== SPLIT CONTROL FLOW ====================*/
	
	printf("SPLITING CONTROL FLOW.\n");
	start_time = gettime();
	correlation_control_flow (data, sizeTimeseries, numTimeseries, numTimesteps, windowSize, precalculations, data_pairs);
	reorder_time = gettime() - start_time;
		
	/*==================== SPLIT DATA FLOW ====================*/

	printf("CORRELATE!\n");
	start_time = gettime();
	correlation_data_flow (numTimesteps, numTimeseries, windowSize, precalculations, data_pairs, correlations, indices);	
	dataflow_time = gettime() - start_time;
	
	total_time = reorder_time + dataflow_time;
	
	printf("Data reordering time: %.5lfs\n", reorder_time);
	printf("Dataflow execution time: %.5lfs\n", dataflow_time);
	printf("Total execution time: %.5lfs\n", total_time);
		
	//Deallocating memory
	free (data_pairs);
	free (precalculations);
	free (correlations);
	free (indices);

	for (uint64_t i=0; i<numTimeseries; i++)
		free (data[i]);
	free (data);
	return 0;
}
