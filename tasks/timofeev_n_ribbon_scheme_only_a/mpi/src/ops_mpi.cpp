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
  std::vector<std::vector<int>> &a = GetInput().first;
  std::vector<std::vector<int>> &b = GetInput().second;
  size_t n = a.size();
  size_t m2 = a[0].size();
  size_t m1 = b.size();
  size_t k = b[0].size();
  if (m2 != m1 || n == 0 || m1 == 0 || m2 == 0 || k == 0) {
    return false;
  }
  return 1;
}

bool TimofeevNRibbonSchemeOnlyAMPI::PreProcessingImpl() {
  return 1;
}

void TimofeevNRibbonSchemeOnlyAMPI::SendingAParts(MatrixType &a, int &size, size_t &k) {
  size_t a_row_size = a[0].size();
  for (size_t i = 0; (i + 1) < (size_t)size - 1; i++) {
    for (size_t j = 0; j < k && i * k + j < a.size(); j++) {                           // по k строчек
      MPI_Send(a[i * k + j].data(), a_row_size, MPI_INT, (i + 1), 0, MPI_COMM_WORLD);  // A[i + j]
    }
  }
  for (size_t i = (size - 2) * k; i < a.size(); i++) {
    MPI_Send(A[i].data(), a_row_size, MPI_INT, (size - 1), 0, MPI_COMM_WORLD);
  }
}

void TimofeevNRibbonSchemeOnlyAMPI::ReceivingCParts(MatrixType &cmatr, int &size, size_t &k, size_t &b_row_size) {
  for (size_t i = 0; (i + 1) < (size_t)size - 1; i++) {
    for (size_t j = 0; j < k && i * k + j < cmatr.size(); j++) {  // по k строчек
      MPI_Recv(cmatr[i * k + j].data(), b_row_size, MPI_INT, i + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }
  for (size_t i = (size - 2) * k; i < cmatr.size(); i++) {
    MPI_Recv(cmatr[i].data(), b_row_size, MPI_INT, (size - 1), 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
}

int TimofeevNRibbonSchemeOnlyAMPI::CalculatingCElement(MatrixType &a_part, MatrixType &b_copy, size_t &i, size_t &j) {
  int summ = 0;
  for (size_t kk = 0; kk < a_part[0].size(); kk++) {
    summ += a_part[i][kk] * b_copy[kk][j];
  }
  return summ;
}

void TimofeevNRibbonSchemeOnlyAMPI::CalculatingCPart(size_t &k, MatrixType &a_part, MatrixType &b_copy,
                                                     MatrixType &c_part) {
  for (size_t i = 0; i < k; i++) {
    for (size_t j = 0; j < b_copy[0].size(); j++) {
      c_part[i][j] = CalculatingCElement(a_part, b_copy, i, j);
    }
  }
}

void TimofeevNRibbonSchemeOnlyAMPI::BroadcastingParameters(size_t &k, size_t &a_size, size_t &a_row_size,
                                                           size_t &b_size, size_t &b_row_size) {
  MPI_Bcast(&k, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&a_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&a_row_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&b_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&b_row_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
}

void TimofeevNRibbonSchemeOnlyAMPI::BroadcastingB(MatrixType &b) {
  size_t b_row_size = b[0].size();
  for (size_t i = 0; i < b.size(); i++) {
    MPI_Bcast(b[i].data(), b_row_size, MPI_INT, 0, MPI_COMM_WORLD);
  }
}

bool TimofeevNRibbonSchemeOnlyAMPI::RunImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0) {
    MatrixType a = GetInput().first;
    MatrixType b = GetInput().second;
    size_t k = 0;
    k = a.size() / (size_t)(size - 1) + (a.size() % (size_t)(size - 1) > 0 ? 1 : 0);
    size_t a_size = a.size();
    size_t a_row_size = a[0].size();
    size_t b_size = b.size();
    size_t b_row_size = b[0].size();
    BroadcastingParameters(k, a_size, a_row_size, b_size, b_row_size);
    BroadcastingB(b);
    SendingAParts(a, size, k);  // вместо этого - функция/макрос 3
    std::vector<std::vector<int>> cmatr(A.size(), std::vector<int>(b_row_size));
    ReceivingCParts(cmatr, size, k, b_row_size);  // принимаем строки матрицы C // вместо этого - функция/макрос 2
    for (size_t i = 0; i < cmatr.size(); i++) {   // рассылка того, что получилось
      MPI_Bcast(cmatr[i].data(), b_row_size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    GetOutput() = cmatr;
  } else {
    size_t k, a_size, a_row_size, b_size, b_row_size;
    BroadcastingParameters(k, a_size, a_row_size, b_size, b_row_size);
    std::vector<std::vector<int>> b_copy(b_size, std::vector<int>(b_row_size));
    BroadcastingB(b_copy);
    if (rank == size - 1) {  // отдельно обрабатываем последний процесс
      k = a_size - (size - 2) * k;
    }
    std::vector<std::vector<int>> a_part(k, std::vector<int>(a_row_size));
    for (size_t j = 0; j < k; j++) {  // принимаем строки A
      MPI_Recv(a_part[j].data(), a_row_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    std::vector<std::vector<int>> c_part(k, std::vector<int>(b_row_size));
    CalculatingCPart(k, a_part, b_copy, c_part);  // можем вычислить k строк (k * b_row_size элементов) матрицы C
    for (size_t i = 0; i < k; i++) {              // у нас есть матрица k * b_row_size
      MPI_Send(c_part[i].data(), b_row_size, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }
    GetOutput().resize(a_size);
    for (size_t i = 0; i < a_size; i++) {  // принимает результат из Bcast
      GetOutput()[i].resize(b_row_size);
      MPI_Bcast(GetOutput()[i].data(), b_row_size, MPI_INT, 0, MPI_COMM_WORLD);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return 1;
}

bool TimofeevNRibbonSchemeOnlyAMPI::PostProcessingImpl() {
  return 1;
}

}  // namespace timofeev_n_ribbon_scheme_only_a
