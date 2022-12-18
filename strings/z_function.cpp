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
