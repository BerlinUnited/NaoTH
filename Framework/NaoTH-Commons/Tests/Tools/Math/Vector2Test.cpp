#include "Tools/Math/Common.h"
#include "Tools/Math/Vector2.h"
#include <gtest/gtest.h>

using namespace std;

class Vector2Test : public testing::Test
{
    public:
    Vector2<double> vec;
    float tolerance;        

    virtual void SetUp() {
        tolerance = 0.0001f;
        vec = Vector2<double>(3,4);
    }
    virtual void TearDown() {
    }
};

TEST_F(Vector2Test,ArrayAccess) {
    ASSERT_EQ(vec.x,vec[0]) << "accessing vector2.x as array failed";
    ASSERT_EQ(vec.y,vec[1]);
}
TEST_F(Vector2Test,Transpose) {
    vec.transpose();
    ASSERT_EQ(vec.x,4);
    ASSERT_EQ(vec.y,3);
}
TEST_F(Vector2Test,RotateLeft) {
    vec.rotateLeft();
    ASSERT_EQ(vec.x,-4);
    ASSERT_EQ(vec.y,3);
}
TEST_F(Vector2Test,RotateRight) {
    vec.rotateRight();
    ASSERT_EQ(vec.x,4);
    ASSERT_EQ(vec.y,-3);
}
TEST_F(Vector2Test,Normalize) {
    vec.normalize();
    ASSERT_NEAR(vec.x,0.6f,tolerance);
    ASSERT_NEAR(vec.y,0.8f,tolerance);
    Vector2<double> vecNull = Vector2<double>(0,0);
    ASSERT_NEAR(vecNull.x,0,tolerance);
    ASSERT_NEAR(vecNull.y,0,tolerance);
}
TEST_F(Vector2Test,Rotate) {
    vec.rotate(Math::pi_2);
    //pi/2 equals 90 degree, hence rotate left
    ASSERT_NEAR(vec.x,-4,tolerance);
    ASSERT_NEAR(vec.y,3,tolerance);
}

TEST_F(Vector2Test,Angle) {
    ASSERT_NEAR((double)vec.angle(),0.927295f,tolerance);
}
