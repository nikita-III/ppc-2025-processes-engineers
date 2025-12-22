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

int TimofeevNRadixMergeSEQ::GetDigit(int num, int digit) {
  int absNum = std::abs(num);

  // потому что без использования возведения в степень (как функции)
  for (int i = 0; i < digit; i++) {
    absNum /= 10;
  }

  return absNum % 10;
}

int TimofeevNRadixMergeSEQ::GetMaxDigits(const std::vector<int> &arr) {
  if (arr.empty()) {
    return 0;
  }

  // Находим максимальное по модулю число
  int MaxAbs = 0;
  for (int num : arr) {
    int AbsNum = std::abs(num);
    if (AbsNum > MaxAbs) {
      MaxAbs = AbsNum;
    }
  }

  int digits = 0;
  while (MaxAbs > 0) {
    digits++;
    MaxAbs /= 10;
  }
  return digits == 0 ? 1 : digits;  // минимум 1 разряд
}

void TimofeevNRadixMergeSEQ::SplitPosNeg(const std::vector<int> &Arr, std::vector<int> &Negative,
                                         std::vector<int> &Positive) {
  for (int num : Arr) {
    if (num < 0) {
      Negative.push_back(-num);
    } else {
      Positive.push_back(num);
    }
  }
}

void TimofeevNRadixMergeSEQ::RadixMergeBucketHelpingFunction(std::vector<int> &Part, int Digit) {
  std::vector<std::vector<int>> Buckets(10);
  for (int num : Part) {
    int d = GetDigit(num, Digit);
    Buckets[d].push_back(num);
  }

  // Собираем числа обратно в вектор
  Part.clear();
  for (int i = 0; i < 10; i++) {
    for (int num : Buckets[i]) {
      Part.push_back(num);
    }
  }
}

void TimofeevNRadixMergeSEQ::RadixMergeSort(std::vector<int> &Part) {
  if (Part.size() <= 1) {
    return;
  }

  // Разделяем на отрицательные и положительные числа
  std::vector<int> Negative, Positive;
  SplitPosNeg(Part, Negative, Positive);

  // Сортируем отрицательные числа (по модулю)
  if (!Negative.empty()) {
    int MaxDigitsNeg = GetMaxDigits(Negative);

    for (int digit = 0; digit < MaxDigitsNeg; digit++) {
      RadixMergeBucketHelpingFunction(Negative, digit);
    }
  }

  // Сортируем положительные числа
  if (!Positive.empty()) {
    int MaxDigitsPos = GetMaxDigits(Positive);

    for (int digit = 0; digit < MaxDigitsPos; digit++) {
      RadixMergeBucketHelpingFunction(Positive, digit);
    }
  }
  // А теперь сливаем всё в один вектор, причём негативные - задом наперёд,
  // позитивные - как было.
  // Вот так.
  if (!Negative.empty()) {
    int j = 0;
    for (auto i = Negative.rbegin(); i != Negative.rend(); i++) {
      Part[j] = -*i;
      j++;
    }
  }
  if (!Positive.empty()) {
    for (size_t i = 0; i < Positive.size(); i++) {
      Part[i + Negative.size()] = Positive[i];
    }
  }
}

bool TimofeevNRadixMergeSEQ::RunImpl() {
  GetOutput().resize(GetInput().size());
  for (size_t i = 0; i < GetInput().size(); i++) {
    GetOutput()[i] = GetInput()[i];
  }
  RadixMergeSort(GetOutput());
  return true;
}

bool TimofeevNRadixMergeSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace timofeev_n_radix_merge_sort
