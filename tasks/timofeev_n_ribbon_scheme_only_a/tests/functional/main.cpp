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
    if (output_data.size() != expected_data_.size() || output_data[0].size() != expected_data_[0].size()) {
      return false;
    }
    bool yesyes = true;
    for (size_t i = 0; i < output_data.size(); i++) {
      for (size_t j = 0; j < output_data[0].size(); j++) {
        yesyes &= output_data[i][j] == expected_data_[i][j];
      }
    }
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

// MatrixType zaglushka = std::vector<std::vector<int>>(3, std::vector<int>(3, 0));
MatrixType V_1_2 = std::vector<std::vector<int>>(1, std::vector<int>(2, 1));
MatrixType V_2_1 = std::vector<std::vector<int>>(2, std::vector<int>(1, 1));

MatrixType V_1_3 = std::vector<std::vector<int>>(1, std::vector<int>(3, 1));
MatrixType V_3_1 = std::vector<std::vector<int>>(3, std::vector<int>(1, 1));

MatrixType V_1_4 = std::vector<std::vector<int>>(1, std::vector<int>(4, 1));
MatrixType V_4_1 = std::vector<std::vector<int>>(4, std::vector<int>(1, 1));

MatrixType V_1_5 = std::vector<std::vector<int>>(1, std::vector<int>(5, 1));
MatrixType V_5_1 = std::vector<std::vector<int>>(5, std::vector<int>(1, 1));

MatrixType Const_2 = std::vector<std::vector<int>>(1, std::vector<int>(1, 2));
MatrixType Const_3 = std::vector<std::vector<int>>(1, std::vector<int>(1, 3));
MatrixType Const_4 = std::vector<std::vector<int>>(1, std::vector<int>(1, 4));
MatrixType Const_5 = std::vector<std::vector<int>>(1, std::vector<int>(1, 5));

MatrixType O_1_1 = std::vector<std::vector<int>>(1, std::vector<int>(1, 0));

MatrixType E_2_2 = {{1, 0}, {0, 1}};
MatrixType One_2_2 = {{1, 1}, {1, 1}};
MatrixType O_2_2 = {{0, 0}, {0, 0}};
MatrixType Seq_2_2 = {{1, 2}, {3, 4}};

MatrixType E_3_3 = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
MatrixType One_3_3 = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
MatrixType O_3_3 = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
MatrixType Seq_3_3 = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

MatrixType E_4_4 = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
MatrixType One_4_4 = {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};
MatrixType O_4_4 = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
MatrixType Seq_4_4 = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};

MatrixType M_2_3 = {{1, 2, 3}, {4, 5, 6}};
MatrixType M_3_2 = {{1, 2}, {3, 4}, {5, 6}};
MatrixType M_2_2 = {{22, 28}, {49, 64}};

MatrixType M_3_4 = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};
MatrixType M_4_3 = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}};
MatrixType M_3_3 = {{70, 80, 90}, {158, 184, 210}, {246, 288, 330}};

// expected, out, in, string
const std::array<TestType, 15> kTestParam = {
    std::make_tuple(E_2_2, O_2_2, std::make_pair(E_2_2, E_2_2), "One_2_2"),
    std::make_tuple(Seq_2_2, O_2_2, std::make_pair(E_2_2, Seq_2_2), "Seq_2_2"),
    std::make_tuple(Seq_2_2, O_2_2, std::make_pair(Seq_2_2, E_2_2), "One_2_2"),
    std::make_tuple(E_3_3, O_3_3, std::make_pair(E_3_3, E_3_3), "One_3_3"),
    std::make_tuple(Seq_3_3, O_3_3, std::make_pair(E_3_3, Seq_3_3), "Seq_3_3"),
    std::make_tuple(Seq_3_3, O_3_3, std::make_pair(Seq_3_3, E_3_3), "One_3_3"),
    std::make_tuple(E_4_4, O_4_4, std::make_pair(E_4_4, E_4_4), "One_4_4"),
    std::make_tuple(Seq_4_4, O_4_4, std::make_pair(E_4_4, Seq_4_4), "Seq_4_4"),
    std::make_tuple(Seq_4_4, O_4_4, std::make_pair(Seq_4_4, E_4_4), "One_4_4"),
    std::make_tuple(M_2_2, O_2_2, std::make_pair(M_2_3, M_3_2), "2_3_by_3_2"),
    std::make_tuple(M_3_3, O_3_3, std::make_pair(M_3_4, M_4_3), "3_4_by_4_3"),
    std::make_tuple(Const_2, O_1_1, std::make_pair(V_1_2, V_2_1), "Vectors_1_2_and_2_1"),
    std::make_tuple(Const_3, O_1_1, std::make_pair(V_1_3, V_3_1), "Vectors_1_3_and_3_1"),
    std::make_tuple(Const_4, O_1_1, std::make_pair(V_1_4, V_4_1), "Vectors_1_4_and_4_1"),
    std::make_tuple(Const_5, O_1_1, std::make_pair(V_1_5, V_5_1), "Vectors_1_5_and_5_1")};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<TimofeevNRibbonSchemeOnlyAMPI, InType>(
                                               kTestParam, PPC_SETTINGS_timofeev_n_ribbon_scheme_only_a),
                                           ppc::util::AddFuncTask<TimofeevNRibbonSchemeOnlyASEQ, InType>(
                                               kTestParam, PPC_SETTINGS_timofeev_n_ribbon_scheme_only_a));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = RibbonRunFuncTestsProcesses::PrintFuncTestName<RibbonRunFuncTestsProcesses>;

INSTANTIATE_TEST_SUITE_P(PicMatrixTests, RibbonRunFuncTestsProcesses, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace timofeev_n_ribbon_scheme_only_a
