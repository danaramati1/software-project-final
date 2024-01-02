#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "utils.h"

#define MAX_ITER 100

struct vector *extractVectors(char* file_name) {
    struct vector *headVec, *currVec, *prevVec = NULL;
    FILE *fp;
    struct cord *headCord, *currCord;
    double n;
    char c;

    fp = fopen(file_name, "r+");

    headCord = malloc(sizeof(struct cord));
    if (headCord == NULL) {
        printErrorMessage();
        return NULL;
    }

    currCord = headCord;
    currCord->next = NULL;

    headVec = malloc(sizeof(struct vector));
    if (headVec == NULL) {
        printErrorMessage();
        free(headCord);
        return NULL;
    }

    currVec = headVec;
    currVec->next = NULL;


    while (fscanf(fp, "%lf%c", &n, &c) == 2) {

        if (c == '\n')
        {
            currCord->value = n;
            currVec->cords = headCord;
            currVec->next = malloc(sizeof(struct vector));
            if (currVec->next == NULL) {
                printErrorMessage();
                freeVectorsList(headVec);
                return NULL;
            }

            prevVec = currVec;
            currVec = currVec->next;
            currVec->next = NULL;
            currVec->cords = NULL;
            headCord = malloc(sizeof(struct cord));
            if (headCord== NULL) {
                printErrorMessage();
                freeVectorsList(headVec);
                return NULL;
            }

            currCord = headCord;
            currCord->next = NULL;
            continue;
        }

        currCord->value = n;
        currCord->next = malloc(sizeof(struct cord));
        if (currCord->next == NULL) {
            printErrorMessage();
            freeVectorsList(headVec);
            return NULL;
        }

        currCord = currCord->next;
        currCord->next = NULL;
        currCord->value = 0.0;
    }

    free(currCord);
    freeVectorsList(currVec);
    fclose(fp);
    prevVec->next = NULL;
    return headVec;
}

int extractVectorAmount(struct vector* vec) {
    int count = 0;
    struct vector *currVector;

    currVector = vec;
    while (currVector->next != NULL) {
        count++;
        currVector = currVector->next;
    }

    return ++count;
}

void printMat(double ** mat, int m, int n){
    int i,j;
    char sep;

    for(i=0; i<m; i++) {
        for(j=0; j<n; j++) {
            sep = j == n-1 ? '\n' : ',';
            printf("%.4f%c", mat[i][j], sep);
        }
        
    }
}

void freeMat(double ** mat, int m) {
    int i;
    for(i=0; i<m; i++) {
       free(mat[i]);
    }
    free(mat);
}

double calcWeightBetweenPoints(struct cord *p1, struct cord *p2) {
    double sum = 0.0;
    struct cord *currCord1 = p1;
    struct cord *currCord2 = p2;

    while (currCord1 != NULL) {
        sum += pow(currCord1->value - currCord2->value, 2);
        currCord1 = currCord1->next;
        currCord2 = currCord2->next;
    }

    return exp(-sum/2);
}

double ** wam(struct vector * headVec, int vectorsAmount) {
    double ** wMat;
    int i,j;
    struct vector *curr1, *curr2;

    wMat = malloc(vectorsAmount * sizeof(double*));
    if (wMat == NULL) {
        printErrorMessage();
        return NULL;
    }

    curr1 = headVec;

    for (i=0; i < vectorsAmount; i++) {
        wMat[i] = malloc(vectorsAmount * sizeof(double));
        if (wMat[i] == NULL) {
            printErrorMessage();
            return NULL;
        }
        
        curr2 = headVec;
        for (j = 0; j < vectorsAmount; j++) {
            wMat[i][j] = i == j ? 0 : calcWeightBetweenPoints(curr1->cords, curr2->cords);
            curr2 = curr2->next;
        }

        curr1 = curr1->next;
    }

    return wMat;
}

double arraySum(double * arr, int len) {
    double result = 0;
    int i;

    for(i=0; i < len; i++){
        result += arr[i];
    }

    return result;
}

double ** ddg(struct vector * headVec, int vectorsAmount){
    double ** wMat, ** dMat;
    double rowSum;
    int i, j;

    wMat = wam(headVec, vectorsAmount);

    dMat = malloc(vectorsAmount * sizeof(double*));
    if (dMat == NULL) {
        printErrorMessage();
        return NULL;
    }

    for (i=0; i < vectorsAmount; i++) {
        dMat[i] = malloc(vectorsAmount * sizeof(double));
        rowSum = arraySum(wMat[i], vectorsAmount);
        if (dMat[i] == NULL) {
            printErrorMessage();
            return NULL;
        }
        
        for (j = 0; j < vectorsAmount; j++) {
            dMat[i][j] = i == j ? rowSum : 0;
        }
    }

    freeMat(wMat, vectorsAmount);

    return dMat;

}

double ** gl(struct vector * headVec, int vectorsAmount) {
    double ** wMat, ** dMat, ** glMat;
    int i, j;

    wMat = wam(headVec, vectorsAmount);
    dMat = ddg(headVec, vectorsAmount);

    glMat = malloc(vectorsAmount * sizeof(double*));
    if (glMat == NULL) {
        printErrorMessage();
        return NULL;
    }

    for (i=0; i < vectorsAmount; i++) {
        glMat[i] = malloc(vectorsAmount * sizeof(double));
        if (glMat[i] == NULL) {
            printErrorMessage();
            return NULL;
        }
        
        for (j = 0; j < vectorsAmount; j++) {
            glMat[i][j] = dMat[i][j] - wMat[i][j];
        }
    }

    freeMat(wMat, vectorsAmount);
    freeMat(dMat, vectorsAmount);

    return glMat;

}

void findPivot(double ** mat, int n, int * pivotIndexes) {
    int i, j;
    double max = -1;

    for (i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            if (fabs(mat[i][j]) > max && i != j) {
                max = fabs(mat[i][j]);
                pivotIndexes[0] = i;
                pivotIndexes[1] = j;
            }
        }
    }
}

double calcTheta(double **mat, int * pivotIndexes) {
    int i, j;

    i = pivotIndexes[0];
    j = pivotIndexes[1];

    return (mat[j][j] - mat[i][i]) / (2 * mat[i][j]);
}

void calcParametes(double **mat, int *pivotIndexes, double *params) {
    int sign;
    double t,c,s,theta;

    theta = calcTheta(mat, pivotIndexes);

    sign = theta >= 0 ? 1 : -1;
    t = sign / (fabs(theta) + sqrt(pow(theta,2)+1));
    c = 1 / (sqrt(pow(t,2)+1));
    s = t*c;

    params[0] = c;
    params[1] = s;
}

double ** buildPivotMat(double ** mat, int n) {
    double ** pivotMat;
    double params[2];
    int pivotIndexes[2];
    int i, j;
    double c,s;

    findPivot(mat, n, pivotIndexes);
    calcParametes(mat, pivotIndexes, params);

    c = params[0];
    s = params[1];

    pivotMat = malloc(n * sizeof(double*));
    if (pivotMat == NULL) {
        printErrorMessage();
        return NULL;
    }

    for (i=0; i < n; i++) {
        pivotMat[i] = malloc(n * sizeof(double));
        if (pivotMat[i] == NULL) {
            printErrorMessage();
            return NULL;
        }
        
        for (j = 0; j < n; j++) {

            if (i == j) {
                if (i == pivotIndexes[0] || i == pivotIndexes[1]) {
                    pivotMat[i][j] = c;
                } else {
                    pivotMat[i][j] = 1;
                }
            } else {
                if (i == pivotIndexes[0] && j == pivotIndexes[1]) {
                    pivotMat[i][j] = s;
                } else if (i == pivotIndexes[1] && j == pivotIndexes[0]) {
                    pivotMat[i][j] = -s;
                } else {
                    pivotMat[i][j] = 0;
                }
            } 
        }
    }

    return pivotMat;

}

double calcOff(double ** mat, int n) {
    int i, j;
    double sum = 0;

    for (i=0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i != j) {
                sum += pow(mat[i][j], 2);
            }
        }
    }

    return sum;
}

double ** transposeMat(double ** mat, int n) {
    int i, j;

    double ** transposed = malloc(n * sizeof(double*));
    if (transposed == NULL) {
        printErrorMessage();
        return NULL;
    }

    for (i = 0; i < n; i++) {
        transposed[i] = malloc(n * sizeof(double));
        if (transposed[i] == NULL) {
            printErrorMessage();
            return NULL;
        }

        for (j = 0; j < n; j++) {
            transposed[i][j] = mat[j][i];
        }
    }

    return transposed;
}

double ** matrixMultiply(double ** mat1, double ** mat2, int n) {
    int i, j, k;
    double ** result = malloc(n * sizeof(double *));
    if (result == NULL) {
        printErrorMessage();
        return NULL;
    }

    for (i = 0; i < n; i++) {
        result[i] = malloc(n * sizeof(double));
        if (result[i] == NULL) {
            printErrorMessage();
            return NULL;
        }

        for (j = 0; j < n; j++) {
            result[i][j] = 0;
            for (k = 0; k < n; k++) {
                result[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
    return result;
}

double ** makePivot(double ** p, double ** a, int n) {
    double **pT, **aTag, **temp;
    pT = transposeMat(p, n);
    temp = matrixMultiply(pT, a, n);
    aTag = matrixMultiply(temp, p, n);

    freeMat(pT, n);
    freeMat(temp, n);

    return aTag;
}

double ** calcEigenVectors(double *** pivotArray, int stepCount, int n) {
    int i;
    double **result = pivotArray[0];
    double ** resultArray[MAX_ITER] = {NULL};

    for (i=1; i < stepCount; i++) {
        resultArray[i-1] = result;
        result = matrixMultiply(result, pivotArray[i], n);
    }

    for (i=0; i < stepCount; i++) {
        freeMat(pivotArray[i], n);
    }

    for (i=1; i < stepCount - 1; i++) {
        freeMat(resultArray[i], n);
    }

    return result;
}

double ** buildSymetricMat(struct vector * headVec, int n) {
    double ** mat;
    int i,j;
    struct vector *currVec;
    struct cord *currCord;

    mat = malloc(n * sizeof(double*));
    if (mat == NULL) {
        printErrorMessage();
        return NULL;
    }

    currVec = headVec;

    for (i=0; i < n; i++) {
        currCord = currVec->cords;
        mat[i] = malloc(n * sizeof(double));
        if (mat[i] == NULL) {
            printErrorMessage();
            return NULL;
        }

        for (j=0; j < n; j++) {
            mat[i][j] = currCord->value;
            currCord = currCord->next;
        }

        currVec = currVec->next;
    }

    return mat;

}

int isMinusZero(double number) {
    int i=0;

    if (number >= 0 ) {
        return 1;
    }

    for (i=0; i<5; i++) {
        if ((int) number != 0) {
            return 1;
        
        } 

        number = number * 10;
    }

    return 0;
}

double ** jacobi(double ** a, int n) {
    double **aTag, **p, **jMat, **eigenVectors;
    double ** pivotArray[MAX_ITER];
    int i, j, stepCount = 0;
    double epsilon = 1.0 * pow(10, -5); 
    double off = epsilon * 2;

    while (stepCount < MAX_ITER && off > epsilon) {
        p = buildPivotMat(a, n);
        pivotArray[stepCount] = p;
        aTag = makePivot(p, a, n);

        off = calcOff(a, n) - calcOff(aTag, n);
        freeMat(a, n);
        a = aTag;
        stepCount++;
    }

    eigenVectors = calcEigenVectors(pivotArray, stepCount, n);

    /* +1 beuacse of eigen values */
    jMat = malloc((n + 1) * sizeof(double*));
    if (jMat == NULL) {
        printErrorMessage();
        return NULL;
    }

    jMat[0] = malloc(n * sizeof(double));
    if (jMat[0] == NULL) {
        printErrorMessage();
        return NULL;
    }
    
    for (i=0; i < n; i++) {
        jMat[0][i] = a[i][i];
    }

    for (i=1; i <= n; i++) {
        jMat[i] = malloc(n * sizeof(double));
        if (jMat[i] == NULL) {
            printErrorMessage();
            return NULL;
        }

        for(j=0; j < n; j++) {
            jMat[i][j] = (isMinusZero(jMat[0][i-1]) == 1) ? 
                          eigenVectors[i-1][j] : -eigenVectors[i-1][j];
        }

        jMat[0][i-1] = (isMinusZero(jMat[0][i-1]) == 1) ? jMat[0][i-1] : -jMat[0][i-1];
    }

    freeMat(eigenVectors, n);
    freeMat(a, n);

    return jMat;

}

int main(int argc, char *argv[]) {
    struct vector *headVector;
    double **wMat, **dMat, **glMat, **jMat, **symetricMat;
    char *goal, *fileName;
    int vectorsAmount;

    if (argc != 3) {
        printErrorMessage();
        return 1;
    }

    goal = argv[1];
    fileName = argv[2];

    headVector = extractVectors(fileName);
    if (headVector == NULL) {
        printErrorMessage();
        return 1;
    }

    vectorsAmount = extractVectorAmount(headVector);

    if (strcmp(goal, "wam") == 0) {
        wMat = wam(headVector, vectorsAmount);
        printMat(wMat, vectorsAmount, vectorsAmount);
        freeMat(wMat, vectorsAmount);
    }

    if (strcmp(goal, "ddg") == 0) {
        dMat = ddg(headVector, vectorsAmount);
        printMat(dMat, vectorsAmount, vectorsAmount);
        freeMat(dMat, vectorsAmount);
    }

    if (strcmp(goal, "gl") == 0) {
        glMat = gl(headVector, vectorsAmount);
        printMat(glMat, vectorsAmount, vectorsAmount);
        freeMat(glMat, vectorsAmount);
    }

    if (strcmp(goal, "jacobi") == 0) {
        symetricMat = buildSymetricMat(headVector, vectorsAmount);
        jMat = jacobi(symetricMat, vectorsAmount);
        printMat(jMat, vectorsAmount + 1, vectorsAmount);
        freeMat(jMat, vectorsAmount + 1);
    }

    freeVectorsList(headVector);

    return 0;

}