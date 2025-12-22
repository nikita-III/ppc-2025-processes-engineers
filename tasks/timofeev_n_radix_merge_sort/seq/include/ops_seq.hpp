#pragma once

#include <cstddef>
#include <vector>

#include "task/include/task.hpp"
#include "timofeev_n_radix_merge_sort/common/include/common.hpp"

namespace timofeev_n_radix_merge_sort {

class TimofeevNRadixMergeSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit TimofeevNRadixMergeSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
  
  // Вспомогательные функции для поразрядной сортировки слиянием
  int getDigit(int num, int digit);
  int getMaxDigits(const std::vector<int>& arr);
  void separateNegativesAndPositives(const std::vector<int>& arr,
                                           std::vector<int>& negatives,
                                           std::vector<int>& positives);
  void distributeToBuckets(const std::vector<int>& arr, int digit,
                                 std::vector<std::vector<int>>& buckets);
  std::vector<int> collectFromBuckets(const std::vector<std::vector<int>>& buckets);
  std::vector<int> radixSortNonNegative(const std::vector<int>& arr);
  std::vector<int> radixSortWithNegatives(const std::vector<int>& arr);
  std::vector<int> mergeTwoArrays(const std::vector<int>& left,
                                        const std::vector<int>& right);
  std::vector<int> radixMergeSortRecursive(const std::vector<int>& arr);
  size_t calculateNextPowerOfTwo(size_t n);
  std::vector<int> padArrayToPowerOfTwo(const std::vector<int>& arr);
  std::vector<int> removeAddedZeros(const std::vector<int>& sorted,
                                          size_t originalSize);
};

}  // namespace timofeev_n_radix_merge_sort
