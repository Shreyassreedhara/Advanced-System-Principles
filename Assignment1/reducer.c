#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

char c, *input, *token, *temp_userID[5], *temp_topic[15];
int act_sum, j = 0, tr, temp_action, length = 0;


struct tuples
{
    int userID[5];
    char topic[15];
    int action;
};

int insert(struct tuples* tuple, int length, char *temp_userID, char *temp_topic, int temp_action);

int main()
{
    struct tuples tuple[100];
    FILE *input_file;
    input_file = fopen("mapper_out.txt","r+");

    input = (char*) malloc (2000);
    int i = 0;
    int ch;
    int g = 0;

    if (input_file == NULL) {
        printf("Could not open the file");
        return 1;
    }

    while (fgets(input, 2000, input_file)) {

        tr = tr + 1;

        token = strtok(&input[1], ",");
        strcpy(temp_userID, token);

        token = strtok(NULL, ",");
        strcpy(temp_topic, token);

        token       = atoi(strtok(NULL,")"));
        temp_action = token;

        length = insert(tuple, length, temp_userID, temp_topic, temp_action);
        i++;

    }

    for (g=0; g<length; g++) {
        printf("(%s,%s,%d)\n",tuple[g].userID,tuple[g].topic,tuple[g].action);
    }

    fclose(input_file);
    return 0;
}

int insert (struct tuples *tuple, int length, char *temp_userID, char *temp_topic,int temp_action)
{
    int i = 0;

    if (length == 0)
    {
        strcpy(tuple[length].userID, temp_userID);
        strcpy(tuple[length].topic, temp_topic);
        tuple[length].action = temp_action;
        length = length + 1;
        return length;
    }
    else
    {
        if (strcmp(tuple[length-1].userID,temp_userID) != 0) {
            for(i=0; i<length; i++)
            {
                printf("(%s,%s,%d)\n",tuple[i].userID,tuple[i].topic,tuple[i].action);
            }
            length = 0;
            strcpy(tuple[length].userID,temp_userID);
            strcpy(tuple[length].topic,temp_topic);
            tuple[length].action = temp_action;
            length = length + 1;
            return length;
        }
        else
        {
            for (i=0; i<length; i++) {
                if (strcmp(tuple[i].topic, temp_topic) == 0) {
                    tuple[i].action = tuple[i].action + temp_action;
                    return length;
                }
            }
            strcpy(tuple[length].userID, temp_userID);
            strcpy(tuple[length].topic, temp_topic);
            tuple[length].action = temp_action;
            length = length + 1;
            return length;
        }
    }
}
