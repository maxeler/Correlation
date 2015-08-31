#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "correlationSAPI.h"

void random_data (double** data, uint64_t numTimeseries, uint64_t sizeTimeseries) {

	srand(time(NULL));
    
	for (uint64_t i=0; i<numTimeseries; i++) {
		for (uint64_t j=0; j<sizeTimeseries; j++) {
			data[i][j] = ((double)rand()/(double)RAND_MAX);
		}
	}
}

// Calculate number of bursts for initializing LMem
static size_t calcNumBursts (size_t numTimeseries) {

	size_t numVectors = 0;
	for (size_t i = 1; i <= numTimeseries; ++i)
		numVectors += (i + (correlation_numPipes - 1)) / correlation_numPipes;

	return (numVectors + (correlation_numVectorsPerBurst-1)) / correlation_numVectorsPerBurst;
}


static void prepare_data_for_dfe (double** data, uint64_t sizeTimeseries, uint64_t numTimeseries, uint64_t numTimesteps, double windowSize, double* precalculations, double* data_pairs) {

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

uint64_t calc_num_correlations(uint64_t numTimeseries) {
	return (numTimeseries*(numTimeseries-1))/2;
}

uint64_t calc_index (uint64_t i, uint64_t j) {
	if (i==j) {
		printf("i and j must not be the same!\n");
		return -1;
	}
	if (i<j) {
		uint64_t tmp;
		tmp = j;
		j = i;
		i = tmp;
	}

	return (i*(i-1))/2+j;
}

void correlate (double** data, uint64_t sizeTimeseries, uint64_t numTimeseries, double* correlations) {

	uint64_t numTimesteps = sizeTimeseries;	
	double windowSize = (uint64_t)sizeTimeseries;

	uint64_t numBursts = calcNumBursts (numTimeseries);
	int32_t loopLength = correlation_get_CorrelationKernel_loopLength();	
	
	double* precalculations = (double*) malloc (2 * numTimeseries * numTimesteps * sizeof(double));
	double* data_pairs = (double*) malloc (2 * numTimeseries * numTimesteps * sizeof(double));
	
	int burstSize = 384/2;//for anything other than ISCA this should be 384
	void* in_memLoad = (void*) malloc (numBursts * burstSize);
	memset(in_memLoad,0,numBursts*burstSize);

	double* out_correlation = (double*) malloc ((numTimesteps * loopLength * correlation_numTopScores * correlation_numPipes + numBursts * 48) * sizeof(double));
	uint32_t* out_indices = (uint32_t*) malloc (2 * numTimesteps * loopLength * correlation_numTopScores * correlation_numPipes * sizeof(uint32_t));	

	prepare_data_for_dfe (data, sizeTimeseries, numTimeseries, numTimesteps, windowSize, precalculations, data_pairs);
	
	correlation_loadLMem(numBursts, &loopLength, in_memLoad);
	printf("LMem initialized!\n");
	
	//Executing correlation action
	correlation(numBursts, numTimesteps, numTimeseries, 1, windowSize,	// scalar inputs 
				precalculations, data_pairs,			// streaming reordered inputs
				out_correlation, out_indices			// streaming unordered outputs
	);

	uint64_t position = 0;
	uint64_t index = 0;
	uint64_t start = (numTimesteps-1) * loopLength * correlation_numTopScores * correlation_numPipes;

	for (uint64_t i=0; i< numTimeseries; i++) {
		memcpy(&correlations[index], &out_correlation[start+position], i*sizeof(double));
		index += i;
		position += ((i/12)+1)*12;	
	}
						
	free (precalculations);
	free (data_pairs);
	free (in_memLoad);
	free (out_correlation);
	free (out_indices);
	
}
