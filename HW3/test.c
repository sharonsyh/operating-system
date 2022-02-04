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

char *q[20];
int head = 0;
int tail = 0;
int qsize = 0;

int write_bytes(int fd,void *a,int len){
    
    char* s = (char*)a;
    
    int i=0;
    
    while(i<len){
        int b;
        b = write(fd,s+i,len-i);
        if(b==0) break;
        i+=b;
    }
    return i;
}

int read_bytes(int fd, void* a ,int len){
    
    char*s = (char*)a;
    int i;
    for(int i=0;i<len;){
        int b;
        b = read(fd,s+i,len-i);
        
        if(b==0) break;
        i+=b;
    }
    return i;
}

void search_dir(DIR *dps,char * p_target_d_path ){
    
    struct dirent *ep_t;
    
    while((ep_t = readdir (dps))){
        
        if((strcmp(ep_t->d_name,".")==0)||(strcmp(ep_t->d_name,"..")==0)) continue;
        
        char * a_target_d_path ;
        a_target_d_path = (char*)malloc(sizeof(char)*(strlen(p_target_d_path) + strlen(ep_t->d_name) +6));
        strcpy(a_target_d_path,p_target_d_path);
        strcat(a_target_d_path,"/");
        strcat(a_target_d_path,ep_t->d_name);
        
        //printf("a_target_d_path = %s\n",a_target_d_path);
        
        if(ep_t->d_type==DT_DIR){
            
            q[tail] = (char*)malloc(sizeof(char)*(strlen(a_target_d_path)+1));
            strcpy(q[tail],a_target_d_path);
            tail+=1;
            qsize+=1;
            
            //strcat(a_target_d_path,"/");
            DIR *dps= opendir(a_target_d_path);
            search_dir(dps,a_target_d_path);
            free(a_target_d_path);
        }
    }
}

char* find(DIR *dp,char ** k_words,char * p_file_path, int n_of_kwords){
		
		printf("IN FIND\n");
        struct dirent *ep_t;
        int n_line=0;
        int count_dir=0;
        int count_files=0;
        char * final_string;

        while((ep_t = readdir (dp))){

			printf("IN WHILE\n");

            if((strcmp(ep_t->d_name,".")==0)||(strcmp(ep_t->d_name,"..")==0)) continue;

            char * a_target_d_path ;
            a_target_d_path = (char*)malloc(sizeof(char)*(strlen(p_file_path) + strlen(ep_t->d_name) +6));
            strcpy(a_target_d_path,p_file_path);
            strcat(a_target_d_path,"/");
            strcat(a_target_d_path,ep_t->d_name);

            printf("a_target_d_path = %s\n",a_target_d_path);
			printf("IN THE REGULAR FILE\n");
            final_string = (char*)malloc(sizeof(char)*100);
            if(ep_t->d_type==DT_DIR){

                count_dir+=1;
            }

            else if(ep_t->d_type==DT_REG){
				
				printf("IN THE REGULAR FILE\n");
                FILE * t_file = fopen(a_target_d_path,"rb");
                const int max = 1024;
                char line[max];
                char *pLine;
                count_files+=1;
                printf("IN THE REGULAR FILE\n");
                while(!feof(t_file)){

                    pLine = fgets(line, max, t_file);
                    n_line+=1;

                    for(int i=0; i < n_of_kwords; i++){
                        if(pLine != NULL){
                            if(strstr(pLine,k_words[i])!=NULL){

                                printf("%s\n",a_target_d_path);
                                printf("%s\n",ep_t->d_name);
                                printf("%d\n",n_line);
                                printf("%s",pLine);
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

    int c;
    int invalid;
    DIR *dp;
    int process_num = 2;
    int case_sensitive = 1;
    int absolute_path = 0;
    int keyword_idx;
    
    int cnt;
    for(int i= 1; i<argc;i++){
        dp = opendir(argv[i]);
        if(dp!=NULL) {
            keyword_idx = i+1;
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
                absolute_path = 1;
                break;
            default :
                break;
        }
    }
    
    if(argc < 4|| invalid == 1){
        puts("Usage : ./pfind <option> <dir> <keyword>");
        printf("---------------------------------------------------\n");
    }
    else {
        
//        char * child ;
//        child = (char*)malloc(sizeof(char)*(strlen("child")+1));
//        child = "child";
//        char process[process_num][10];
        int final_dir_num=0;
        int final_line_num=0;
        int final_file_num=0;
        int process_c [process_num] ;
        
        search_dir(dp,argv[keyword_idx-1]);
        for(int i=0;i<tail;i++){
            printf("string in q %s\n",q[i]);
        }
 
                    

        //읽은 task 를 수행 후 반환 결과 channel_2에 write
        char * f_string;
        //int fd_2 = open("channel_2",O_RDONLY | O_SYNC );
        f_string = (char*)malloc(sizeof(char)*128);
        DIR* dpt = opendir(argv[keyword_idx-1]);
        f_string = find(dpt,&argv[keyword_idx],"hdir1",argc-keyword_idx);
        printf("%s\n",f_string);
    
    //close(fd);
    //close(fd_2);
                
    }
    }

    


