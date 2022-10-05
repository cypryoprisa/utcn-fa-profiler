#include "Profiler.h"

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
* an iterative function for finding the minimum element in a vector
*/
int array_minimum_iter(int *v, int size)
{
    int min = v[0];
    Operation o = profiler.createOperation("array_min_iter", size);
    for(int i=1; i<size; ++i) {
        o.count();
        if(v[i] < min) {
            min = v[i];
        }
    }
    return min;
}

/**
* a recursive function for finding the minimum element in a vector
*/
int array_minimum_rec(int *v, int from, int to, Operation &o)
{
    if(from == to) {
        return v[from];
    } else {
        int middle = (from + to) / 2;
        int m1 = array_minimum_rec(v, from, middle, o);
        int m2 = array_minimum_rec(v, middle + 1, to, o);
        o.count();
        if(m1 < m2) {
            return m1;
        } else {
            return m2;
        }
    }
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
    const int max_size1 = 2000;
    int a[max_size1];
    FillRandomArray(a, max_size1);
    for(n=100; n<max_size1; n+=100) {
        hasDuplicates(a, n); //we're not actually interested in the result
    }

    //third example: array minimum - iterative vs recursive
    profiler.reset("demo-array-min");
    printf("Finding array minimum...\n");
    const int step = 1000;
    const int nr_tests = 100;
    const int max_size2 = step * 100;
    int b[max_size2];
    FillRandomArray(b, max_size2);
    
    for(n=step; n<=max_size2; n+=step) {
        array_minimum_iter(b, n); //we're not actually interested in the result
        
        Operation op = profiler.createOperation("array_min_rec", n);
        array_minimum_rec(b, 0, n-1, op); //we're not actually interested in the result
    }

    for(n=step; n<=max_size2; n+=step) {
        profiler.startTimer("array_min_iter", n);
        for(int test=0; test<nr_tests; ++test) {
            array_minimum_iter(b, n); //we're not actually interested in the result
        }
        profiler.stopTimer("array_min_iter", n);

        Operation op = profiler.createOperation("array_min_rec", n);
        profiler.startTimer("array_min_rec", n);
        for(int test=0; test<nr_tests; ++test) {
            array_minimum_rec(b, 0, n-1, op); //we're not actually interested in the result
        }
        profiler.stopTimer("array_min_rec", n);
    }
    profiler.createGroup("array_min", "array_min_iter", "array_min_rec");

    profiler.showReport();
    return 0;
}
