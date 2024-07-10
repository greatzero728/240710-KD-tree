#ifndef PRNG_H
#define PRNG_H

namespace CS170 {
    namespace Utils {
        unsigned rand();                        // returns a random 32-bit integer
        float    frand();                       // return a random 32-bit floating point number
        void     srand(unsigned, unsigned);     // seed the generator
        int      Random(int low, int high);     // range
        float    Random(float low, float high); // range
    }
}

#endif