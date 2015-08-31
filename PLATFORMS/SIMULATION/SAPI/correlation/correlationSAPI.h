/**\file */
#ifndef SLIC_DECLARATIONS_correlation_H
#define SLIC_DECLARATIONS_correlation_H
#include "MaxSLiCInterface.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define correlation_maxNumVariables (6000)
#define correlation_numTopScores (10)
#define correlation_numPipes (12)
#define correlation_PCIE_ALIGNMENT (16)
#define correlation_numVectorsPerBurst (2)


/*----------------------------------------------------------------------------*/
/*---------------------------- Interface loadLMem ----------------------------*/
/*----------------------------------------------------------------------------*/



/**
 * \brief Auxiliary function to evaluate expression for "CorrelationKernel.loopLength".
 */
int correlation_loadLMem_get_CorrelationKernel_loopLength( void );


/**
 * \brief Basic static function for the interface 'loadLMem'.
 * 
 * \param [in] param_numBursts Interface Parameter "numBursts".: The number of bursts to be written to LMem.
 * \param [out] param_CorrelationKernel_loopLength Parameter-readback "CorrelationKernel_loopLength".
 * \param [in] instream_in_memLoad The stream should be of size (param_numBursts * 192) bytes.
 */
void correlation_loadLMem(
	uint64_t param_numBursts,
	int32_t *param_CorrelationKernel_loopLength,
	const void *instream_in_memLoad);

/**
 * \brief Basic static non-blocking function for the interface 'loadLMem'.
 * 
 * Schedule to run on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 * 
 * 
 * \param [in] param_numBursts Interface Parameter "numBursts".: The number of bursts to be written to LMem.
 * \param [out] param_CorrelationKernel_loopLength Parameter-readback "CorrelationKernel_loopLength".
 * \param [in] instream_in_memLoad The stream should be of size (param_numBursts * 192) bytes.
 * \return A handle on the execution status, or NULL in case of error.
 */
max_run_t *correlation_loadLMem_nonblock(
	uint64_t param_numBursts,
	int32_t *param_CorrelationKernel_loopLength,
	const void *instream_in_memLoad);

/**
 * \brief Advanced static interface, structure for the engine interface 'loadLMem'
 * 
 */
typedef struct { 
	uint64_t param_numBursts; /**<  [in] Interface Parameter "numBursts".: The number of bursts to be written to LMem. */
	int32_t *param_CorrelationKernel_loopLength; /**<  [out] Parameter-readback "CorrelationKernel_loopLength". */
	const void *instream_in_memLoad; /**<  [in] The stream should be of size (param_numBursts * 192) bytes. */
} correlation_loadLMem_actions_t;

/**
 * \brief Advanced static function for the interface 'loadLMem'.
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in,out] interface_actions Actions to be executed.
 */
void correlation_loadLMem_run(
	max_engine_t *engine,
	correlation_loadLMem_actions_t *interface_actions);

/**
 * \brief Advanced static non-blocking function for the interface 'loadLMem'.
 *
 * Schedule the actions to run on the engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in] interface_actions Actions to be executed.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *correlation_loadLMem_run_nonblock(
	max_engine_t *engine,
	correlation_loadLMem_actions_t *interface_actions);

/**
 * \brief Group run advanced static function for the interface 'loadLMem'.
 * 
 * \param [in] group Group to use.
 * \param [in,out] interface_actions Actions to run.
 *
 * Run the actions on the first device available in the group.
 */
void correlation_loadLMem_run_group(max_group_t *group, correlation_loadLMem_actions_t *interface_actions);

/**
 * \brief Group run advanced static non-blocking function for the interface 'loadLMem'.
 * 
 *
 * Schedule the actions to run on the first device available in the group and return immediately.
 * The status of the run must be checked with ::max_wait. 
 * Note that use of ::max_nowait is prohibited with non-blocking running on groups:
 * see the ::max_run_group_nonblock documentation for more explanation.
 *
 * \param [in] group Group to use.
 * \param [in] interface_actions Actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *correlation_loadLMem_run_group_nonblock(max_group_t *group, correlation_loadLMem_actions_t *interface_actions);

/**
 * \brief Array run advanced static function for the interface 'loadLMem'.
 * 
 * \param [in] engarray The array of devices to use.
 * \param [in,out] interface_actions The array of actions to run.
 *
 * Run the array of actions on the array of engines.  The length of interface_actions
 * must match the size of engarray.
 */
void correlation_loadLMem_run_array(max_engarray_t *engarray, correlation_loadLMem_actions_t *interface_actions[]);

/**
 * \brief Array run advanced static non-blocking function for the interface 'loadLMem'.
 * 
 *
 * Schedule to run the array of actions on the array of engines, and return immediately.
 * The length of interface_actions must match the size of engarray.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * \param [in] engarray The array of devices to use.
 * \param [in] interface_actions The array of actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *correlation_loadLMem_run_array_nonblock(max_engarray_t *engarray, correlation_loadLMem_actions_t *interface_actions[]);

/**
 * \brief Converts a static-interface action struct into a dynamic-interface max_actions_t struct.
 *
 * Note that this is an internal utility function used by other functions in the static interface.
 *
 * \param [in] maxfile The maxfile to use.
 * \param [in] interface_actions The interface-specific actions to run.
 * \return The dynamic-interface actions to run, or NULL in case of error.
 */
max_actions_t* correlation_loadLMem_convert(max_file_t *maxfile, correlation_loadLMem_actions_t *interface_actions);



/*----------------------------------------------------------------------------*/
/*---------------------------- Interface default -----------------------------*/
/*----------------------------------------------------------------------------*/



/**
 * \brief Auxiliary function to evaluate expression for "CorrelationKernel.loopLength".
 */
int correlation_get_CorrelationKernel_loopLength( void );


/**
 * \brief Basic static function for the interface 'default'.
 * 
 * \param [in] param_numBursts Interface Parameter "numBursts".: The size of data of one correlation step in number of bursts.
 * \param [in] param_numSteps Interface Parameter "numSteps".: The number of (full) correlation steps.
 * \param [in] param_numVariables Interface Parameter "numVariables".: The number of (input) variables.
 * \param [in] param_outputLastStep Interface Parameter "outputLastStep".: Non-zero iff all correlations of the last step shall be returned.
 * \param [in] param_windowSize Interface Parameter "windowSize".: The window size.
 * \param [in] instream_in_precalculations The stream should be of size (((param_numSteps * param_numVariables) * 2) * 8) bytes.
 * \param [in] instream_in_variable_pair The stream should be of size (((param_numSteps * param_numVariables) * 2) * 8) bytes.
 * \param [out] outstream_out_correlation The stream should be of size (((((param_numSteps * param_CorrelationKernel_loopLength) * 10) * 12) * 8) + ((((param_outputLastStep == 0) ? 0 : 1) == 0) ? 0 : (param_numBursts * 192))) bytes.
 * \param [out] outstream_out_indices The stream should be of size (((((param_numSteps * param_CorrelationKernel_loopLength) * 10) * 12) * 2) * 4) bytes.
 */
void correlation(
	uint64_t param_numBursts,
	uint64_t param_numSteps,
	uint64_t param_numVariables,
	uint64_t param_outputLastStep,
	double param_windowSize,
	const double *instream_in_precalculations,
	const double *instream_in_variable_pair,
	double *outstream_out_correlation,
	uint32_t *outstream_out_indices);

/**
 * \brief Basic static non-blocking function for the interface 'default'.
 * 
 * Schedule to run on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 * 
 * 
 * \param [in] param_numBursts Interface Parameter "numBursts".: The size of data of one correlation step in number of bursts.
 * \param [in] param_numSteps Interface Parameter "numSteps".: The number of (full) correlation steps.
 * \param [in] param_numVariables Interface Parameter "numVariables".: The number of (input) variables.
 * \param [in] param_outputLastStep Interface Parameter "outputLastStep".: Non-zero iff all correlations of the last step shall be returned.
 * \param [in] param_windowSize Interface Parameter "windowSize".: The window size.
 * \param [in] instream_in_precalculations The stream should be of size (((param_numSteps * param_numVariables) * 2) * 8) bytes.
 * \param [in] instream_in_variable_pair The stream should be of size (((param_numSteps * param_numVariables) * 2) * 8) bytes.
 * \param [out] outstream_out_correlation The stream should be of size (((((param_numSteps * param_CorrelationKernel_loopLength) * 10) * 12) * 8) + ((((param_outputLastStep == 0) ? 0 : 1) == 0) ? 0 : (param_numBursts * 192))) bytes.
 * \param [out] outstream_out_indices The stream should be of size (((((param_numSteps * param_CorrelationKernel_loopLength) * 10) * 12) * 2) * 4) bytes.
 * \return A handle on the execution status, or NULL in case of error.
 */
max_run_t *correlation_nonblock(
	uint64_t param_numBursts,
	uint64_t param_numSteps,
	uint64_t param_numVariables,
	uint64_t param_outputLastStep,
	double param_windowSize,
	const double *instream_in_precalculations,
	const double *instream_in_variable_pair,
	double *outstream_out_correlation,
	uint32_t *outstream_out_indices);

/**
 * \brief Advanced static interface, structure for the engine interface 'default'
 * 
 */
typedef struct { 
	uint64_t param_numBursts; /**<  [in] Interface Parameter "numBursts".: The size of data of one correlation step in number of bursts. */
	uint64_t param_numSteps; /**<  [in] Interface Parameter "numSteps".: The number of (full) correlation steps. */
	uint64_t param_numVariables; /**<  [in] Interface Parameter "numVariables".: The number of (input) variables. */
	uint64_t param_outputLastStep; /**<  [in] Interface Parameter "outputLastStep".: Non-zero iff all correlations of the last step shall be returned. */
	double param_windowSize; /**<  [in] Interface Parameter "windowSize".: The window size. */
	const double *instream_in_precalculations; /**<  [in] The stream should be of size (((param_numSteps * param_numVariables) * 2) * 8) bytes. */
	const double *instream_in_variable_pair; /**<  [in] The stream should be of size (((param_numSteps * param_numVariables) * 2) * 8) bytes. */
	double *outstream_out_correlation; /**<  [out] The stream should be of size (((((param_numSteps * param_CorrelationKernel_loopLength) * 10) * 12) * 8) + ((((param_outputLastStep == 0) ? 0 : 1) == 0) ? 0 : (param_numBursts * 192))) bytes. */
	uint32_t *outstream_out_indices; /**<  [out] The stream should be of size (((((param_numSteps * param_CorrelationKernel_loopLength) * 10) * 12) * 2) * 4) bytes. */
} correlation_actions_t;

/**
 * \brief Advanced static function for the interface 'default'.
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in,out] interface_actions Actions to be executed.
 */
void correlation_run(
	max_engine_t *engine,
	correlation_actions_t *interface_actions);

/**
 * \brief Advanced static non-blocking function for the interface 'default'.
 *
 * Schedule the actions to run on the engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in] interface_actions Actions to be executed.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *correlation_run_nonblock(
	max_engine_t *engine,
	correlation_actions_t *interface_actions);

/**
 * \brief Group run advanced static function for the interface 'default'.
 * 
 * \param [in] group Group to use.
 * \param [in,out] interface_actions Actions to run.
 *
 * Run the actions on the first device available in the group.
 */
void correlation_run_group(max_group_t *group, correlation_actions_t *interface_actions);

/**
 * \brief Group run advanced static non-blocking function for the interface 'default'.
 * 
 *
 * Schedule the actions to run on the first device available in the group and return immediately.
 * The status of the run must be checked with ::max_wait. 
 * Note that use of ::max_nowait is prohibited with non-blocking running on groups:
 * see the ::max_run_group_nonblock documentation for more explanation.
 *
 * \param [in] group Group to use.
 * \param [in] interface_actions Actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *correlation_run_group_nonblock(max_group_t *group, correlation_actions_t *interface_actions);

/**
 * \brief Array run advanced static function for the interface 'default'.
 * 
 * \param [in] engarray The array of devices to use.
 * \param [in,out] interface_actions The array of actions to run.
 *
 * Run the array of actions on the array of engines.  The length of interface_actions
 * must match the size of engarray.
 */
void correlation_run_array(max_engarray_t *engarray, correlation_actions_t *interface_actions[]);

/**
 * \brief Array run advanced static non-blocking function for the interface 'default'.
 * 
 *
 * Schedule to run the array of actions on the array of engines, and return immediately.
 * The length of interface_actions must match the size of engarray.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * \param [in] engarray The array of devices to use.
 * \param [in] interface_actions The array of actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *correlation_run_array_nonblock(max_engarray_t *engarray, correlation_actions_t *interface_actions[]);

/**
 * \brief Converts a static-interface action struct into a dynamic-interface max_actions_t struct.
 *
 * Note that this is an internal utility function used by other functions in the static interface.
 *
 * \param [in] maxfile The maxfile to use.
 * \param [in] interface_actions The interface-specific actions to run.
 * \return The dynamic-interface actions to run, or NULL in case of error.
 */
max_actions_t* correlation_convert(max_file_t *maxfile, correlation_actions_t *interface_actions);

/**
 * \brief Initialise a maxfile.
 */
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

