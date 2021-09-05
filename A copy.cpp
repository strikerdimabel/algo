#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

using namespace std;

typedef __int64 int64;

template <typename T>
T read() {
    T n;
    cin >> n;
    return n;
}

template <typename T>
T* read_array(int n, T* arr) {
    for (int i = 0; i < n; ++i) {
        cin >> arr[i];
    }
    return arr;
}

template <typename T>
T* read_array(int n) {
    return read_array(n, new T[n]);
}

int64 get_max_sum(int n, int64* arr) {
    int64 max_sum = 0, max_pending = 0;
    int64 negative_sum = 0, positive_sum = 0, positive_offset = 0;
    int64 prev = 0;
    bool is_positive = false;
    for (int i = 0; i < n; ++i) {
        bool even = ((i & 1) == 0);
        int64 a = even ? arr[i] : -arr[i];

        // Sign change
        if (is_positive && a < 0) {
            max_pending = max(
                max_pending + negative_sum + positive_sum,
                positive_sum - positive_offset
            );
            max_sum = max(max_sum, max_pending);
            negative_sum = 0;
            positive_sum = 0;
            positive_offset = 0;
            is_positive = false;
        } else if (!is_positive && a > 0) {
            if (!even) {
                positive_offset = min(-prev, a);
            }
            is_positive = true;
        }

        // Pending sums change
        if (a < 0) {
            negative_sum += a;
        } else {
            positive_sum += a;
        }

        prev = a;
    }
    max_pending = max(
        max_pending + negative_sum + positive_sum,
        positive_sum - positive_offset
    );
    max_sum = max(max_sum, max_pending);
    return max_sum;
}

int64 sol1(int n, int64* arr) {
    int64 max_sum_even = get_max_sum(n, arr);
    int64 max_sum_odd = get_max_sum(n - 1, arr + 1);
    return max(max_sum_even, max_sum_odd);
}

int64 sol2(int n, int64* arr) {
    int64* even_sums = new int64[n + 1];
    even_sums[0] = 0ll;
    even_sums += 1;
    int64* odd_sums = new int64[n];
    odd_sums[0] = 0ll;

    int64 sum = 0;
    for (int i = 0; i < n; ++i) {
        if (i & 1 == 1) {
            sum -= arr[i];
        } else {
            sum += arr[i];
        }
        even_sums[i] = sum;
    }

    sum = 0;
    for (int i = 1; i < n; ++i) {
        if (i & 1 == 1) {
            sum += arr[i];
        } else {
            sum -= arr[i];
        }
        odd_sums[i] = sum;
    }

    int64 max_even_sum = 0;
    for (int i = 0; i < n; i += 2) {
        for (int j = i; j < n; ++j) {
            max_even_sum = max(max_even_sum, even_sums[j] - even_sums[i - 1]);
        }
    }

    int64 max_odd_sum = 0;
    for (int i = 1; i < n; i += 2) {
        for (int j = i; j < n; ++j) {
            max_odd_sum = max(max_odd_sum, odd_sums[j] - odd_sums[i - 1]);
        }
    }

    return max(max_even_sum, max_odd_sum);
}

int main()
{
    int64* arr = new int64[10];
    for (int k = 0; k < 1000000; ++k) {
        int n = rand() % 10 + 1;
        for (int i = 0; i < n; ++i) {
            arr[i] = rand() % 11 - 5;
        }
        int64 res_sol1 = sol1(n, arr);
        int64 res_sol2 = sol2(n, arr);
        if (res_sol1 != res_sol2) {
            cout << n << endl;
            for (int i = 0; i < n; ++i) {
                cout << arr[i] << ' ';
            }
            cout << endl;
            cout << res_sol1 << " " << res_sol2 << endl << endl;
        }
    }

    return 0;
}

