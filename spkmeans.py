import math
import numpy as np
import pandas as pd
import sys
import mykmeanssp as spkmeans

def calc_dist_two_points(p1, p2):
    return math.sqrt(sum([(p1[i] - p2[i]) ** 2 for i in range(len(p1))]))

def find_D(point, centroids):
    minimal_distance = math.inf
    for center in centroids:
        distance = calc_dist_two_points(point, center)
        if distance < minimal_distance:
            minimal_distance = distance

    return minimal_distance


def calc_next_centroid(data_points, centroids):
    d_values = [find_D(p, centroids) for p in data_points.values.tolist()]
    d_sum = sum(d_values)
    prob = [d_value / d_sum for d_value in d_values]

    index = int(np.random.choice(data_points.index.tolist(), p=prob))

    return index, data_points.iloc[index].tolist()


def initialize_centroids(k, data_points):
    centroids = []
    indexes = []
    np.random.seed(0)
    data_points = pd.DataFrame(data_points)

    index = int(np.random.choice(data_points.index.tolist()))

    first_centroid = data_points.iloc[index].tolist()
    indexes.append(index)
    centroids.append(first_centroid)

    while len(centroids) < k:
        index, next_centroid = calc_next_centroid(data_points, centroids)
        indexes.append(index)
        centroids.append(next_centroid)

    return indexes, centroids


def get_data_points(file_path):
    return pd.read_csv(file_path, header=None).values.tolist()

def print_matrix(mat):
    for row in mat:
        new_row = ['%.4f' % value for value in row]
        print(*new_row, sep=',')


def print_index_list(indexes):
    print(','.join([str(i) for i in indexes]))


def calc_k(eigen_vals):
    eigen_vals.sort()
    max_gap = 0
    k = 0
    for i in range(1, math.floor(len(eigen_vals)/2)):
        gap = abs(eigen_vals[i] - eigen_vals[i+1])
        if max_gap < gap:
            max_gap = gap
            k = i

    return k+1

def extract_args():
    goal = sys.argv[2] if len(sys.argv) == 4 else sys.argv[1]
    file_name = sys.argv[3] if len(sys.argv) == 4 else sys.argv[2]

    return goal, file_name


def main():
    try:
        goal, file_name = extract_args()
        data_points = get_data_points(file_name)

        if (goal == "spk"):
            GL_matrix = spkmeans.gl([data_points])

            # calc Jacobi mat of L
            eigen_bundle = spkmeans.jacobi([GL_matrix])
            df = pd.DataFrame(eigen_bundle)

            # get values row
            first_row = df.ix[df.first_valid_index()]

            # sort vectors by eigen values
            eigen_vectors = df[first_row.argsort()].values.tolist()[1:]

            # calc K
            k = sys.argv[1] if len(sys.argv) == 4 else calc_k(eigen_bundle[0])
            k = int(k)

            # get data points (rows of U)
            U_rows = [v[:k] for v in eigen_vectors]

            # kmeans++ on U rows
            indexes, centroids = initialize_centroids(k, U_rows)
            centroids = spkmeans.spk([k, centroids, U_rows])

            print_index_list(indexes)
            print_matrix(centroids)

        if (goal == "wam"):
            wMat = spkmeans.wam([data_points])
            print_matrix(wMat)

        if (goal == "ddg"):
            ddgMat = spkmeans.ddg([data_points])
            print_matrix(ddgMat)

        if (goal == "gl"):
            glMat = spkmeans.gl([data_points])
            print_matrix(glMat)

        if (goal == "jacobi"):
            jMat = spkmeans.jacobi([data_points])
            print_matrix(jMat)

    except Exception as ex:
        print(ex)
        print('An Error Has Occurred')


if __name__ == '__main__':
    main()
