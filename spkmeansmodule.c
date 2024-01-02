#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "utils.h"
#include "kmeans.h"
#include "spkmeans.h"

#define SPK_DOC_STRING "Runs the k-means clustering algorithm on the given data points using the provided initial centroids.\n\n"\
                       "The algorithm will run for a maximum of max_iter iterations or until the centroids stop moving more than epsilon distance.\n\n"\
                       "Parameters:\n"\
                       "\tk (int): The number of clusters to create.\n"\
                       "\tcentroids (list): A list of initialized centroids\n"\
                       "\tdata (list): A list of data points to cluster.\n\n"\
                       "Returns:\n"\
                       "\tA list of the calculated centroids for the clusters.\n"

#define WAM_DOC_STRING "Runs the wam algorithm on the given data points.\n"

#define DDG_DOC_STRING "Runs the ddg algorithm on the given data points.\n"

#define GL_DOC_STRING "Runs the gl algorithm on the given data points.\n"

#define JACOBI_DOC_STRING "Runs the jacobi algorithm on the given data points.\n"

int getK(PyObject *lst) {
    PyObject *item;

    item = PyList_GetItem(lst, 0);
    return (int) PyLong_AsLong(item);
}

void extractVectorsArray(int numOfVectors, int vectorLength, double vectorsArray[numOfVectors][vectorLength], PyObject *vectors) {
    int i, j;
    double cord;
    PyObject *item, *vector;

    for (i = 0; i < numOfVectors; i++) {
        vector = PyList_GetItem(vectors, i);

        for (j = 0; j < vectorLength; j++) {
            item = PyList_GetItem(vector, j);
            cord = PyFloat_AsDouble(item);
            vectorsArray[i][j] = cord;
        }
    }
}

static PyObject* cKmeans(PyObject *self, PyObject *args) {
    PyObject *lst, *centroid, *centroids, *points, *centroidsList, *centroidCords, *listCord;
    int n, k, maxIter, vectorLength, numOfPoints, i=0, j=0;
    double epsilon;
    struct vector *headCentroid, *currCentroid;
    struct cord *currCord;

    if(!PyArg_ParseTuple(args, "O", &lst)) {
        printErrorMessage();
        return NULL;
    }

    n = PyObject_Length(lst);
    if (n < 0) {
        printErrorMessage();
        return NULL;
    }

    k = getK(lst);
    maxIter = 300;
    epsilon = 0;

    centroids = PyList_GetItem(lst, 1);
    centroid = PyList_GetItem(centroids, 0);
    vectorLength = PyObject_Length(centroid);

    double centroidsArray[k][vectorLength];
    extractVectorsArray(k, vectorLength, centroidsArray, centroids);

    points = PyList_GetItem(lst, 2);
    numOfPoints = PyObject_Length(points);

    double pointsArray[numOfPoints][vectorLength];
    extractVectorsArray(numOfPoints, vectorLength, pointsArray, points);

    headCentroid = kmeans(k, maxIter, epsilon, vectorLength, numOfPoints, pointsArray, centroidsArray);

    centroidsList = PyList_New(k);
    currCentroid = headCentroid;

    while(currCentroid != NULL) {
        j=0;
        centroidCords = PyList_New(vectorLength);
        currCord = currCentroid->cords;
        PyList_SetItem(centroidsList, i, centroidCords);

        while (currCord != NULL) {
            listCord = Py_BuildValue("d", currCord->value);
            PyList_SetItem(centroidCords, j, listCord);
            currCord = currCord->next;
            j++;
        }

        currCentroid = currCentroid->next;
        i++;
    }

    freeVectorsList(headCentroid);
    
    return centroidsList;
}


static struct vector* getVectorFromPyObject(PyObject *lst, int lstIndex) {
    PyObject *points, *point;
    int numOfPoints, vectorLength;

    points = PyList_GetItem(lst, lstIndex);
    numOfPoints = PyObject_Length(points);

    point = PyList_GetItem(points, 0);
    vectorLength = PyObject_Length(point);

    double pointsArray[numOfPoints][vectorLength];

    extractVectorsArray(numOfPoints, vectorLength, pointsArray, points);

    return extractVectorsListFromArray(numOfPoints, vectorLength, pointsArray);
}


static PyObject * cWam(PyObject *self, PyObject *args) {
    PyObject *lst, *points, *wMatPython, *row, *listVal;
    double **wMat;
    struct vector *headVector;
    int n, numOfPoints, i, j;

    if(!PyArg_ParseTuple(args, "O", &lst)) {
        printErrorMessage();
        return NULL;
    }

    n = PyObject_Length(lst);

    if (n < 0) {
        printErrorMessage();
        return NULL;
    }

    points = PyList_GetItem(lst, 0);
    numOfPoints = PyObject_Length(points);

    headVector = getVectorFromPyObject(lst, 0);
    wMat = wam(headVector, numOfPoints);
    wMatPython = PyList_New(numOfPoints);

    for (i=0; i < numOfPoints; i++){
        row = PyList_New(numOfPoints);
        PyList_SetItem(wMatPython, i, row);

        for (j=0; j < numOfPoints; j++){
            listVal = Py_BuildValue("d", wMat[i][j]);
            PyList_SetItem(row, j, listVal);
        }
    }

    freeMat(wMat, numOfPoints);

    return wMatPython;
}

static PyObject * cDdg(PyObject *self, PyObject *args) {
    PyObject *lst, *points, *dMatPython, *row, *listVal;
    double **dMat;
    struct vector *headVector;
    int n, numOfPoints, i, j;

    if(!PyArg_ParseTuple(args, "O", &lst)) {
        printErrorMessage();
        return NULL;
    }

    n = PyObject_Length(lst);

    if (n < 0) {
        printErrorMessage();
        return NULL;
    }

    points = PyList_GetItem(lst, 0);
    numOfPoints = PyObject_Length(points);

    headVector = getVectorFromPyObject(lst, 0);
    dMat = ddg(headVector, numOfPoints);

    dMatPython = PyList_New(numOfPoints);

    for (i=0; i < numOfPoints; i++){
        row = PyList_New(numOfPoints);
        PyList_SetItem(dMatPython, i, row);

        for (j=0; j < numOfPoints; j++){
            listVal = Py_BuildValue("d", dMat[i][j]);
            PyList_SetItem(row, j, listVal);
        }
    }

    freeMat(dMat, numOfPoints);

    return dMatPython;
}

static PyObject * cGl(PyObject *self, PyObject *args) {
    PyObject *lst, *points, *gMatPython, *row, *listVal;
    double ** gMat;
    struct vector *headVector;
    int n, numOfPoints, i, j;

    if(!PyArg_ParseTuple(args, "O", &lst)) {
        printErrorMessage();
        return NULL;
    }

    n = PyObject_Length(lst);

    if (n < 0) {
        printErrorMessage();
        return NULL;
    }

    points = PyList_GetItem(lst, 0);
    numOfPoints = PyObject_Length(points);

    headVector = getVectorFromPyObject(lst, 0);
    gMat = gl(headVector, numOfPoints);

    gMatPython = PyList_New(numOfPoints);

    for (i=0; i < numOfPoints; i++){
        row = PyList_New(numOfPoints);
        PyList_SetItem(gMatPython, i, row);

        for (j=0; j < numOfPoints; j++){
            listVal = Py_BuildValue("d", gMat[i][j]);
            PyList_SetItem(row, j, listVal);
        }
    }

    freeMat(gMat, numOfPoints);

    return gMatPython;
}

static PyObject * cJacobi(PyObject *self, PyObject *args) {
    PyObject *lst, *points, *jMatPython, *row, *listVal;
    double **jMat, **symMat;
    struct vector *headVector;
    int n, numOfPoints, i, j;

    if(!PyArg_ParseTuple(args, "O", &lst)) {
        printErrorMessage();
        return NULL;
    }

    n = PyObject_Length(lst);

    if (n < 0) {
        printErrorMessage();
        return NULL;
    }

    points = PyList_GetItem(lst, 0);
    numOfPoints = PyObject_Length(points);

    headVector = getVectorFromPyObject(lst, 0);
    symMat = buildSymetricMat(headVector, numOfPoints);
    jMat = jacobi(symMat, numOfPoints);
    
    jMatPython = PyList_New(numOfPoints+1);

    for (i=0; i <= numOfPoints; i++){
        row = PyList_New(numOfPoints);
        PyList_SetItem(jMatPython, i, row);

        for (j=0; j < numOfPoints; j++){
            listVal = Py_BuildValue("d", jMat[i][j]);
            PyList_SetItem(row, j, listVal);
        }
    }

    freeMat(jMat, numOfPoints+1);

    return jMatPython;
}

static PyMethodDef cKmeans_FunctionsTable[] = {
    {
        "spk", 
        cKmeans,
        METH_VARARGS,
        SPK_DOC_STRING
    } , {
        "wam", 
        cWam,
        METH_VARARGS,
        WAM_DOC_STRING
    } , {
        "ddg", 
        cDdg,
        METH_VARARGS,
        DDG_DOC_STRING
    } , {
        "gl", 
        cGl,
        METH_VARARGS,
        GL_DOC_STRING
    } , {
        "jacobi", 
        cJacobi,
        METH_VARARGS,
        JACOBI_DOC_STRING
    } , {
        NULL, NULL, 0, NULL
    }
};

static struct PyModuleDef cKmeans_Module = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",
    "Python wrapper for custom C kmeans algorithm implemntation",
    -1,
    cKmeans_FunctionsTable
};

PyMODINIT_FUNC PyInit_mykmeanssp(void) {
    return PyModule_Create(&cKmeans_Module);
}