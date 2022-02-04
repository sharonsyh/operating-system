#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#define OPTION "option"

int final_dir_num=0;
int final_line_num=0;
int final_file_num=0;

void  INThandler(int sig)
{
     char  c;

     signal(sig, SIG_IGN);
     printf("\nDo you really want to quit? [y/n] ");
     c = getchar();
     if (c == 'y' || c == 'Y'){
	 	 printf("*******************************************\n");
	 	 printf("SEARCHED DIRECTORY NUM : %d\n",final_dir_num );	
		 printf("SEARCHED FILE NUM : %d\n",final_file_num );	
		 printf("FOUND LINE NUM : %d\n", final_line_num);	
		 printf("*******************************************\n");
	 	 exit(0); 
     }    
     else
          signal(SIGINT, INThandler);
     	  getchar();
}

int main(int argc, char * argv[]){

	int c;
    int invalid;
    DIR *dp;
    int process_num = 2;
    int case_sensitive = 1;
    int absolute_path = 0;
    int keyword_idx;
    int cnt;
	signal(SIGINT, INThandler);

    for(int r= 1; r<argc;r++){
        dp = opendir(argv[r]);
        if(dp!=NULL) {
            keyword_idx = r+1;
            break;
        }
        else cnt+=1;
    }
    if(cnt == argc-1) invalid=1;
    while((c = getopt (argc, argv, "p:ac")) != -1){
        
        switch (c)
        {
            case 'p':
                if(atoi(optarg)>0) process_num = atoi(optarg);
                break;
            case 'c':
                case_sensitive = 0;
                break;
            case 'a':
                absolute_path =1;
                break;
            default :
                break;
        }
    }

	if(argc < 4|| invalid == 1){
        puts("Usage : ./pfind <option> <dir> <keyword>");
        printf("---------------------------------------------------\n");
    }

	else{
        int process_c [process_num] ;
        int b;
		char* k_string = (char*)malloc(sizeof(char)*256);

        if(mkfifo("channel",0666)){
            if(errno!=EEXIST){
                perror("failed to open fifo: ");
                exit(1);
            }
        }

        if(mkfifo("channel_2",0667)){
           if(errno!=EEXIST){
               perror("failed to open fifo: ");
               exit(1);
           }
        }

		for(int i=keyword_idx;i<argc;i++){
			strcat(k_string,argv[i]);
			strcat(k_string," ");
		}

		char case_s[5];
		char a_path[5]; 

		sprintf(case_s,"%d",case_sensitive);
		sprintf(a_path,"%d",absolute_path); 
		strcat(k_string,case_s);
		strcat(k_string,":");
		strcat(k_string,a_path);
		
		int fdq = open("channel",O_RDWR);
		int ft = open("channel_2",O_RDWR);

		size_t length_t = strlen(argv[keyword_idx-1]) ;
		
		pid_t worker_p[process_num];

		for(int i=0;i<process_num;i++){
			worker_p[i] = fork();

			if(worker_p[i] == 0){
				execl("./worker","./worker",k_string,NULL);
			}
			else{
				//
			}
		}
		int p=0;
		int terminate = 0;
		while(1){
			int status;
			for(int i=0;i<process_num;i++){

				pid_t return_pid = waitpid(worker_p[i], &status, WNOHANG);

				if (return_pid == -1) {
    				/* error */
				} else if (return_pid == 0) {
    				/* child is still running */
				} else if (return_pid == worker_p[i]) {
    				/* child is finished. exit status in   status */
					terminate+=1;
				}
			}
			if(terminate == process_num){

         		printf("*******************************************\n"); 
				printf("SEARCHED DIRECTORY NUM : %d\n",final_dir_num );
         		printf("SEARCHED FILE NUM : %d\n",final_file_num );
         		printf("FOUND LINE NUM : %d\n", final_line_num);
				printf("*******************************************\n");
				exit(0);
			}//else printf("terminated process num = %d \n",terminate);


			if(p==0) {
				write(fdq,&length_t,sizeof(length_t));
				b = write(fdq,argv[keyword_idx-1],length_t);
				p+=1;
			}
			int cnt=0;
			char s[256];
			char dir_num_[20]={0,};
			char file_num_[20];
			char line_num_[20];
            char line_num[20];
            char dir_num[20];
            char file_num[20];
			char sub_dir[256]={0,};
			int fdp = open("channel",O_RDWR);
            int fdp_2 = open("channel_2", O_RDWR);
			size_t length;
			size_t len;

			flock(fdp_2,LOCK_EX);
			read(fdp_2,&len,sizeof(len));
			cnt = read(fdp_2,s,len);
			flock(fdp_2,LOCK_UN);
			if(cnt!=len) break;

			int j;
			int k;
			
			char sub_dir_[256]={0,};
			char _dir_num[20];
			char _line_num[20];
			char _file_num[20];

			for(int i=0;i<cnt;i++){
					//+hdir/hdir2:+hdir/dir3:*6:@3:&9:  

					if(s[i]=='+'){
						
						char sub[256]={0,};
						k=i+1;
						while(s[k]!=':'){
                            j=0;
							
                            sub_dir[j] = s[k];
							sprintf(sub_dir_, "%c", sub_dir[j]);
							strcat(sub,sub_dir_);
                            j+=1;k+=1;
						}
					    size_t length_s = strlen(sub);
						flock(fdp,LOCK_EX);
                        write(fdp,&length_s,sizeof(length_s));
						write(fdp,sub,length_s);
                   		flock(fdp,LOCK_UN);
						i=k;
					}
                    if(s[i]=='*'){
						char dn[256]={0,};
						k=i+1;
                        while(s[k]!=':'){
                            j=0;
                            dir_num[j] = s[k];
							sprintf(_dir_num, "%c", dir_num[j]);
							strcat(dn,_dir_num);
                            j+=1;k+=1;
                        }
						i=k;
                        final_dir_num += atoi(dn);
                    }

                    if(s[i]=='@'){
						char fn[256]={0,};
						k=i+1;
                        while(s[k]!=':'){
                            j=0;
                            file_num[j]=s[k];
							sprintf(_file_num, "%c", file_num[j]);
							strcat(fn,_file_num);
                            j+=1;k+=1;
                        }
						i=k;
                        final_file_num += atoi(fn);
                    }
                    if(s[i]=='&'){
						char ln[256]={0,};
						k=i+1;
                        while(s[k]!=':'){
                            j=0;
                            line_num[j]=s[k];
							sprintf(_line_num, "%c", line_num[j]);
                            strcat(ln,_line_num);
                            j+=1;k+=1;
                        }
						i=k;
                        final_line_num += atoi(ln);
                    }
                }

		}
	}
}
