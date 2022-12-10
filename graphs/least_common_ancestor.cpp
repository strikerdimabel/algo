// Taken from https://e-maxx.ru/algo/lca

#include <vector>

using namespace std;

typedef vector < vector<int> > graph;
typedef vector<int>::const_iterator const_graph_iter;


vector<int> _lca_h, _lca_dfs_list, _lca_first, _lca_tree;
vector<char> _lca_dfs_used;

void _lca_dfs (const graph & g, int v, int h = 1)
{
	_lca_dfs_used[v] = true;
	_lca_h[v] = h;
	_lca_dfs_list.push_back (v);
	for (const_graph_iter i = g[v].begin(); i != g[v].end(); ++i)
		if (!_lca_dfs_used[*i])
		{
			_lca_dfs (g, *i, h+1);
			_lca_dfs_list.push_back (v);
		}
}

void _lca_build_tree (int i, int l, int r)
{
	if (l == r)
		_lca_tree[i] = _lca_dfs_list[l];
	else
	{
		int m = (l + r) >> 1;
		_lca_build_tree (i+i, l, m);
		_lca_build_tree (i+i+1, m+1, r);
		if (_lca_h[_lca_tree[i+i]] < _lca_h[_lca_tree[i+i+1]])
			_lca_tree[i] = _lca_tree[i+i];
		else
			_lca_tree[i] = _lca_tree[i+i+1];
	}
}

void lca_prepare (const graph & g, int root)
{
	int n = (int) g.size();
	_lca_h.resize (n);
	_lca_dfs_list.reserve (n*2);
	_lca_dfs_used.assign (n, 0);

	_lca_dfs (g, root);

	int m = (int) _lca_dfs_list.size();
	_lca_tree.assign (_lca_dfs_list.size() * 4 + 1, -1);
	_lca_build_tree (1, 0, m-1);

	_lca_first.assign (n, -1);
	for (int i = 0; i < m; ++i)
	{
		int v = _lca_dfs_list[i];
		if (_lca_first[v] == -1)
			_lca_first[v] = i;
	}
}

int _lca_tree_min (int i, int sl, int sr, int l, int r)
{
	if (sl == l && sr == r)
		return _lca_tree[i];
	int sm = (sl + sr) >> 1;
	if (r <= sm)
		return _lca_tree_min (i+i, sl, sm, l, r);
	if (l > sm)
		return _lca_tree_min (i+i+1, sm+1, sr, l, r);
	int ans1 = _lca_tree_min (i+i, sl, sm, l, sm);
	int ans2 = _lca_tree_min (i+i+1, sm+1, sr, sm+1, r);
	return _lca_h[ans1] < _lca_h[ans2] ? ans1 : ans2;
}

int lca (int a, int b)
{
	int left = _lca_first[a],
		right = _lca_first[b];
	if (left > right)  swap (left, right);
	return _lca_tree_min (1, 0, (int)_lca_dfs_list.size()-1, left, right);
}

int main()
{
	graph g;
	int root;
	// ... чтение графа ...

	lca_prepare (g, root);

	for (;;)
	{
		int v1, v2; // поступил запрос
		int v = lca (v1, v2); // ответ на запрос
	}
}
