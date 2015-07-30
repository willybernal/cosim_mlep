/* Give S-function a name */
#define S_FUNCTION_NAME  writeoutput
#define S_FUNCTION_LEVEL 2

/* Include SimStruct definition and file I/O functions */
#include "simstruc.h"
#include <stdio.h>

#define FILE_NAME_IDX 0
#define FILE_NAME_PARAM(S) ssGetSFcnParam(S,FILE_NAME_IDX)

#define TIME_STEP_IDX 1
#define TIME_STEP_PARAM(S) ssGetSFcnParam(S,TIME_STEP_IDX)

#define NPARAMS 2

/* #define DEBUG_FLAG */

#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)
/* Function: mdlCheckParameters =============================================
 * Abstract:
 *    Validate our parameters to verify they are okay.
 */
static void mdlCheckParameters(SimStruct *S)
{
    /* Check 1st parameters: FILE NAME parameters */
    {
    if (!mxIsChar(FILE_NAME_PARAM(S))) {
        ssSetErrorStatus(S,"The File name (1st parameter) "
                "must be char");
        return;
    }
    }
    /* Check 2nd parameters: TIME STEP parameters */
    {
        if (!mxIsDouble(TIME_STEP_PARAM(S))) {
            ssSetErrorStatus(S,"The TIME STEP(2nd parameter) "
                    "must be double");
            return;
        }
    }
}
#endif /* MDL_CHECK_PARAMETERS */



/* Called at the beginning of the simulation */
static void mdlInitializeSizes(SimStruct *S)
{
    
    ssSetNumSFcnParams(S, NPARAMS);
#if defined(MATLAB_MEX_FILE)
    if(ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {
        mdlCheckParameters(S);
        if(ssGetErrorStatus(S) != NULL) return;
    } else {
        return; /* The Simulink engine reports a mismatch error. */
    }
#endif
    
    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);
    
    if (!ssSetNumInputPorts(S, 1)) return;
    if (!ssSetNumOutputPorts(S, 0)) return;
    ssSetInputPortWidth(S, 0, 1);
    ssSetInputPortDataType(S,0,SS_DOUBLE);
    /* Set FeedThrough to True */
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    
    ssSetNumPWork(S,1);
    ssSetNumSampleTimes(S, 1);
}


/* Set sample times for the block */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    /* Set Sample Time */
    ssSetSampleTime(S, 0, *mxGetPr(TIME_STEP_PARAM(S)));
    /* Set Offset */
    ssSetOffsetTime(S, 0, 0.0);
}



#define MDL_START  /* Change to #undef to remove function */
#if defined(MDL_START)
/* Function: mdlStart =======================================================
 * Abstract:
 *    This function is called once at start of model execution. If you
 *    have states that should be initialized once, this is the place
 *    to do it.
 */
static void mdlStart(SimStruct *S)
{
    /*at start of model execution, open the file and store the pointer
     *in the pwork vector */
    void** pwork = ssGetPWork(S);
    FILE *datafile = NULL;
    char_T filename[300] = "";           /* File Name */
    
    /*at start of model execution, open the file and store the name */
    int n = mxGetString(FILE_NAME_PARAM(S), filename, 300);  /* Get param */
    
    datafile = fopen(filename,"w");
    if (datafile != NULL)
    {
        pwork[0] =  datafile;
#if defined(DEBUG_FLAG)
        printf("File exists\n");
#endif
    }else{
        printf("File Does Not Exist\n");
    }
}
#endif /*  MDL_START */



/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block. Generally outputs are placed in the output vector, ssGetY(S).
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    /*at start of model execution, open the file and store the pointer
     *in the pwork vector */
    void** pwork = ssGetPWork(S);
    
    /* get pointer to the block's output signal */
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);/* Input Pointers*/
    /*     real_T *y    = ssGetOutputPortRealSignal(S,0);    */          /* Output Port */
    
    /*read a floating point number and then the comma delimiter
     *store the result in y*/
    fprintf(pwork[0],"%f,",*uPtrs[0]);
}



/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{
    /*close the file */
    if (ssGetPWork(S) != NULL) {
        FILE *fPtr;
        fPtr = (FILE *) ssGetPWorkValue(S,0);
        if (fPtr != NULL) {
            fclose(fPtr);
        }
        ssSetPWorkValue(S,0,NULL);
    }
}



/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
