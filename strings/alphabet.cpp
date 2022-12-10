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
