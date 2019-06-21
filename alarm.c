/**********************************************************************
* Copyright (c) 2006 Chris Procter
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of this software nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/


#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

static struct option longopts[] = {
  { "detach",  no_argument,            NULL,   'd' },
  { "timer",   required_argument,      NULL,   't' },
  { "signal",  required_argument,      NULL,   's' },
  { "repeat",  required_argument,      NULL,   'r' },
  { NULL,  0,  NULL,  0 }
};

int usage();
int lookupsig(char * signame);

int main(int argc, char *argv[])
{
  pid_t child;
  int ch,i,ret,j;
  int sleeptime=0;
  int signal=15;
  int pid=0;
  int dflag=0;
  int repeatcount=1;
  int *pidlist;

  pidlist=(int *) malloc(sizeof(int) * argc);
	

  opterr=0;
  while((ch = getopt_long_only(argc, argv, "-t:s:",longopts,NULL)) != -1)
    {
      switch (ch) {
      case 't':
		sleeptime = strtol(optarg,NULL,10);
		break;
      case 's':
		signal = lookupsig(optarg);
		break;
      case 'n':
		signal = strtol(optarg,NULL,10);
		break;
      case 'd':
		dflag = 1;
		break;
      case 'r':
		repeatcount = strtol(optarg,NULL,10);
		break;
      case 1://its not an option so its a pid
		*(pidlist+pid)=strtol(optarg,NULL,10);
		//printf("%d %d\n",pid,*(pidlist+pid));
		pid++;
		break;
      case '?'://its an option but not as we know it so its a signal type
		// printf("%s\n",(argv[optind-1])+1);
       	signal=lookupsig((argv[optind-1])+1);
		break;
      case 'h':
		usage();
      default:
		usage();
      }
    }

  if(pid==0){
    usage();
  }


  if((dflag==1)&&(child=fork())){
    // printf("forked\n");
    exit(0);
  }



  for(i=repeatcount;i!=0;i--){
	// printf("sleep for %d\n",sleeptime);
	sleep(sleeptime);
	
	if(signal>0){
	  for(j=0;j<pid;j++){

		//printf("kill %d pid %d\n",signal,*(pidlist+j));

		ret=kill(*(pidlist+j),signal);
		if(ret!=0){
		  switch(errno){
		  case EINVAL:
			fprintf(stderr,"%d - Invalid signal\n",signal);
			break;
		  case EPERM:
			fprintf(stderr,"%d - Permission denied\n",*(pidlist+j));
			break;
		  case ESRCH:
			fprintf(stderr,"%d - No Such Process\n",*(pidlist+j));
			break;
		  }
		}
	  }

	}
  }
  exit(0);
}

     
int usage(){
  printf("alarm [-d] [-t time] [-s signal | -signal] pid\n");
  printf(" -d\t\tdetach process to run in background\n");
  printf(" -r [count]\trepeat the signal [count] times (default 1)\n");
  printf(" -t [time]\tkill the process in [time] seconds (default=0)\n");
  printf(" -s [signal]\tSend [signal] (default=SIGINT)\n");
  printf(" -[signal]\tSend [signal]\n");
  printf(" pid \t\tprocess ID to signal\n");
  exit(0);
}




int lookupsig(char * signame){
  int i;
  char *sig[32];

  //this is nasty but the signal names in signal.h are all macros 
  //so we cant use them dynamically
  sig[0]="IGNORE";
  sig[1]="SIGHUP";
  sig[2]="SIGINT";
  sig[3]="SIGQUIT";
  sig[4]="SIGILL";
  sig[5]="SIGTRAP";
  sig[6]="SIGABRT";
  sig[7]="SIGBUS";
  sig[8]="SIGFPE";
  sig[9]="SIGKILL";
  sig[10]="SIGUSR1";
  sig[11]="SIGSEGV";
  sig[12]="SIGUSR2";
  sig[13]="SIGPIPE";
  sig[14]="SIGALRM";
  sig[15]="SIGTERM";
  sig[16]="SIGSTKFLT";
  sig[17]="SIGCHLD";
  sig[18]="SIGCONT";
  sig[19]="SIGSTOP";
  sig[20]="SIGTSTP";
  sig[21]="SIGTTIN";
  sig[22]="SIGTTOU";
  sig[23]="SIGURG";
  sig[24]="SIGXCPU";
  sig[25]="SIGXFSZ";
  sig[26]="SIGVTALRM";
  sig[27]="SIGPROF";
  sig[28]="SIGWINCH";
  sig[29]="SIGIO";
  sig[30]="SIGPWR";
  sig[31]="SIGSYS";

  //if its a numeric signalname i.e. -9 etc just turn it into an int
  if((*signame > 47)&&(*signame<58)){
    return strtol(signame,NULL,10);
  }
  
  //if its a string name translate into a numeric name and return that.
  for(i=0;i<32;i++){
    if(strcmp(sig[i],signame)==0){
      return i;
    }
  }

  return -1;
}



