#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

int num_of_reducer;
int num_of_slots;
char filename[100];
int arr_length = 0;
int count = 0;
int* in_pointer;
int* out_pointer;
int* ptr_unqID;
struct tuples* pointer; 						    // Check if this points to one structure or to an array of structures
int* counter_buf;
int done = 0;
//pthread_mutex_t *lock_pointer;					// pointer which points to the array of locks. This will be accessed by both mapper and reducer
sem_t* lock_pointer;
//pthread_cond_t *cond_fill_pointer;
//pthread_cond_t *cond_empty_pointer;
sem_t* fill_pointer;
sem_t* empty_pointer;
bool flag = 0;


struct tuples
{
	int userID;
	char topic[15];
	int action;
};

/* Assigns the buffer starting address to the in and out arrays. Is called only once at the beginning of the mapper program */
int assigner()
{
	//printf("Entered the assigner function\n");
	int i;
	for (i = 0; i < num_of_reducer; i++) {
		in_pointer[i] = 0;
	}
	for (i = 0; i < num_of_reducer; i++) {
		out_pointer[i] = 0;
	}
	return 0;
}

/*Searches if the given userId is already alloted or not.If not it will add it and return the value where it added. Else it will return the address where it found the repeated value*/
int search_userID(int rec_userid, int* arr_length)
{
	//printf("Entered the search_userID function\n");
	int f;
	//printf("The userID received is: %.4d\n",rec_userid);
	for (f = 0; f < (*arr_length); f++)
	{
		if (ptr_unqID[f] == rec_userid)
		{
			//printf("The userid %.4d is repeating at position %d\n",rec_userid,f);
			return f;
		}
	}
	ptr_unqID[(*arr_length)] = rec_userid;
	(*arr_length)++;
	//printf("The value returned by the search_userID function is %d\n",(*arr_length)-1);
	return (*arr_length) - 1;
}

/*This function should return the matching buffer ID to the mapper */
int matcher(int position)
{
	//printf("Entered the matcher function of mapper\n");
	//printf("Input argument is %d\n",position);
	int ret_val_match = (num_of_slots * position) + (in_pointer[position] % num_of_slots);	//returns the value of which buffer to insert to
	in_pointer[position] = (in_pointer[position] + 1) % num_of_slots;
	//printf("The value returned by the matcher function is %d\n",ret_val_match);		
	return ret_val_match;
}

/* Check the count and return 0 if it is full and 1 if it is not full */
int check_count(int ret_val_srch)
{
	//printf("Entered the check_count function\n");
	//printf("Input to the function is %d\n",ret_val_srch);
	if (counter_buf[ret_val_srch] == num_of_slots)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

//replace the character of action by the corresponding value
int action_value(char ch)
{
	//printf("Entered the action_value function\n");
	int ret_action;
	switch (ch)
	{
	case 'P': ret_action = 50;
		return ret_action;
		break;
	case 'S': ret_action = 40;
		return ret_action;
		break;
	case 'L': ret_action = 20;
		return ret_action;
		break;
	case 'D': ret_action = -10;
		return ret_action;
		break;
	case 'C': ret_action = 30;
		return ret_action;
		break;
	}
}


//==========================================================        Mapper       ==============================================================


/* This is the mapper function. It takes care of filling the buffer with values */
void* mapper(void* threadid)
{
	int userid_arr[num_of_reducer];					// initialize an array which holds the unique userids
	ptr_unqID = &userid_arr;						// Point a pointer to that array
	char c;
	int temp_userID;
	char temp_action;
	char temp_topic[15];
	char full[3000];
	char* strpointer;


	if (stdin == NULL)
	{
		printf("Unable to open the input file in mapper thread\n");
	}

	while ((c = getc(stdin)) != EOF)
	{
		if (c == ')')
		{
			count++;
		}
	}
	fseek(stdin, 0, SEEK_SET);
	//printf("count: %d\n",count);
	struct tuples buffer[num_of_reducer * num_of_slots];
	pointer = &buffer;										// point a pointer to the array of structures.
	int count_buffer[num_of_reducer];						// create an array to hold the value of the how many times we have written to corresponding buffer
	for (int i = 0; i < num_of_reducer; i++)
	{
		count_buffer[i] = 0;
	}
	counter_buf = &count_buffer;							// Point a global variable to the count array. This will be accessed by mapper and reducer. So put it in main?

	fgets(full, 3000, stdin);

	int in_arr[num_of_reducer];								// in array initialized
	int out_arr[num_of_reducer];							//out array initialized
	in_pointer = &in_arr;									/* point pointers to these two arrays */
	out_pointer = &out_arr;
	//printf("Calling the assigner function from the mapper\n");
	assigner();												/* Calls assignment function which sets in all elements of in and out array to zero */

	flag = 1;												// flag is used to signal main to create reducer threads
	usleep(100);

	strpointer = strtok(&full[1], ",");						//get the first userID
	//printf("After first strtok: %s\n", strpointer);
	while (strpointer != NULL)
	{
		//printf("Calling the search_userID function from mapper\n");
		int ret_val_srch = search_userID(atoi(strpointer), &arr_length);	/* Calls the ret_val_srch function which returns the value in which buffer the userID has to be put */
		//printf("The value returned by the search_userID function to the mapper is %d\n",ret_val_srch);
		//The value of where the thread Id is repeated is given

		temp_userID = atoi(strpointer);
		//printf("UserID is stored : %.4d\n",temp_userID);

		strpointer = strtok(NULL, ",");
		temp_action = *strpointer;
		//printf("Action: %c\n",temp_action);
		//printf("Calling the action_value function\n");
		int ret_temp_action = action_value(temp_action);
		//printf("The value returned by the action_value is: %d\n",ret_temp_action);

		strpointer = strtok(NULL, ")");
		strcpy(temp_topic, strpointer);
		//printf("The topic got after strtok is %s\n",temp_topic);

		strpointer = strtok(NULL, "(");
		if (strpointer)
		{
			strpointer = strtok(NULL, ",");
		}

		//pthread_mutex_lock(&lock_pointer[ret_val_srch]);								// Acquire the lock corresponding to the userID	
		sem_wait(&lock_pointer[ret_val_srch]);

		while (check_count(ret_val_srch) == 0)										//Checking if the corresponding buffer is full
		{
			//pthread_cond_signal(&cond_fill_pointer[ret_val_srch]);
			sem_post(&fill_pointer[ret_val_srch]);
			//pthread_cond_wait(&cond_empty_pointer[ret_val_srch], &lock_pointer[ret_val_srch]);
			sem_post(&lock_pointer[ret_val_srch]);
			sem_wait(&empty_pointer[ret_val_srch]);
			sem_wait(&lock_pointer[ret_val_srch]);
		}
		int ret_val_match = matcher(ret_val_srch);
		//The value of the buffer where it has to be filled is given
		//printf("The value returned by the ret_val_match function to the mapper is %d\n",ret_val_match);	
		pointer[ret_val_match].userID = temp_userID;
		pointer[ret_val_match].action = ret_temp_action;
		strcpy(pointer[ret_val_match].topic, temp_topic);
		counter_buf[ret_val_srch] = counter_buf[ret_val_srch] + 1;
		//in_pointer[ret_val_srch] = in_pointer[ret_val_srch] + 1;								// in_pointer has already been incremented in the function matcher
		//pthread_cond_signal(&cond_fill_pointer[ret_val_srch]);
		sem_post(&fill_pointer[ret_val_srch]);
		//pthread_mutex_unlock(&lock_pointer[ret_val_srch]);									// Give away the lock corresponding to the userID
		sem_post(&lock_pointer[ret_val_srch]);
	}
	done = 1;
	for (int f = 0; f < num_of_reducer; f++)
	{
		usleep(1000);
		//pthread_cond_signal(&cond_fill_pointer[f]);
		sem_post(&fill_pointer[f]);
	}
	pthread_exit(NULL);
}




//+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
//*                                                                                                                                                                                +
//+                                                             mapper ends, reducer starts                                                                                        *
//*                                                                                                                                                                                +
//+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*





/* This function allocates the buffer created to the corresponding reducer threads and tells from which point it has to read */
int red_matcher(int tid)
{
	int ret_all_buf;
	ret_all_buf = (tid * num_of_slots) + (out_pointer[tid] % num_of_slots);
	out_pointer[tid] = (out_pointer[tid] + 1) % num_of_slots;
	return ret_all_buf;
}

/* This function checks if the topic is already there in the received_array and updates it with the new value */
int check_and_update(struct tuples* rec_struct, struct tuples* red_pointer, int* rec_arr_count)
{
	//printf("The rec_arr_count is: %d\n",(*rec_arr_count));
	//printf("Entered the check and update function\n");
	int ret_cau;
	int f;
	for (f = 0; f < (*rec_arr_count); f++)
	{
		ret_cau = strcmp(rec_struct->topic, red_pointer[f].topic);
		if (ret_cau == 0)
		{
			//printf("This topic repeats at %d position of received array\n",f);
			red_pointer[f].action = red_pointer[f].action + rec_struct->action;
			return 0;
		}
	}
	red_pointer[(*rec_arr_count)].userID = rec_struct->userID;
	red_pointer[(*rec_arr_count)].action = rec_struct->action;
	strcpy(red_pointer[(*rec_arr_count)].topic, rec_struct->topic);
	(*rec_arr_count)++;
	return 0;
}


/* ===========================================================     Reducer     ============================================================== */


/* This is the reducer thread function. This reads the value from the buffer and prints it */
void* reducer(void* threadid)
{

	int tid;
	tid = (int)threadid;											// type casting the pointer and storing its value
	//printf("Printing in reducer, %d thread created\n",tid);
	//struct tuples* red_pointer;
	struct tuples received_array[count];							// array of structures to hold the structures the reducer thread takes out of buffer
	//red_pointer = &received_array;								// point the structure pointer to the array of structures
	//int red_pointer;
	int rec_arr_count = 0;											// variable to know at what position in the array to store the structure taken out of the buffer 		

	while (1)
	{
		// acquire the lock corresponding to the threadid, if not available, put conditional variable
		// pthread_mutex_lock(&lock_pointer[tid]);
		sem_wait(&lock_pointer[tid]);
		while (counter_buf[tid] == 0 && done == 0)
		{
			//pthread_cond_signal(&cond_empty_pointer[tid]);
			sem_post(&empty_pointer[tid]);
			//pthread_cond_wait(&cond_fill_pointer[tid],&lock_pointer[tid]);
			sem_post(&lock_pointer[tid]);
			sem_wait(&fill_pointer[tid]);
			sem_wait(&lock_pointer[tid]);
		}
		if (done == 0)
		{
			while (counter_buf[tid] != 0)
			{
				int ret_all_buf = red_matcher(tid);								// The place from which the elements should be taken out from the buffer is returned 
				//printf("return value of red_matcher for %d thread id is %d\n",tid,ret_all_buf);
				int ret_cau = check_and_update(&pointer[ret_all_buf], &received_array, &rec_arr_count);		// Pass the received array by reference to the function
				//printf("return value of cau function for %d thread id is %d\n",tid,ret_cau);
				counter_buf[tid] = counter_buf[tid] - 1;
				//out_pointer[tid] = (out_pointer[tid] + 1) % num_of_slots;
				//rec_arr_count++;
			}
		}
		if (done == 1)
		{
			while (counter_buf[tid] != 0)									// Remove anything that is in the buffer and process it
			{
				int ret_all_buf = red_matcher(tid);							// The place from which the elements should be taken out from the buffer is returned 
				int ret_cau = check_and_update(&pointer[ret_all_buf], &received_array, &rec_arr_count);
				counter_buf[tid] = counter_buf[tid] - 1;
				//out_pointer[tid] = (out_pointer[tid] + 1) % num_of_slots;
			}
			for (int f = 0; f < rec_arr_count; f++)
			{
				//printf("ID: %d\t",tid);
				printf("(%.4d,", received_array[f].userID);
				printf("%s,", received_array[f].topic);
				printf("%d)\n", received_array[f].action);
			}
			break;
		}
		//pthread_mutex_unlock(&lock_pointer[tid]);
		sem_post(&lock_pointer[tid]);
	}
	//pthread_mutex_unlock(&lock_pointer[tid]);
	sem_post(&lock_pointer[tid]);
	pthread_exit(NULL);
}



//+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
//*                                                                                                                                                                                +
//+                                                                 reducer ends, main starts                                                                                      *
//*                                                                                                                                                                                +
//+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*



/* ===================================================================        Main            ====================================================== */

/* Main function. Initializes the mapper and reducer threads */
int main(int argc, char* argv[])
{
	int rc;
	long t;
	num_of_slots = atoi(argv[1]);
	num_of_reducer = atoi(argv[2]);
	//strcpy(filename,argv[3]);
	/*printf("filename is: %s\n",filename); */
	if (argc < 2)
	{
		printf("Enter 2 parameters\n");
		return 1;
	}

	if (!(num_of_reducer > 0))
	{
		printf("ERROR: YOU NEED TO ENTER A VALID VALUE FOR NUMBER_OF_REDUCERS THAT IS GREATER THAN ZERO AND A WHOLE NUMBER!!\n");
		printf("PROCESS KILLED, EXITING!!\n");
		return 1;
	}

	if (!(num_of_slots > 0))
	{
		printf("ERROR: YOU NEED TO ENTER A VALID VALUE FOR SLOT SIZE OF THE BUFFER THAT IS GREATER THAN ZERO AND A WHOLE NUMBER!!\n");
		printf("PROCESS KILLED, EXITING!!\n");
		return 1;
	}

	sem_t lock[num_of_reducer];
	for (int f = 0; f < num_of_reducer; f++)
	{
		sem_init(&lock[f], 0, 1);
	}
	lock_pointer = &lock;


	sem_t filled[num_of_reducer], empty[num_of_reducer];
	for (int f = 0; f < num_of_reducer; f++)
	{
		sem_init(&filled[f], 0, 0);
		sem_init(&empty[f], 0, 0);
	}
	fill_pointer = &filled;
	empty_pointer = &empty;


	pthread_t mapper1;
	pthread_t thread[num_of_reducer];

	//printf("In main, creating mapper thread\n");
	rc = pthread_create(&mapper1, NULL, mapper, NULL);

	while (flag == 0);
	for (t = 0; t < num_of_reducer; t++)
	{
		//printf("In main, creating reducer thread %ld\n",t);
		rc = pthread_create(&thread[t], NULL, reducer, (void*)t);
		if (rc != 0)
		{
			printf("Error; return code from pthread_t create is %d\n", rc);
			return -1;
		}
	}
	pthread_exit(NULL);
	return 0;
}