array<int> a(10000);
int size = 10000;

for i in <size> {
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

for k in <size> {
    if (a[k] > 0) {
        print(k);
    };
};

jawohl;