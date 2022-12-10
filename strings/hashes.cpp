#include <string>

using namespace std;

typedef unsigned __int64 uint64;

#define P 31  // for small latin letters; should be a prime a bit larger than the alphabet size


uint64* init_powers(size_t n) {
    uint64* powers = new uint64[n];
    powers[0] = 1;
    for (size_t i = 1; i < n; ++i) {
        powers[i] = powers[i - 1] * P;
    }
    return powers;
}

uint64* generate_prefix_hashes(const string& s, uint64* powers) {
    size_t n = s.length();
    uint64* hashes = new uint64[n];
    for (size_t i = 0; i < n; ++i) {
        hashes[i] = powers[i] * (s[i] - 'a' + 1);
        if (i != 0) {
            hashes[i] += hashes[i - 1];
        }
    }
    return hashes;
}

/**
 * Returns a normalized hash for a substring.
*/
uint64 get_sustring_hash(size_t from_inclusive, size_t to_inclusive, size_t n, uint64* prefix_hashes, uint64* powers) {
    uint64 hash = prefix_hashes[to_inclusive];
    if (from_inclusive != 0) {
        hash -= prefix_hashes[from_inclusive - 1];
    }
    hash *= powers[n - 1 - to_inclusive];
    return hash;
}
