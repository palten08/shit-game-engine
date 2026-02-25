#include "../include/matrix.h"

/** Creates an identity matrix. */
/** The identity matrix is a square matrix with ones on the main diagonal and zeros elsewhere. */
/** Multiplying it against a vector4 leaves the vector unchanged. */
/** Returns: A 4x4 identity matrix. */
Matrix4 identity() {
    Matrix4 result = {0};
    for (int i = 0; i < 4; i++) {
        result.m[i][i] = 1.0f;
    }
    return result;
}