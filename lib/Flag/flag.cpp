#include "flag.h"

String extractFlag(const String& message, const uint8_t* idx, size_t n) {
  String out;
  out.reserve(n);
  for (size_t i = 0; i < n; i++) {
    if (idx[i] < message.length()) {
      out += message[idx[i]];
    }
  }
  return out;
}
