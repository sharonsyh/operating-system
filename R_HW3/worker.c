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

char* find(DIR *dpf,char ** k_words,char * p_file_path,int n_of_kwords,char** s){
		
        struct dirent *ep;
        
        char *final_string;
		final_string = (char*)malloc(sizeof(char)*1000);
        char * temp;
        int n_line=0;
		int line_t;
        int count_dir=0;
        int count_files=0;
        while((ep = readdir (dpf))){
            
            if((strcmp(ep->d_name,".")==0)||(strcmp(ep->d_name,"..")==0)) continue;
            line_t = 0;

            char * a_target_d_path ;
            a_target_d_path = (char*)malloc(sizeof(char)*(strlen(p_file_path) + strlen(ep->d_name) +6));
            strcpy(a_target_d_path,p_file_path);
            strcat(a_target_d_path,"/");
            strcat(a_target_d_path,ep->d_name);

            if(ep->d_type==DT_DIR){
				strcat(final_string,"+");
				strcat(final_string,a_target_d_path);
				strcat(final_string,":");
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
                while(!feof(t_file)){
					
                    pLine = fgets(line, max, t_file);
					line_t+=1;
                    for(int i=0; i < n_of_kwords; i++){
                        if(pLine != NULL){
							if(atoi(s[0])==1){
                            	if(strstr(pLine,k_words[i])!=NULL){
									n_line+=1;
                                	printf("====================================================\n");
									if(atoi(s[1])==0) printf("./%s\n",a_target_d_path);
									else if(atoi(s[1])==1) {
										if(realpath(a_target_d_path, buf) == NULL) { 
										perror("realpath error"); 
										}
										printf("%s\n",buf);
									}
									else{
										//not reached
									}
                                	printf("%s\n",ep->d_name);
                                	printf("%d\n",line_t);
                                	printf("%s",pLine);
                                	printf("====================================================\n");
                           		}
							}
							else if(atoi(s[0])==0){
								if(strcasestr(pLine,k_words[i])!=NULL){
                                    n_line+=1;
                                    printf("====================================================\n");
                                    if(atoi(s[1])==0) printf("./%s\n",a_target_d_path);
                                    else if(atoi(s[1])==1) {
                                        if(realpath(a_target_d_path, buf) == NULL) { 
                                            perror("realpath error"); 
                                        }
                                        printf("%s\n",buf);
                                    }
                                    else{
                                        //not reached
                                    }
                                    printf("%s\n",ep->d_name);
                                    printf("%d\n",line_t);
                                    printf("%s",pLine);
                                    printf("====================================================\n");
                                }
							}
                        }
                    }
                }
                fclose(t_file);
            }
        }
        char count_dir_[20];
        char count_files_[20];
        char n_line_[20];
        sprintf(count_dir_,"%d",count_dir);
        sprintf(count_files_,"%d",count_files);
        sprintf(n_line_,"%d",n_line);
    
        strcat(final_string,"*");
        strcat(final_string,count_dir_);
        strcat(final_string,":");
        strcat(final_string,"@");
        strcat(final_string,count_files_);
        strcat(final_string,":");
        strcat(final_string,"&");
        strcat(final_string,n_line_);
        strcat(final_string,":");
        return final_string;
}

int main(int argc, char * argv[]){

	int i=0;
	char *sArr[30] = { NULL, }; 
	char *s[30] = { NULL, };
	char *ptr = strtok(argv[1], " ");
	char r_temp[30];

	while (ptr != NULL)            
    {
       
		if(strstr(ptr,":")!=NULL){
			strcpy(r_temp,ptr);
		} 
		else sArr[i] = ptr;

        i++;                       
        ptr = strtok(NULL, " ");   
    }
	int k=0;
	char *temp;

	for(int j=0;j<i;j++){

      if(strstr(r_temp,":")!=0){
          temp = strtok(r_temp, ":");
          while (temp != NULL) {
            s[k] = temp;
            k++;
            temp = strtok(NULL, ":");
          }
          break;
       }
    }
	
	while(1){

		char st[256];

		int fd = open("channel",O_RDWR);
        int fd_2 = open("channel_2", O_RDWR);	
		int cnt=0;
		size_t length;
		flock(fd,LOCK_EX);
		read(fd,&length,sizeof(length));
        cnt = read(fd,st,length);
        flock(fd,LOCK_UN);
		int tk = strlen(st)-1;

		while(tk!=(cnt-1)) {
			st[tk]='\0';
			tk-=1;
		}
		DIR* dpt;
		if ((dpt = opendir (st)) == NULL) {

		//	printf("WHY NOT OPEN %s\n",st);
		//	printf("WHY NOT OPEN %lu\n",strlen(st));
        	//perror ("Cannot open .");
            exit (1);
        }
		char f_string[128];  
		strcpy(f_string, find(dpt,&sArr[0],st,i-1,&s[0]));
		size_t t_length = strlen(f_string) ;
		write(fd_2,&t_length,sizeof(t_length));
        int k =write(fd_2,f_string,t_length);
	}
	
}
