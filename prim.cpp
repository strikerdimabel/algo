#include <vector>
#include <set>

using namespace std;

const int INF = INT_MAX;

struct Edge {
    int to_id;
    int weight;

    Edge(int to_id = -1, int weight = INF): to_id(to_id), weight(weight) {
    }
};

/**
 * Returns a minimal spanning tree.
 * Returns NULL if there is no such tree.
 * n - a number of vertices.
 * Complexity: n^2.
 * For dense graphs you can improve complexity up to O(m * log(n)).
 * vertex_id is 0 based.
 * The returned graph contains n - 1 edges; edges[i] connects i-th and edges[i].to_id-th vertices.
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
