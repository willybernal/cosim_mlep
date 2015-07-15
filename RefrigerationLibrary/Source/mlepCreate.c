#ifdef __APPLE__
#  define error printf
#endif

#include"mlepCreate.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include<sys/time.h>
/* #define DEBUG_FLAG */

#define MYPORT 0
/* how many pending connections queue will hold */
#define BACKLOG 10

char *buffer_global;

void sigchld_handler(int s)
{
    while(wait(NULL) >0);
}

int startProcess(char progname[], char args[], char env[], char workdir[])
{
    /* cmd variable */
    char cmd[80];
    
    /* Create CMD to execute */
    sprintf(cmd, "%s %s > mlep.log &", progname, args);
    
    /* Run cmd and get result */
    int status = system(cmd);
    
    /* Check if simulation running */
    if (status != 0)
    {
        printf("Error while starting external co-simulation program.");
    }
    
    /* Return E+ status */
    return status;
}


char* mlepEncodeRealData(int VERNUMBER, int flag, float currentTime, float values[], int numvalues)
{
    char buffer[1000];
    char add_buffer[1000];
    char *new_buffer;
    /* MLEPENCODEREALDATA This function encodes real data for the co-simulation environment */
    /* check flag 0 is good */
    if (flag == 0)
    {
#if defined(DEBUG_FLAG)
        printf("INSIDE ENCODE\n");
#endif
        /* Create string to add values */
        sprintf(buffer, "%d 0 %d 0 0 %20.15e ",VERNUMBER, numvalues, currentTime);
        /* for each real input */
        int iter = 0;
        while (iter<numvalues)
        {
            sprintf(add_buffer,"%20.15e ",values[iter]);
            new_buffer = strcat(buffer, add_buffer);
            strcpy(buffer, new_buffer);
            iter++;
        }
        new_buffer = strcat(buffer,"\n");
    }
    else
    {
        /* Error in simulation */
        sprintf(new_buffer, "%d %d",VERNUMBER, flag);
    }
    
    return new_buffer;
}

struct decodedPacket mlepDecodePacket(char readpacket[])
{
    /* MLEPDECODEPACKET This functions decodes the packets coming from the co-simulation instance */
    /* MLEPDECODEPACKET Decode packet to data.
     *   [flag, timevalue, realvalues, intvalues, boolvalues] =
     *           mlepDecodePacket(packet)
     *
     *   Decode a packet (a string) to data.  The packet format follows the
     *   BCVTB co-simulation communication protocol.
     *
     *   Inputs:
     *       packet: the packet to be decoded (a string).
     *
     *   Outputs:
     *       flag: an integer specifying the (status) flag. Refer to the BCVTB
     *                   protocol for allowed flag values.
     *       timevalue: a real value which is the current simulation time in
     *                   seconds.
     *       realvalues: a vector of received real value data.
     *       intvalues: a vector of received integer value data.
     *       boolvalues: a vector of received boolean value data.
     *
     *       Each of the received data vector can be empty if there is no data
     *       of that type sent.
     *
     *   See also:
     *       MLEPENCODEDATA, MLEPENCODEREALDATA, MLEPENCODESTATUS
     *
     * (C) 2015 by Willy Bernal (Willy.BernalHeredia@nrel.gov
     */
    
    
    /* function [flag, timevalue, realvalues, intvalues, boolvalues] = mlepDecodePacket(packet) */
    struct decodedPacket decodedpacket;
    const char s[2] = " ";
    char *token;
    int v;
    /* char temp[80]; */
    /* char rpack[200]; */
    int flag;
    int dummy1;
    int dummy2;
    float timevalue;
    int numvalues;
    float realvalues[100];
    sscanf(readpacket,"%d %d %d %d %d %f", &v, &flag, &numvalues,&dummy1,&dummy2, &timevalue);
    
#if defined(DEBUG_FLAG)
    printf("Num Values: %d\n",numvalues);
#endif
    
    /* Read First */
    token = strtok(readpacket, s);
    int iter = 1;
    int j = 0;
    /* walk through other tokens */
    while( token != NULL )
    {
        if (iter >= 7 && iter < 7+numvalues)
        {
            sscanf(token, "%f", &realvalues[j]);
            j++;
        }
        iter++;
        token = strtok(NULL, s);
    }
    
#if defined(DEBUG_FLAG)
    printf("CREATING DECODEPACKET\n");
#endif
    
    /* Create DecodedPackage */
    decodedpacket.flag = flag;
#if defined(DEBUG_FLAG)
    printf("FLAG: %d\n",decodedpacket.flag);
#endif
    
    decodedpacket.timeValue = timevalue;
#if defined(DEBUG_FLAG)
    printf("TIMEVALUE: %f\n",decodedpacket.timeValue);
#endif
    
    decodedpacket.numValue = numvalues;
#if defined(DEBUG_FLAG)
    printf("NUMVALUE: %d\n",decodedpacket.numValue);
#endif
    
    /* decodedpacket.realValue = realvalues; */
    iter = 0;
    while(iter < decodedpacket.numValue){
        decodedpacket.realValue[iter] = realvalues[iter];
        iter++;
    }
#if defined(DEBUG_FLAG)
    printf("REALVALUES\n");
    iter = 0;
    while(iter < decodedpacket.numValue){
        printf("%d: %f\n",iter+1,decodedpacket.realValue[iter]);
        iter++;
    }
#endif
    
    return decodedpacket;
    /* Protocol Version 1 & 2:
     * Packet has the form:
     *       v f dr di db t r1 r2 ... i1 i2 ... b1 b2 ... \n
     * where
     *   v    - version number (1,2)
     *   f    - flag (0: communicate, 1: finish, -10: initialization error,
     *                -20: time integration error, -1: unknown error)
     *   dr   - number of real values
     *   di   - number of integer values
     *   db   - number of boolean values
     *   t    - current simulation time in seconds (format %20.15e)
     *   r1 r2 ... are real values (format %20.15e)
     *   i1 i2 ... are integer values (format %d)
     *   b1 b2 ... are boolean values (format %d)
     *   \n   - carriage return
     *
     * Note that if f is non-zero, other values after it will not be processed.
     */
}

struct CosimInstance mlepCreate(char progname[], char arguments[], int timeout0, int port, char host[], char bcvtbdir[], char configfile[], char env[], char execcmd[])
{
    /* MLEPCREATE Create a new co-simulation instance.
     *	This function creates a new co-simulation instance by starting a givne
     *	external simulation program in a separate process and establishing a
     *	TCP/IP connection to it. The simulation program must support the BCVTB
     * 	co-simulation protocol.
     */
    int sockfd, new_fd;
    /* my address information*/
    struct sockaddr_in my_addr;
    struct sockaddr_in my_addr1;
    socklen_t my_addr_len = sizeof(my_addr1);
    /* connector's address information */
    struct sockaddr_in their_addr;
    int sin_size;
    /* struct sigaction sa; */
    int yes = 1;
    /* char buffer[256]; */
    
    int status  = 0;
    int pid = 0;
    char msg[] = "msg";
    char cmd[] = "runenergyplus 1ZoneUncontrolled.idf USA_CO_Golden-NREL.724666_TMY3.epw";
    char args[] = "args";
    char workdir[] = "workdir";
    int cosimPort;
    
    
    struct CosimInstance cosim;
    
#if defined(DEBUG_FLAG)
    printf("INSIDE MLEPCREATE\n");
#endif
    configfile = "socket.cfg";
    
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM,0)) == -1)
    {
        perror("Server-socket() error!");
        exit(1);
    }
    else
    {
#if defined(DEBUG_FLAG)
        printf("Server-socket() sockfd is OK...\n");
#endif
        
    }
    
    
    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
            sizeof(timeout)) < 0)
        error("setsockopt failed\n");
    
    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
            sizeof(timeout)) < 0)
        error("setsockopt failed\n");
    
    /* host byte order */
    my_addr.sin_family = AF_INET;
    /* short, network byte order */
    my_addr.sin_port = htons(MYPORT);
    /* automatically fill with my IP */
    my_addr.sin_addr.s_addr = INADDR_ANY;
    
#if defined(DEBUG_FLAG)
    printf("Server-Using %s and port %d...\n", inet_ntoa(my_addr.sin_addr), MYPORT);
#endif
    
    /* zero the rest of the struct */
    memset(&(my_addr.sin_zero), '\0', 8);
    
    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Server-bind() error");
        exit(1);
    }
    else{
        
#if defined(DEBUG_FLAG)
        printf("Server-bind() is OK...\n");
#endif
        
    }
    
    if(listen(sockfd, BACKLOG) == -1)
    {
        perror("Server-listen() error");
        exit(1);
    }
#if defined(DEBUG_FLAG)
    printf("Server-listen() is OK...Listening...\n");
#endif
    
    
    if (getsockname(sockfd, (struct sockaddr *)&my_addr1, &my_addr_len) == -1)
        perror("getsockname");
    else
        cosimPort = (int)ntohs(my_addr1.sin_port);
    
    /* Write Socket Config to file */
    char socket_config[] = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
            "<BCVTB-client>\n"
            "<ipc>\n"
            "<socket port=\"%d\" hostname=\"%s\"/>\n"
            "</ipc>\n"
            "</BCVTB-client>";
    
    FILE * fid;
    fid = fopen(configfile,"w+");
    char hostname[1024];
    int dum = gethostname(hostname, 1024);
    
    fprintf(fid, socket_config, cosimPort, hostname);
    fclose(fid);
    
    /* set environment */
    setenv("BCVTB_HOME",bcvtbdir,1);
    
    /* Start Co-Simulation Process*/
    /* printf("ARGUMENTS: %s\n",arguments); */
    cosim.status = startProcess(progname, arguments, env, workdir);
    
    /* Accept Socket */
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
    {
        perror("Server-accept() error");
    }
    else
    {
#if defined(DEBUG_FLAG)
        printf("Server-accept() is OK...\n");
#endif
    }
#if defined(DEBUG_FLAG)
    printf("Server-new socket, new_fd is OK...\n");
#endif
    
    printf("Server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr));
    
    /* Assign cosim  */
    cosim.serversock = sockfd;
    cosim.simsock = new_fd;
    
    /* Return cosim */
    return cosim;
}









