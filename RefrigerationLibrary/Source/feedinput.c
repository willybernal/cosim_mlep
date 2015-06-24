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


#define NPARAMS 2


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
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return;
    }

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    if (!ssSetNumInputPorts(S, 0)) return;
    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, 1);
    ssSetOutputPortDataType(S,0,SS_SINGLE);

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
      FILE *datafile;
      char_T filename[300];                                 /* File Name */
      int n = mxGetString(FILE_NAME_PARAM(S), filename, 300);  /* Get param */
      
      datafile = fopen(filename,"r");
      if (datafile != NULL)
      {
          printf("File exists\n");
          pwork[0] =  datafile;
      }else{
#if !defined(MATLAB_MEX_FILE)
        printf("File Does Not Exist");
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
    /* get pointer to the block's output signal */
    real_T       *y = ssGetOutputPortSignal(S,0);
    
    /*get pointer to array of pointers, where the first element is the address
     *of the open file */

    void** pwork = ssGetPWork(S);
    
    /*read a floating point number and then the comma delimiter
     *store the result in y*/
    fscanf(pwork[0],"%f%*c",y);
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
    void** pwork = ssGetPWork(S);
      FILE *datafile;
      
      datafile = pwork[0];
      fclose(datafile);
      
}



/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
