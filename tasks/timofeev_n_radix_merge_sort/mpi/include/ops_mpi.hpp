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
  void RadixMergeSort(std::vector<int> &part);
  int GetDigit(int num, int digit);
  int GetMaxDigits(const std::vector<int>& arr);
  void SplitPosNeg(const std::vector<int>& Arr, 
                  std::vector<int>& Negative, 
                  std::vector<int>& Positive);
  void RadixMergeBucketHelpingFunction(std::vector<int> &Part, int Digit);
  void SliyanieHelp(std::vector<std::vector<int>> &Received, std::vector<int> &Indexes, std::vector<int> &Out, int &i);
  void Sliyanie(std::vector<std::vector<int>> &Received, std::vector<int> &Out);
  void HandleTwoProcesses(std::vector<int> &In, std::vector<int> &Out);
  bool HandleZeroRank(std::vector<int> &In, std::vector<int> &Out, int &size);
};

}  // namespace timofeev_n_radix_merge_sort
