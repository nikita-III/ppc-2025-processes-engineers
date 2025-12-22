#include "timofeev_n_radix_merge_sort/mpi/include/ops_mpi.hpp"

#include <mpi.h>
#include <climits>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

#include "timofeev_n_radix_merge_sort/common/include/common.hpp"

namespace timofeev_n_radix_merge_sort {

TimofeevNRadixMergeMPI::TimofeevNRadixMergeMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>(0);
}

bool TimofeevNRadixMergeMPI::ValidationImpl() {
  return !GetInput().empty();
}

bool TimofeevNRadixMergeMPI::PreProcessingImpl() {
  return true;
}

int TimofeevNRadixMergeMPI::GetDigit(int num, int digit) {
  int absNum = std::abs(num);
  
  // потому что без использования возведения в степень (как функции)
  for (int i = 0; i < digit; i++) {
      absNum /= 10;
  }
  
  return absNum % 10;
}

int TimofeevNRadixMergeMPI::GetMaxDigits(const std::vector<int>& arr) {
  if (arr.empty()) return 0;
  
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
  std::cout << "-----GetMaxDigits------" << (digits == 0 ? 1 : digits) << "\n";
  return digits == 0 ? 1 : digits; // минимум 1 разряд
}

void TimofeevNRadixMergeMPI::SplitPosNeg(const std::vector<int>& Arr, 
                                         std::vector<int>& Negative, 
                                         std::vector<int>& Positive) {
  for (int num : Arr) {
    if (num < 0) {
      Negative.push_back(-num);
    } else {
       Positive.push_back(num);
    }
  }
  std::cout << "----------SplitPosNeg-------- ";
  for (size_t i = 0; i < Negative.size(); i++) {
    std::cout << Negative[i] << " ";
  }
  std::cout << "\n";
  std::cout << "----------SplitPosNeg-------- ";
  for (size_t i = 0; i < Positive.size(); i++) {
    std::cout << Positive[i] << " ";
  }
  std::cout << "\n";
}

void TimofeevNRadixMergeMPI::RadixMergeBucketHelpingFunction(std::vector<int> &Part, int Digit) {
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

  std::cout << "----------RadixMergeBucketHelpingFunction-------- ";
  for (size_t i = 0; i < Part.size(); i++) {
    std::cout << Part[i] << " ";
  }
  std::cout << "\n";
}

void TimofeevNRadixMergeMPI::RadixMergeSort(std::vector<int> &Part) {
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

  std::cout << "----------Negative-------- ";
  for (size_t i = 0; i < Negative.size(); i++) {
    std::cout << Negative[i] << " ";
  }
  std::cout << "\n";
  std::cout << "----------Positive-------- ";
  for (size_t i = 0; i < Positive.size(); i++) {
    std::cout << Positive[i] << " ";
  }
  std::cout << "\n";
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

void TimofeevNRadixMergeMPI::SliyanieHelp(std::vector<std::vector<int>> &Received, std::vector<int> &Indexes, std::vector<int> &Out, int &i) {
  int Smales = INT_MAX;
  int IndexSmales = 0;
  for (size_t j = 0; j < Received.size(); j++) {
    if (static_cast<size_t>(Indexes[j]) < Received[j].size() && Received[j][Indexes[j]] <= Smales) {
      IndexSmales = j;
      Smales = Received[j][Indexes[j]];
    }
  }
  Out[i] = Received[IndexSmales][Indexes[IndexSmales]];
  Indexes[IndexSmales]++;
}

void TimofeevNRadixMergeMPI::Sliyanie(std::vector<std::vector<int>> &Received, std::vector<int> &Out) {
  std::vector<int> Indexes(Received.size(), 0);
  for (int i = 0; static_cast<size_t>(i) < Out.size(); i++) {
    SliyanieHelp(Received, Indexes, Out, i);
  }
}

bool TimofeevNRadixMergeMPI::RunImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  if (size == 1) {
    // работа одним процессом
    GetOutput().resize(GetInput().size());
    for (size_t i = 0; i < GetInput().size(); i++) {
      GetOutput()[i] = GetInput()[i];
    }
    RadixMergeSort(GetOutput());
    std::cout << "----------GETOUT-------- ";
    for (size_t i = 0; i < GetOutput().size(); i++) {
      std::cout << GetOutput()[i] << " ";
    }
    std::cout << "\n";

    std::cout << rank <<  " конец\n";
    //MPI_Barrier(MPI_COMM_WORLD);
    return true;
  }

  // количество фактически работающих процессов - степень двойки
  // ибо проще строить бинарное дерево
  
  if (rank == 0) {
    if (GetInput().empty()) {
      return false;
    }
    if (size == 2) {
      // работа только 1-м процессом
      int yes = 1;
      MPI_Send(&yes, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
      size_t SizeSize = GetInput().size();
      MPI_Send(&SizeSize, 1, MPI_UNSIGNED_LONG, 1, 0, MPI_COMM_WORLD);
      MPI_Send(GetInput().data(), static_cast<int>(SizeSize), MPI_INT, 1, 0, MPI_COMM_WORLD);
      GetOutput().resize(GetInput().size());
      MPI_Recv(GetOutput().data(), static_cast<int>(SizeSize), MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      std::cout << "----------GETOUT-------- ";
      for (size_t i = 0; i < GetOutput().size(); i++) {
        std::cout << GetOutput()[i] << " ";
      }
      std::cout << "\n";

      size_t ConcdlusionSize = GetOutput().size();
      MPI_Bcast(&ConcdlusionSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
      MPI_Bcast(GetOutput().data(), static_cast<int>(ConcdlusionSize), MPI_INT, 0, MPI_COMM_WORLD);
      std::cout << rank <<  " конец\n";
      MPI_Barrier(MPI_COMM_WORLD);
      return true;
    }
    int ServedSize = size - 1;
    std::cout << rank << " " << ServedSize <<  " ServedSize получилось вот таким\n";


    // Отдаём, принимаем, отдаём, принимаем, ...
    // Каждый раз создаём контейнеры по размеру того, что отдали,
    // один раз рассылаем всем по одному кусочку, потом по два побольше, потом ещё бОльшие...
    // Уже обработаки самые маленькие кусочки, поэтому берём только часть процессов
    //std::vector<int> ToSort = GetInput();
    size_t k = GetInput().size() / static_cast<size_t>(ServedSize) + (GetInput().size() % static_cast<size_t>(ServedSize) > 0 ? 1 : 0);
    std::cout << rank << " " << k <<  " k получилось вот таким\n";
    std::vector<int> SizesForProcesses(ServedSize, 0);
    size_t ToSortSize = GetInput().size();
    for (size_t i = 0; i < SizesForProcesses.size() - 1; i += k) {
      SizesForProcesses[i] = ((ToSortSize - k) > 0 ? k : 0);
      std::cout << rank << " " << SizesForProcesses[i] << " при i = " << i <<  " SizesForProcesses[i] получилось вот таким\n";
      ToSortSize -= (ToSortSize > k ? k : 0);
    }
    std::cout << rank << " " << ToSortSize <<  " ToSortSize получилось вот таким\n";
    SizesForProcesses[ServedSize - 1] = ToSortSize;
    std::cout << rank << " процессу с номером " << (ServedSize) <<  " отдали ToSortSize\n";
    std::cout << rank <<  " оповещаем, надо ли вооще работать\n";
    for (int i = 0; i < ServedSize; i++) {
      int yes = 1;
      MPI_Send(&yes, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
      std::cout << rank << " говорит: " << i + 1 <<  " надо работать\n";
    }
    std::cout << rank <<  " тем, кого мы отбросили, не надо\n";
    for (int i = ServedSize; i < size - 1; i++) {
      int no = 0;
      MPI_Send(&no, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
      std::cout << rank << " говорит: " << i + 1 <<  " не надо работать\n";
    }
    std::cout << rank <<  " рассылаем самые маленькие\n";
    int AccumulatedSize = 0;
    // оформить в функцию - не знаю, надо ли
    for (size_t i = 0; i < SizesForProcesses.size(); i++) {
      std::vector<int> BufVec(GetInput().begin() + AccumulatedSize, GetInput().begin() + AccumulatedSize + SizesForProcesses[i]);
      size_t CurSize = BufVec.size();
      MPI_Send(&CurSize, 1, MPI_UNSIGNED_LONG, static_cast<int>(i + 1), 0, MPI_COMM_WORLD);
      //if (CurSize > 0) {
      MPI_Send(BufVec.data(), static_cast<int>(CurSize), MPI_INT, static_cast<int>(i + 1), 0, MPI_COMM_WORLD);
      AccumulatedSize += SizesForProcesses[i];
      //}
    }
    std::cout << rank <<  " принимаем самые маленькие\n";
    // оформить в функцию?
    std::vector<std::vector<int>> Received(SizesForProcesses.size(),std::vector<int>(1, 0));
    for (size_t i = 0; i < SizesForProcesses.size(); i++) {
      std::cout << rank << " " << SizesForProcesses[i] <<  " = SizesForProcesses[i]\n";
      Received[i].resize(SizesForProcesses[i]);
      MPI_Recv(Received[i].data(), SizesForProcesses[i], MPI_INT, static_cast<int>(i + 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      //}
    }

    //std::cout << rank <<  " день сурка\n";
    if (GetOutput().size() != GetInput().size()) {
      GetOutput().resize(GetInput().size());
    }
    for (size_t i = 0; i < GetInput().size(); i++) {
      GetOutput()[i] = GetInput()[i];
    }
    // здесь слияние
    Sliyanie(Received, GetOutput());
    
    std::cout << "----------GETOUT-------- ";
    for (size_t i = 0; i < GetOutput().size(); i++) {
      std::cout << GetOutput()[i] << " ";
    }
    std::cout << "\n";

    size_t ConcdlusionSize = GetOutput().size();
    MPI_Bcast(&ConcdlusionSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(GetOutput().data(), static_cast<int>(ConcdlusionSize), MPI_INT, 0, MPI_COMM_WORLD);
    std::cout << rank <<  " конец\n";
  } else {
    // Сначала забираем маленькие кусочки, потом - цикл:
    // пока приходит оповещение о том, что нам надо ещё кусочек обработать (в конце предыдущего приходит),
    // мы принимаем размер следующего кусочка, и обрабатываем его
    int ShouldIWork;
    MPI_Recv(&ShouldIWork, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (!ShouldIWork) {
      // приём результата через Bcast
      size_t ConcdlusionSize;
      MPI_Bcast(&ConcdlusionSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
      GetOutput().resize(ConcdlusionSize);
      MPI_Bcast(GetOutput().data(), static_cast<int>(ConcdlusionSize), MPI_INT, 0, MPI_COMM_WORLD);
      std::cout << rank <<  " уходим с работы домой\n";
      MPI_Barrier(MPI_COMM_WORLD);
      return true;
    }
    std::cout << rank <<  " я сегодня работаю с MPI_Init(...) до MPI_Finalize()\n";
    // if (size == 2) {
    //   std::cout << rank <<  " ситуация, когда нужно просто принять от 0-го всё, ему всё отдать и потом принять результат\n";
    //   std::cout << rank <<  " мы принимаем размер кусочка, и обрабатываем его\n";
    //   size_t CurSize;
    //   MPI_Recv(&CurSize, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //   std::vector<int> BufVec(CurSize);
    //   MPI_Recv(BufVec.data(), static_cast<int>(CurSize), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //   std::cout << rank <<  " сортировка\n";
    //   RadixMergeSort(BufVec);
    //   std::cout << rank << " У этого процесса вот такой вектор: ";
    //   for (size_t i = 0; i < BufVec.size(); i++) {
    //     std::cout << BufVec[i] << " ";
    //   }
    //   std::cout << "\n";

    //   std::cout << rank <<  " отдаём самые маленькие\n";
    //   std::cout << rank << " " << CurSize <<   " =  CurSize\n";
    //   MPI_Send(BufVec.data(), static_cast<int>(CurSize), MPI_INT, 0, 0, MPI_COMM_WORLD);
      
    //   std::cout << rank <<  " мы принимаем размер финального кусочка, и принимаем его через BCast\n";
    //   size_t ConcdlusionSize;
    //   MPI_Bcast(&ConcdlusionSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    //   GetOutput().resize(ConcdlusionSize);
    //   MPI_Bcast(GetOutput().data(), static_cast<int>(ConcdlusionSize), MPI_INT, 0, MPI_COMM_WORLD);
    //   std::cout << rank <<  " конец\n";
    //   MPI_Barrier(MPI_COMM_WORLD);
    //   return true;
    // }
    std::cout << rank <<  " мы принимаем размер кусочка, и обрабатываем его\n";
    size_t CurSize;
    MPI_Recv(&CurSize, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::vector<int> BufVec(CurSize);
    MPI_Recv(BufVec.data(), static_cast<int>(CurSize), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::cout << rank <<  " сортировка\n";
    RadixMergeSort(BufVec);
    std::cout << rank << " У этого процесса вот такой вектор: ";
    for (size_t i = 0; i < BufVec.size(); i++) {
      std::cout << BufVec[i] << " ";
    }
    std::cout << "\n";

    std::cout << rank <<  " отдаём самые маленькие\n";
    std::cout << rank << " " << CurSize <<   " =  CurSize\n";
    MPI_Send(BufVec.data(), static_cast<int>(CurSize), MPI_INT, 0, 0, MPI_COMM_WORLD);
    
    std::cout << rank <<  " мы принимаем размер финального кусочка, и принимаем его через BCast\n";
    size_t ConcdlusionSize;
    MPI_Bcast(&ConcdlusionSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    GetOutput().resize(ConcdlusionSize);
    MPI_Bcast(GetOutput().data(), static_cast<int>(ConcdlusionSize), MPI_INT, 0, MPI_COMM_WORLD);
    std::cout << rank <<  " конец\n";
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool TimofeevNRadixMergeMPI::PostProcessingImpl() {
  return true;
}

}  // namespace timofeev_n_radix_merge_sort
