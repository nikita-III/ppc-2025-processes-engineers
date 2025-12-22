#pragma once

#include <cstddef>
#include <vector>

#include "task/include/task.hpp"
#include "timofeev_n_radix_merge_sort/common/include/common.hpp"

namespace timofeev_n_radix_merge_sort {

class TimofeevNRadixMergeMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit TimofeevNRadixMergeMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
  std::vector<int> statusQuo;
  int DecideIfProceeding(std::vector<std::vector<int>> Receiving, size_t ConclusionSize);
  void RadixMergeSort(std::vector<int> &part);
  void DistributeReceiveParts(std::vector<int> &statusQuo, std::vector<std::vector<int>> &Received, int Shag);
  int GetPervTwoPower(int size);
};

}  // namespace timofeev_n_radix_merge_sort
