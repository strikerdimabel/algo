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

/**
 * Returns an n-length array describing all even length palindroms.
 * Array[i] contains in integer meaning the number of palinrdroms
 * which center is located at the i-th position.
 */
vector<int> palindroms1(const string& s) {
    size_t n = s.length();
    vector<int> res(n);
    int l = 0;
    int r = -1;
    for (int i = 0; i < n; ++i) {
        int k = i > r ? 1 : min(res[l + r - i], r - i + 1);
        while (i + k < n
            && i - k >= 0
            && s[i + k] == s[i - k]
        ) {
            ++k;
        }
        res[i] = k;
        if (i + k - 1 > r) {
            l = i - k + 1;
            r = i + k - 1;
        }
    }
    return res;
}

/**
 * Returns an n-length array describing all odd length palindroms.
 * Array[i] contains in integer meaning the number of palinrdroms
 * which center is located between the (i - 1)-th and i-th positions.
 */
vector<int> palindroms2(const string& s) {
    size_t n = s.length();
    vector<int> res(n);
    int l = 0;
    int r = -1;
    for (int i = 0; i < n; ++i) {
        int k = i > r ? 0 : min(res[l + r - i + 1], r - i + 1);
        while (i + k < n
            && i - k - 1 >= 0
            && s[i + k] == s[i - k - 1]
        ) {
            ++k;
        }
        res[i] = k;
        if (i + k - 1 > r) {
            l = i - k;
            r = i + k - 1;
        }
    }
    return res;
}

int main()
{
    return 0;
}
