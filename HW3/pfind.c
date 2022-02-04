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

char* find(DIR *dpf,char ** k_words,char * p_file_path, int n_of_kwords){

        struct dirent *ep;
        
        char * final_string;
        char * temp;
        printf("received p_file_path = %s\n",p_file_path);
        int n_line;
        int count_dir;
        int count_files;
        while((ep = readdir (dpf))){
            
            printf("HERE IN READDIR\n");
            if((strcmp(ep->d_name,".")==0)||(strcmp(ep->d_name,"..")==0)) continue;
            
            n_line=0;
            count_dir=0;
            count_files=0;

            char * a_target_d_path ;
            a_target_d_path = (char*)malloc(sizeof(char)*(strlen(p_file_path) + strlen(ep->d_name) +6));
            strcpy(a_target_d_path,p_file_path);
            strcat(a_target_d_path,"/");
            strcat(a_target_d_path,ep->d_name);

            printf("a_target_d_path = %s\n",a_target_d_path);
            final_string = (char*)malloc(sizeof(char)*100);

            if(ep->d_type==DT_DIR){

                count_dir+=1;
            }

            else if(ep->d_type==DT_REG){

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
                                printf("================================\n");
                                printf("%s\n",a_target_d_path);
                                printf("%s\n",ep->d_name);
                                printf("%d\n",n_line);
                                printf("%s",pLine);
                                printf("================================\n");
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
 
        int final_dir_num=0;
        int final_line_num=0;
        int final_file_num=0;
        int process_c [process_num] ;
        int b;
        
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
        
        int fdq = open("channel",O_RDWR);
        q[0] = (char*)malloc(sizeof(char)*(strlen(argv[keyword_idx-1])+1));
        strcpy(q[0],argv[keyword_idx-1]);
        tail+=1;
        qsize+=1;
        search_dir(dp,argv[keyword_idx-1]);
        printf("qsize = %d\n", qsize);
        for(int i=0;i<tail;i++){
            printf("string in q %s\n",q[i]);
        }
        
        int length_t = strlen(q[head]) ;
        printf("q[head] = %s\n",q[head]);
        b = write(fdq,q[head],length_t);
        printf("b = %d\n",b);
        
        //write_bytes(fdq,&length_t,sizeof(length_t));
        //write_bytes(fdq,q[head],strlen(q[head]));
        head+=1;
        qsize-=1;
        printf("------------------------------------\n");

        for(int i=0;i<process_num;i++){
            
            pid_t child = fork();
            
            if (child==0) {
                
                int fdt = open("channel",O_RDWR);
                int fd_2 = open("channel_2", O_RDWR);
                
                while(1){
                    
                    printf("HERE IN CHILD PROESS\n");
                    char st[128];
                    size_t length,bs;
                    flock(fdt,LOCK_EX);
//                    if(read_bytes(fdt,&length,sizeof(length))!=sizeof(length)){
//                        flock(fdt,LOCK_UN);
//                        break;
//                    }
                    //bs = read_bytes(fdt,st,length);
                    bs = read(fdt,st,128);
                    printf("st = %s\n",st);
                    
                    flock(fdt,LOCK_UN);
                    printf("HERE IN CHILD PROESS2\n");
                    //if(bs!=length) break;
                    //printf("st = %s\n",st);
                    char* subtasks_ = (char*)malloc(sizeof(char)*(bs+4));
//                    strcpy(subtasks_, "./") ;
                    char subtasks[128];

                    strcpy(subtasks_,st);
                    printf("substasks_ = %s\n",subtasks_);
                    //}
                    //읽은 task 를 수행 후 반환 결과 channel_2에 write
                    char * f_string;
                    //int fd_2 = open("channel_2",O_RDONLY | O_SYNC );
                    f_string = (char*)malloc(sizeof(char)*128);
                    DIR* dpt ;
                    //strcat("./",subtasks_);
                    //subtasks_[strlen(subtasks_)-1] = '\0';
//                    subtasks_[strlen(subtasks_)-2] = '\0';
                    printf("length = %lu\n",strlen(subtasks_));
                    
                    if ((dpt = opendir (subtasks_)) == NULL) {
                        perror ("Cannot open .");
                        exit (1);
                    }
                    else printf("subtasks_ OPENED\n");
                
                    flock(fd_2,LOCK_EX);
                    f_string = find(dpt,&argv[keyword_idx],subtasks_,argc-keyword_idx);
                    printf("result string = %s\n",f_string);
                    
                    int t_length = strlen(f_string) ;
                    write(fd_2,f_string,t_length);
                    flock(fd_2,LOCK_UN);
                            
                }
            }
        }
        
        while(qsize!=0){
            
            char str[128];
            size_t length;
            int cnt;
            int fdp = open("channel",O_RDWR);
            int fdp_2 = open("channel_2", O_RDWR);
            printf("HERE IN PARENT PROCESS!\n");
            cnt = read(fdp,str,length);
            
            int length_t = strlen(q[head])+1 ;
            char *delete_Trash = (char*)malloc(sizeof(char)*(length_t+2));
            write(fdp,q[head],length_t);
            head+=1;
            qsize-=1;

            printf("Qsize = %d\n", qsize);
            
            while(1){
                //manager 가 child 에서 쓴 결과값들을 channel 에서 읽는 과정
               
                char s[128];
                char *dir_num_ = (char*)malloc(sizeof(char)*128);
                char *file_num_ = (char*)malloc(sizeof(char)*128);
                char *line_num_ = (char*)malloc(sizeof(char)*128);
                char line_num[20];
                char dir_num[20];
                char file_num[20];
//                printf("here\n");
                size_t length,bs;
                flock(fdp_2,LOCK_EX);

                bs = read(fdp_2,s,length);
                
                flock(fdp_2,LOCK_UN);
    
                if(bs!=length) break;
//                printf("here3\n");
                for(int i=0;i<bs;i++){
                    if(s[i]=='*'){
                        printf("* DISCOVERED\n");
                        while(s[i]!=':'){
                            int j=0;
                            dir_num[j] = s[i];
                            j+=1;
                            strcpy(dir_num_,dir_num);
                            printf("dir_num_ = %s\n",dir_num_);
                            final_dir_num += atoi(dir_num_);
                        }
                    }
    
                    if(s[i]=='@'){
                       
                        while(s[i]!=':'){
                            int j=0;
                            file_num[j]=s[i];
                            j+=1;
                            strcpy(file_num_,file_num);
                            final_file_num += atoi(file_num_);
                        }
                    }
                    if(s[i]=='&'){
                        while(s[i]!=':'){
                            int j=0;
                            line_num[j]=s[i];
                            j+=1;
                            strcpy(line_num_,line_num);
                            final_line_num += atoi(line_num_);
                        }
                    }
                }
            }
        }
        
        printf("SEARCHED LINE NUM = %d\n",final_line_num);
        printf("SEARCHED DIR NUM = %d\n",final_line_num);
        printf("SEARCHED FILE NUM = %d\n",final_line_num);
        
    }
}
