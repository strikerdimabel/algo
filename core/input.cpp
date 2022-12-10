template <typename T>
T read() {
    T n;
    cin >> n;
    return n;
}

template <typename T>
T* read_array(int n) {
    T* arr = new T[n];
    for (int i = 0; i < n; ++i) {
        cin >> arr[i];
    }
    return arr;
}

template <typename T>
T* read_matrix(int n, int m) {
    T** matrix = new T*[n];
    for (int i = 0; i < n; ++i) {
        matrix[i] = read_array<T>(m);
    }
    return matrix;
}
