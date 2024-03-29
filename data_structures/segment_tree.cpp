#include <algorithm>

using namespace std;

/**
 * vertex is 1-based
 */
void _build_segment_tree(int* values, int* res, int vertex, int from, int to) {
	if (from == to)
		res[vertex] = values[from];
	else {
		int mid = (from + to) / 2;
		_build_segment_tree(values, res, vertex * 2, from, mid);
		_build_segment_tree(values, res, vertex * 2 + 1, mid + 1, to);
		res[vertex] = res[vertex * 2] + res[vertex * 2 + 1];
	}
}

/**
 * vertex is 1-based
 */
int* build_segment_tree(int* values, int n) {
    int* res = new int[4 * n];
    _build_segment_tree(values, res, 1, 0, n - 1);
    return res;
}

/**
 * vertex is 1-based
 */
int sum_segment_tree(int* tree, int vertex, int t_from, int t_to, int from, int to) {
	if (from > to)
		return 0;
	if (from == t_from && to == t_to)
		return tree[vertex];
	int t_mid = (t_from + t_to) / 2;
	return sum_segment_tree(tree, vertex * 2, t_from, t_mid, from, min(to, t_mid))
		+ sum_segment_tree(tree, vertex * 2 + 1, t_mid + 1, t_to, max(from, t_mid + 1), to);
}

/**
 * vertex is 1-based
 */
void update_segment_tree(int* tree, int vertex, int from, int to, int pos, int new_val) {
	if (from == to)
		tree[vertex] = new_val;
	else {
		int mid = (from + to) / 2;
		if (pos <= mid)
			update_segment_tree(tree, vertex * 2, from, mid, pos, new_val);
		else
			update_segment_tree(tree, vertex * 2 + 1, mid + 1, to, pos, new_val);
		tree[vertex] = tree[vertex * 2] + tree[vertex * 2 + 1];
	}
}
