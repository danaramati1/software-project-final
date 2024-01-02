#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void printErrorMessage() {
    printf("An Error Has Occurred");
}

void freeVectorCords(struct vector *v) {
    struct cord *currCord, *nextCord;
    currCord = v->cords;
    nextCord = NULL;
    
    if (currCord != NULL) { 
        nextCord = currCord->next;
    }

    while(nextCord != NULL) {
        free(currCord);
        currCord = nextCord;
        nextCord = currCord->next;
    }

    if (currCord != NULL) { 
        free(currCord);
    }
}

void freeVectorsList(struct vector *headVector) {
    struct vector *currVector, *nextVector;
    currVector = headVector;
    nextVector = currVector->next;

    while(nextVector != NULL) {
        freeVectorCords(currVector);
        free(currVector);
        currVector = nextVector;
        nextVector = currVector->next;
    }

    freeVectorCords(currVector);
    free(currVector);
}
