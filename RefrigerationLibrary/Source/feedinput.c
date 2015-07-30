/* Give S-function a name */
#define S_FUNCTION_NAME  feedinput
#define S_FUNCTION_LEVEL 2

/* Include SimStruct definition and file I/O functions */
#include "simstruc.h"
#include <stdio.h>

#define FILE_NAME_IDX 0
#define FILE_NAME_PARAM(S) ssGetSFcnParam(S,FILE_NAME_IDX)

#define TIME_STEP_IDX 1
#define TIME_STEP_PARAM(S) ssGetSFcnParam(S,TIME_STEP_IDX)

#define FEED_STEP_IDX 2
#define FEED_STEP_PARAM(S) ssGetSFcnParam(S,FEED_STEP_IDX)

#define NPARAMS 3

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
    /* Check 3rd parameters: FEED STEP parameters */
    {
        if (!mxIsDouble(FEED_STEP_PARAM(S))) {
            ssSetErrorStatus(S,"The FEED STEP(3rd parameter) "
                    "must be double");
            return;
        }
        if ((int)*mxGetPr(FEED_STEP_PARAM(S)) % (int)*mxGetPr(TIME_STEP_PARAM(S)) != 0) {
            ssSetErrorStatus(S,"The FEED STEP(3rd parameter) "
                    "must be multiple of TIME STEP param.");
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
    
    if (!ssSetNumInputPorts(S, 0)) return;
    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, 1);
    ssSetOutputPortDataType(S,0,SS_DOUBLE);
    /* Set FeedThrough to False */
    /* ssSetInputPortDirectFeedThrough(S,0,0); */
    
    /* DWork Vector */
    ssSetNumDWork(S,0);
    /* PWork Vector */
    ssSetNumPWork(S,1);
    ssSetNumRWork(S,1);
    ssSetNumIWork(S,1);
    ssSetNumModes(S,0);
    
    /* Set Number of Sample Times */
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
    
    /* IWork Vector */
    int_T *step = (int_T*) ssGetIWork(S);
    /* Init to 0 */
    step[0] = 0;
    /* Set Filename */
    int n = mxGetString(FILE_NAME_PARAM(S), filename, 300);  /* Get param */
    /* Set number of steps */
    int numSteps = (int)*mxGetPr(FEED_STEP_PARAM(S))/(int)*mxGetPr(TIME_STEP_PARAM(S));
    
#if defined(DEBUG_FLAG)
    printf("INIT: File: %s NumSteps: %d Steps: %d\n",filename,numSteps,step[0]);
#endif
    
    datafile = fopen(filename,"r");
    if (datafile != NULL)
    {
        pwork[0] =  datafile;
    }else{
#if !defined(MATLAB_MEX_FILE)
        printf("File Does Not Exist");
        exit(0);
#else
        /* Break from Simulink Simulation */
        ssSetErrorStatus(S,"File Does not Exist");
        return;
#endif
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
    
    /*get pointer to array of pointers, where the first element is the address
     *of the open file */
    void** pwork = ssGetPWork(S);
    /* get pointer to the block's output signal */
    real_T *y = ssGetOutputPortSignal(S,0);
    /* IWork Vector */
    int_T *step = (int_T*) ssGetIWork(S);
    /* RWork Vector */
    real_T *value = (real_T*) ssGetRWork(S);
    
    /* int_T *numSteps = (int_T*) ssGetDWork(S,1); */
    char_T filename[300];                                 /* File Name */
    int numSteps = (int)*mxGetPr(FEED_STEP_PARAM(S))/(int)*mxGetPr(TIME_STEP_PARAM(S));
    int n;
    float v = 0;
    /* set filename */
    n = mxGetString(FILE_NAME_PARAM(S), filename, 300);  /* Get param */
    
#if defined(DEBUG_FLAG)
    printf("Filename: %s Steps: %d NumSteps: %d\n",filename,numSteps,step[0]);
#endif
    
    /* Check Feed Step */
    if (step[0] % numSteps == 0) {
        
        /*read a floating point number and then the comma delimiter */
        n = fscanf(pwork[0],"%f,",&v);
        if (n > 0){
            value[0] = (real_T)v;
        }
#if defined(DEBUG_FLAG)
        printf("GOT INSIDE: %s %f=============\n",filename,value[0]);
        printf("Value: %f %d\n",v,n);
#endif
        y[0] = value[0];
        step[0] = 0;
    }
    else{
        /* Assign output*/
        y[0] = value[0];
    }
    
}

#define MDL_UPDATE
/* Function: mdlUpdate ========================================================
 * Abstract:
 *    This function is called once for every major integration time step.
 *    Discrete states are typically updated here, but this function is useful
 *    for performing any tasks that should only take place once per integration
 *    step.
 */
static void mdlUpdate(SimStruct *S, int_T tid)
{
    /* DWork Vector */
    int_T *step = (int_T*) ssGetIWork(S);
    
    /*
     * Increment the state by the input
     */
    step[0] += 1;
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
