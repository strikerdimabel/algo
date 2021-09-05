#include <vector>
#include <set>

using namespace std;

const int INF = INT_MAX;

struct Edge {
    int to_id;
    int length;

    Edge(int to_id = -1, int length = INF): to_id(to_id), length(length) {
    }
};

/**
 * Returns a minimal spanning tree.
 * Returns NULL if there is no such tree.
 * n - a number of vertices.
 * Complexity: n^2.
 * For dense graphs you can improve complexity up to O(m * log(n)).
 */
Edge* prim(int n, const vector<Edge>* adjacency_list) {
    Edge* min_edges = new Edge[n];
    min_edges[0] = Edge(-1, 0);

    bool* visited = new bool[n];
    fill(visited, visited + n, false);

    for (int i = 0; i < n; ++i) {
        int min_id = -1;
        int min_w = INF;
        for (int j = 0; j < n; ++j) {
            int w = min_edges[j].length;
            if (!visited[j] && (w < min_w)) {
                min_id = j;
                min_w = w;
            }
        }
        if (min_id == -1) {
            return NULL;
        }
        for (const Edge& to : adjacency_list[min_id]) {
            if (!visited[to.to_id] && to.length < min_edges[to.to_id].length) {
                min_edges[to.to_id] = Edge(min_id, to.length);
            }
        }
        visited[min_id] = true;
    }
    return min_edges + 1;
}

/**
 * Returns the weight of a tree.
 */
int get_weight(int n, Edge* tree) {
    int weight = 0;
    for (int i = 0; i < n - 1; ++i) {
        weight += tree[i].length;
    }
    return weight;
}
