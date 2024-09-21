//
// Created by sohaibalam on 26/05/24.
//
#include "interval.h"
const interval interval::empty(infinity, -infinity);
const interval interval::universe(-infinity, infinity);

interval operator+(const interval &ival, float displacement) {
  return interval(ival.min + displacement, ival.max + displacement);
}