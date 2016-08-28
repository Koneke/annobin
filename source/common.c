#include <stdint.h>

int min(int a, int b) { return a < b ? a : b; }

int max(int a, int b) { return a > b ? a : b; }

int clamp(int value, int low, int high) { return min(max(low, value), high); }
