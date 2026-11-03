#include "detector_filter.h"
#include <math.h>
#include <stdio.h>

int main(void) {
    enum { COUNT = 600 };
    double raw[COUNT], filtered[COUNT], truth[COUNT];
    double raw_error = 0.0, filtered_error = 0.0;
    for (size_t i = 0; i < COUNT; ++i) {
        double x = 6.0 * (double)i / (COUNT - 1);
        truth[i] = 1.0 + 0.3 * sin(2.0 * x);
        raw[i] = truth[i] + 0.1 * sin(41.0 * x) + (i % 67 == 0 ? 1.5 : 0.0);
    }
    if (detector_filter(raw, COUNT, 9, filtered) != DETECTOR_OK) return 1;
    puts("sample,raw,filtered,synthetic_truth");
    for (size_t i = 0; i < COUNT; ++i) {
        double a = raw[i] - truth[i], b = filtered[i] - truth[i];
        raw_error += a * a;
        filtered_error += b * b;
        printf("%zu,%.17g,%.17g,%.17g\n", i, raw[i], filtered[i], truth[i]);
    }
    fprintf(stderr, "Synthetic C demo: raw_rmse=%.9g filtered_rmse=%.9g\n",
            sqrt(raw_error / COUNT), sqrt(filtered_error / COUNT));
    return filtered_error < raw_error * 0.25 ? 0 : 1;
}
