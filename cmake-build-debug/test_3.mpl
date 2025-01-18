array<int> a(100000);
int size = 100000;

for i in <0, size> {
    a[i] = 1;
};

a[0] = 0;
a[1] = 0;

for i in <size> {
    if (i < 101) {
        if (a[i] > 0) {
            for j in <size> {
                int multiply = i * j;
                if (multiply < size) {
                    a[multiply] = 0;
               };
            };
        };
    };
};

for k in <0, size, 1> {
    if (a[k] > 0) {
        print(k);
    };
};