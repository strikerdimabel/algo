template <typename T>
void print_array(const T& left, const T& right) {
    if (left == right) {
        return;
    }
    T iter = left;
    while (true) {
        cout << *iter;
        iter = ++iter;
        if (iter != right) {
            cout << ' ';
        } else {
            break;
        }
    }    
}

template <typename T>
void print_matrix(int n, int m, T** matrix) {
    for (int i = 0; i < n; ++i) {
        print_array(matrix[i], matrix[i] + m);
        cout << endl;
    }
}
