#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <array>
#include <cstddef>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "timofeev_n_radix_merge_sort/common/include/common.hpp"
#include "timofeev_n_radix_merge_sort/mpi/include/ops_mpi.hpp"
#include "timofeev_n_radix_merge_sort/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace timofeev_n_radix_merge_sort {

class RadixMergeRunFuncTestsProcesses : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
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
    if (output_data.size() != expected_data_.size()) {
      return false;
    }
    bool yesyes = true;
    for (size_t i = 0; i < output_data.size(); i++) {
      yesyes &= output_data[i] == expected_data_[i];
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

TEST_P(RadixMergeRunFuncTestsProcesses, MatmulFromPic) {
  ExecuteTest(GetParam());
}

std::vector<int> Sorted5 = {0, 1, 2, 3, 4};
std::vector<int> Reverse5 = {4, 3, 2, 1, 0};
std::vector<int> Zeros5(5);

std::vector<int> Sorted4 = {0, 1, 2, 3};
std::vector<int> Reverse4 = {3, 2, 1, 0};
std::vector<int> Zeros4(4);

std::vector<int> Sorted3 = {0, 1, 2};
std::vector<int> Reverse3 = {2, 1, 0};
std::vector<int> Zeros3(3);

std::vector<int> Sorted2 = {0, 1};
std::vector<int> Reverse2 = {1, 0};
std::vector<int> Zeros2(2);

std::vector<int> Sorted1 = {0};
std::vector<int> Reverse1 = {0};
std::vector<int> Zeros1(1);

std::vector<int> SortedNeg5 = {-4, -3, -2, -1, 0};
std::vector<int> ReverseNeg5 = {0, -1, -2, -3, -4};

std::vector<int> SortedNeg4 = {-4, -3, -2, -1};
std::vector<int> ReverseNeg4 = {-1, -2, -3, -4};

std::vector<int> SortedNeg3 = {-4, -3, -2};
std::vector<int> ReverseNeg3 = {-2, -3, -4};

std::vector<int> SortedNeg2 = {-4, -3};
std::vector<int> ReverseNeg2 = {-3, -4};

std::vector<int> SortedNeg1 = {-4};
std::vector<int> ReverseNeg1 = {-4};

std::vector<int> SortedTHEGRANDFINAL = {-4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
std::vector<int> ReverseTHEGRANDFINAL = {5, 4, 3, 2, 1, 0, -1, -2, -3, -4};
std::vector<int> ZerosTHEGRANDFINAL(10);

const std::array<TestType, 11> kTestParam = {
    std::make_tuple(Sorted5, Zeros5, Reverse5, "first"),
    std::make_tuple(Sorted4, Zeros4, Reverse4, "second"),
    std::make_tuple(Sorted3, Zeros3, Reverse3, "third"),
    std::make_tuple(Sorted2, Zeros2, Reverse2, "fourth"),
    std::make_tuple(Sorted1, Zeros1, Reverse1, "fifth"),
    std::make_tuple(SortedNeg5, Zeros5, ReverseNeg5, "firstNeg"),
    std::make_tuple(SortedNeg4, Zeros4, ReverseNeg4, "secondNeg"),
    std::make_tuple(SortedNeg3, Zeros3, ReverseNeg3, "thirdNeg"),
    std::make_tuple(SortedNeg2, Zeros2, ReverseNeg2, "fourthNeg"),
    std::make_tuple(SortedNeg1, Zeros1, ReverseNeg1, "fifthNeg"),
    std::make_tuple(SortedTHEGRANDFINAL, ZerosTHEGRANDFINAL, ReverseTHEGRANDFINAL, "THEGRANDFINAL")};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<TimofeevNRadixMergeMPI, InType>(kTestParam, PPC_SETTINGS_timofeev_n_radix_merge_sort),
    ppc::util::AddFuncTask<TimofeevNRadixMergeSEQ, InType>(kTestParam, PPC_SETTINGS_timofeev_n_radix_merge_sort));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = RadixMergeRunFuncTestsProcesses::PrintFuncTestName<RadixMergeRunFuncTestsProcesses>;

INSTANTIATE_TEST_SUITE_P(PicMatrixTests, RadixMergeRunFuncTestsProcesses, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace timofeev_n_radix_merge_sort
