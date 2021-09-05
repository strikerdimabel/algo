#include <vector>

using namespace std;

int get_char_count() {
    return 1 << (sizeof(char) << 3);
}

/**
 * Returns an alphabet and a normalized string (a copy).
 * The first letter in the alphabet goes to (char) 0, the second - to (char) 1 and so on.
 * Chars in the alphabet are sorted as unsigned chars.
 */
pair<vector<char>*, char*> normalize_alphabet(int n, const char* s) {
    int char_count = get_char_count();
    bool* found_chars = new bool[char_count];
    fill(found_chars, found_chars + char_count, false);
    for (int i = 0; i < n; ++i) {
        found_chars[(unsigned char) s[i]] |= true;
    }
    int* alphabet_indices = new int[char_count];
    vector<char>* alphabet = new vector<char>;
    for (int i = 0; i < char_count; ++i) {
        if (found_chars[i]) {
            alphabet_indices[i] = alphabet->size();
            alphabet->push_back(i);
        }
    }
    char* new_arr = new char[n];
    for (int i = 0; i < n; ++i) {
        new_arr[i] = alphabet_indices[(unsigned char) s[i]];
    }

    delete[] found_chars;
    delete[] alphabet_indices;
    
    return pair<vector<char>*, char*>(alphabet, new_arr);
}

/**
 * Returns p - a cyclic shift array.
 * Let's sort all cyclic shifts of the string in lexicographic order.
 * Then p[i] - an index of the i-th cycle shift.
 * Complexity: n * log(n)
 */
int* get_cyclic_shift_array(int n, const char* s) {
    const pair<vector<char>*, char*>& normalized_res = normalize_alphabet(n, s);
    s = normalized_res.second;
    int alphabet = normalized_res.first->size();

    // First phase

    // Count letters
    int* counts = new int[max(n, alphabet)];
    memset (counts, 0, alphabet * sizeof(int));
    for (int i = 0; i < n; ++i) {
        counts[s[i]] += 1;
    }

    // Fill p
    for (int i = 1; i < alphabet; ++i) {
        counts[i] += counts[i - 1];
    }
    int* p = new int[n];
    for (int i=0; i<n; ++i) {
        p[--counts[s[i]]] = i;
    }

    // Fill c
    int* c = new int[n];
    c[p[0]] = 0;
    int classes = 1;
    for (int i = 1; i < n; ++i) {
        if (s[p[i]] != s[p[i - 1]]) {
            ++classes;
        }
        c[p[i]] = classes - 1;
    }

    // Next phases
    int* pn = new int[n];
    int* cn = new int[n];
    for (int len = 1; len < n; len <<= 1) {
        // Fill pn
        for (int i = 0; i < n; ++i) {
            pn[i] = p[i] - len;
            if (pn[i] < 0) {
                pn[i] += n;
            }
        }

        // Fill counts
        memset(counts, 0, classes * sizeof(int));
        for (int i = 0; i < n; ++i) {
            ++counts[c[pn[i]]];
        }

        // Fill p
        for (int i = 1; i < classes; ++i) {
            counts[i] += counts[i - 1];
        }
        for (int i = n - 1; i >= 0; --i) {
            p[--counts[c[pn[i]]]] = pn[i];
        }

        // Fill c
        cn[p[0]] = 0;
        classes = 1;
        for (int i = 1; i < n; ++i) {
            int mid1 = (p[i] + len) % n,  mid2 = (p[i - 1] + len) % n;
            if (c[p[i]] != c[p[i - 1]] || c[mid1] != c[mid2]) {
                ++classes;
            }
            cn[p[i]] = classes - 1;
        }

        // Swap c
        swap(c, cn);
    }

    delete[] normalized_res.first;
    delete[] normalized_res.second;
    delete[] counts;
    delete[] c;
    delete[] pn;
    delete[] cn;

    return p;
}
