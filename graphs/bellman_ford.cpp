#include <numeric>

using namespace std;

/**
 * The least meaningful absolute value to avoid comparison glitches.
 */
const double EPS = 1e-12;

/**
 * Double infinity.
 */
const double INF = DBL_MAX;


struct Edge {
    /**
     * An index of the first vertex, 0-based
     */
    int first_id;

    /**
     * An index of the second vertex, 0-based
     */
    int second_id;

    /**
     * The cost of going from the first vertex to the second one. 
     */
    double cost;

    /**
     * The cost to reach the second vertex through the first one.
     */
    double get_cost(double first_cost) const {
        return first_cost + cost;
    }

    bool is_new_cost_better(double old_cost, double new_cost) const {
        return new_cost < old_cost && abs(new_cost - old_cost) > EPS;
    }
};

/**
 * Returns NULL if there is a "negative" cycle.
 * Otherwise, returns a best cost to reach each vertex from vertex x.
 * n - vertices count - the size of initial_costs.
 * m - edges count - the size of edges.
 * Complexity: n*m.
 */
double* _get_best_costs(int n, const double* initial_costs, int m, const Edge* edges) {
    double* costs = new double[n];
    copy(initial_costs, initial_costs + n, costs);
    bool changed;
    for (int i = 0; i < n; ++i) {
        changed = false;
        for (int j = 0; j < m; ++j) {
            const Edge& edge = edges[j];
            double new_cost = edge.get_cost(costs[edge.first_id]);
            if (edge.is_new_cost_better(costs[edge.second_id], new_cost)) {
                costs[edge.second_id] = new_cost;
                changed = true;
            }
        }
        if (!changed) {
            return costs;
        }
    }
    return NULL;
}

/**
 * Returns NULL if there is a "negative" cycle.
 * Otherwise, returns a best cost to reach each vertex from x.
 * x - a vertex to start from, 0-based
 * n - vertices count - the size of initial_costs
 * m - edges count - the size of edges
 * Complexity: n*m.
 */
double* get_best_costs(int n, int x, int m, const Edge* edges) {
    // Initial costs
    double* initial_costs = new double[n];
    fill(initial_costs, initial_costs + n, INF);
    initial_costs[x] = 0;

    // Run
    double* best_costs = _get_best_costs(n, initial_costs, m, edges);
    
    // Cleanup
    delete[] initial_costs;
    
    return best_costs;
}
