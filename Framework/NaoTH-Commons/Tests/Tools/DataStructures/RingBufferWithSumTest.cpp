#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools/Math/Common.h"
#include <vector>
#include <gtest/gtest.h>

using namespace std;

class RingBufferWithSumTest : public testing::Test
{
    public:
    RingBufferWithSum<double, 29> buffer;
    float tolerance;        

    std::vector<double> test_vector;

    virtual void SetUp() {
        tolerance = 0.0001f;

        for(size_t i = 0; i < 17; i++) {
          double x = Math::random();
          buffer.add(x);
        }

        for(size_t i = 0; i < 29; i++) {
          double x = Math::random();
          test_vector.push_back(x);
          buffer.add(x);
        }
    }
    virtual void TearDown() {
    }
};

TEST_F(RingBufferWithSumTest,Size) {
  ASSERT_EQ(buffer.size(),test_vector.size());
  ASSERT_EQ(buffer.size(),buffer.getMaxEntries());
}

TEST_F(RingBufferWithSumTest,ArrayAccess) {
  ASSERT_EQ(buffer.size(),test_vector.size());

  for(size_t i = 0; i < test_vector.size(); i++) {
    ASSERT_DOUBLE_EQ(test_vector[test_vector.size()-i-1],buffer[i]);
    ASSERT_DOUBLE_EQ(buffer.getEntry(i),buffer[i]);
  }

  EXPECT_DEATH(buffer.getEntry(buffer.size()), "");
  EXPECT_DEATH(buffer.getEntry(-1), "");
}

TEST_F(RingBufferWithSumTest,Average) {
  
  double sum = 0.0;
  for(int i = 0; i < buffer.size(); i++) {
    sum += buffer[i];
  }
  ASSERT_DOUBLE_EQ(sum/buffer.size(), buffer.getAverage());
}

