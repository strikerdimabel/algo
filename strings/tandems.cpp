#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <algorithm>
#include <numeric>
#include <math.h>

using namespace std;


int* z_function(const string& s) {
	int n = s.length();
	int* z = new int[n];
    memset(z, 0, sizeof(int) * n);
	for (int i = 1, l = 0, r = 0; i < n; ++i) {
		if (i <= r) {
			z[i] = min(r - i + 1, z[i - l]);
        }
		while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
			z[i] += 1;
        }
		if (i + z[i] - 1 > r) {
			l = i;
            r = i + z[i] - 1;
        }
	}
	return z;
}

inline int get_z(int* z, int i, int n) {
	return 0 <= i && i < n ? z[i] : 0;
}

struct Tandem {
    int from;
    int to;  // Exclusive.

    Tandem(int from, int to):
        from(from), to(to) {
    }

    int get_l() const {
        return get_length() / 2;
    }

    int get_length() const {
        return to - from;
    }
};

/**
 * Formulas:
 * l1 + l2 = l
 * l1 <= k1
 * l2 <= k2
 * 
 * Left tandem:
 * 
 *    l1    l2       l
 * |-------|---|-----------|
 *  a b c d e f a b c d e f a
 *  |       |               |
 *  from    cntr            to
 * 
 * Right tandem:
 * 
 *       l      l1    l2
 * |-----------|---|-------|
 *  a b c d e f a b c d e f a
 *  |             |         |
 *  from          cntr      to
*/
struct TandemGroup {

    bool left;
    int cntr;
    int l;
    int k1;
    int k2;

    TandemGroup(bool left, int cntr, int l, int k1, int k2):
        left(left), cntr(cntr), l(l), k1(k1), k2(k2) {
    }

    int get_from(int l1) const {
        if (this->left) {
            return this->cntr - l1;
        } else {
            return this->cntr - this->l - l1 + 1;
        }
    }

    /**
     * Exclusive.
    */
    int get_to(int l1) const {
        return this->get_from(l1) + 2 * this->l;
    }

    int get_first_l1() const {
        return max(1, this->l - this->k2);
    }

    int get_last_l1() const {
        if (this->left) {
            return min(this->l - 1, k1);
        } else {
            return min(this->l, k1);
        }
    }

    int get_l2(int l1) const {
        return this->l - l1;
    }

    vector<Tandem> get_tandems() {
        vector<Tandem> tandems;
        int last_l1 = this->get_last_l1();
        for (int l1 = this->get_first_l1(); l1 <= last_l1; ++l1) {
            tandems.push_back(Tandem(this->get_from(l1), this->get_to(l1)));
        }
    }
};
 
void output_tandems(const string& s, const TandemGroup& tandem_group) {
    int last_l1 = tandem_group.get_last_l1();
	for (int l1 = tandem_group.get_first_l1(); l1 <= last_l1; ++l1) {
	    cout << "[" << tandem_group.get_from(l1) << ".." << tandem_group.get_to(l1) - 1 << "] = "
            << s.substr(tandem_group.get_from(l1), 2 * tandem_group.l) << endl;
	}
}

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
 * matrix[i][j] = true if there is a tandem at position i and half length j.
*/
bool** build_tandem_matrix(const vector<TandemGroup>& tandem_groups, int n) {
    bool** tandem_matrix = new_matrix<bool>(n, n / 2 + 1);
    memset_matrix(n, n / 2 + 1, tandem_matrix, false);
    for (const TandemGroup& tandem_group : tandem_groups) {
        int last_l1 = tandem_group.get_last_l1();
        for (int l1 = tandem_group.get_first_l1(); l1 <= last_l1; ++l1) {
            tandem_matrix[tandem_group.get_from(l1)][tandem_group.l] = true;
        }
    }
    return tandem_matrix;
}

/**
 * Removes tandems from matrix if they consist of 2 smaller tandems.
 * Remains only simple tandems.
*/
void remove_complex_tandems(bool** tandem_matrix, int n) {
    for (int from = 0; from <= n - 4; ++from) {
        for (int l = 1; l <= (n - from) / 4; ++l) {
            if (!tandem_matrix[from + l][l] || !tandem_matrix[from + 2 * l][l]) {
                continue;
            }
            for (int to = from + 4 * l; to <= n && tandem_matrix[to - 2 * l][l]; to += l) {
                int max_ll = (to - from) / 2;
                for (int ll = 2 * l; ll <= max_ll; ll += l) {
                    tandem_matrix[to - ll][ll] = false;
                }
            }
        }
    }
}

/**
 * index[i] contains all tandems which contain position i element.
*/
vector<Tandem>* build_tandem_index(const vector<TandemGroup>& tandem_groups, int n) {
    vector<Tandem>* tandem_index = new vector<Tandem>[n];
    for (const TandemGroup& tandem_group : tandem_groups) {
        int last_l1 = tandem_group.get_last_l1();
        for (int l1 = tandem_group.get_first_l1(); l1 <= last_l1; ++l1) {
            int from = tandem_group.get_from(l1);
            int to = tandem_group.get_to(l1);
            for (int i = from; i < to; ++i) {
                tandem_index[i].push_back(Tandem(from, to));
            }
        }
    }
    return tandem_index;
}

/**
 * index[i] contains all tandems which contain position i element.
*/
vector<Tandem>* build_tandem_index(bool** tandem_matrix, int n) {
    vector<Tandem>* tandem_index = new vector<Tandem>[n];
    for (int l = n / 2; l > 0; --l) {
        int max_ind = n - 2 * l;
        for (int from = 0; from <= max_ind; ++from) {
            if (tandem_matrix[from][l]) {
                int to = from + 2 * l;
                for (int i = from; i < to; ++i) {
                    tandem_index[i].push_back(Tandem(from, to));
                }
            }
        }
    }
    return tandem_index;
}

void _find_main_lorentz_tandems(const string& s, vector<TandemGroup>& tandem_groups, int shift) {
	int n = s.length();
	if (n == 1) {
        return;
    }
 
	int nu = n / 2;
    int nv = n - nu;
	string u = s.substr(0, nu);
    string v = s.substr(nu);
	string ru = string(u.rbegin(), u.rend());
    string rv = string(v.rbegin(), v.rend());
 
	_find_main_lorentz_tandems(u, tandem_groups, shift);
	_find_main_lorentz_tandems(v, tandem_groups, shift + nu);
 
	int* z1 = z_function(ru);
    int z1_size = ru.size();
	int* z2 = z_function(v + '#' + u);
    int z2_size = v.size() + 1 + u.size();
	int* z3 = z_function(ru + '#' + rv);
    int z3_size = ru.size() + 1 + rv.size();
	int* z4 = z_function(v);
    int z4_size = v.size();
	for (int cntr = 0; cntr < n; ++cntr) {
		int l, k1, k2;
		if (cntr < nu) {
			l = nu - cntr;
			k1 = get_z(z1, l, z1_size);
			k2 = get_z(z2, nv + 1 + cntr, z2_size);
		} else {
			l = cntr - nu + 1;
			k1 = get_z(z3, nu + 1 + nv - l, z3_size);
			k2 = get_z(z4, l, z4_size);
		}
		if (k1 + k2 >= l) {
            tandem_groups.push_back(TandemGroup(cntr < nu, shift + cntr, l, k1, k2));
        }
	}
    delete[] z1;
    delete[] z2;
    delete[] z3;
    delete[] z4;
}

vector<TandemGroup> find_main_lorentz_tandems(const string& s) {
    vector<TandemGroup> tandem_groups;
    _find_main_lorentz_tandems(s, tandem_groups, 0);
    return tandem_groups;
}

int main()
{
    string s("acaacababaee");
    // string s("acaaca");
    vector<TandemGroup>& tandem_groups = find_main_lorentz_tandems(s);
    for (TandemGroup& tandem_group : tandem_groups) {
        output_tandems(s, tandem_group);
    }
    return 0;
}
