#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#define correlation_maxNumTimeseries (6000)
#define correlation_numTopScores (10)

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

void correlation_data_flow (uint64_t numTimesteps, uint64_t numTimeseries, uint64_t windowSize, double* precalculations, double* data_pairs, double* correlations, uint32_t* indices) {

	uint64_t numCorrelations = (numTimeseries*(numTimeseries-1))/2;
	uint64_t index_correlation;
	
	double* sums_xy = (double*) calloc (numCorrelations, sizeof(double));
	double* correlations_step = (double*) calloc (numCorrelations, sizeof(double)); 		// all correlations in current step
	uint32_t* indices_step = (uint32_t*) calloc (2*numCorrelations, sizeof(uint32_t)); 		// corresponding indices for correlations_step
	double* correlations_top = (double*) malloc (correlation_numTopScores*sizeof(double)); 		// top correlations in current step
	uint32_t* indices_top = (uint32_t*) malloc (2*correlation_numTopScores*sizeof(uint32_t));	// corresponding indices for correlations_top
	
	for (uint64_t s=0; s<numTimesteps; s++) {

		index_correlation = 0;
		
		for (uint64_t i=0; i<numTimeseries; i++) {

			double old_x = data_pairs[2*s*numTimeseries + 2*i + 1];
			double new_x = data_pairs[2*s*numTimeseries + 2*i];	

			for (uint64_t j=i+1; j<numTimeseries; j++) {
	
				double old_y = data_pairs[2*s*numTimeseries + 2*j + 1];
				double new_y = data_pairs[2*s*numTimeseries + 2*j];

	
				sums_xy[index_correlation] += new_x*new_y - old_x*old_y;		

				correlations_step[index_correlation]	= 	(windowSize*sums_xy[index_correlation]-
										precalculations[2*s*numTimeseries + 2*i]*precalculations[2*s*numTimeseries + 2*j])*
										precalculations[2*s*numTimeseries + 2*i+1]*precalculations[2*s*numTimeseries + 2*j+1];
		
				indices_step[2*index_correlation] = j;
				indices_step[2*index_correlation+1] = i;
	
				index_correlation++;
				
			}
		}
		topCorrelations (correlations_step, indices_step, numCorrelations, correlations_top, indices_top, correlation_numTopScores);
		
		memcpy(&correlations[s*correlation_numTopScores], correlations_top, correlation_numTopScores*sizeof(double));
		memcpy(&indices[2*s*correlation_numTopScores], indices_top, 2*correlation_numTopScores*sizeof(uint32_t));

	}
	
	free(sums_xy);
	free(correlations_step);
	free(indices_step);
	free(correlations_top);
	free(indices_top);

}

