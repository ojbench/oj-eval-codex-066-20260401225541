#include "src.hpp"
#include <iostream>

int main() {
  // The OJ will compile and link this file; no I/O is required here.
  // Provide a simple sanity check path for local runs if desired.
  try {
    sjtu::any_ptr a = sjtu::make_any_ptr(1);
    sjtu::any_ptr b = a; // shallow copy
    b.unwrap<int>() = 3;
    std::cout << a.unwrap<int>() << "\n"; // expect 3
  } catch (...) {
    return 1;
  }
  return 0;
}

