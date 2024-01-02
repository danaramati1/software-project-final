# ifndef SPKMEANS_H_
# define SPKMEANS_H_

double ** wam(struct vector * headVec, int vectorsAmount);

double ** ddg(struct vector * headVec, int vectorsAmount);

double ** gl(struct vector * headVec, int vectorsAmount);

double ** buildSymetricMat(struct vector * headVec, int n);

double ** jacobi(double ** a, int n);

void freeMat(double ** mat, int m);
void printMat(double ** mat, int m, int n);


#endif