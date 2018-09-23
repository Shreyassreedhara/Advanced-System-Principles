
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define Buff_length 3000
#define NUM_REDUCER 150

//initializations of the variables
int num_buffer=0;   
int num_reducer=0;
int tuple_cnt=0;
int unique_user_val=0;
int tot_slots=1;
int user_cnt[Buff_length]={0}; 
int new[NUM_REDUCER]={0};
bool mapper_flag = 0;
pthread_mutex_t lock;
pthread_mutex_t *mtx_ptr;
pthread_cond_t *cond_ptr;
int str;
int nred_array[NUM_REDUCER];

/* structure for segregating the tuples */
struct tup
	{
	int userID;
	char topic[15];
	int score;
	}tuple[Buff_length];

/* For working on tuples with same topics */
struct topics
	{
	char topic[15];	
	}topic_list[Buff_length];


// Reducer takes input from the buffer and prints the output on the screen

void *reducerthread(void *threadid)
	{
	int thrID;
	int counter_nred = 0;
	pthread_mutex_t* mtx = &mtx_ptr[thrID];
	pthread_cond_t* cond = &cond_ptr[thrID];
	thrID = (int)threadid;

	// Make a list of all the topics for the user
	int topic_decider=0;
	new[thrID]=0;
	pthread_mutex_lock(&lock);
	for(int newuser=0; newuser<user_cnt[thrID]; newuser++)
		{
		if(newuser==0)
			{
			strncpy(topic_list[new[thrID]].topic, tuple[(thrID*num_buffer)+newuser].topic, 15);
			new[thrID]++;
			topic_decider=1;
			}
		for(int user=0; user<new[thrID]; user++)
			{
			if(strncmp(tuple[(thrID*num_buffer)+newuser].topic, topic_list[user].topic, 15)==0)
				{
				topic_decider = 1;
				break;
				}
			}
		if(topic_decider==0)
			{
			strncpy(topic_list[new[thrID]].topic, tuple[(thrID*num_buffer)+newuser].topic, 15);
			new[thrID]++;
			}
		topic_decider=0;
		}

	for(int trs=0;trs<NUM_REDUCER-1;trs++)
		{
		if(counter_nred == 0)
			{
			if(trs==0)
				{
				nred_array[trs] = trs;
				trs++;
				}
			else
				{
				nred_array[trs] = trs-1;
				}
			}
		else
			{
			continue;
			}
		if(trs == nred_array[trs])
			{
			nred_array[trs] == 0;
			}
		}

		
	
	// for loop to add the values of similar topics with same userIDs
	for(int a=0; a<new[thrID]; a++)
		{	
		int total_score=0;	
		for(int b=0; b<user_cnt[thrID]; b++)
			{
			if(strncmp(tuple[b].topic, topic_list[a].topic, 15)==0)
				{
				total_score = total_score + tuple[b].score;
				}
			}
		printf("\(%04i,%.15s,%d)\n", tuple[thrID*num_buffer].userID, topic_list[a].topic, total_score);
		}
	//release the lock
	pthread_mutex_unlock(&lock);	
	pthread_exit(NULL);
}


// Mapper program takes the input and copies it into the buffer

void *mapperthread(void *threadid)
	{
	char topic[15];
	char action;
	int value;
	int userID;
	char ch;
	char temp;

	pthread_mutex_lock(&lock);	
	while ((ch = getc(stdin)) !=EOF)
		{							
		if(ch == '(')
			{
			scanf("%d", &userID);				

			temp = getc(stdin);				
			action = getc(stdin);				
			temp = getc(stdin);				

			for(int f=0; f<15; f++)
				{
				topic[f] = getc(stdin);			
				}
			switch(action)
			{
				case 'P': value=50;
					   break;
				case 'L': value=20;
					   break;
				case 'D': value=-10;
					   break;
				case 'C': value=30;
					   break;
				case 'S': value=40;
					   break;
			}
			
			// add the tuples to buffer
			if(tuple_cnt==0)
				{
				tuple[0].userID = userID;
				strncpy(tuple[0].topic, topic, 15);
				tuple[0].score = value;
				user_cnt[0] = user_cnt[0] + 1;
				unique_user_val = unique_user_val + 1;
				}
			
			else
				{
				for(int f=0; f<unique_user_val; f++)
					{
					if(userID==tuple[f*num_buffer].userID)
						{
						tuple[(f*num_buffer)+user_cnt[f]].userID = userID;
						strncpy(tuple[(f*num_buffer)+user_cnt[f]].topic, topic, 15);
						tuple[(f*num_buffer)+user_cnt[f]].score = value;
						user_cnt[f]++;
						mapper_flag = 1;
						}
					}

				if(mapper_flag==0)
					{
					tuple[(unique_user_val)*num_buffer].userID = userID;
					strncpy(tuple[(unique_user_val)*num_buffer].topic, topic, 15);
					tuple[(unique_user_val)*num_buffer].score = value;
					user_cnt[unique_user_val]++;
					unique_user_val = unique_user_val + 1;
					}
				mapper_flag = 0;
				
			}

			tuple_cnt = tuple_cnt + 1;
		}
	}
	pthread_mutex_unlock(&lock);	
	pthread_exit(NULL);
}

 

// main program creates the mapper and reducer thread and joins them

int main (int argc, char *argv[]){
	
	// Read arguments
	num_buffer = atoi(argv[1]);
	num_reducer = atoi(argv[2]);
	tot_slots = num_buffer * num_reducer;
		
	
	pthread_t threadid[num_reducer+1];

	pthread_mutex_t mtx[num_reducer];
	pthread_cond_t cond[num_reducer];
	mtx_ptr = mtx;
	cond_ptr = cond;
	
	//Create Mapper thread
	pthread_create(&threadid[0], NULL, mapperthread, NULL);
	
	pthread_join(threadid[0], NULL);
	
	//Create Reducer threads
	long t;
	for(t=0; t<num_reducer; t++){
		pthread_create(&threadid[t+1], NULL, reducerthread, (void *)t);	
	}
	
	pthread_exit(NULL);
}
