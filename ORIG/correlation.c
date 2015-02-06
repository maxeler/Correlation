/**
 *	File: correlation.c
 *
 *	Correlation formula:
 *		scalar r(x,y) = (n*SUM(x,y) - SUM(x)*SUM(y))*SQRT_INVERSE(x)*SQRT_INVERSE(y)
 *		where:
 *			x,y,...			- Time series data to be correlated
 *			n			- window for correlation (minimum size of 2)
 *			SUM(x)			- sum of all elements inside a window 
 *			SQRT_INVERSE(x)		- 1/sqrt(n*SUM(x^2)- (SUM(x)^2))
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


//Time measuring
double gettime(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

//Generating random data
void random_data (double** data, uint64_t numTimeseries, uint64_t sizeTimeseries) {

	srand(time(NULL));

	for (uint64_t i=0; i<numTimeseries; i++) {
		for (uint64_t j=0; j<sizeTimeseries; j++) {
			data[i][j] = ((double)rand()/(double)RAND_MAX);
		}
	}
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
 

void correlation (double** data, uint64_t sizeTimeseries, uint64_t numTimeseries, uint64_t numTimesteps, uint64_t windowSize, double* correlations, uint32_t* indices) {

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

	uint64_t numCorrelations = (numTimeseries*(numTimeseries-1))/2;

	double* sums = (double*) calloc (numTimeseries,sizeof(double));  
	double* sums_sq = (double*) calloc (numTimeseries, sizeof(double));
	double* sums_xy = (double*) calloc (numCorrelations, sizeof(double));
	double* correlations_step = (double*) calloc (numCorrelations, sizeof(double)); 		// all correlations in current step
	uint32_t* indices_step = (uint32_t*) calloc (2*numCorrelations, sizeof(uint32_t)); 		// corresponding indices for correlations_step
	double* correlations_top = (double*) malloc (correlation_numTopScores*sizeof(double)); 		// top correlations in current step
	uint32_t* indices_top = (uint32_t*) malloc (2*correlation_numTopScores*sizeof(uint32_t));	// corresponding indices for correlations_top

	uint64_t index_correlation;
	
	for (uint64_t s=0; s<numTimesteps; s++) {

		index_correlation = 0;

		for (uint64_t i=0; i<numTimeseries; i++) {

			double old = s>=windowSize ? data[i][s-windowSize] : 0;
			double new = data [i][s];

			sums[i] += new - old;
			sums_sq[i] += new*new - old*old;
		}
		
		for (uint64_t i=0; i<numTimeseries; i++) {

			double old_x = s>=windowSize ? data[i][s-windowSize] : 0;
			double new_x = data [i][s];	

			for (uint64_t j=i+1; j<numTimeseries; j++) {
	
				double old_y = s>=windowSize ? data[j][s-windowSize] : 0;
				double new_y = data [j][s];
	
				sums_xy[index_correlation] += new_x*new_y - old_x*old_y;		

				correlations_step[index_correlation]	= 	(windowSize*sums_xy[index_correlation]-sums[i]*sums[j])/
										(sqrt(windowSize*sums_sq[i]-sums[i]*sums[i])*sqrt(windowSize*sums_sq[j]-sums[j]*sums[j]));
		
				indices_step[2*index_correlation] = j;
				indices_step[2*index_correlation+1] = i;
	
				index_correlation++;
				
			}
		}
		topCorrelations (correlations_step, indices_step, numCorrelations, correlations_top, indices_top, correlation_numTopScores);
		
		memcpy(&correlations[s*correlation_numTopScores], correlations_top, correlation_numTopScores*sizeof(double));
		memcpy(&indices[2*s*correlation_numTopScores], indices_top, 2*correlation_numTopScores*sizeof(uint32_t));

	}

	free(sums);
	free(sums_sq);	
	free(sums_xy);
	free(correlations_step);
	free(indices_step);
	free(correlations_top);
	free(indices_top);
}


int main () {

	uint64_t numTimesteps = 12; 
	uint64_t numTimeseries = 200; 	 
	double windowSize = 9;

	uint64_t sizeTimeseries = 100;

	double time;
	
	double** data = (double**) malloc (numTimeseries*sizeof (double*));
	for (uint64_t i=0; i < numTimeseries; i++)
		data[i] = (double*) malloc (sizeTimeseries*sizeof(double));
	
	double* correlations = (double*) malloc (numTimesteps*correlation_numTopScores*sizeof(double));
	uint32_t* indices = (uint32_t*) malloc (2*numTimesteps*correlation_numTopScores*sizeof(uint32_t));

	printf("Generating random data.\n");
	random_data (data, numTimeseries, sizeTimeseries);

	printf("Correlate.\n");
	time = gettime();
	correlation (data, sizeTimeseries, numTimeseries, numTimesteps, windowSize, correlations, indices);	
	printf("Total correlation time: %.5lfs\n", gettime()-time);
	 	
	//Deallocating memory
	free (correlations);
	free (indices);	
	for (uint64_t i=0; i<numTimeseries; i++)
		free (data[i]);
	free (data);

	return 0;
}
