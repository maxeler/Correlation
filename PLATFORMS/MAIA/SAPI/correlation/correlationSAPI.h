/**\file */
#ifndef SLIC_DECLARATIONS_correlation_H
#define SLIC_DECLARATIONS_correlation_H
#include "MaxSLiCInterface.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define correlation_numVectorsPerBurst (4)
#define correlation_maxNumVariables (6000)
#define correlation_numTopScores (10)
#define correlation_numPipes (12)



/*============================ Action loadLMem ============================*/

/*------------------------- Basic Static Interface ------------------------*/


/* Run the action 'loadLMem'. */

void correlation_loadLMem(
	uint64_t param_numBursts, 				/* [in] Link to "numBursts".					*/
	int32_t *param_CorrelationKernel_loopLength,		/* [out] Link from "CorrelationKernel_loopLength".		*/
	const void *instream_in_memLoad 			/* [in] The array is size of (param_numBursts * 384) bytes.	*/
);


/* Schedule to run the action 'loadLMem' on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.*/
  
max_run_t *correlation_loadLMem_nonblock( 			/* Returns a handle on the execution status, or NULL in case of error.	*/
	uint64_t param_numBursts, 				/* [in] Link to "numBursts".						*/
	int32_t *param_CorrelationKernel_loopLength, 		/* [out] Link from "CorrelationKernel_loopLength".			*/
	const void *instream_in_memLoad 			/* [in] The array is size of (param_numBursts * 384) bytes.		*/
);


/*----------------------- Advanced Static Interface -----------------------*/


/* Structure containing parameters for executing action 'loadLMem' */

typedef struct { 
	uint64_t param_numBursts; 				/* [in] Link to "numBursts".					*/
	int32_t *param_CorrelationKernel_loopLength; 		/* [out] Link from "CorrelationKernel_loopLength".		*/
	const void *instream_in_memLoad; 			/* [in] The array is size of (param_numBursts * 384) bytes.	*/
} correlation_loadLMem_actions_t;


/* Run the action 'loadLMem'. */

void correlation_loadLMem_run(
	max_engine_t *engine, 					/* [in] The engine on which the actions will be executed.	*/
	correlation_loadLMem_actions_t *interface_actions  	/* [in, out] Structure containing the parameters for the action.*/
);


/* Schedule to run the action 'loadLMem' on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.*/
  
max_run_t *correlation_loadLMem_run_nonblock( 			/* Returns a handle on the execution status, or NULL in case of error.	*/
	max_engine_t *engine, 					/* [in] The engine on which the actions will be executed.		*/
	correlation_loadLMem_actions_t *interface_actions 	/* [in, out] Structure containing the parameters for the action.	*/
);


/* Group run of action 'loadLMem'. */

void correlation_loadLMem_run_group(
	max_group_t *group, 					/* [in] Group to use for executing action.			*/
	correlation_loadLMem_actions_t *interface_actions 	/* [in, out] Structure containing the parameters for the action.*/
);


/* Schedule the actions to run on the first device available in the group and return immediately.
 * The status of the run must be checked with ::max_wait. 
 * Note that use of ::max_nowait is prohibited with non-blocking running on groups:
 * see the ::max_run_group_nonblock documentation for more explanation. */
 
max_run_t *correlation_loadLMem_run_group_nonblock( 		/* Returns a handle on the execution status, or NULL in case of error.	*/
	max_group_t *group, 					/* [in] Group to use for executing action.				*/
	correlation_loadLMem_actions_t *interface_actions 	/* [in, out] Structure containing the parameters for the action.	*/
);


/* Array run of action 'loadLMem'. */

void correlation_loadLMem_run_array(
	max_engarray_t *engarray, 				/* [in] The array of devices to use.						*/
	correlation_loadLMem_actions_t *interface_actions[]	/* [in, out] Array of structures containing the parameters for the actions.	*/
);


/* Schedule to run the array of actions on the array of engines, and return immediately.
 * The length of interface_actions must match the size of engarray.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released. */
 
max_run_t *correlation_loadLMem_run_array_nonblock( 		/* Returns a handle on the execution status, or NULL in case of error.		*/
	max_engarray_t *engarray, 				/* [in] The array of devices to use.						*/
	correlation_loadLMem_actions_t *interface_actions[]	/* [in, out] Array of structures containing the parameters for the actions.	*/
);


/* Converts a static-interface action struct into a dynamic-interface max_actions_t struct.
 * Note that this is an internal utility function used by other functions in the static interface. */
 
max_actions_t* correlation_loadLMem_convert(			/* Returns the dynamic-interface actions to run, or NULL in case of error.	*/
	max_file_t *maxfile, 					/* [in] The maxfile to use.							*/
	correlation_loadLMem_actions_t *interface_actions 	/* [in] The interface-specific actions to run.					*/
);


/* Auxiliary function to evaluate expression for "CorrelationKernel_loopLength". */

int correlation_loadLMem_get_CorrelationKernel_loopLength(void);



/*============================= Action default ============================*/

/*------------------------- Basic Static Interface ------------------------*/


/* Run the action 'default'. */

void correlation(
	uint64_t param_numBursts, 				/* [in] Link to "numBursts" -> The size of data of one correlation step in number of bursts.		*/
	uint64_t param_numSteps, 				/* [in] Link to "numSteps" -> The number of (full) correlation steps.					*/
	uint64_t param_numVariables, 				/* [in] Link to "numVariables" -> The number of (input) variables.					*/
	uint64_t param_outputLastStep, 				/* [in] Link to "outputLastStep" -> Non-zero iff all correlations of the last step shall be returned.	*/
	double param_windowSize, 				/* [in] Link to "windowSize" -> The window size.							*/
	const double *instream_in_precalculations, 		/* [in] The array is size of (2 * param_numSteps * param_numVariables).					*/
	const double *instream_in_variable_pair, 		/* [in] The array is size of (2 * param_numSteps * param_numVariables).					*/
	double *outstream_out_correlation, 			/* [out] The array is size of (10 * 12 * param_numSteps * param_CorrelationKernel_loopLength) + 
								(param_outputLastStep==0 ? param_numBursts * 48 : 0).							*/
	uint32_t *outstream_out_indices 			/* The array is size of (2 * 10 * 12 * param_numSteps * param_loopLength).				*/
);


/* Schedule to run the action 'default' on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.*/ 
 
max_run_t* correlation_nonblock( 				/* Returns a handle on the execution status, or NULL in case of error.					*/
	uint64_t param_numBursts, 				/* [in] Link to "numBursts" -> The size of data of one correlation step in number of bursts.		*/
	uint64_t param_numSteps, 				/* [in] Link to "numSteps" -> The number of (full) correlation steps.					*/
	uint64_t param_numVariables, 				/* [in] Link to "numVariables" -> The number of (input) variables.					*/
	uint64_t param_outputLastStep, 				/* [in] Link to "outputLastStep" -> Non-zero iff all correlations of the last step shall be returned.	*/
	double param_windowSize, 				/* [in] Link to "windowSize" -> The window size.							*/
	const double *instream_in_precalculations, 		/* [in] The array is size of (2 * param_numSteps * param_numVariables).					*/
	const double *instream_in_variable_pair, 		/* [in] The array is size of (2 * param_numSteps * param_numVariables).					*/
	double *outstream_out_correlation, 			/* [out] The array is size of (10 * 12 * param_numSteps * param_CorrelationKernel_loopLength) + 
								(param_outputLastStep==0 ? param_numBursts * 48 : 0).							*/
	uint32_t *outstream_out_indices 			/* The array is size of (2 * 10 * 12 * param_numSteps * param_loopLength).				*/
);


/*----------------------- Advanced Static Interface -----------------------*/


/* Structure containing parameters for executing action 'default'. */

typedef struct { 
	uint64_t param_numBursts; 				/* [in] Link to "numBursts" -> The size of data of one correlation step in number of bursts.		*/
	uint64_t param_numSteps; 				/* [in] Link to "numSteps" -> The number of (full) correlation steps.					*/
	uint64_t param_numVariables; 				/* [in] Link to "numVariables" -> The number of (input) variables.					*/
	uint64_t param_outputLastStep; 				/* [in] Link to "outputLastStep" -> Non-zero iff all correlations of the last step shall be returned.	*/
	double param_windowSize; 				/* [in] Link to "windowSize" -> The window size.							*/
	const double *instream_in_precalculations; 		/* [in] The array is size of (2 * param_numSteps * param_numVariables).					*/
	const double *instream_in_variable_pair; 		/* [in] The array is size of (2 * param_numSteps * param_numVariables).					*/
	double *outstream_out_correlation; 			/* [out] The array is size of (10 * 12 * param_numSteps * param_CorrelationKernel_loopLength) + 
								(param_outputLastStep==0 ? param_numBursts * 48 : 0).							*/
	uint32_t *outstream_out_indices; 			/* The array is size of (2 * 10 * 12 * param_numSteps * param_loopLength).				*/
} correlation_actions_t;


/* Run the actions 'default'. */

void correlation_run( 
	max_engine_t *engine, 					/* [in] The engine on which the actions will be executed.		*/
	correlation_actions_t *interface_actions		/* [in, out] Structure containing the parameters for the action.	*/
);

/* Schedule to run the action 'default' on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.*/
 
max_run_t *correlation_run_nonblock(				/* Returns a handle on the execution status, or NULL in case of error.	*/
	max_engine_t *engine,					/* [in] The engine on which the actions will be executed.		*/
	correlation_actions_t *interface_actions);		/* [in, out] Structure containing the parameters for the action.	*/


/* Group run of action 'default'. */

void correlation_run_group( 
	max_group_t *group, 					/* [in] Group to use for executing action.				*/
	correlation_actions_t *interface_actions		/* [in, out] Structure containing the parameters for the action.	*/
);


/* Schedule the actions to run on the first device available in the group and return immediately.
 * The status of the run must be checked with ::max_wait. 
 * Note that use of ::max_nowait is prohibited with non-blocking running on groups:
 * see the ::max_run_group_nonblock documentation for more explanation. */
 
max_run_t *correlation_run_group_nonblock(			/* Returns a handle on the execution status, or NULL in case of error.	*/
	max_group_t *group, 					/* [in] Group to use for executing action.				*/
	correlation_actions_t *interface_actions		/* [in, out] Structure containing the parameters for the action.	*/
);


/* Array run of action 'default'. */

void correlation_run_array(
	max_engarray_t *engarray, 				/* [in] The array of devices to use.						*/
	correlation_actions_t *interface_actions[]		/* [in, out] Array of structures containing the parameters for the actions.	*/
);


/* Schedule to run the array of actions on the array of engines, and return immediately.
 * The length of interface_actions must match the size of engarray.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released. */
 
max_run_t *correlation_run_array_nonblock( 			/* Returns a handle on the execution status, or NULL in case of error.		*/
	max_engarray_t *engarray, 				/* [in] The array of devices to use.						*/
	correlation_actions_t *interface_actions[]		/* [in, out] Array of structures containing the parameters for the actions.	*/
);


/* Converts a static-interface action struct into a dynamic-interface max_actions_t struct.
 * Note that this is an internal utility function used by other functions in the static interface. */
 
max_actions_t* correlation_convert(				/* Returns the dynamic-interface actions to run, or NULL in case of error.	*/
	max_file_t *maxfile, 					/* [in] The maxfile to use.							*/
	correlation_actions_t *interface_actions		/* [in] The interface-specific actions to run.					*/
);

/* Auxiliary function to evaluate expression for "CorrelationKernel_loopLength". */

int correlation_get_CorrelationKernel_loopLength(void);


/* Initialise a maxfile. */
max_file_t* correlation_init(void);

/* Error handling functions */
int correlation_has_errors(void);
const char* correlation_get_errors(void);
void correlation_clear_errors(void);
/* Free statically allocated maxfile data */
void correlation_free(void);
/* returns: -1 = error running command; 0 = no error reported */
int correlation_simulator_start(void);
/* returns: -1 = error running command; 0 = no error reported */
int correlation_simulator_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* SLIC_DECLARATIONS_correlation_H */

