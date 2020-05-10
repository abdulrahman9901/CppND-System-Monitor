#include "format.h"

#include <string>

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  string hours = to_string(seconds / 360);
  seconds %= 360;
  string min = to_string(seconds / 60);
  seconds %= 60;

  return hours + ":" + min + ":" + to_string(seconds);
}