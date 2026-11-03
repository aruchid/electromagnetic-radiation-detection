#ifndef DETECTOR_FILTER_H
#define DETECTOR_FILTER_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DETECTOR_OK = 0,
    DETECTOR_INVALID_ARGUMENT = 1,
    DETECTOR_NONFINITE_INPUT = 2,
    DETECTOR_ALLOCATION_FAILED = 3
} detector_status;

/* Batch median filter (reflect padding), then moving mean (edge padding).
 * Input: count finite doubles, odd window in [1, count]. Output: count doubles.
 * Input and output may alias. On error, output is unchanged.
 * Not an RF demodulator or a causal streaming filter. No fast-math support.
 */
detector_status detector_filter(const double *samples, size_t count,
                                size_t window, double *output);
#ifdef __cplusplus
}
#endif
#endif
