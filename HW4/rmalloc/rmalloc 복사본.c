#include <unistd.h>
#include <stdio.h>
#include "rmalloc.h" 
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/*
typedef enum {
    BestFit, WorstFit, FirstFit
} rm_option ;


struct _rm_header {
    struct _rm_header * next ;
    size_t size ;
} ;

typedef struct _rm_header   rm_header ;
typedef struct _rm_header * rm_header_ptr ;
*/ //rmalloc.h

/*
#include "rmalloc.h"

int 
main ()
{
    void *p1, *p2, *p3, *p4 ;

    rmprint() ;

    p1 = rmalloc(2000) ; 
    printf("rmalloc(2000):%p\n", p1) ; 
    rmprint() ;

    p2 = rmalloc(2500) ; 
    printf("rmalloc(2500):%p\n", p2) ; 
    rmprint() ;

    rfree(p1) ;                                                                                           
    printf("rfree(%p)\n", p1) ;
    rmprint() ;

    p3 = rmalloc(1000) ;
    printf("rmalloc(1000):%p\n", p3) ;
    rmprint() ;

    p4 = rmalloc(1000) ;
    printf("rmalloc(1000):%p\n", p4) ;
    rmprint() ;
}
*/
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

rm_header rm_free_list = { 0x0, 0 } ;
rm_header rm_used_list = { 0x0, 0 } ;

rm_header_ptr store_add[100];
int k=0;
void * rmalloc (size_t s) 
{
	
	int *p;
	int i;

	size_t p_size = getpagesize();
	rm_header_ptr temp,itr,temp2;
	rm_header_ptr ptr;

	rm_header n_ptr,n_ptr2;

	if(rm_free_list.next == 0X0) {
		
		ptr = (rm_header_ptr)mmap(0,p_size, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON, 0, 0);
		store_add[k] = ptr; k+=1;
		ptr->next = 0X0;
		ptr->size = p_size-sizeof(rm_header) ;
		printf("ptr->size = %zu\n",ptr->size);
		if(ptr == MAP_FAILED) handle_error("mmap");
		
		rm_free_list.next = ptr ;
	}



	for (itr = rm_free_list.next; itr != 0x0 ; itr = itr->next ) {

		if(itr->size == s) {
			//used list 에 memory region 을 넣고 starting address 를 return

			for (temp = rm_used_list.next; temp != 0x0 ; temp = temp->next) {

            }
			temp = itr + itr->size - (s+sizeof(rm_header)) ;
			temp->next = 0X0;
			temp->size = s + sizeof(rm_header);
			
			itr->size = itr->size - s - sizeof(rm_header);

			return temp;
		}
		else if(itr->size > s){
			// itr->size 를 split 해서 첫 부분에 s 할당, 나머지는 free list 로 넣어줌
			for (temp = &rm_used_list; temp->next != 0x0 ; temp = temp->next) {
            }
			for(temp2 = &rm_free_list; temp2->next!=itr; temp2 = temp2->next){
				
            }
			rm_header_ptr t_node;
			t_node = (rm_header_ptr)((void*)itr + sizeof(rm_header)+itr->size - (s+sizeof(rm_header)));
			t_node->size = s + sizeof(rm_header);
			
			temp->next = t_node;
			t_node->next = 0X0; 
			
			itr->size = itr->size - (s + sizeof(rm_header));

			return temp->next;
		}
		else{
				//새로운 page 할당
			printf("*ALLOCATE NEW PAGE*\n");
			ptr = (rm_header_ptr)mmap(ptr,p_size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON, 0, 0);
            if(ptr == MAP_FAILED) handle_error("mmap");
			store_add[k] = ptr; k+=1;
			ptr->size = p_size - sizeof(rm_header) - (s+sizeof(rm_header));
			ptr->next = 0X0;

			for (temp = rm_free_list.next; temp->next != 0x0 ; temp = temp->next) {
                
            }

			temp->next = ptr;

			for (temp = &rm_used_list; temp->next != 0x0 ; temp = temp->next) {
                
            }

			rm_header_ptr new_node ;
			new_node = (rm_header_ptr)((void*)ptr+ptr->size);
			new_node->size = s + sizeof(rm_header);
			new_node->next = 0X0;
			temp->next = new_node;
			
			return temp->next;
			
        }

	}
	k+=1;
	return 0X0;
}

void rfree (void * p) 
{
	int flg;
	struct _rm_header * temp;
	struct _rm_header *t ;

	rm_header_ptr tmp,tmp2,tmp3,tmp4,tmp5,tmp6;

	for (tmp = rm_used_list.next ; tmp != p ; tmp = tmp->next) {
		
    }
	
	for (tmp4 = &rm_used_list; tmp4 != tmp->next ; tmp4 = tmp4->next) {

   	}

	for (tmp6 = &rm_used_list; tmp6->next != tmp ; tmp6 = tmp4->next) {

    }

	for (tmp3 = rm_free_list.next; tmp3 != 0x0 ; tmp3 = temp->next) {

    } 
	for(tmp5 = rm_free_list.next;tmp5->next!=tmp3;tmp5=tmp5->next){

	}
	tmp5->next = p;
	((rm_header_ptr)p)->next = 0X0;
	
	tmp6->next = tmp4;
}

void * rrealloc (void * p, size_t s) 
{
	// TODO
	return 0x0 ; // erase this 
}

void rmshrink () 
{
	// TODO
	printf("k = %d\n",k);
	for(int t=0;t<k;t++){

		if(store_add[t]->size == 0) {
			munmap(store_add[t],0);
		}
		//printf("stored add = %p",store_add[t]);
	}
}

void rmconfig (rm_option opt) 
{
	// TODO
	if(opt == BestFit){
		
	}
	else if (opt == WorstFit){
		
	}
	else if(opt == FirstFit){
		
	}
}


void 
rmprint () 
{
	rm_header_ptr itr ;
	int i ;

	printf("==================== rm_free_list ====================\n") ;
	for (itr = rm_free_list.next, i = 0 ; itr != 0x0 ; itr = itr->next, i++) {
		printf("%3d:%p:%8d:", i, ((void *) itr) + sizeof(rm_header), (int) itr->size) ;

		int j ;
		char * s = ((char *) itr) + sizeof(rm_header) ;
		for (j = 0 ; j < (itr->size >= 8 ? 8 : itr->size) ; j++) 
			printf("%02x ", s[j]) ;
		printf("\n") ;
	}
	printf("\n") ;

	printf("==================== rm_used_list ====================\n") ;
	for (itr = rm_used_list.next, i = 0 ; itr != 0x0 ; itr = itr->next, i++) {
		printf("%3d:%p:%8d:", i, ((void *) itr) + sizeof(rm_header), (int) itr->size) ;

		int j ;
		char * s = ((char *) itr) + sizeof(rm_header) ;
		for (j = 0 ; j < (itr->size >= 8 ? 8 : itr->size) ; j++) 
			printf("%02x ", s[j]) ;

		printf("\n") ;
	}
	printf("\n") ;

}
