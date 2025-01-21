def factorial(a) {
    if (a == 1) {
        return 1;
    };
    return factorial(a - 1) * a;
};

print(factorial(20));

jawohl;