#include <vector>

using namespace std;

const int sides[4][2] = {
    {-1, 0},
    {0, -1},
    {1, 0},
    {0, 1}
};

template <typename T>
T** new_matrix(int n, int m) {
    T** matrix = new T*[n];
    for (int i = 0; i < n; ++i) {
        matrix[i] = new T[m];
    }
    return matrix;
}

template <typename T>
void delete_matrix(int n, T** matrix) {
    for (int i = 0; i < n; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

template <typename T>
void memset_array(int n, T* array, const T& val) {
    fill(array, array + n, val);
}

template <typename T>
void memset_matrix(int n, int m, T** matrix, const T& val) {
    for (int i = 0; i < n; ++i) {
        memset_array(m, matrix[i], val);
    }
}
