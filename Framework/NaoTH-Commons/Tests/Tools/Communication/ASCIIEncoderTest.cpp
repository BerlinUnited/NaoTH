/**
 * @file ASCIIEncoderTest.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief test ASCIIEncoder
 *
 */

#include "Tools/Communication/ASCIIEncoder.h"
#include <gtest/gtest.h>

using namespace std;

class ASCIIEncoderTest : public testing::Test, public ASCIIEncoder {
};

TEST_F(ASCIIEncoderTest, EncodeAndDecodeUnsigned)
{
  for (size_t size = 1; size <= 6; size++)
  {
    unsigned int max = maxUnsigned(size);
    unsigned int step = std::max(1u, max / 100);
    for (unsigned int i = 0; i <= max - step; i += step)
    {
      string s = encode(i, size);
      EXPECT_EQ(size, s.size());

      unsigned int v = decodeUnsigned(s);
      ASSERT_EQ(v, i) << "encoded and decoded unsigned int did not match original";
    }
  }
}

TEST_F(ASCIIEncoderTest, EncodeAndDecodeSigned)
{
  for (size_t size = 1; size <= 6; size++)
  {
    int max = maxSigned(size);
    int step = std::max(1, max / 100);
    for (int i = -max; i <= max - step; i += step)
    {
      string s = encode(i, size);
      EXPECT_EQ(size, s.size()) << "size of encoded signed int did not match expected size";

      int v = decodeSigned(s);
      ASSERT_EQ(v, i) << "encoded and decoded signed int did not match original";
    }
  }
}

TEST_F(ASCIIEncoderTest, EncodeAndDecodeDouble)
{
  for (size_t size = 1; size <= 6; size++)
  {
    double max = maxSigned(size);
    double step = std::max(1.0, max / 100);
    for (double i = -max; i <= max - step; i += step)
    {
      string s = encode(i, max, size);
      EXPECT_EQ(size, s.size()) << "size of encoded double did not match ";

      double v = decodeDouble(s, max);
      ASSERT_NEAR(v, i, max * 0.0003) << "encoded and decoded double did not match original";
    }
  }
}

TEST_F(ASCIIEncoderTest, EncodeAndDecodeVector2D)
{
  for (size_t size = 4; size <= 20; size++)
  {
    double max = sqrt(static_cast<double>(maxUnsigned(size)))/2;
    double step = std::max(1.0, max / 10);
    for (double i = -max; i <= max - step; i += step)
    {
      for (double j = -max; j <= max - step; j += step)
      {
        Vector2<double> maxVec = Vector2<double>(max, max);
        string s = encode(Vector2<double>(i, j), maxVec, size);
        EXPECT_EQ(size, s.size()) << "size and encoded size of vector did not match";

        Vector2<double> v = decodeVector2D(s, maxVec);
        EXPECT_NEAR(v.x, i, max * 0.0003);
        ASSERT_NEAR(v.y, j, max * 0.0003) << "encoded and decoded Vector2D did not match original";
      }
    }
  }
}

TEST_F(ASCIIEncoderTest, EncodeAndDecodePose2D)
{
  double anglePiece = Math::fromDegrees(10);
  double angeStep = Math::fromDegrees(35);
  for (size_t size = 5; size <= 20; size++)
  {
    double max = sqrt(static_cast<double>(maxUnsigned(size))) / 2 / anglePiece;
    double step = std::max(1.0, max / 10);
    for (double i = -max; i <= max - step; i += step)
    {
      for (double j = -max; j <= max - step; j += step)
      {
        for(double k=-Math::pi; k< Math::pi - angeStep; k+=angeStep)
        {
          Vector2<double> maxVec = Vector2<double>(max, max);
          string s = encode(Pose2D(k, i, j), maxVec, anglePiece, size);
          EXPECT_EQ(size, s.size()) << "encoded size of Pose2D did not match expected size";

          Pose2D v = decodePose2D(s, maxVec, anglePiece);
          EXPECT_NEAR(v.translation.x, i, max * 0.0003) << "encoded and decoded Pose2D did not match original ";
          EXPECT_NEAR(v.translation.y, j, max * 0.0003)  << "encoded and decoded Pose2D did not match original ";
          ASSERT_NEAR(v.rotation, k, anglePiece)  << "encoded and decoded Pose2D did not match original ";
        }
      }
    }
  }
}
