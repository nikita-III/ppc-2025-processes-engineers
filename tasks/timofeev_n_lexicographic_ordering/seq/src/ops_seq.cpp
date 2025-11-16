#include "timofeev_n_lexicographic_ordering/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "timofeev_n_lexicographic_ordering/common/include/common.hpp"
#include "util/include/util.hpp"

namespace timofeev_n_lexicographic_ordering {

TimofeevNLexicographicOrderingSEQ::TimofeevNLexicographicOrderingSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::pair<int, int>(1, 1);
}

bool TimofeevNLexicographicOrderingSEQ::ValidationImpl() {
  return (GetInput().first != "") && (GetInput().second != "");
}

bool TimofeevNLexicographicOrderingSEQ::PreProcessingImpl() {
  return 1;
}

bool TimofeevNLexicographicOrderingSEQ::Compare(char a, char b) {
  return a < b;
}

// bool SCoompare(char a, char b) {
//   return a < b;
// }

bool TimofeevNLexicographicOrderingSEQ::RunImpl() {
  auto input = GetInput();
  if (input.first.length() < 1 || input.second.length() < 1) {
    return false;
  }

  // only true if comparison is true on every step
  for (int i = 0; i < (int)input.first.length() - 1; i++) {
    GetOutput().first &= input.first[i] <= input.first[i + 1];
  }
  for (int i = 0; i < (int)input.second.length() - 1; i++) {
    GetOutput().second &= input.second[i] <= input.second[i + 1];
  }

  return GetOutput().first >= 0 && GetOutput().second >= 0;
}

bool TimofeevNLexicographicOrderingSEQ::PostProcessingImpl() {
  return 1;
}

}
