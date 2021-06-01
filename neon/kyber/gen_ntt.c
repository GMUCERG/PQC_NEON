#include <stdio.h>
#include "params.h"
#include "reduce.h"

#define KYBER_ROOT_OF_UNITY 17

const int16_t ref_zetas[128] = {
    -1044, -758, -359, -1517, 1493, 1422, 287, 202,
    -171, 622, 1577, 182, 962, -1202, -1474, 1468,
    573, -1325, 264, 383, -829, 1458, -1602, -130,
    -681, 1017, 732, 608, -1542, 411, -205, -1571,
    1223, 652, -552, 1015, -1293, 1491, -282, -1544,
    516, -8, -320, -666, -1618, -1162, 126, 1469,
    -853, -90, -271, 830, 107, -1421, -247, -951,
    -398, 961, -1508, -725, 448, -1065, 677, -1275,
    -1103, 430, 555, 843, -1251, 871, 1550, 105,
    422, 587, 177, -235, -291, -460, 1574, 1653,
    -246, 778, 1159, -147, -777, 1483, -602, 1119,
    -1590, 644, -872, 349, 418, 329, -156, -75,
    817, 1097, 603, 610, 1322, -1285, -1465, 384,
    -1215, -136, 1218, -1335, -874, 220, -1187, -1659,
    -1185, -1530, -1278, 794, -1510, -854, -870, 478,
    -108, -308, 996, 991, 958, -1460, 1522, 1628};

static int16_t fqmul(int16_t a, int16_t b)
{
  return montgomery_reduce((int32_t)a * b);
}

static const uint8_t tree[128] = {
    0, 64, 32, 96, 16, 80, 48, 112, 8, 72, 40, 104, 24, 88, 56, 120,
    4, 68, 36, 100, 20, 84, 52, 116, 12, 76, 44, 108, 28, 92, 60, 124,
    2, 66, 34, 98, 18, 82, 50, 114, 10, 74, 42, 106, 26, 90, 58, 122,
    6, 70, 38, 102, 22, 86, 54, 118, 14, 78, 46, 110, 30, 94, 62, 126,
    1, 65, 33, 97, 17, 81, 49, 113, 9, 73, 41, 105, 25, 89, 57, 121,
    5, 69, 37, 101, 21, 85, 53, 117, 13, 77, 45, 109, 29, 93, 61, 125,
    3, 67, 35, 99, 19, 83, 51, 115, 11, 75, 43, 107, 27, 91, 59, 123,
    7, 71, 39, 103, 23, 87, 55, 119, 15, 79, 47, 111, 31, 95, 63, 127};

void init_ntt(int16_t zetas[128])
{
  unsigned int i;
  int16_t tmp[128];

  tmp[0] = MONT;
  for (i = 1; i < 128; i++)
    tmp[i] = fqmul(tmp[i - 1], MONT * KYBER_ROOT_OF_UNITY % KYBER_Q);

  for (i = 0; i < 128; i++)
  {
    zetas[i] = tmp[tree[i]];
    if (zetas[i] > KYBER_Q / 2)
      zetas[i] -= KYBER_Q;
    if (zetas[i] < -KYBER_Q / 2)
      zetas[i] += KYBER_Q;
  }
}

void print_array(int16_t *arr, int k, const char* string)
{
  int i, j;
  printf("%s [%d] = {\n", string, k);
  for (i = 0; i < k; i += 8)
  {
    if (i == k / 2)
    {
      printf("\n");
    }
    for (j = i; j < i + 8; j++)
    {
      printf("%d, ", arr[j]);
    }
    printf("\n");
  }
  printf("};\n\n");
}

void init_neon_ntt(int16_t *neon_zetas)
{
  int16_t zetas[128];
  unsigned int i, j, loop, k = 0, k_ref = 2;

  init_ntt(zetas);
  for (i = 0; i < 128; i++)
  {
    if (zetas[i] != ref_zetas[i])
    {
      printf("Error\n");
    }
  }

  for (loop = 0; loop < 2; loop++)
  {
    // Layer 6
    neon_zetas[k++] = zetas[k_ref];
    k_ref += 2 + loop;

    // Layer 5
    for (i = 0; i < 2; i++)
    {
      neon_zetas[k++] = zetas[k_ref++];
    }
    k_ref += 2 * (1 + loop);

    // Layer 4
    for (i = 0; i < 4; i++)
    {
      neon_zetas[k++] = zetas[k_ref++];
    }
    k_ref += 4 * (1 + loop);

    // Layer 3
    for (i = 0; i < 8; i++)
    {
      neon_zetas[k++] = zetas[k_ref++];
    }
    k_ref += 8 * (1 + loop);

    // Redundant, align memory
    neon_zetas[k++] = zetas[k_ref];

    // Layer 2: Here comes the duplication
    for (i = 0; i < 8; i++)
    {
      for (j = 0; j < 4; j++)
      {
        neon_zetas[k++] = zetas[k_ref];
      }
      k_ref += 2;
      for (j = 0; j < 4; j++)
      {
        neon_zetas[k++] = zetas[k_ref];
      }
      k_ref -= 1;

      if (i & 1)
      {
        k_ref += 2;
      }
    }
    k_ref += 16 * (1 + loop);

    // Layer 1
    for (i = 0; i < 32; i++)
    {
      neon_zetas[k++] = zetas[k_ref++];
    }
    k_ref = 3;
  }

  neon_zetas[15] = zetas[1];
}

void init_neon_invntt(int16_t *neon_zetas_inv)
{
  int16_t zetas[128];
  unsigned int i, j, loop, k = 0, k_ref = 127;
  const int16_t f = 1441; // mont^2/128

  init_ntt(zetas);
  for (i = 0; i < 128; i++)
  {
    if (zetas[i] != ref_zetas[i])
    {
      printf("Error\n");
    }
  }

  for (loop = 0; loop < 2; loop++)
  {
    // Layer 1
    for (i = 0; i < 32; i++)
    {
      neon_zetas_inv[k++] = zetas[k_ref--];
    }
    k_ref -= 16 * (2 - loop);

    // Layer 2: Here comes the duplication
    for (i = 0; i < 8; i++)
    {
      for (j = 0; j < 4; j++)
      {
        neon_zetas_inv[k++] = zetas[k_ref];
      }
      k_ref -= 2;
      for (j = 0; j < 4; j++)
      {
        neon_zetas_inv[k++] = zetas[k_ref];
      }
      k_ref += 1;

      if (i & 1)
      {
        k_ref -= 2;
      }
    }
    k_ref -= 8 * (2 - loop);

    // Layer 3
    for (i = 0; i < 8; i++)
    {
      for (j = 0; j < 4; j++)
      {
        neon_zetas_inv[k++] = zetas[k_ref];
      }
      k_ref -= 1;
    }
    k_ref -= 4 * (2 - loop);

    // Layer 4
    for (i = 0; i < 4; i++)
    {
      neon_zetas_inv[k++] = zetas[k_ref--];
    }
    k_ref -= 2 * (2 - loop);

    // Layer 5
    for (i = 0; i < 2; i++)
    {
      neon_zetas_inv[k++] = zetas[k_ref--];
    }
    k_ref -= (2 - loop);

    // Layer 6
    neon_zetas_inv[k++] = zetas[k_ref];
    // // Redundant, align memory
    neon_zetas_inv[k++] = zetas[k_ref];

    k_ref = 95;
  }
  neon_zetas_inv[271] = fqmul(zetas[1], f);
}

int main()
{
  int16_t neon_zetas[224] = {0}, neon_zetas_inv[272] = {0};

  init_neon_ntt(neon_zetas);
  print_array(neon_zetas, 224, "const int16_t neon_zetas");

  init_neon_invntt(neon_zetas_inv);
  print_array(neon_zetas_inv, 272, "const int16_t neon_zetas_inv");

  return 0;
}
