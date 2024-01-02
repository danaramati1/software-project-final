#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"


void freeClusters(struct vector ** clusters, int k) {
    int i=0;

    for(i=0; i<k; i++) {
        freeVectorsList(clusters[i]);
    }

    free(clusters);
}

double calcDistanceBetweenPoints(struct cord *p1, struct cord *p2) {
    double sum = 0.0;
    struct cord *currCord1 = p1;
    struct cord *currCord2 = p2;

    while (currCord1 != NULL) {
        sum += pow(currCord1->value - currCord2->value, 2);
        currCord1 = currCord1->next;
        currCord2 = currCord2->next;
    }

    return sqrt(sum);
}

int getClosestCentroidIndex(struct vector* headCentroid, struct vector* v) {
    double minDist = 0.0;
    double dist = 0.0;
    int minIndex = 0;
    int index = 0;
    struct vector *currCentroid = headCentroid;

    minDist = calcDistanceBetweenPoints(currCentroid->cords, v->cords);
    currCentroid = currCentroid->next;
    
    while (currCentroid != NULL) {
        index++;
        dist = calcDistanceBetweenPoints(currCentroid->cords, v->cords);
        if (dist < minDist) {
            minDist = dist;
            minIndex = index;
        }

        currCentroid = currCentroid->next;
    }

    return minIndex;
}

void copyCordToNewVector(struct vector *source, struct vector *dest) {
    struct cord *currSource = source->cords;
    struct cord *currDest;

    currDest = malloc(sizeof(struct cord));
    currDest->next = NULL;
    dest->cords = currDest;

    while(currSource->next != NULL) {
        currDest->value = currSource->value;
        currDest->next = malloc(sizeof(struct cord));
        currDest = currDest->next;
        currDest->next = NULL;
        currSource = currSource->next;
    }

    currDest->value = currSource->value;
}

void copyCordsBetweenVectors(struct vector *source, struct vector *dest) {
    struct cord *currSource = source->cords;
    struct cord *currDest = dest->cords;

    while(currSource != NULL) {
        currDest->value = currSource->value;
        currDest = currDest->next;
        currSource = currSource->next;
    }
}

void updateClusters(struct vector *clusters[], 
                    struct vector *headCentroid, 
                    struct vector *headVector) {
    struct vector *currVector, *insertedVector;
    int centroidIndex = 0;

    currVector = headVector;

    while (currVector != NULL) {
        centroidIndex = getClosestCentroidIndex(headCentroid, currVector);
        insertedVector = malloc(sizeof(struct vector));
        copyCordToNewVector(currVector, insertedVector);
        insertedVector->next = clusters[centroidIndex];
        clusters[centroidIndex] = insertedVector;
        currVector = currVector->next;
    }

}

struct vector * calculateNewCentroid(struct vector *headCluster, int vectorLen) {
    int i = 0;
    int vectorCount = 0;
    double sums[vectorLen];
    struct vector *currVector, *newCentroid;
    struct cord *currCord, *newCentroidCord;

    if (sums == NULL) {
        printErrorMessage();
        freeVectorsList(headCluster);
        return NULL;
    }

    currVector = headCluster;
    for(i=0; i < vectorLen; i++) {
        sums[i] = 0.0;
    }

    while (currVector != NULL) {
        i=0;
        currCord = currVector->cords;
        while(currCord != NULL) {
            sums[i] += currCord->value;
            currCord = currCord->next;
            i++;
        }
        vectorCount++;
        currVector = currVector->next;
    }
    
    i=0;

    newCentroid = malloc(sizeof(struct vector));
    if (newCentroid == NULL) {
        printErrorMessage();
        freeVectorsList(headCluster);
        return NULL;
    }

    newCentroid->next = NULL;
    newCentroidCord = malloc(sizeof(struct cord));
    if (newCentroid == NULL) {
        printErrorMessage();
        freeVectorsList(headCluster);
        freeVectorsList(newCentroid);
        return NULL;
    }

    newCentroidCord->next = NULL;
    newCentroid->cords = newCentroidCord;
    newCentroidCord->value = sums[i] / vectorCount;

    for (i=1; i<vectorLen; i++) {
        newCentroidCord->next = malloc(sizeof(struct cord));
        if (newCentroid == NULL) {
            printErrorMessage();
            freeVectorsList(headCluster);
            freeVectorsList(newCentroid);
            return NULL;
        }

        newCentroidCord = newCentroidCord->next;
        newCentroidCord->next = NULL;
        newCentroidCord->value = sums[i] / vectorCount;
    }

    return newCentroid;
}

struct vector ** initializeClustersArray(int k) {
    int i=0;
    struct vector ** clusters = malloc(k*sizeof(struct vector*));
    if (clusters == NULL) {
        printErrorMessage();
        return NULL;
    }

    for(i=0; i<k; i++) {
        clusters[i] = NULL;
    }

    return clusters;
}

struct vector* extractVectorsListFromArray(int pointsAmount, int vectorLength, double points[pointsAmount][vectorLength]) {
    int i, j;
    struct vector *headVec, *currVec;
    struct cord *headCord, *currCord;

    headVec = malloc(sizeof(struct vector));
    if (headVec == NULL) {
        printErrorMessage();
        return NULL;
    }

    currVec = headVec;
    currVec->next = NULL;

    for (i=0; i < pointsAmount; i++) {
        headCord = malloc(sizeof(struct cord));
        if (headCord == NULL) {
            printErrorMessage();
            return NULL;
        }

        currCord = headCord;
        currCord->next = NULL;
        currVec->cords = headCord;

        for (j=0; j < vectorLength; j++) {
            currCord->value = points[i][j];
            if (j == vectorLength - 1) {
                currCord->next = NULL;
            } else {
                currCord->next = malloc(sizeof(struct cord));
                if (currCord->next == NULL) {
                    printErrorMessage();
                    freeVectorsList(headVec);
                    return NULL;
                }
            }

            currCord = currCord->next;
        }

        if (i == pointsAmount - 1) {
            currVec->next = NULL;
        } else {
            currVec->next = malloc(sizeof(struct vector));
            if (currVec->next == NULL) {
                printErrorMessage();
                freeVectorsList(headVec);
                return NULL;
            }
        }

        currVec = currVec->next;
    }

    return headVec;
}

struct vector * kmeans(int k, int maxIter, double epsilon, int vectorLength, int pointsAmount, 
                       double points[pointsAmount][vectorLength], double centroids[k][vectorLength]) {
    int i = 0;
    int firstIter = 1;
    int iterCount = 0;
    double delta = 0.0;
    double maxDelta = 0.0;
    struct vector *currOldCentroid, *currNewCentroid, *headCentroid, *headVector;
    struct vector ** clusters = NULL;

    headCentroid = extractVectorsListFromArray(k, vectorLength, centroids);
    headVector = extractVectorsListFromArray(pointsAmount, vectorLength, points);

    while (iterCount <= maxIter && (firstIter || maxDelta > epsilon)) {
        if (!firstIter) {
            freeClusters(clusters, k);
        }

        firstIter = 0;
        clusters = initializeClustersArray(k);

        if (clusters == NULL) {
            freeVectorsList(headVector);
            freeVectorsList(headCentroid);
            return NULL;
        }

        currOldCentroid = headCentroid;
        updateClusters(clusters, headCentroid, headVector);
        
        while (currOldCentroid != NULL) {
            currNewCentroid = calculateNewCentroid(clusters[i], vectorLength);
            if (currNewCentroid == NULL) {
                freeVectorsList(headVector);
                freeVectorsList(headCentroid);
                freeClusters(clusters, k);
                return NULL;
            }

            delta = calcDistanceBetweenPoints(currNewCentroid->cords, currOldCentroid->cords);
            
            if (delta > maxDelta) {
                maxDelta = delta;
            }

            copyCordsBetweenVectors(currNewCentroid, currOldCentroid);
            freeVectorsList(currNewCentroid);
            currOldCentroid = currOldCentroid->next;
            i++;
        }

        iterCount++;
        i=0;
    }
    
    freeClusters(clusters, k);
    freeVectorsList(headVector);

    return headCentroid;
}
