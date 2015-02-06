/**
 *	File: correlationCPUCode.c
 *	Purpose: calling correlationSAPI.h for correlation.max
 *
 *	Correlation formula:
 *		scalar r(x,y) = (n*SUM(x,y) - SUM(x)*SUM(y))*SQRT_INVERSE(x)*SQRT_INVERSE(y)
 *		where:
 *			x,y,... 		- Time series data to be correlated
 *			n 			- window for correlation (minimum size of 2)
 *			SUM(x) 			- sum of all elements inside a window
 *			SQRT_INVERSE(x)		- 1/sqrt(n*SUM(x^2)- (SUM(x)^2))
 *	
 *	Streams:
 *
 * 		Action 'loadLMem':
 *		------------------
 *			[in] memLoad		- content which LMem will be initialized with
 *
 *
 *		Action 'default':
 *		-----------------
 *
 *			[in] precalculations		- {SUM(x), SQRT_INVERSE(x)} for all timeseries for every timestep
 *
 *			[in] data_pair			- {..., x[i], x[i-n], y[i], y[i-n], ... , x[i+1], x[i-n+1], y[i+1], y[i-n+1], ...} for all timeseries for every timestep; 
 *							IF (i-n)<0 => x[i-n]=0
 *
 *			[out] correlation		- numPipes * CorrelationKernel_loopLength * topScores correlations for every timestep
 *
 * 			[out] indices			- pair of timeseries indices for each result in correlation stream  
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>


#include "correlationSAPI.h"


//Time measuring
double gettime(void) {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return tv.tv_sec + 1e-6 * tv.tv_usec;
}

// Calculate number of bursts for initializing LMem
size_t calcNumBursts (size_t numTimeseries) {

	size_t numVectors = 0;
	for (size_t i = 1; i <= numTimeseries; ++i)
		numVectors += (i + (correlation_numPipes - 1)) / correlation_numPipes;

	return (numVectors + (correlation_numVectorsPerBurst-1)) / correlation_numVectorsPerBurst;
}


//Calculate top correlations
void topCorrelations (double* correlations, uint32_t* indices, uint64_t numCorrelations, double* correlations_top, uint32_t* indices_top, int numTopScores) {
	
	for (uint64_t step=0; step<numCorrelations-1; ++step) {
		for (uint64_t i=0; i<numCorrelations-step-1; ++i) {
			if (correlations[i]<correlations[i+1]) {
				double temp_corr = correlations[i];
				uint32_t temp_in1 = indices[2*i];
				uint32_t temp_in2 = indices[2*i+1];
				
				correlations[i] = correlations[i+1];
				indices[2*i] = indices[2*(i+1)];
				indices[2*i+1] = indices[2*(i+1)+1];

				correlations[i+1] = temp_corr;
				indices[2*(i+1)] = temp_in1;
				indices[2*(i+1)+1] = temp_in2;
					
			}	
		}	
	}

	memcpy(correlations_top, correlations, numTopScores*sizeof(double));
	memcpy(indices_top, indices, 2*numTopScores*sizeof(uint32_t));
}


void prepare_data_for_dfe (double** data, uint64_t sizeTimeseries, uint64_t numTimeseries, uint64_t numTimesteps, double windowSize, double* precalculations, double* data_pairs) {

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

	srand(time(NULL));
	
	for (uint64_t i=0; i<numTimeseries; i++) {
		for (uint64_t j=0; j<sizeTimeseries; j++) {
			data[i][j] = ((double)rand()/(double)RAND_MAX);
		}
	}
}


int main () {

	uint64_t numTimesteps = 12;	// not limited by DFE
	uint64_t numTimeseries = 200;	// DFE supports 200 - 6000, Simulation supports 250 - 6000
	double windowSize = 9;

	uint64_t sizeTimeseries = 100;	// number of elements in the timeseries

	uint64_t numBursts = calcNumBursts (numTimeseries);
	int32_t loopLength = correlation_get_CorrelationKernel_loopLength();
	double start_time, reorder_time, DFE_time, sort_time, total_time;

	
	/*===================== INITIALIZING =====================*/
	
	double** data = (double**) malloc (numTimeseries*sizeof (double*));
	for (uint64_t i=0; i < numTimeseries; i++) 
		data[i] = (double*) malloc (sizeTimeseries*sizeof(double));

	printf("Generating data!\n");
	random_data (data, numTimeseries, sizeTimeseries);		

	double* precalculations = (double*) malloc (2 * numTimeseries * numTimesteps * sizeof(double));
	double* data_pairs = (double*) malloc (2 * numTimeseries * numTimesteps * sizeof(double));

	void* in_memLoad = (void*) malloc (numBursts * 384);
	memset(in_memLoad,0,numBursts*384);
	
	// DFE outputs
	double* out_correlation = (double*) malloc (numTimesteps * loopLength * correlation_numTopScores * correlation_numPipes * sizeof(double));
	uint32_t* out_indices = (uint32_t*) malloc (2 * numTimesteps * loopLength * correlation_numTopScores * correlation_numPipes * sizeof(uint32_t));
		
	// Top correlations for every timestep	
	double* correlations_final = (double*) malloc (correlation_numTopScores*numTimesteps*sizeof(double));
	uint32_t* indices_final = (uint32_t*) malloc (2 * correlation_numTopScores*numTimesteps*sizeof(double));
	
	
	/*==================== REORDERING DATA for the DFE ====================*/
	
	printf("Reordering data for the DFE.\n");
	start_time = gettime();
	prepare_data_for_dfe (data, sizeTimeseries, numTimeseries, numTimesteps, windowSize, precalculations, data_pairs);
	reorder_time = gettime() - start_time;
		
	/*==================== Computation ====================*/

	printf("CORRELATE!\n");
	start_time = gettime();
	
	//Executing loadLMem action
	correlation_loadLMem(numBursts, &loopLength, in_memLoad);
	printf("LMem initialized!\n");
	
	//Executing correlation action
	correlation(numBursts, numTimesteps, numTimeseries, 0, windowSize,	// scalar inputs 
				precalculations, data_pairs,			// streaming reordered inputs
				out_correlation, out_indices			// streaming unordered outputs
				);				
				
	DFE_time = gettime() - start_time;
	printf("DFE done!\n");
	
	//CPU sorting outputs
	int correlations_per_step = loopLength * correlation_numTopScores * correlation_numPipes; // number of correlations in output per timestep 
	start_time = gettime();
	
	printf("Sorting outputs.\n");
	for (uint64_t i=0; i<numTimesteps; i++) {
		topCorrelations (&out_correlation[i*correlations_per_step], &out_indices[2*i*correlations_per_step], correlations_per_step,
		&correlations_final[i*correlation_numTopScores], &indices_final[i*correlation_numTopScores], correlation_numTopScores); 	
	}
	sort_time = gettime() - start_time;
	
	total_time = reorder_time + DFE_time + sort_time;
	
	printf("Data reordering time: %.5lfs\n", reorder_time);
	printf("DFE execution time: %.5lfs\n", DFE_time);
	printf("Sorting time: %.5lfs\n", sort_time);
	printf("Total execution time: %.5lfs\n", total_time);
	
	//Deallocating memory
	free (out_indices);
	free (out_correlation);
	free (in_memLoad);
	free (data_pairs);
	free (precalculations);
	free (correlations_final);
	free (indices_final);

	for (uint64_t i=0; i<numTimeseries; i++)
		free (data[i]);
	free (data);
	return 0;
}
