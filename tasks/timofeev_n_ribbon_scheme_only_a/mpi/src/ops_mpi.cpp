#include "timofeev_n_ribbon_scheme_only_a/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>
#include <vector>

#include "timofeev_n_ribbon_scheme_only_a/common/include/common.hpp"
#include "util/include/util.hpp"

namespace timofeev_n_ribbon_scheme_only_a {

TimofeevNRibbonSchemeOnlyAMPI::TimofeevNRibbonSchemeOnlyAMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<std::vector<int>>(0, std::vector<int>(0));
}

bool TimofeevNRibbonSchemeOnlyAMPI::ValidationImpl() {
  std::vector<std::vector<int>>& A = GetInput().first;
  std::vector<std::vector<int>>& B = GetInput().second;
  size_t n = A.size();
  size_t m2 = A[0].size();
  size_t m1 = B.size();
  size_t k = B[0].size();
  if (m2 != m1 || n == 0 || m1 == 0 || m2 == 0 || k == 0) {
    return false;
  }
  return 1;
}

bool TimofeevNRibbonSchemeOnlyAMPI::PreProcessingImpl() {
  return 1;
}

void TimofeevNRibbonSchemeOnlyAMPI::sendingAParts (MatrixType &A, int &size, size_t &k) {
  size_t aRowSize = A[0].size();
  for (size_t i = 0; (i + 1) < (size_t)size - 1; i++)
    for (size_t j = 0; j < k && i * k + j < A.size(); j++) // по k строчек
      MPI_Send(A[i * k + j].data(), aRowSize, MPI_INT, (i + 1), 0, MPI_COMM_WORLD); // A[i + j]
  for (size_t i = (size - 2) * k; i < A.size(); i++)
    MPI_Send(A[i].data(), aRowSize, MPI_INT, (size - 1), 0, MPI_COMM_WORLD);
}

void TimofeevNRibbonSchemeOnlyAMPI::receivingCParts (MatrixType &Cmatr, int &size, size_t &k, size_t &bRowSize) {
  for (size_t i = 0; (i + 1) < (size_t)size - 1; i++)
    for (size_t j = 0; j < k && i * k + j < Cmatr.size(); j++) // по k строчек
      MPI_Recv(Cmatr[i * k + j].data(), bRowSize, MPI_INT, i + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  for (size_t i = (size - 2) * k; i < Cmatr.size(); i++)
    MPI_Recv(Cmatr[i].data(), bRowSize, MPI_INT, (size - 1), 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

int TimofeevNRibbonSchemeOnlyAMPI::calculatingCElement (MatrixType &aPart, MatrixType &bCopy, size_t &i, size_t &j) {
  int summ = 0;
  for (size_t kk = 0; kk < aPart[0].size(); kk++)
    summ += aPart[i][kk] * bCopy[kk][j];
  return summ;
}

void TimofeevNRibbonSchemeOnlyAMPI::calculatingCPart (size_t &k, MatrixType &aPart, MatrixType &bCopy, MatrixType &cPart) {
  for (size_t i = 0; i < k; i++)
    for (size_t j = 0; j < bCopy[0].size(); j++)
      cPart[i][j] = calculatingCElement (aPart, bCopy, i, j);
}

void TimofeevNRibbonSchemeOnlyAMPI::BroadcastingParameters(size_t &k, size_t &aSize, size_t &aRowSize, size_t &bSize, size_t &bRowSize) {
  MPI_Bcast(&k, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&aSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&aRowSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&bSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&bRowSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
}

void TimofeevNRibbonSchemeOnlyAMPI::BroadcastingB(MatrixType &B) {
  size_t bRowSize = B[0].size();
  for (size_t i = 0; i < B.size(); i++)
    MPI_Bcast(B[i].data(), bRowSize, MPI_INT, 0, MPI_COMM_WORLD);
}

bool TimofeevNRibbonSchemeOnlyAMPI::RunImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  if (rank == 0) {
    MatrixType A = GetInput().first;
    MatrixType B = GetInput().second;
    size_t k = 0;
    k = A.size() / (size_t)(size - 1) + (A.size() % (size_t)(size - 1) > 0 ? 1 : 0);
    size_t aSize = A.size();
    size_t aRowSize = A[0].size();
    size_t bSize = B.size();
    size_t bRowSize = B[0].size();
    BroadcastingParameters(k, aSize, aRowSize, bSize, bRowSize);
    BroadcastingB(B);
    sendingAParts (A, size, k); // вместо этого - функция/макрос 3
    std::vector<std::vector<int>> Cmatr(A.size(), std::vector<int>(bRowSize));
    receivingCParts (Cmatr, size, k, bRowSize); // принимаем строки матрицы C // вместо этого - функция/макрос 2
    for (size_t i = 0; i < Cmatr.size(); i++)// рассылка того, что получилось
      MPI_Bcast(Cmatr[i].data(), bRowSize, MPI_INT, 0, MPI_COMM_WORLD);
    GetOutput() = Cmatr;
  } else {
    size_t k, aSize, aRowSize, bSize, bRowSize;
    BroadcastingParameters(k, aSize, aRowSize, bSize, bRowSize);
    std::vector<std::vector<int>> bCopy(bSize, std::vector<int>(bRowSize));
    BroadcastingB(bCopy);
    if (rank == size - 1)// отдельно обрабатываем последний процесс
      k = aSize - (size - 2) * k;
    std::vector<std::vector<int>> aPart(k, std::vector<int>(aRowSize));
    for (size_t j = 0; j < k; j++)// принимаем строки A
      MPI_Recv(aPart[j].data(), aRowSize, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::vector<std::vector<int>> cPart(k, std::vector<int>(bRowSize));
    calculatingCPart (k, aPart, bCopy, cPart);// можем вычислить k строк (k * bRowSize элементов) матрицы C
    for (size_t i = 0; i < k; i++)// у нас есть матрица k * bRowSize
      MPI_Send(cPart[i].data(), bRowSize, MPI_INT, 0, 1, MPI_COMM_WORLD);
    GetOutput().resize(aSize);
    for (size_t i = 0; i < aSize; i++) {// принимает результат из Bcast
      GetOutput()[i].resize(bRowSize);
      MPI_Bcast(GetOutput()[i].data(), bRowSize, MPI_INT, 0, MPI_COMM_WORLD);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return 1;
}

bool TimofeevNRibbonSchemeOnlyAMPI::PostProcessingImpl() {
  return 1;
}

}  // namespace timofeev_n_ribbon_scheme_only_a
