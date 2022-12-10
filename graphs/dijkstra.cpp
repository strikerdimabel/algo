#include <vector>
#include <set>

using namespace std;

const int INF = INT_MAX;

struct Edge {
    int to_id;
    int length;

    Edge(int to_id, int length = 1): to_id(to_id), length(length) {
    }
};

struct Vertex {
    int id;
    int distance;

    Vertex(int id, int distance): id(id), distance(distance) {
    }
};
auto vertex_compare = [](const Vertex& v1, const Vertex& v2) {
    if (v1.distance != v2.distance) {
        return v1.distance < v2.distance;
    }
    return v1.id < v2.id;
};

/**
 * n - a number of vertices
 * x - a vertex to start from
 * Returns minimal distances between x and each vertex.
 * Complexity: (m + n) * log(n) = n ^ 2 * log(n).
 */
int* dijkstra(int n, int x, const vector<Edge>* adjacency_list) {
    // init distances
    int* distances = new int[n]; 
    int* sources = new int[n];
    fill(distances, distances + n, INF);
    distances[x] = 0;

    // init priority queue
    set<Vertex, decltype(vertex_compare)> vertices(vertex_compare);
    vertices.insert(Vertex(x, 0));

    // Run algo
    while (!vertices.empty()) {
        auto first_iter = vertices.begin();
        Vertex v = *first_iter;
        vertices.erase(first_iter);
        const vector<Edge>& edges = adjacency_list[v.id];
        for (const Edge& edge: edges) {
            int new_distance = v.distance + edge.length;
            int old_distance = distances[edge.to_id];
            if (new_distance < old_distance) {
                if (old_distance != INF) {
                    vertices.erase(Vertex(edge.to_id, old_distance));
                }
                vertices.insert(Vertex(edge.to_id, new_distance));
                distances[edge.to_id] = new_distance;
                sources[edge.to_id] = v.id;
            }
        }
    }

    return distances;
}

/**
 * Builds a path from source to dest using given sources array.
 * sources[i] - a source for i-th vertex
 * Complexity: O(n).
 */
vector<int> build_path(int source, int dest, int* sources) {
    vector<int> path;
    int iter = dest;
    while (iter != source) {
        path.push_back(iter);
        iter = sources[iter];
    }
    path.push_back(source);
    reverse(path.begin(), path.end());
    return path;
}
