#include<stdlib.h>
#include<string.h>
#include<semaphore.h>
#include<stdio.h>
#include <sys/mman.h>
int done[50];
int count;
struct tuples
	{
		char userID[5];
		int action;
		char topic[15];
	};
struct tuples **Buffer;
int map_value(char ch);
int buf_size,num_thread;
int *in,*out,*cnt;

sem_t sem[50],Full[50],Empty[50];


int main(int argc,char *argv[])
	{	
		int i;
		buf_size = atoi(argv[1]);
		num_thread = atoi(argv[2]);
		
		if(buf_size<=num_thread)
		{
			buf_size = buf_size+num_thread;
			
		}
		
		Buffer = (struct tuples **) mmap (NULL, sizeof(struct tuples *) *num_thread * buf_size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED,-1,0);
		*Buffer = (struct tuples *) mmap (NULL, sizeof(struct tuples) *num_thread * buf_size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED,-1,0);
		for(i=0;i < num_thread;i++)
			{
        		sem_init (&sem[i],1,1);
        		sem_init(&Full[i],1,0);
				sem_init(&Empty[i],1,0);
				
			}
		
		for(i=0;i < num_thread;i++)
			{
        		Buffer[i] = (*Buffer + buf_size * i);
			}
		
		in=(int *)malloc(num_thread * sizeof(int));
		out=(int *)malloc(num_thread * sizeof(int));
		cnt=(int *)malloc(num_thread * sizeof(int));
		
		for(i=0;i<num_thread;i++)
			{
				in[i]=0;
				out[i]=0;
				cnt[i]=0;
			}	
				
				char *fp;
			    char *buffer,value,topic[15];
			    int action,counter,var=0;
			    int j,len,m=0,n;
				i=0;
				buffer = (char *)malloc(2000);
			    while (fgets(buffer,2000,stdin) != NULL)
			 
			    while (buffer[i] != '\0')
			    {
			        if (buffer[i] == ')')
			        {
			            count++;
			        }
			        
			        i++;
			    }
	
			for(int trs=0;trs<count-1;trs++)
			{
				int nred_array[trs];
				if(count == 0)
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

			    char *array[count];
				char *array1[count];
				struct tuples tuple[count];
				i=0;
			    fp = strtok(&buffer[1],",");       
				strcpy(tuple[i].userID,fp);
				while (fp != NULL)
			    {
			        fp = strtok(NULL,",");
			        value = *fp;
			        action =(map_value(value));
			        tuple[i].action = (action);	
				
			        fp = strtok(NULL,")");
			        strcpy(tuple[i].topic,fp);

			        if (i==(count-1))
			            {
			                break;
			            }

			        fp = strtok(NULL,"(");

			        fp = strtok(NULL,",");	
				
			        strcpy(tuple[i+1].userID,fp);	 
			    	i++;       
			    }

			    for(i=0;i<count;i++)
				{
					array[m]=tuple[i].userID;
					m++;
				}
				m=0;
				for(i=0;i<count;i++)
				{					
					for(j=0;j<i;j++)
					{

						if((strcmp(array[i],array[j]))==0)
							{					
								break;
							}
						
					}	
					 if (i==j)
					  {
						array1[m]=array[i];
						m++;				
					  }
				}
		
		n=0;
		var=buf_size;
		for(i=0;i < num_thread;i++)
		{
			n=0;
			for (j=0;j < count;j++)
			{
				if((strcmp(array1[i],tuple[j].userID))==0)
				{
					sem_wait(&sem[i]);
					if(in[i]==var)
						{					
							sem_post(&Empty[i]);
							
							printf("\n");
							sem_post(&sem[i]);
							sem_wait(&Full[i]);
							sem_wait(&sem[i]);
						
							in[i]=0;
							n=0;
						}
					else if(in[i] < var)
						{
							strcpy(Buffer[i][n].userID,tuple[j].userID);
							strcpy(Buffer[i][n].topic,tuple[j].topic);
							Buffer[i][n].action=tuple[j].action;
							in[i]=in[i]+1;
							cnt[i]=cnt[i]+1;
							n++;	
						}
					sem_post(&sem[i]);
					}
				}
			done[i]=1;
			sem_post(&Empty[i]);
		
			
			}

		
						
				
	for (i=0;i<num_thread;i++)
		{
					switch(fork())
					{


						case 0:
								{	
										int h=0,j,n,k;
										k=0;
										int flag;
										int count1[num_thread];
										struct tuples local_buf[100];
										for(k=0;k<num_thread;k++)
										{
											count1[k]=0;
										}
										
										while(1)
										{
											sem_wait(&sem[i]);
										
											while(in[i]-out[i]==0 && done[i]==0)
											{
												
												sem_post(&sem[i]);
												sem_wait(&Empty[i]);
												sem_wait(&sem[i]);
												
											}
											while(in[i]-out[i]>0)
											{
												
												count1[i]=count1[i]+1;
												
												for(j=0;j<in[i];j++)
												{	

													out[i]++;
													if(j==0 && cnt[i]>buf_size)

													{
														strcpy(local_buf[h].userID,Buffer[i][j].userID);
														strcpy(local_buf[h].topic,Buffer[i][j].topic);
														local_buf[h].action=Buffer[i][j].action;
														h++;
													}
													else
													{
														for(n=0;n<h;n++)
														{
															if(((strcmp(Buffer[i][j].topic,local_buf[n].topic)) == 0))
															{
																local_buf[n].action = local_buf[n].action + Buffer[i][j].action;
																flag = 1;
																break;	
															}
														}
															if (flag != 1)
															{
																strcpy(local_buf[h].userID,Buffer[i][j].userID);
																strcpy(local_buf[h].topic,Buffer[i][j].topic);
																local_buf[h].action=Buffer[i][j].action;
																h++;

															}	
														flag=0;
													}
												}
											}
										if(done[i]==1 && in[i]==out[i])
										{
											for(j=0;j<h;j++)
											{
												printf("(%s,%s,%d)\n",local_buf[j].userID,local_buf[j].topic,local_buf[j].action);
											}
											sem_post(&sem[i]);
											
											break;
										}
										out[i]=0;
										in[i]=0;
										sem_post(&Full[i]);
										sem_post(&sem[i]);
									}
									exit(0);
							}
						default :
								break;

		}
	}
		
		while (wait(NULL) > 0)
  			{
   				 
   				
  			}
		return 0;
	}

int map_value(char ch)
{   int act;
    switch(ch)
        {
            case 'S': act=40;
                        return act;
                        break;
            case 'P': act=50;
                        return act;
                        break;
            case 'L': act=20;
                        return act;
                        break;
            case 'D': act=-10;
                        return act;
                        break;
            case 'C': act=30;
                        return act;
                        break;
		}
}
