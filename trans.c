/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose (int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */


#define PARTITION(length) \
  for (ia = 0; ia < N; ia += length) \
  for (ja = 0; ja < M; ja += length) \
  for (ib = ia; ib < ia + length && ib < N; ib++) { \
  for (jb = ja; jb < ja + length && jb < M; jb++)

char transpose_submit_desc[] = "Transpose submission";
void trans (int M, int N, int A[N][M], int B[M][N]);
void transpose_submit (int M, int N, int A[N][M], int B[M][N]) {
  int ia, ja, ib, jb;
  int temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
  switch (M) {
    case 32:
      PARTITION(8) {
              if (ib != jb)
                B[jb + 0][ib + 0] = A[ib + 0][jb + 0];
              else {
                temp1 = A[ib + 0][jb + 0];
                temp2 = ib;
              }
            }
            if (ia == ja)
              B[temp2][temp2] = temp1;
          }
      break;
    case 61:
      PARTITION(23) {
              if (ib != jb)
                B[jb + 0][ib + 0] = A[ib + 0][jb + 0];
              else {
                temp1 = A[ib + 0][jb + 0];
                temp2 = ib;
              }
            }
            if (ia == ja)
              B[temp2][temp2] = temp1;
          }
      break;
    case 64:
      for (ia = 0; ia < M; ia += 8) {
        for (ja = 0; ja < M; ja += 8) {
          for (ib = ia; ib < ia + 4; ib++) {
            temp1 = A[ib + 0][ja + 0];
            temp2 = A[ib + 0][ja + 1];
            temp3 = A[ib + 0][ja + 2];
            temp4 = A[ib + 0][ja + 3];
            temp5 = A[ib + 0][ja + 4];
            temp6 = A[ib + 0][ja + 5];
            temp7 = A[ib + 0][ja + 6];
            temp8 = A[ib + 0][ja + 7];
            B[ja + 0][ib + 0] = temp1;
            B[ja + 0][ib + 4] = temp5;
            B[ja + 1][ib + 0] = temp2;
            B[ja + 1][ib + 4] = temp6;
            B[ja + 2][ib + 0] = temp3;
            B[ja + 2][ib + 4] = temp7;
            B[ja + 3][ib + 0] = temp4;
            B[ja + 3][ib + 4] = temp8;
          }
          for (jb = ja; jb < ja + 4; jb++) {
            temp5 = A[ia + 4][jb + 0];
            temp6 = A[ia + 5][jb + 0];
            temp7 = A[ia + 6][jb + 0];
            temp8 = A[ia + 7][jb + 0];
            temp1 = B[jb + 0][ia + 4];
            temp2 = B[jb + 0][ia + 5];
            temp3 = B[jb + 0][ia + 6];
            temp4 = B[jb + 0][ia + 7];
            B[jb + 0][ia + 4] = temp5;
            B[jb + 0][ia + 5] = temp6;
            B[jb + 0][ia + 6] = temp7;
            B[jb + 0][ia + 7] = temp8;
            B[jb + 4][ia + 0] = temp1;
            B[jb + 4][ia + 1] = temp2;
            B[jb + 4][ia + 2] = temp3;
            B[jb + 4][ia + 3] = temp4;
          }
          for (jb = ja + 4; jb < ja + 8; jb++) {
            temp1 = A[ia + 4][jb + 0];
            temp2 = A[ia + 5][jb + 0];
            temp3 = A[ia + 6][jb + 0];
            temp4 = A[ia + 7][jb + 0];
            B[jb + 0][ia + 4] = temp1;
            B[jb + 0][ia + 5] = temp2;
            B[jb + 0][ia + 6] = temp3;
            B[jb + 0][ia + 7] = temp4;
          }
        }
      }
      break;
    default:
      trans (M, N, A, B);
  }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans (int M, int N, int A[N][M], int B[M][N]) {
  int i, j, tmp;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; j++) {
      tmp = A[i][j];
      B[j][i] = tmp;
    }
  }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions () {
  /* Register your solution function */
  registerTransFunction (transpose_submit, transpose_submit_desc);

  /* Register any additional transpose functions */
  registerTransFunction (trans, trans_desc);

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose (int M, int N, int A[N][M], int B[M][N]) {
  int i, j;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; ++j) {
      if (A[i][j] != B[j][i]) {
        return 0;
      }
    }
  }
  return 1;
}

