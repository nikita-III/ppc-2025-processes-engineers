#include "timofeev_n_lexicographic_ordering/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>
#include <vector>

#include "timofeev_n_lexicographic_ordering/common/include/common.hpp"
#include "util/include/util.hpp"

namespace timofeev_n_lexicographic_ordering {

TimofeevNLexicographicOrderingMPI::TimofeevNLexicographicOrderingMPI(const InType &in) {
   SetTypeOfTask(GetStaticTypeOfTask());
   GetInput() = in;
   GetOutput() = std::pair<int, int>(1, 1);
}

bool TimofeevNLexicographicOrderingMPI::ValidationImpl() {
  return (GetInput().first != "") && (GetInput().second != "");
}

bool TimofeevNLexicographicOrderingMPI::PreProcessingImpl() {
  return 1;
}

bool TimofeevNLexicographicOrderingMPI::Compare(char a, char b) {
  return a < b;
}



// bool MCoompare(char a, char b) {
//   return a < b;
// }

bool TimofeevNLexicographicOrderingMPI::RunImpl() {
  auto input = GetInput();
  if (input.first.length() < 1 || input.second.length() < 1) {
    return false;
  }

  


  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    GetOutput().first = 1;
    // only true if comparison is true on every step
    for (int i = 0; i < (int)input.first.length() - 1; i++) {
      GetOutput().first &= input.first[i] < input.first[i + 1];
    }
  } else if (rank == 1) {
    GetOutput().second = 1;
    for (int i = 0; i < (int)input.second.length() - 1; i++) {
      GetOutput().second &= input.second[i] < input.second[i + 1];
    }
  }

  if (rank == 1) {
    MPI_Send(&GetOutput().second, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  if (rank == 0) {
    MPI_Recv(&GetOutput().second, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(&GetOutput().first, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
  }
  if (rank == 1) {
    MPI_Recv(&GetOutput().first, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if (rank > 1) {
    return 0;
  }
  //std::cout <<'\n' << '\n' << rank << GetOutput().first << GetOutput().second << '\n';
  return 1;//GetOutput().first == 1 && GetOutput().second == 1; //(GetOutput().first != 1 || rank != 0) && (rank != 1 || GetOutput().second != 1)
}

bool TimofeevNLexicographicOrderingMPI::PostProcessingImpl() {
  // idk what else to put there
  return 1;
}

}
