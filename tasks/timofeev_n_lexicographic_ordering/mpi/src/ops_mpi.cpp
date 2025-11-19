#include "timofeev_n_lexicographic_ordering/mpi/include/ops_mpi.hpp"

#include <mpi.h>


#include "timofeev_n_lexicographic_ordering/common/include/common.hpp"
#include "util/include/util.hpp"

namespace timofeev_n_lexicographic_ordering {

TimofeevNLexicographicOrderingMPI::TimofeevNLexicographicOrderingMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::pair<int, int>(1, 1);
}

bool TimofeevNLexicographicOrderingMPI::ValidationImpl() {
  return true;
}

bool TimofeevNLexicographicOrderingMPI::PreProcessingImpl() {
  return true;
}

bool TimofeevNLexicographicOrderingMPI::RunImpl() {
  auto input = GetInput();

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  GetOutput() = std::pair<int, int>(1, 1);

  if (rank == 0) {
    // only true if comparison is true on every step
    for (size_t i = 0; i < input.first.length() - 1; i++) {
      GetOutput().first &= static_cast<int>(input.first[i] <= input.first[i + 1]);
    }
  } else if (rank == 1) {
    for (size_t i = 0; i < input.second.length() - 1; i++) {
      GetOutput().second &= static_cast<int>(input.second[i] <= input.second[i + 1]);
    }
  }

  MPI_Bcast(&GetOutput().first, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&GetOutput().second, 1, MPI_INT, 1, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool TimofeevNLexicographicOrderingMPI::PostProcessingImpl() {
  // idk what else to put there
  return true;
}

}  // namespace timofeev_n_lexicographic_ordering
