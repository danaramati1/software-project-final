# ifndef KMEANS_H_
# define KMEANS_H_

struct vector * kmeans(int k, int maxIter, double epsilon, int vectorLength, int pointsAmount, 
                       double points[pointsAmount][vectorLength], double centroids[k][vectorLength]);

struct vector* extractVectorsListFromArray(int pointsAmount, int vectorLength, double points[pointsAmount][vectorLength]);

#endif