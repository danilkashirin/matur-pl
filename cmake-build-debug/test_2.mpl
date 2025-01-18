array<int> a = random(100000);
int temp = 0;

int size = 100000;
int size_2 = size - 1;

for i in <size> {
    for j in <size_2> {
        int ind = j + 1;
        if (a[j] > a[ind]) {
            temp = a[j];
            a[j] = a[ind];
            a[ind] = temp;
        };
    };
};

for i in <size> {
    print(a[i]);
};