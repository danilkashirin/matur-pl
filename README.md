# MATUR-PL

## Authors: 
- Kashirin Danil (lead, all scopes)
- Meshechkin Igor (parser, bytecode generator & vm for functions, ir generator)
- Vakulenko Nazar (vm, garbage collector)
- Rodezkiy Nikita (ast optimization)
- Chill guy from Miami (support & debug)

# Attention
In the MATUR-PL language, it is mandatory to add the keyword "jawohl" at the end of the executable file. This serves as a signal for the end of the program; otherwise, you will receive warnings!

## Supported Functionality

### 1. **Integer Arithmetic**
MATUR-PL supports the following basic arithmetic operations:
- Addition `+`
- Subtraction `-`
- Multiplication `*`
- Integer division `/`
- Modulus (remainder) `%`

#### Example:
```matur-pl
int a = 5 + 5;
int b = a - 3;
int c = b * 2 - a;
int d = c % 5 - c / b;
```

### 2. **Array Operations**
Arrays are implemented with a user-friendly and flexible syntax:

#### Examples:
```matur-pl
array<int> a(5);                // Creates an array of 5 zeros
array<int> b(5) = [1, 2, 3];    // Creates an array [1, 2, 3, 0, 0]
array<int> c(3) = [1, 2, 3, 4, 5]; // Length exceeds the specified size, but no error
array<int> d = [1, 2];          // Creates an array without specifying the length
array<int> e = random(5);       // Creates an array of 5 random numbers
```

---

### 3. **Loops**
The `for` loop is supported with several syntax options:
- `for i in <start, finish, step>`: start, end, step.
- `for i in <start, finish>`: step defaults to 1.
- `for i in <finish>`: start defaults to 0, step defaults to 1.

#### Example:
```matur-pl
for i in <0, 10, 2> {
  print(i);
}

for j in <1, 5> {
  print(j);
}

for k in <10> {
  print(k);
}
```

---

### 4. **Nested Loops and Conditionals**
Nested loops and `if` statements are fully supported:

#### Example:
```matur-pl
for i in <0, 3> {
  for j in <0, 3> {
    if (i % 2 == 0) {
      print(i * j);
    }
  }
}
```

---

### 5. **Functions and Recursion**
The language supports a function call stack with recursion.

#### Example:
```matur-pl
func factorial(n) {
  if (n == 1) {
    return 1;
  }
  return n * factorial(n - 1);
}

print(factorial(5));
```

---

### 6. **Garbage Collector**
**MATUR_PL** is equipped with a built-in garbage collector implemented using the **Mark and Sweep** algorithm. This ensures automatic memory management and prevents memory leaks.

---

## Features
- **Simple and intuitive syntax**.
- **User-friendly array operations**.
- **Flexible loops** with various range options.
- **Recursion support**.
- **Efficient memory management** through the built-in Garbage Collector.

---

The creation of **MATUR_PL** is a testament to how a cohesive team of professionals can develop a powerful programming tool.
