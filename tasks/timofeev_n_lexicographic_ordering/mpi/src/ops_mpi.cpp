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
    // only true if comparison is true on every step
    for (int i = 0; i < (int)input.first.length() - 1; i++) {
      GetOutput().first &= input.first[i] < input.first[i + 1];
    }
  } else {
    for (int i = 0; i < (int)input.second.length() - 1; i++) {
      GetOutput().second &= input.second[i] < input.second[i + 1];
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  return GetOutput().first >= 0 && GetOutput().second >= 0;
}

bool TimofeevNLexicographicOrderingMPI::PostProcessingImpl() {
  // idk what else to put there
  return 1;
}

}
