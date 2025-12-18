#pragma once

#include "timofeev_n_ribbon_scheme_only_a/common/include/common.hpp"
#include "task/include/task.hpp"

namespace timofeev_n_ribbon_scheme_only_a {

class TimofeevNRibbonSchemeOnlyASEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit TimofeevNRibbonSchemeOnlyASEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
  int calculatingCElement(MatrixType &A, MatrixType &B, size_t &i, size_t &j);
};

}  // namespace timofeev_n_ribbon_scheme_only_a
