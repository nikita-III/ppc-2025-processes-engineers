#include "timofeev_n_radix_merge_sort/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

#include "timofeev_n_radix_merge_sort/common/include/common.hpp"

namespace timofeev_n_radix_merge_sort {

TimofeevNRadixMergeSEQ::TimofeevNRadixMergeSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>(0);
}

bool TimofeevNRadixMergeSEQ::ValidationImpl() {
  return !GetInput().empty();
}

bool TimofeevNRadixMergeSEQ::PreProcessingImpl() {
  return true;
}

int TimofeevNRadixMergeSEQ::getDigit(int num, int digit) {
  if (num < 0) {
    num = -num;
  }
  return (num / static_cast<int>(pow(10, digit))) % 10;
}

int TimofeevNRadixMergeSEQ::getMaxDigits(const std::vector<int>& arr) {
  int maxAbs = 0;
  
  for (int num : arr) {
    int absNum = std::abs(num);
    if (absNum > maxAbs) {
      maxAbs = absNum;
    }
  }
  
  int digits = 0;
  while (maxAbs > 0) {
    maxAbs /= 10;
    digits++;
  }
  
  return digits == 0 ? 1 : digits;
}

void TimofeevNRadixMergeSEQ::separateNegativesAndPositives(const std::vector<int>& arr,
                                                          std::vector<int>& negatives,
                                                          std::vector<int>& positives) {
  for (int num : arr) {
    if (num < 0) {
      negatives.push_back(-num);
    } else {
      positives.push_back(num);
    }
  }
}

void TimofeevNRadixMergeSEQ::distributeToBuckets(const std::vector<int>& arr, int digit,
                                                std::vector<std::vector<int>>& buckets) {
  for (int num : arr) {
    int digitVal = TimofeevNRadixMergeSEQ::getDigit(num, digit);
    buckets[digitVal].push_back(num);
  }
}

std::vector<int> TimofeevNRadixMergeSEQ::collectFromBuckets(const std::vector<std::vector<int>>& buckets) {
  std::vector<int> result;
  
  for (int i = 0; i < 10; i++) {
    const std::vector<int>& bucket = buckets[i];
    result.insert(result.end(), bucket.begin(), bucket.end());
  }
  
  return result;
}

std::vector<int> TimofeevNRadixMergeSEQ::radixSortNonNegative(const std::vector<int>& arr) {
  std::vector<int> result = arr;
  int maxDigits = TimofeevNRadixMergeSEQ::getMaxDigits(arr);
  
  for (int digit = 0; digit < maxDigits; digit++) {
    std::vector<std::vector<int>> buckets(10);
    TimofeevNRadixMergeSEQ::distributeToBuckets(result, digit, buckets);
    result = TimofeevNRadixMergeSEQ::collectFromBuckets(buckets);
  }
  
  return result;
}

std::vector<int> TimofeevNRadixMergeSEQ::radixSortWithNegatives(const std::vector<int>& arr) {
  std::vector<int> negatives;
  std::vector<int> positives;
  TimofeevNRadixMergeSEQ::separateNegativesAndPositives(arr, negatives, positives);
  
  negatives = TimofeevNRadixMergeSEQ::radixSortNonNegative(negatives);
  positives = TimofeevNRadixMergeSEQ::radixSortNonNegative(positives);
  
  std::vector<int> result;
  result.reserve(arr.size());
  
  for (auto it = negatives.rbegin(); it != negatives.rend(); ++it) {
    result.push_back(-(*it));
  }
  
  result.insert(result.end(), positives.begin(), positives.end());
  
  return result;
}

std::vector<int> TimofeevNRadixMergeSEQ::mergeTwoArrays(const std::vector<int>& left,
                                                       const std::vector<int>& right) {
  std::vector<int> result;
  size_t leftIdx = 0;
  size_t rightIdx = 0;
  size_t leftSize = left.size();
  size_t rightSize = right.size();
  
  while (leftIdx < leftSize && rightIdx < rightSize) {
    if (left[leftIdx] <= right[rightIdx]) {
      result.push_back(left[leftIdx]);
      leftIdx++;
    } else {
      result.push_back(right[rightIdx]);
      rightIdx++;
    }
  }
  
  while (leftIdx < leftSize) {
    result.push_back(left[leftIdx]);
    leftIdx++;
  }
  
  while (rightIdx < rightSize) {
    result.push_back(right[rightIdx]);
    rightIdx++;
  }
  
  return result;
}

std::vector<int> TimofeevNRadixMergeSEQ::radixMergeSortRecursive(const std::vector<int>& arr) {
  if (arr.size() <= 32) {
    return TimofeevNRadixMergeSEQ::radixSortWithNegatives(arr);
  }
  
  size_t mid = arr.size() / 2;
  std::vector<int> leftPart(arr.begin(), arr.begin() + mid);
  std::vector<int> rightPart(arr.begin() + mid, arr.end());
  
  leftPart = TimofeevNRadixMergeSEQ::radixMergeSortRecursive(leftPart);
  rightPart = TimofeevNRadixMergeSEQ::radixMergeSortRecursive(rightPart);
  
  return TimofeevNRadixMergeSEQ::mergeTwoArrays(leftPart, rightPart);
}

size_t TimofeevNRadixMergeSEQ::calculateNextPowerOfTwo(size_t n) {
  size_t nextPower = 1;
  while (nextPower < n) {
    nextPower <<= 1;
  }
  
  return nextPower;
}

std::vector<int> TimofeevNRadixMergeSEQ::padArrayToPowerOfTwo(const std::vector<int>& arr) {
  size_t n = arr.size();
  size_t nextPower = TimofeevNRadixMergeSEQ::calculateNextPowerOfTwo(n);
  
  if (nextPower == n) return arr;
  
  std::vector<int> padded = arr;
  padded.resize(nextPower, 0);
  return padded;
}

std::vector<int> TimofeevNRadixMergeSEQ::removeAddedZeros(const std::vector<int>& sorted,
                                                         size_t originalSize) {
  // Подсчитываем количество нулей в исходном массиве
  int originalZeroCount = 0;
  for (int num : GetInput()) {
    if (num == 0) {
      originalZeroCount++;
    }
  }
  
  // Собираем результат, сохраняя правильное количество нулей
  std::vector<int> result;
  result.reserve(originalSize);
  
  int zeroCount = 0;
  for (int num : sorted) {
    if (result.size() >= originalSize) break;
    
    if (num == 0) {
      if (zeroCount < originalZeroCount) {
        result.push_back(0);
        zeroCount++;
      }
    } else {
      result.push_back(num);
    }
  }
  
  return result;
}

bool TimofeevNRadixMergeSEQ::RunImpl() {
  std::vector<int> input = GetInput();
  size_t originalSize = input.size();
  
  std::vector<int> padded = TimofeevNRadixMergeSEQ::padArrayToPowerOfTwo(input);
  std::vector<int> sorted = TimofeevNRadixMergeSEQ::radixMergeSortRecursive(padded);
  sorted = TimofeevNRadixMergeSEQ::removeAddedZeros(sorted, originalSize);
  
  GetOutput() = sorted;
  return true;
}

bool TimofeevNRadixMergeSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace timofeev_n_radix_merge_sort
