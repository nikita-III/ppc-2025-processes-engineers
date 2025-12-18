#pragma once

#include "timofeev_n_ribbon_scheme_only_a/common/include/common.hpp"
#include "task/include/task.hpp"

namespace timofeev_n_ribbon_scheme_only_a {

class TimofeevNRibbonSchemeOnlyAMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit TimofeevNRibbonSchemeOnlyAMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
  void sendingAParts (MatrixType &A, int &size, size_t &k);
  void receivingCParts (MatrixType &Cmatr, int &size, size_t &k, size_t &bRowSize);
  int calculatingCElement (MatrixType &aPart, MatrixType &bCopy, size_t &i, size_t &j);
  void calculatingCPart (size_t &k, MatrixType &aPart, MatrixType &bCopy, MatrixType &cPart);
  void BroadcastingParameters(size_t &k, size_t &aSize, size_t &aRowSize, size_t &bSize, size_t &bRowSize);
  void BroadcastingB(MatrixType &B);
};

}  // namespace timofeev_n_ribbon_scheme_only_a
