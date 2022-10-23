#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <algorithm>
#include <numeric>

using namespace std;

typedef __int64 int64;

template <typename T>
T read() {
    T n;
    cin >> n;
    return n;
}

template <typename T>
T* read_array(int n) {
    T* arr = new T[n];
    for (int i = 0; i < n; ++i) {
        cin >> arr[i];
    }
    return arr;
}

const int INF = INT_MAX;

struct Edge {
    int to_id;
    int weight;

    Edge(int to_id = -1, int weight = INF): to_id(to_id), weight(weight) {
    }
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
void memset_array(int n, T* array, const T& val) {
    fill(array, array + n, val);
}

template <typename T>
void memset_matrix(int n, int m, T** matrix, const T& val) {
    for (int i = 0; i < n; ++i) {
        memset_array(m, matrix[i], val);
    }
}

/**
 * Returns a minimal spanning tree.
 * Returns NULL if there is no such tree.
 * n - a number of vertices.
 * Complexity: n^2.
 * For dense graphs you can improve complexity up to O(m * log(n)).
 * vertex_id is 0 based.
 * The returned array contains n - 1 edges; edges[i] connects i-th and edges[i].to_id-th vertices.
 */
vector<Edge>* prim(int n, const vector<Edge>* adjacency_list) {
    Edge* min_edges = new Edge[n];
    min_edges[0] = Edge(-1, 0);

    bool* visited = new bool[n];
    fill(visited, visited + n, false);

    for (int i = 0; i < n; ++i) {
        int min_id = -1;
        int min_w = INF;
        for (int j = 0; j < n; ++j) {
            int w = min_edges[j].weight;
            if (!visited[j] && (w < min_w)) {
                min_id = j;
                min_w = w;
            }
        }
        if (min_id == -1) {
            return NULL;
        }
        for (const Edge& to : adjacency_list[min_id]) {
            if (!visited[to.to_id] && to.weight < min_edges[to.to_id].weight) {
                min_edges[to.to_id] = Edge(min_id, to.weight);
            }
        }
        visited[min_id] = true;
    }

    // build the result tree
    vector<Edge>* result = new vector<Edge>[n];
    for (int i = 1; i < n; ++i) {
        result[i].push_back(min_edges[i]);
        result[min_edges[i].to_id].push_back(Edge(i, min_edges[i].weight));
    }
    delete[] min_edges;
    delete[] visited;

    return result;
}

/**
 * Returns the weight of a tree.
 */
int get_weight(int n, vector<Edge>* tree) {
    int weight = 0;
    for (int i = 0; i < n; ++i) {
        for (Edge edge: tree[i]) {
            weight += edge.weight;
        }
    }
    return weight / 2;
}

vector<int> get_max_edge_matrix(vector<Edge>* tree, int** max_edge_matrix, int vertex, int n, int parent)
{
    int children = tree[vertex].size();
    vector<int> all_child_vertices;
    Edge* edges = new Edge[n];
    for (int i = 0; i < children; ++i) {
        const Edge& edge = tree[vertex][i];
        if (edge.to_id == parent) {
            continue;
        }
        vector<int> child_vertices = get_max_edge_matrix(tree, max_edge_matrix, edge.to_id, n, vertex);
        for (int child: child_vertices) {
            edges[child] = edge;
            all_child_vertices.push_back(child);
        }
    }
    edges[vertex] = Edge(vertex, 0);
    all_child_vertices.push_back(vertex);
    int all_children = all_child_vertices.size();
    for (int i = 0; i < all_children; ++i) {
        for (int j = 0; j < i; ++j) {
            int v1 = all_child_vertices[i];
            int v2 = all_child_vertices[j];
            const Edge& edge1 = edges[v1];
            const Edge& edge2 = edges[v2];
            if (edge1.to_id == edge2.to_id) {
                continue;
            }
            int max_edge = max(
                max(max_edge_matrix[v1][edge1.to_id], edge1.weight),
                max(max_edge_matrix[v2][edge2.to_id], edge2.weight)
            );
            max_edge_matrix[v1][v2] = max_edge;
            max_edge_matrix[v2][v1] = max_edge;
        }
    }
    delete[] edges;
    return all_child_vertices;
}

int** get_max_edge_matrix(vector<Edge>* tree, int n)
{
    int** max_edge_matrix = new_matrix<int>(n, n);
    memset_matrix(n, n, max_edge_matrix, 0);
    get_max_edge_matrix(tree, max_edge_matrix, 0, n, -1);
    return max_edge_matrix;
}

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

int get_second_min_weight(vector<Edge>* graph, bool** tree_adjacent_matrix, int** max_edge_matrix, int n, int min_weight)
{
    int second_min_weight = -1;
    for (int i = 0; i < n; ++i) {
        for (Edge edge: graph[i]) {
            if (!tree_adjacent_matrix[i][edge.to_id]) {
                int max_edge = max_edge_matrix[i][edge.to_id];
                if (second_min_weight == -1 || second_min_weight > min_weight + edge.weight - max_edge) {
                    second_min_weight = min_weight + edge.weight - max_edge;
                }
            }
        }
    }
    return second_min_weight;
}

int main()
{
    int n = read<int>();
    int m = read<int>();
    vector<Edge>* graph = new vector<Edge>[n];
    for (int i = 0; i < m; ++i) {
        int x = read<int>() - 1;
        int y = read<int>() - 1;
        int w = read<int>();
        graph[x].push_back(Edge(y, w));
        graph[y].push_back(Edge(x, w));
    }
    vector<Edge>* tree = prim(n, graph);
    int min_weight = get_weight(n, tree);
    cout << "Cost: " << min_weight << endl;
    int** max_edge_matrix = get_max_edge_matrix(tree, n);
    bool** tree_adjacent_matrix = get_adjacent_matrix(tree, n);
    int second_min_weight = get_second_min_weight(graph, tree_adjacent_matrix, max_edge_matrix, n, min_weight);
    cout << "Cost: " << second_min_weight << endl;
    return 0;
}

/*
4 6
1 2 2
2 3 2
3 4 2
4 1 2
1 3 1
2 4 1

Cost: 4
Cost: 4

-----------------

3 2
1 2 2
2 3 2

Cost: 4
Cost: -1

-----------------

8 9
1 2 1
2 3 5
3 4 3
4 5 4
3 6 1
3 7 2
3 8 9
6 8 9
7 8 8

Cost: 24
Cost: 25

-----------------

2 1
1 2 3

Cost: 3
Cost: -1

-----------------

4 6
1 2 1
1 3 5
3 4 1
4 2 3
4 1 8
2 3 0

Cost: 2
Cost: 4

-----------------

4 4
1 2 4
2 3 3
3 4 2
2 4 10

Cost: 9
Cost: 16

-----------------

6 6
1 2 8
2 3 3
3 5 2
2 4 7
4 6 2
2 5 6

Cost: 22
Cost: 25

-----------------

5 6
1 2 2
1 5 1
2 3 1
2 4 1
1 3 4
4 5 3

Cost: 5
Cost: 6

-----------------

4 4
1 2 2
2 3 1
2 4 3
3 4 1

Cost: 4
Cost: 6

-----------------

1 0


*/

/*122992QB*/
