/* #include<iostream> */
/* #define MLEPPORT */

/* Struct to include cosimulation instance */
struct CosimInstance
{
	int serversock;	/* server socket ID/object */
	int simsock;		/* connection socket to the external program */
	int status;		/* status returned by external program */
	char msg[20];		/* string message returned by the external program */
	int pid;		/* process ID of the external program (currently not needed) */
};

struct decodedPacket
{
	int flag;		/* co-simulation flag */
	float timeValue;	/* Simulation timestep */
	float *realValue;	/* Co-simulation real values */
	int numValue;		/* Number of values */
};

struct CosimInstance mlepCreate(char progname[], char arguments[], int timeout,
int port, char host[], char bcvtbdir[], char configfile[], char env[], char execcmd[]);

int startProcess(char cmd[], char args[], char env[], char workdir[]);

struct decodedPacket mlepDecodePacket(char readpacket[]);

char* mlepEncodeRealData(int VERNUMBER, int flag, float currentTime, float values[], int numvalues);










