#include "Profiler.h"

#define MAX_SIZE 2000

Profiler profiler("demo-power");

/**
* a function that slowly computes x to the power n
*/
int slow_pow(int x, int n)
{
    int p = 1;
    int i;
    Operation o = profiler.createOperation("slow_pow", n);
    for(i=0; i<n; ++i) {
        //count the multiplications
        o.count();
        p *= x;
    }
    return p;
}

/**
* also computes x to the power n but "a little" faster
*/
int fast_pow(int x, int n, int size)
{
    int y;
    if(n == 0) {
        return 1;
    } else if(n == 1) {
        return x;
    } else {
        y = fast_pow(x, n/2, size);
        if(n % 2 == 0) {
            //count multiplications
            profiler.countOperation("fast_pow", size);
            return y * y;
        } else {
            //we can also count two multiplications at once
            profiler.countOperation("fast_pow", size, 2);
            return y * y * x;
        }
    }
}

/**
* the iterative version of the previous algorithm
*/
int fast_pow_iter(int x, int n)
{
    Operation o = profiler.createOperation("fast_pow_iter", n);
    int res = 1;
    while(n > 0) {
        if(n & 1) {
            o.count();
            res *= x;
        }
        o.count();
        x *= x;
        n >>= 1;
    }
    return res;
}

/**
* a naive function for finding duplicates in a vector
*/
bool hasDuplicates(int *v, int size)
{
    int i, j;
    bool dup = false;
    Operation o = profiler.createOperation("duplicates-comparation", size);
    for(i=0; i<size-1; ++i) {
        for(j=i+1; j<size; ++j) {
            o.count();
            if(v[i] == v[j]) {
                dup = true;
            }
        }
    }
    return dup;
}

/**
* the iterative version of the factorial function
*/
int factorial(int n)
{
    int res = 1;
    Operation o = profiler.createOperation("factorial_iter", n);
    for(int i=2; i<=n; ++i) {
        o.count();
        res *= i;
    }
    return res;
}

/**
* the recursive version of the factorial function
*/
int factorial_rec(int n, Operation &o)
{
    int res;
    if(n <= 1) {
        res = 1;
    } else {
        o.count();
        res = n * factorial_rec(n - 1, o);
    }
    return res;
}

int main(void)
{
    //compute some powers of 5
    printf("Computing powers...\n");
    int x = 5, p1, p2;
    int n;
    //increase n with 1 if smaller than 10
    //increase with 10 otherwise
    for(n=0; n<200; n += (n<10?1:10)) {
        p1 = slow_pow(x, n);
        p2 = fast_pow(x, n, n);
        if(p1 != p2) {
            printf("[ERROR] invalid result\n");
        }
    }
    //we would like the two series to be displayed on the same chart
    profiler.createGroup("power", "slow_pow", "fast_pow");

    //second example: find duplicates in a vector
    profiler.reset("demo-duplicates");
    printf("Finding duplicates...\n");
    int v[MAX_SIZE];
    FillRandomArray(v, MAX_SIZE);
    for(n=100; n<MAX_SIZE; n+=100) {
        hasDuplicates(v, n); //we're not actually interested in the result
    }

    //third example: factorial - iterative vs recursive
    profiler.reset("demo-factorial");
    printf("Computing factorial...\n");
    const int step = 1000;
    const int nr_tests = 100;
    for(n=step; n<=100*step; n+=step) {
        factorial(n);

        Operation op = profiler.createOperation("factorial_rec", n);
        factorial_rec(n, op);
    }
    for(n=step; n<=100*step; n+=step) {
        profiler.startTimer("factorial_iter", n);
        for(int test=0; test<nr_tests; ++test) {
            factorial(n);
        }
        profiler.stopTimer("factorial_iter", n);

        Operation op = profiler.createOperation("factorial_rec", n);
        profiler.startTimer("factorial_rec", n);
        for(int test=0; test<nr_tests; ++test) {
            factorial_rec(n, op);
        }
        profiler.stopTimer("factorial_rec", n);
    }
    profiler.createGroup("factorial", "factorial_iter", "factorial_rec");

    profiler.showReport();
    return 0;
}
