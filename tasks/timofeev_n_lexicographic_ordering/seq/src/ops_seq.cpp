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
  return 1;
}

bool TimofeevNLexicographicOrderingSEQ::PreProcessingImpl() {
  return 1;
}

bool TimofeevNLexicographicOrderingSEQ::RunImpl() {
  auto input = GetInput();

  // only true if comparison is true on every step
  for (int i = 0; i < (int)input.first.length() - 1; i++) {
    GetOutput().first &= input.first[i] <= input.first[i + 1];
  }
  for (int i = 0; i < (int)input.second.length() - 1; i++) {
    GetOutput().second &= input.second[i] <= input.second[i + 1];
  }

  return 1;
}

bool TimofeevNLexicographicOrderingSEQ::PostProcessingImpl() {
  return 1;
}

}  // namespace timofeev_n_lexicographic_ordering
