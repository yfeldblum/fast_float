#include "fast_float/fast_float.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <ios>
#include <iostream>

template <typename T> char *to_string(T d, char *buffer) {
  auto written = std::snprintf(buffer, 128, "%.*e",
                               64, d);
  return buffer + written;
}

void all_32bit_values() {
  char buffer[128];
  for (uint64_t w = 0; w <= 0xFFFFFFFF; w++) {
    float v32;
    if ((w % 1048576) == 0) {
      std::cout << ".";
      std::cout.flush();
    }
    uint32_t word = uint32_t(w);
    memcpy(&v32, &word, sizeof(v32));
    double v = v32;

    {
      const char *string_end = to_string(v, buffer);
      double result_value;
      auto result = fast_float::from_chars(buffer, string_end, result_value);
      // Starting with version 4.0 for fast_float, we return result_out_of_range if the
      // value is either too small (too close to zero) or too large (effectively infinity).
      // So std::errc::result_out_of_range is normal for well-formed input strings.
      if (result.ec != std::errc() && result.ec != std::errc::result_out_of_range) {
        std::cerr << "parsing error ? " << buffer << std::endl;
        abort();
      }
      if (std::isnan(v)) {
        if (!std::isnan(result_value)) {
          std::cerr << "not nan" << buffer << std::endl;
          abort();
        }
      } else if(copysign(1,result_value) != copysign(1,v)) {
        std::cerr << "I got " << std::hexfloat << result_value << " but I was expecting " << v
              << std::endl;
        abort();
      } else if (std::isnan(v)) {
        if (!std::isnan(result_value)) {
          std::cerr << "not nan" << buffer << std::endl;
          abort();
        }
      } else if (result_value != v) {
        std::cerr << "no match ? " << buffer << std::endl;
        std::cout << "started with " << std::hexfloat << v << std::endl;
        std::cout << "got back " << std::hexfloat << result_value << std::endl;
        std::cout << std::dec;
        abort();
      }
    }
  }
  std::cout << std::endl;
}

int main() {
  all_32bit_values();
  std::cout << std::endl;
  std::cout << "all ok" << std::endl;
  return EXIT_SUCCESS;
}
