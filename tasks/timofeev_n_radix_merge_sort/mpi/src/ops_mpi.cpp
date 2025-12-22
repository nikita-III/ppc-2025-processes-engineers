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

int TimofeevNRadixMergeMPI::DecideIfProceeding(std::vector<std::vector<int>> Receiving, size_t ConclusionSize) {
  bool YesYes = true;
  for (size_t i = 0; i < Receiving[0].size() - 1; i++) {
    YesYes &= Receiving[0][i] < Receiving[0][i + 1];
  }
  return Receiving[0].size() == ConclusionSize && YesYes;
}

void TimofeevNRadixMergeMPI::RadixMergeSort(std::vector<int> &part) {
  if (part.size() <= 1) {
    return;
  }
}

// оно и вычисляет, что рассылать, и склеивает, и рассылает, что надо, и принимает
// У неё есть и &statusQuo, и &Received, и StepChocolate (как Shag)
// плохо... спроектировано...
void TimofeevNRadixMergeMPI::DistributeReceiveParts(std::vector<int> &statusQuo, std::vector<std::vector<int>> &Received, int Shag) {
  size_t i = 0;
  for (; i < statusQuo.size(); i += Shag) {
    // идём с шагом Shag, всё, что между i и i + Shag - обнуляется,
    // эти процессы посылают всё своё i-му
    for (size_t j = i + 1; j < i + static_cast<size_t>(Shag); j++) {
      // если текущий statusQuo[i] был 1, то нам есть что отправлять
      // сначала прибавляем его вектор к нашему, обновляя размер, <delta1>
      Received[i].insert(Received[i].end(), Received[j].begin(), Received[j].end());
      // все зависимости от вышеописанного - этот процес больше не активен
      statusQuo[j] = 0;
    }
    // i += Shag; // указывает на первый слева ненулевой элемент (отвечает активному процессу)

    // <delta1> потом отсылаем i-му процессу,
    size_t CurSize = Received[i].size();
    MPI_Send(&CurSize, 1, MPI_UNSIGNED_LONG, static_cast<int>(i + 1), 0, MPI_COMM_WORLD);
    MPI_Send(Received[i].data(), static_cast<int>(CurSize), MPI_INT, static_cast<int>(i + 1), 0, MPI_COMM_WORLD);
    //  а потом принимаем, и записываем это в (ресайзнутый инсёртом) Received[i]
    MPI_Recv(Received[i].data(), static_cast<int>(CurSize), MPI_INT, static_cast<int>(i + 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  // i = 1;
  // for (; i < statusQuo.size(); i++) {
    
  // }
}

// с упором на предположение о том, что не будет переполнения
int TimofeevNRadixMergeMPI::GetPervTwoPower(int size) {
  int i = 1;
  while (i <= size) {
    i *= 2;
  }
  i /= 2;
  return i;
}

bool TimofeevNRadixMergeMPI::RunImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  if (size == 1) {
    // работа одним процессом
    return true;
  }

  // количество фактически работающих процессов - степень двойки
  // ибо проще строить бинарное дерево
  
  if (rank == 0) {
    if (size == 2) {
      // работа только 1-м процессом

      MPI_Barrier(MPI_COMM_WORLD);
      return true;
    }
    int ServedSize = GetPervTwoPower(size - 1);
    std::cout << rank << " " << ServedSize <<  " ServedSize получилось вот таким\n";

    statusQuo.resize(ServedSize);

    // Отдаём, принимаем, отдаём, принимаем, ...
    // Каждый раз создаём контейнеры по размеру того, что отдали,
    // один раз рассылаем всем по одному кусочку, потом по два побольше, потом ещё бОльшие...
    // Уже обработаки самые маленькие кусочки, поэтому берём только часть процессов
    std::vector<int> ToSort = GetInput();
    size_t k = ToSort.size() / static_cast<size_t>(ServedSize) + (ToSort.size() % static_cast<size_t>(ServedSize) > 0 ? 1 : 0);
    std::cout << rank << " " << k <<  " k получилось вот таким\n";
    std::vector<int> SizesForProcesses(ServedSize, 0);
    size_t ToSortSize = ToSort.size();
    for (size_t i = 0; i < SizesForProcesses.size() - 1; i += k) {
      SizesForProcesses[i] = ((ToSortSize - k) > 0 ? k : 0);
      std::cout << rank << " " << SizesForProcesses[i] << " при i = " << i <<  " SizesForProcesses[i] получилось вот таким\n";
      ToSortSize -= k;
    }
    std::cout << rank << " " << ToSortSize <<  " ToSortSize получилось вот таким\n";
    SizesForProcesses[ServedSize - 1] = ToSortSize;
    std::cout << rank << " процессу с номером " << (ServedSize) <<  " отдали ToSortSize\n";
    std::cout << rank <<  " оповещаем, надо ли вооще работать\n";
    for (int i = 0; i < ServedSize; i++) {
      int yes = 1;
      MPI_Send(&yes, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
      std::cout << rank << "говорит: " << i + 1 <<  " надо работать\n";
    }
    std::cout << rank <<  " тем, кого мы отбросили, не надо\n";
    for (int i = ServedSize; i < size - 1; i++) {
      int no = 0;
      MPI_Send(&no, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
      std::cout << rank << "говорит: " << i + 1 <<  " не надо работать\n";
    }
    std::cout << rank <<  " рассылаем самые маленькие\n";
    std::vector<int> BufVec;
    int AccumulatedSize = 0;
    // оформить в функцию - не знаю, надо ли
    for (size_t i = 0; i < SizesForProcesses.size(); i++) {
      //std::copy(ToSort.begin() + (i > 0 ? AccumulatedSize + 1 : 0), ToSort.begin() + AccumulatedSize + SizesForProcesses[i], std::back_inserter(BufVec));
      //BufVec = (ToSort.begin() + (i > 0 ? AccumulatedSize + 1 : 0), ToSort.begin() + AccumulatedSize + SizesForProcesses[i]);
      BufVec.resize(SizesForProcesses[i]);
      size_t CurSize = BufVec.size();
      MPI_Send(&CurSize, 1, MPI_UNSIGNED_LONG, static_cast<int>(i + 1), 0, MPI_COMM_WORLD);
      //if (CurSize > 0) {
      MPI_Send(BufVec.data(), static_cast<int>(CurSize), MPI_INT, static_cast<int>(i + 1), 0, MPI_COMM_WORLD);
      AccumulatedSize += SizesForProcesses[i];
      statusQuo[i] = 1;
      //}
    }
    std::cout << rank <<  " принимаем самые маленькие\n";
    // оформить в функцию?
    std::vector<std::vector<int>> Received(SizesForProcesses.size());
    for (size_t i = 0; i < SizesForProcesses.size(); i++) {
      std::cout << rank << " " << SizesForProcesses[i] <<  " = SizesForProcesses[i]\n";
      Received[i].resize(SizesForProcesses[i]);
      //std::copy(ToSort.begin() + (i > 0 ? SizesForProcesses[i - 1] + 1 : 0), ToSort.begin() + SizesForProcesses[i], std::back_inserter(Received[i]));
      //Received[i] = (ToSort.begin() + (i > 0 ? SizesForProcesses[i - 1] + 1 : 0), ToSort.begin() + SizesForProcesses[i]);
      //if (CurSize > 0) {
      MPI_Recv(Received[i].data(), SizesForProcesses[i], MPI_INT, static_cast<int>(i + 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      //}
    }
    //std::cout << rank <<  " день сурка\n";
    // здесь слияние
    
    Received[0].resize(GetInput().size());// заглушка
    Received[0] = GetInput();// заглушка

    GetOutput() = Received[0];
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

      std::cout << rank <<  " уходим с работы домой\n";
      MPI_Barrier(MPI_COMM_WORLD);
      return true;
    }
    std::cout << rank <<  " я сегодня работаю с MPI_Init(...) до MPI_Finalize()\n";
    if (size == 2) {
      std::cout << rank <<  " ситуация, когда нужно просто принять от 0-го всё, ему всё отдать и потом принять результат\n";
      MPI_Barrier(MPI_COMM_WORLD);
      return true;
    }
    std::cout << rank <<  " мы принимаем размер кусочка, и обрабатываем его\n";
    size_t CurSize;
    MPI_Recv(&CurSize, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::vector<int> BufVec(CurSize);
    //if (CurSize > 0) {
    MPI_Recv(BufVec.data(), static_cast<int>(CurSize), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //}
    std::cout << rank <<  " сортировка\n";

    std::cout << rank <<  " отдаём самые маленькие\n";
    std::cout << rank << " " << CurSize <<   " =  CurSize\n";
    MPI_Send(BufVec.data(), static_cast<int>(CurSize), MPI_INT, 0, 0, MPI_COMM_WORLD);
    
    std::cout << rank <<  " мы принимаем размер финального кусочка, и принимаем его через BCast\n";
    size_t ConcdlusionSize;
    MPI_Bcast(&ConcdlusionSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
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
