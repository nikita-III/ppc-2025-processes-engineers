#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "timofeev_n_ribbon_scheme_only_a/common/include/common.hpp"
#include "timofeev_n_ribbon_scheme_only_a/mpi/include/ops_mpi.hpp"
#include "timofeev_n_ribbon_scheme_only_a/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace timofeev_n_ribbon_scheme_only_a {

class RibbonRunFuncTestsProcesses : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::get<3>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    input_data_ = std::get<2>(params);
    expected_data_ = std::get<0>(params);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if (output_data.size() != expected_data_.size() || output_data[0].size() != expected_data_[0].size())
      return 0;
    bool yesyes = true;
    for (size_t i = 0; i < output_data.size(); i++)
      for(size_t j = 0; j < output_data[0].size(); j++)
        yesyes &= output_data[i][j] == expected_data_[i][j];
    return yesyes;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  OutType expected_data_;
};

namespace {

TEST_P(RibbonRunFuncTestsProcesses, MatmulFromPic) {
  ExecuteTest(GetParam());
}

MatrixType zaglushka = std::vector<std::vector<int>>(3, std::vector<int>(3, 0));
// matrix, out, in, string
const std::array<TestType, 1> kTestParam = {std::make_tuple(zaglushka, zaglushka, std::make_pair(zaglushka, zaglushka), "0")};
                                            // std::make_tuple(5, "1"),
                                            // std::make_tuple(7, "all1")

const auto kTestTasksList =
    std::tuple_cat(ppc::util::AddFuncTask<TimofeevNRibbonSchemeOnlyAMPI, InType>(kTestParam, PPC_SETTINGS_timofeev_n_ribbon_scheme_only_a),
                   ppc::util::AddFuncTask<TimofeevNRibbonSchemeOnlyASEQ, InType>(kTestParam, PPC_SETTINGS_timofeev_n_ribbon_scheme_only_a));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = RibbonRunFuncTestsProcesses::PrintFuncTestName<RibbonRunFuncTestsProcesses>;

INSTANTIATE_TEST_SUITE_P(PicMatrixTests, RibbonRunFuncTestsProcesses, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace timofeev_n_ribbon_scheme_only_a
