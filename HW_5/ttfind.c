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
#include <pthread.h>
#include <sched.h>
#include <assert.h>
#define OPTION "option"

//int final_dir_num=0;
//int final_line_num=0;
//int final_file_num=0;
char **keywords ;
char p_path[100];
int num;
DIR* initial_dir;
 
int TestAndSet(int *old_ptr, int new) {
    int old = *old_ptr;
    *old_ptr = new;
    return old;
}

typedef struct lock_t{
	int flag;
    queue_t *q;
    int guard;
}lock_t;

void lock_init(lock_t *m) {
    m->flag = 0;
    m->guard = 0;
    queue_init(m->q);
}

void lock(lock_t *m) {
    
    while (TestAndSet(&m->guard, 1) == 1);
    
    if (m->flag == 0){
        m->flag = 1;
        m->guard = 0;
    }
    else{
        queue_add(m->q, gettid());
        m->guard = 0;
        park();
    }
}

void unlock(lock_t *m) {
    while (TestAndSet(&m->guard, 1) == 1);
    if (queue_empty(m->q)) m->flag = 0;
    else unpark(queue_remove(m->q));
    m->guard = 0;
}

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

void find(DIR *dpf,char **k_words,char * p_file_path,int n_of_kwords){

		struct dirent *ep;
        char * temp;
        int n_line=0;
		int line_t;
        int count_dir=0;
        int count_files=0;
		
        strcpy(temp,ep->d_name);
        
        lock_init(&m);
        
    
		while((ep = readdir (dpf))){

            if((strcmp(ep->d_name,".")==0)||(strcmp(ep->d_name,"..")==0)) continue;
            line_t = 0;

            char * a_target_d_path ;
            a_target_d_path = (char*)malloc(sizeof(char)*(strlen(p_file_path) + strlen(ep->d_name) +6));
            strcpy(a_target_d_path,p_file_path);
            strcat(a_target_d_path,"/");
            strcat(a_target_d_path,ep->d_name);

            if(ep->d_type==DT_DIR){
      			DIR *dps= opendir(a_target_d_path);
      			find(dps,k_words,a_target_d_path,n_of_kwords);
     		 	free(a_target_d_path);
                count_dir+=1;
            }

            else if(ep->d_type==DT_REG){

                FILE * t_file = fopen(a_target_d_path,"rb");
                const int max = 1024;
                char line[max];
                char *pLine;
				char buf[1024];
                count_files+=1;
				line_t=0;
				lock(&m);
                while(!feof(t_file)){

                    pLine = fgets(line, max, t_file);
					line_t+=1;
                    for(int i=0; i < n_of_kwords; i++){
                        if(pLine != NULL){
                            if(strstr(pLine,k_words[i])!=NULL){
                                n_line+=1;
                                printf("====================================================\n");

                                if(realpath(a_target_d_path, buf) == NULL) {
                                perror("realpath error");
                                }
                                printf("%s\n",buf);

                                printf("%s\n",ep->d_name);
                                printf("%d\n",line_t);
                                printf("%s",pLine);
                                printf("====================================================\n");
                            }
                        }
                    }
                }
                fclose(t_file);
				unlock(&m);
            }
        }
       
		
        char count_dir_[20];
        char count_files_[20];
        char n_line_[20];
        sprintf(count_dir_,"%d",count_dir);
        sprintf(count_files_,"%d",count_files);
        sprintf(n_line_,"%d",n_line);
}

void *tunnel(){

    
    find(initial_dir,keywords,p_path,num);
    
    return 0;
}

int main(int argc, char * argv[]){

	int c;
    int invalid;
    DIR *dp;
	int worker_num;
    
    while((c = getopt (argc, argv, "t: ")) != -1){
        
        switch (c)
        {
            case 't':
                //if(invalid == 1) break;
                if(atof(optarg)<16) {
					worker_num = atof(optarg);		
				}
                else {
                    printf("There are no more than 16 worker threads!\n");
                    invalid=1;
                }
                break;
            default :
                printf("ERROR!!!!!\n");
        }
    }
	if(argc < 5|| invalid == 1){
        puts("Usage : ./tfind -t <num> <dir> [<keyword>]+");
        printf("---------------------------------------------------\n");
    }
	else{

        DIR *dp;
        dp = opendir(argv[3]);
        keywords = &argv[4];
        strcpy(p_path,argv[3]);
        num = argc-4;
        initial_dir = dp;

		if (dp != NULL){
            //file=fopen(argv[3],"ab");
			pthread_t p[worker_num];
        	for(int i=0;i<worker_num;i++){
        	    int ret = pthread_create(&p[i],NULL,tunnel,NULL);
                printf("return = %d\n",ret);
        	}
			//printf("00\n");
			//find(dp,&argv[4],argv[3],argc-4);
            //find(dp,file,argv[4]);
            //fclose(file);
        }
		else{
			//not reached
		}

		for(int i=0;i<worker_num;i++){
			pthread_join(p[i], NULL);
		}
		
	}

}
