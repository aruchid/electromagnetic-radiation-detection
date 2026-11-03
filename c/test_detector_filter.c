#include "detector_filter.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    double samples[] = {1, 2, 100, 4, 5}, output[5];
    /* Median [2,2,4,5,4], edge-padded mean. */
    double expected[] = {2, 8.0/3, 11.0/3, 13.0/3, 13.0/3};
    assert(detector_filter(samples, 5, 3, output) == DETECTOR_OK);
    for (size_t i = 0; i < 5; ++i) assert(fabs(output[i]-expected[i]) < 1e-14);
    assert(detector_filter(samples, 5, 3, samples) == DETECTOR_OK);
    for (size_t i = 0; i < 5; ++i) assert(fabs(samples[i]-expected[i]) < 1e-14);
    double maxes[105], result[105];
    for (int sign = -1; sign <= 1; sign += 2) {
        for (size_t i = 0; i < 105; ++i) maxes[i] = sign * DBL_MAX;
        assert(detector_filter(maxes, 105, 101, result) == DETECTOR_OK);
        for (size_t i = 0; i < 105; ++i) {
            assert(isfinite(result[i]));
            assert(fabs(result[i]/maxes[i]-1) < 1e-13);
        }
    }
    double bad[] = {1, NAN, 3}, sentinel[] = {7, 8, 9};
    assert(detector_filter(bad, 3, 3, sentinel) == DETECTOR_NONFINITE_INPUT);
    assert(sentinel[0]==7 && sentinel[1]==8 && sentinel[2]==9);
    bad[1] = INFINITY;
    assert(detector_filter(bad, 3, 3, sentinel) == DETECTOR_NONFINITE_INPUT);
    assert(detector_filter(NULL, 3, 3, sentinel) == DETECTOR_INVALID_ARGUMENT);
    assert(detector_filter(bad, 0, 1, sentinel) == DETECTOR_INVALID_ARGUMENT);
    assert(detector_filter(bad, 3, 2, sentinel) == DETECTOR_INVALID_ARGUMENT);
    assert(detector_filter(bad, 3, 5, sentinel) == DETECTOR_INVALID_ARGUMENT);
    assert(detector_filter(bad, SIZE_MAX, 1, sentinel) == DETECTOR_INVALID_ARGUMENT);
    double one = 3.5;
    assert(detector_filter(&one, 1, 1, &one) == DETECTOR_OK && one == 3.5);
    puts("C filter tests passed");
    return 0;
}
