/*  File    : mlepCosim.c
 *  Abstract:
 *
 *      Time Varying Discrete Transfer Function block
 *
 *      This block implements a discrete time transfer function
 *      whose transfer function polynomials are passed in via
 *      the input vector.  This is useful for discrete time
 *      adaptive control applications.
 *
 *	This block has multiple sample times and will not work correctly
 *	in a multitasking environment. It is designed to be used in
 *	a single tasking (or variable step) simulation environment.
 *	Because this block accesses the input signal in both tasks,
 *	it cannot specify the sample times of the input and output ports
 *	(SS_OPTION_PORT_SAMPLE_TIMES_ASSIGNED).
 *
 *  Copyright 1990-2013 The MathWorks, Inc.
 */

#define S_FUNCTION_NAME mlepCosim
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
/* #define DEBUG_FLAG */

/* MLEP LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include "mlepCreate.c"
/* MLEP VARIABLES */
char arguments[1000];
struct CosimInstance a;
/* int timeout; */
int port = 0;
char host[100] = "";
/* char bcvtbdir[300] = "/home/avenger/Documents/Git/mlep_v2.0/MLE+/bcvtb"; */
char configfile[100] = "socket.cfg";
char env[1000] = "/home/avenger/Documents/Git/mlep_v2.0/MLE+/bcvtb";
char execcmd[1000] = "";
int kStep = 1;

#define U(element) (*uPtrs[element])  /* Pointer to Input Port0 */

#define PROGNAME_IDX 0
#define PROGNAME_PARAM(S) ssGetSFcnParam(S,PROGNAME_IDX)

#define IDF_IDX 1
#define IDF_PARAM(S) ssGetSFcnParam(S,IDF_IDX)

#define WEATHER_IDX 2
#define WEATHER_PARAM(S) ssGetSFcnParam(S,WEATHER_IDX)

#define TIMEOUT_IDX 3
#define TIMEOUT_PARAM(S) ssGetSFcnParam(S,TIMEOUT_IDX)

#define SAMPLE_TIME_IDX 4
#define SAMPLE_TIME_PARAM(S) ssGetSFcnParam(S,SAMPLE_TIME_IDX)

#define NUMBER_INPUTS_IDX 5
#define NUMBER_INPUTS_PARAM(S) ssGetSFcnParam(S,NUMBER_INPUTS_IDX)

#define NUMBER_OUTPUTS_IDX 6
#define NUMBER_OUTPUTS_PARAM(S) ssGetSFcnParam(S,NUMBER_OUTPUTS_IDX)

#define BCVTB_PATH_IDX 7
#define BCVTB_PATH_PARAM(S) ssGetSFcnParam(S,BCVTB_PATH_IDX)

#define NPARAMS 8


/*====================*
 * S-function methods *
 *====================*/


#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)
/* Function: mdlCheckParameters =============================================
 * Abstract:
 *    Validate our parameters to verify they are okay.
 */
static void mdlCheckParameters(SimStruct *S)
{
    /* Check 1stparameters: PROGNAME parameters */
    {
    if (!mxIsChar(PROGNAME_PARAM(S))) {
        ssSetErrorStatus(S,"The E+ Executable (1st parameter) "
                "must be char");
        return;
    }
    }
    
    /* Check 2nd parameters: IDF parameters */
    {
        if (!mxIsChar(IDF_PARAM(S))) {
            ssSetErrorStatus(S,"The IDF FILE (2nd parameter) "
                    "must be char");
            return;
        }
    }
    
    /* Check 3rd parameters: WEATHER parameters */
    {
        if (!mxIsChar(WEATHER_PARAM(S))) {
            ssSetErrorStatus(S,"The WEATHER file (3rd parameter) "
                    "must be char");
            return;
        }
    }
    
    /* Check 4th parameters: TIMEOUT parameters */
    {
        if (!mxIsDouble(TIMEOUT_PARAM(S))) {
            ssSetErrorStatus(S,"The TIMEOUT (4th parameter) "
                    "must be double");
            return;
        }
    }
    /* Check 5th parameters: SAMPLE_TIME parameters */
    {
        if (!mxIsDouble(SAMPLE_TIME_PARAM(S))) {
            ssSetErrorStatus(S,"The SAMPLE_TIME (5th parameter) "
                    "must be double");
            return;
        }
    }
    /* Check 6th parameters: NUMBER_OUTPUTS parameters */
    {
        if (!mxIsDouble(NUMBER_INPUTS_PARAM(S))) {
            ssSetErrorStatus(S,"The NUMBER_INPUTS (6th parameter) "
                    "must be double");
            return;
        }
    }
    /* Check 7th parameters: NUMBER_OUTPUTS parameters */
    {
        if (!mxIsDouble(NUMBER_OUTPUTS_PARAM(S))) {
            ssSetErrorStatus(S,"The NUMBER_OUTPUTS (7th parameter) "
                    "must be double");
            return;
        }
    }
    /* Check 8th parameters: BCVTB_PATH parameters */
    {
        if (!mxIsChar(BCVTB_PATH_PARAM(S))) {
            ssSetErrorStatus(S,"The BCVTB PATH (8th parameter) "
                    "must be char");
            return;
        }
    }
    
}
#endif /* MDL_CHECK_PARAMETERS */



/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    
    ssSetNumSFcnParams(S, NPARAMS);  /* Number of expected parameters */
#if defined(MATLAB_MEX_FILE)
    if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {
        mdlCheckParameters(S);
        if (ssGetErrorStatus(S) != NULL) {
            return;
        }
    } else {
        return; /* Parameter mismatch will be reported by Simulink */
    }
#endif
    
    /* Continuous States */
    ssSetNumContStates(S, 0);
    /* Discrete States */
    ssSetNumDiscStates(S, 0);
    
    /* Set Number of Input Ports */
    if (!ssSetNumInputPorts(S, 1)) return;
    /* Set Width of Input Port 0 */
    ssSetInputPortWidth(S, 0, 1);
    /* Set FeedThrough to True */
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    
    /* Set Number of Output Ports */
    if (!ssSetNumOutputPorts(S, 3)) return;
    /* Set Width of Output Port 0 - Flag */
    ssSetOutputPortWidth(S, 0, 1);
    /* Set Width of Output Port 1 - Time */
    ssSetOutputPortWidth(S, 1, 1);
    /* Set Width of Output Port 3 - Output */
    ssSetOutputPortWidth(S, 2, *mxGetPr(NUMBER_OUTPUTS_PARAM(S)));
    
    /* Set Number of Sample Times */
    ssSetNumSampleTimes(S, 1);
    
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
    
}



/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    Specifiy that we inherit our sample time from the driving block.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    /* Set Sample Time */
    ssSetSampleTime(S, 0, *mxGetPr(SAMPLE_TIME_PARAM(S)));
    /* Set Offset */
    ssSetOffsetTime(S, 0, 0.0);
    /* Set Sample Time Inheritance*/
    ssSetModelReferenceSampleTimeDefaultInheritance(S);
}



#define MDL_INITIALIZE_CONDITIONS
/* Function: mdlInitializeConditions ========================================
 * Abstract:
 *    Initialize the discrete states to zero and
 *    squirrel away the initial numerator and denominator
 */
static void mdlInitializeConditions(SimStruct *S)
{
    /* Create arguments */
    char_T progname_buffer[500];                /* Program Name Buffer */
    char_T idf_buffer[500];                     /* Idf Name Buffer  */
    char_T weather_buffer[500];                 /* Weather Name Buffer */
    char_T bcvtbdir[500];                       /* BCVTB Directory */
    int timeout = *mxGetPr(TIMEOUT_PARAM(S));   /* Socket Timeout */
    
    int n = 0;
    n = mxGetString(PROGNAME_PARAM(S), progname_buffer, 500);
    n = mxGetString(IDF_PARAM(S), idf_buffer, 500);
    n = mxGetString(WEATHER_PARAM(S), weather_buffer, 500);
    n = mxGetString(BCVTB_PATH_PARAM(S), bcvtbdir, 500);
    
    /* Assign Arguments */
    /* printf("IDF: %s WEATHER: %s\n",idf_buffer,weather_buffer); */
    sprintf(arguments, "%s %s",idf_buffer,weather_buffer);
    printf("ARGUMENTS: %s\n",arguments);
    
    /* Call mlepCreate */
    a = mlepCreate(progname_buffer,arguments,timeout,port,host, bcvtbdir, configfile, env, execcmd);
    /* Set Variables */
    /*printf("=====================Iterations====================\n\n"); */
}

/* Function: mdlOutputs =======================================================*/
static void mdlOutputs(SimStruct *S, int_T tid)
{
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);/* Input Pointers*/
    real_T *flag = ssGetOutputPortRealSignal(S,0);              /* Flag Port */
    real_T *time = ssGetOutputPortRealSignal(S,1);              /* Time Port */
    real_T *y    = ssGetOutputPortRealSignal(S,2);              /* Output Port */
    int_T numOutput = *mxGetPr(NUMBER_OUTPUTS_PARAM(S));        /* Number of Ouptuts from E+ */
    int_T numInput = *mxGetPr(NUMBER_INPUTS_PARAM(S));          /* Number of Intuts from E+ */
    time_T taskTime = ssGetT(S);                                /* Simulink Time */
    char com_buffer[1000];                                      /* Communication Buffer */
    struct decodedPacket decodedpacket;                         /* Packet Decoded from E+ */
    float values[100];                                          /* vector for E+ inputs */
    int deltaT = *mxGetPr(SAMPLE_TIME_PARAM(S));                /* Sample Time for S-function (in seconds) */
    int n = 0;
    int i = 0;
    
    UNUSED_ARG(tid); /* not used in single tasking mode */
    
    /* Read Packet from Co-simulation Instance*/
    n = read(a.simsock,com_buffer,999);
    /* printf("N READ: %d\n",n); */
    /* Receive String from co-simulation*/
    if (n<0)
    {
#if !defined(MATLAB_MEX_FILE)
        perror("Error reading from socket");
        exit(1);
#else
        ssSetErrorStatus(S,"Reading Erorr");
        return;
#endif
    }
    else
    {
        decodedpacket = mlepDecodePacket(com_buffer);
    }
    
    /* Terminate if got flag != 0 */
    if (decodedpacket.flag != 0){
#if !defined(MATLAB_MEX_FILE)
        printf("Flag Not 0");
#else
        /* Break from while loop */
        ssSetErrorStatus(S,"Flag not 0");
        return;
#endif
    }
    /*============ INPUTS ============*/
    for (i = 0; i < numInput; i++) {
        values[i] = *uPtrs[i];
    }
    
    char* test = mlepEncodeRealData(2, 0, (kStep-1)*deltaT, values, 1);
    
    /* Write Packet to Co-Simulation Instance */
    n = send(a.simsock, test, strlen(test), 0);
    if (n<0)
    {
#if !defined(MATLAB_MEX_FILE)
        printf("Error Writing to Socket");
#else
        /* Break from while loop */
        ssSetErrorStatus(S,"Error Writing to Socket");
        /* return; */
#endif
    }
    else
    {
    }
    
    /*============ OUTPUTS ============*/
    int temp = 0;
    while(temp < numOutput){
        y[temp] = decodedpacket.realValue[temp];
        temp = temp + 1;
    }
    
    /* E+ Time */
    time[0] = decodedpacket.timeValue; /* (real_T)taskTime; */
    /* Flag */
    flag[0] = decodedpacket.flag;
    
    /* Read Packet from Co-simulation Instance*/
    kStep++;
}

/* Function: mdlTerminate =====================================================
 * Abstract:
 *    No termination needed, but we are required to have this routine.
 */
static void mdlTerminate(SimStruct *S)
{
    int n;
    char buffer[1000];
    sprintf(buffer, "2 0 -1\n");
    /* Write Packet to Co-Simulation Instance */
    n = send(a.simsock, buffer, strlen(buffer), 0);
    printf("Finished Co-Simulation");

#if !defined(MATLAB_MEX_FILE)
    /* return; */
    return;
#else
    /* return; */
#endif
    UNUSED_ARG(S); /* unused input argument */
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
