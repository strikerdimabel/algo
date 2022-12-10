#include <vector>

using namespace std;

template <typename T>
T** new_matrix(int n, int m) {
    T** matrix = new T*[n];
    for (int i = 0; i < n; ++i) {
        matrix[i] = new T[m];
    }
    return matrix;
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

struct Edge {
    int to_id;

    Edge(int to_id = -1): to_id(to_id) {
    }
};

bool** get_adjacent_matrix(vector<Edge>* graph, int n)
{
    bool** adjacent_matrix = new_matrix<bool>(n, n);
    memset_matrix(n, n, adjacent_matrix, false);
    for (int i = 0; i < n; ++i) {
        for (Edge edge: graph[i]) {
            adjacent_matrix[i][edge.to_id] = true;
            adjacent_matrix[edge.to_id][i] = true;
        }
    }
    return adjacent_matrix;
}
