#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

struct tuples
{
    int userID;
    char action;
    char topic[15];
};

int main()
{
    int i = 0, j = 0, count = 0;
    char c;
    char full[3000],token[15];

    char *strpointer;

    FILE *fpointer;
    FILE *fout;

    fpointer    = fopen("input.txt","r+");
    fout        = fopen("mapper_out.txt","w+");

    if (fpointer == NULL) {
        printf("Could not open the file");
        return 1;
    }

    if (fout == NULL) {
        printf("Could not open the file");
        return 1;
    }

    while ((c = getc(fpointer)) != EOF) {

        if (c == ')') {
            count++;
        }

    }

    printf("The count is: %d\n",count);
    fseek(fpointer,0,SEEK_SET);
    struct tuples tuple[count];

    int userID_array[count];
    char action_array[count];
    char *topicptr[15];

    fgets(full,3000,fpointer);
    printf("The input got from the file is: %s\n",full);

    strpointer = strtok(&full[1],",");

    while (strpointer != NULL) {

        tuple[i].userID = atoi(strpointer);
        userID_array[j] = tuple[i].userID;
        printf("UserID: %.4d\t",tuple[i].userID);

        strpointer=strtok(NULL,",");
        tuple[i].action = *strpointer;
        action_array[j] = tuple[i].action;
        printf("action: %c\t",tuple[i].action);

        strpointer = strtok(NULL,")");
        strcpy(tuple[i].topic,strpointer);
        printf("topic: %s\n",tuple[i].topic);
        //topicptr[j] = tuple[i].topic;

        strpointer = strtok(NULL,"(");
        if (strpointer) {                          /* At last the strpointer will already have become null value. Should not try to split it again */
            strpointer = strtok(NULL, ",");
        }

        i++;
        j++;

    }

    for (j=0; j<=count; j++) {

        if (action_array[j] == 'P') {
            action_array[j] = 50;
            tuple[j].action = action_array[j];
        }

        else if (action_array[j] == 'S') {
            action_array[j] = 40;
            tuple[j].action = action_array[j];
        }

        else if (action_array[j] == 'L') {
            action_array[j] = 20;
            tuple[j].action = action_array[j];
        }

        else if (action_array[j] == 'C') {
            action_array[j] = 30;
            tuple[j].action = action_array[j];
        }

        else if (action_array[j] == 'D') {
            action_array[j] = -10;
            tuple[j].action = action_array[j];
        }

    }

    // for (j=0; j<count; j++) {
    //   tuple[j].userID = userID_array[j];
    // }

    for (j=0; j<count; j++) {

        printf("(%.4d,%.15s,%d)\n",tuple[j].userID,tuple[j].topic,tuple[j].action);
        fprintf(fout,"(%.4d,%.15s,%d)\n",tuple[j].userID,tuple[j].topic,tuple[j].action);

    }

    fclose(fout);
    fclose(fpointer);
    getch();
    return 0;
}
