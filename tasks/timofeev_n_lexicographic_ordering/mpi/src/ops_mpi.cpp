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
  return 1;
}

bool TimofeevNLexicographicOrderingMPI::PreProcessingImpl() {
  return 1;
}

bool TimofeevNLexicographicOrderingMPI::RunImpl() {
  auto input = GetInput();

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  GetOutput() = std::pair<int, int>(1, 1);

  if (rank == 0) {
    // only true if comparison is true on every step
    for (int i = 0; i < (int)input.first.length() - 1; i++) {
      GetOutput().first &= input.first[i] <= input.first[i + 1];
    }
  } else if (rank == 1) {
    for (int i = 0; i < (int)input.second.length() - 1; i++) {
      GetOutput().second &= input.second[i] <= input.second[i + 1];
    }
  }

  MPI_Bcast(&GetOutput().first, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&GetOutput().second, 1, MPI_INT, 1, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  return 1;
}

bool TimofeevNLexicographicOrderingMPI::PostProcessingImpl() {
  // idk what else to put there
  return 1;
}

}  // namespace timofeev_n_lexicographic_ordering
