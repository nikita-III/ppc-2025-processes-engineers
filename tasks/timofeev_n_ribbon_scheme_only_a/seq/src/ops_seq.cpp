#include "timofeev_n_ribbon_scheme_only_a/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "timofeev_n_ribbon_scheme_only_a/common/include/common.hpp"
#include "util/include/util.hpp"

namespace timofeev_n_ribbon_scheme_only_a {

TimofeevNRibbonSchemeOnlyASEQ::TimofeevNRibbonSchemeOnlyASEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<std::vector<int>>(0, std::vector<int>(0));
}

bool TimofeevNRibbonSchemeOnlyASEQ::ValidationImpl() {
  std::vector<std::vector<int>>* A = &(GetInput().first);
  std::vector<std::vector<int>>* B = &(GetInput().second);
  size_t n = A->size();
  size_t m2 = A[0].size();
  size_t m1 = B->size();
  size_t k = B[0].size();
  if (m2 != m1 || n == 0 || m1 == 0 || m2 == 0 || k == 0) {
    return false;
  }
  return 1;
}

bool TimofeevNRibbonSchemeOnlyASEQ::PreProcessingImpl() {
  return 1;
}

int TimofeevNRibbonSchemeOnlyASEQ::calculatingCElement(MatrixType &A, MatrixType &B, size_t &i, size_t &j) {
  int summ = 0;
  for (size_t kk = 0; kk < A[0].size(); kk++)
    summ += A[i][kk] * B[kk][j];
  return summ;
}

bool TimofeevNRibbonSchemeOnlyASEQ::RunImpl() {
  MatrixType A = GetInput().first;
  MatrixType B = GetInput().second;
  std::vector<std::vector<int>> C(A.size(), std::vector<int>(B[0].size()));
  for (size_t i = 0; i < A.size(); i++)
    for (size_t j = 0; j < B[0].size(); j++)
      C[i][j] = calculatingCElement(A, B, i, j);
  GetOutput() = C;
  return 1;
}

bool TimofeevNRibbonSchemeOnlyASEQ::PostProcessingImpl() {
  return 1;
}

}  // namespace timofeev_n_ribbon_scheme_only_a
