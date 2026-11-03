#include "detector_filter.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

static int compare_double(const void *a, const void *b) {
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}

/* window <= count means reflection never needs more than one boundary. */
static size_t reflected(size_t i, size_t j, size_t half, size_t count) {
    if (j < half) {
        size_t delta = half - j;
        return i < delta ? delta - i : i - delta;
    }
    size_t delta = j - half, remaining = count - 1 - i;
    return delta > remaining ? count - 1 - (delta - remaining) : i + delta;
}

static size_t edge(size_t i, size_t j, size_t half, size_t count) {
    if (j < half) {
        size_t delta = half - j;
        return i < delta ? 0 : i - delta;
    }
    size_t delta = j - half;
    return delta > count - 1 - i ? count - 1 : i + delta;
}

detector_status detector_filter(const double *samples, size_t count,
                                size_t window, double *output) {
    if (!samples || !output || !count || !window || !(window % 2) ||
        window > count || count > SIZE_MAX / sizeof(double)) {
        return DETECTOR_INVALID_ARGUMENT;
    }
    for (size_t i = 0; i < count; ++i) {
        if (!isfinite(samples[i])) return DETECTOR_NONFINITE_INPUT;
    }
    double *median = malloc(count * sizeof(double));
    double *scratch = malloc(window * sizeof(double));
    if (!median || !scratch) {
        free(median);
        free(scratch);
        return DETECTOR_ALLOCATION_FAILED;
    }
    size_t half = window / 2;
    double scale = 0.0;
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = 0; j < window; ++j)
            scratch[j] = samples[reflected(i, j, half, count)];
        qsort(scratch, window, sizeof(double), compare_double);
        median[i] = scratch[half];
        if (fabs(median[i]) > scale) scale = fabs(median[i]);
    }
    double weight = 1.0 / (double)window;
    for (size_t i = 0; i < count; ++i) {
        double sum = 0.0;
        if (scale != 0.0) {
            for (size_t j = 0; j < window; ++j)
                sum += (median[edge(i, j, half, count)] / scale) * weight;
        }
        output[i] = fmax(-1.0, fmin(1.0, sum)) * scale;
    }
    free(scratch);
    free(median);
    return DETECTOR_OK;
}
