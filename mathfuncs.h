#ifndef _MATHFUNCS_H
#define _MATHFUNCS_H

inline int minimum(int x, int y)
{
  return y ^ ((x ^ y) & -(x < y));
}

inline int maximum(int x, int y)
{
  return x ^ ((x ^ y) & -(x < y));
}

#endif