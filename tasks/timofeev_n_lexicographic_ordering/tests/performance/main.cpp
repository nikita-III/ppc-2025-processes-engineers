#include <gtest/gtest.h>

#include "timofeev_n_lexicographic_ordering/common/include/common.hpp"
#include "timofeev_n_lexicographic_ordering/mpi/include/ops_mpi.hpp"
#include "timofeev_n_lexicographic_ordering/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace timofeev_n_lexicographic_ordering {

class LexicographicRunPerfTestProcesses : public ppc::util::BaseRunPerfTests<InType, OutType> {
  //const int kCount_ = 100;
  InType input_data_{};
  OutType expected_data_;

  void SetUp() override {
    std::string sPerf;
    for (char i = 'a'; i <= 'z'; i++) {
      sPerf += i;
    }
    input_data_ = std::make_pair(sPerf, sPerf);
    expected_data_ = std::make_pair(1, 1);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data.first == expected_data_.first && output_data.second == expected_data_.second;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(LexicographicRunPerfTestProcesses, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, TimofeevNLexicographicOrderingMPI, TimofeevNLexicographicOrderingSEQ>(PPC_SETTINGS_timofeev_n_lexicographic_ordering);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = LexicographicRunPerfTestProcesses::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, LexicographicRunPerfTestProcesses, kGtestValues, kPerfTestName);

}
