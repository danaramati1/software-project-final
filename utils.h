# ifndef UTILS_H_
# define UTILS_H_

struct cord {
    double value;
    struct cord *next;
};

struct vector {
    struct vector *next;
    struct cord *cords;
};

void printErrorMessage();

void freeVectorCords(struct vector *v);

void freeVectorsList(struct vector *headVector);

#endif