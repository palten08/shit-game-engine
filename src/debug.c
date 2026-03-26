#include <stdio.h>

#include "../include/debug.h"
#include "../include/types.h"
#include "../include/matrix_operations.h"
#include "../include/vector_operations.h"
#include "../include/coordinates.h"
#include "../include/clipping.h"
#include "../include/virtual_camera.h"

void print_stack_usage() {
    FILE *f = fopen("/proc/self/status", "r");
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "VmStk:", 6) == 0 || strncmp(line, "VmPeak:", 7) == 0) {
            printf("%s", line);
        }
    }
    fclose(f);
}