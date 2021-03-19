import math
import unittest

from naoth import math as m3d


class TestMath3D(unittest.TestCase):

    def test_add(self):
        a = m3d.Vector3(100, 100, 100)
        b = m3d.Vector3(1000, 100, 10)
        c = m3d.Vector3(-100, -100, -100)
        d = m3d.Vector3(-5000, 5.3)
        e = m3d.Vector3(0.245, -32.28)
        f = m3d.Vector3()

        ab = a + b
        self.assertEqual(ab.x, 1100)
        self.assertEqual(ab.y, 200)
        self.assertEqual(ab.z, 110)

        ac = a + c
        self.assertEqual(ac.x, 0)
        self.assertEqual(ac.y, 0)
        self.assertEqual(ac.z, 0)

        de = d + e
        self.assertEqual(de.x, -4999.755)
        self.assertEqual(de.y, -26.98)
        self.assertEqual(de.z, 0)

        cc = c + c
        self.assertEqual(cc.x, -200)
        self.assertEqual(cc.y, -200)
        self.assertEqual(cc.z, -200)

        ff = f + f
        self.assertEqual(ff.x, 0)
        self.assertEqual(ff.y, 0)
        self.assertEqual(ff.z, 0)

    def test_sub(self):
        a = m3d.Vector3(100, 100, 100)
        b = m3d.Vector3(1000, 100, 10)
        c = m3d.Vector3(-100, -100, -100)
        d = m3d.Vector3(-5000, 5.3)
        e = m3d.Vector3(0.245, -32.28)
        f = m3d.Vector3()

        ab = a - b
        self.assertEqual(ab.x, -900)
        self.assertEqual(ab.y, 0)
        self.assertEqual(ab.z, 90)

        ac = a - c
        self.assertEqual(ac.x, 200)
        self.assertEqual(ac.y, 200)
        self.assertEqual(ac.z, 200)

        de = d - e
        self.assertEqual(de.x, -5000.245)
        self.assertEqual(de.y, 37.58)
        self.assertEqual(de.z, 0)

        cc = c - c
        self.assertEqual(cc.x, 0)
        self.assertEqual(cc.y, 0)
        self.assertEqual(cc.z, 0)

        ff = f - f
        self.assertEqual(ff.x, 0)
        self.assertEqual(ff.y, 0)
        self.assertEqual(ff.z, 0)

    def test_neg(self):
        a = m3d.Vector3(100, 100, 100)
        b = -a
        self.assertEqual(b.x, -100)
        self.assertEqual(b.y, -100)
        self.assertEqual(b.z, -100)

    def test_abs(self):
        a = m3d.Vector3(100, 0, 0)
        b = a.abs()
        self.assertEqual(b, 100)

    def test_mul(self):
        a = m3d.Vector3(50, -50, 50)
        b = m3d.Vector3(1, 0, 0)
        c = m3d.Vector3(0, 1, 1)
        d = m3d.Vector3(0.5, 7, 2)

        # scalar product
        ab = a * b
        ac = a * c
        ad = a * d

        self.assertEqual(ab, 50)
        self.assertEqual(ac, 0)
        self.assertEqual(ad, -225)

        e = 0.5
        f = 0
        g = -2

        ea = e * a  # Test for __rmul__
        ae = a * e
        fa = a * f
        ga = a * g

        self.assertEqual(ea.x, 25)
        self.assertEqual(ea.y, -25)
        self.assertEqual(ea.z, 25)

        self.assertEqual(ae.x, 25)
        self.assertEqual(ae.y, -25)
        self.assertEqual(ae.z, 25)

        self.assertEqual(fa.x, 0)
        self.assertEqual(fa.y, 0)
        self.assertEqual(fa.z, 0)

        self.assertEqual(ga.x, -100)
        self.assertEqual(ga.y, 100)
        self.assertEqual(ga.z, -100)

    # Matrix3x3 Methods
    def test_init(self):
        col1 = m3d.Vector3(1, 2, 3)
        col2 = m3d.Vector3(4, 5, 6)
        col3 = m3d.Vector3(7, 8, 9)
        matrix = m3d.Matrix3x3(col1, col2, col3)
        self.assertEqual(matrix.c1.x, 1)
        self.assertEqual(matrix.c1.y, 2)
        self.assertEqual(matrix.c1.z, 3)
        self.assertEqual(matrix.c2.x, 4)
        self.assertEqual(matrix.c2.y, 5)
        self.assertEqual(matrix.c2.z, 6)
        self.assertEqual(matrix.c3.x, 7)
        self.assertEqual(matrix.c3.y, 8)
        self.assertEqual(matrix.c3.z, 9)

    def test_matrix_multiplication(self):
        col1 = m3d.Vector3(1, 2, 3)
        col2 = m3d.Vector3(4, 5, 6)
        col3 = m3d.Vector3(7, 8, 9)
        matrix1 = m3d.Matrix3x3(col1, col2, col3)

        col1 = m3d.Vector3(2, 0, 0)
        col2 = m3d.Vector3(0, 2, 0)
        col3 = m3d.Vector3(0, 0, 2)
        matrix2 = m3d.Matrix3x3(col1, col2, col3)

        result1 = matrix1 * matrix2

        self.assertEqual(result1.c1.x, 2)
        self.assertEqual(result1.c1.y, 4)
        self.assertEqual(result1.c1.z, 6)
        self.assertEqual(result1.c2.x, 8)
        self.assertEqual(result1.c2.y, 10)
        self.assertEqual(result1.c2.z, 12)
        self.assertEqual(result1.c3.x, 14)
        self.assertEqual(result1.c3.y, 16)
        self.assertEqual(result1.c3.z, 18)

        result2 = matrix1 * m3d.Vector3(1, 1, 1)
        self.assertEqual(result2.x, 12)
        self.assertEqual(result2.y, 15)
        self.assertEqual(result2.z, 18)

    def test_transpose(self):
        col1 = m3d.Vector3(1, 2, 3)
        col2 = m3d.Vector3(4, 5, 6)
        col3 = m3d.Vector3(7, 8, 9)
        matrix = m3d.Matrix3x3(col1, col2, col3)
        matrix = matrix.transpose()
        self.assertEqual(matrix.c1.x, 1)
        self.assertEqual(matrix.c1.y, 4)
        self.assertEqual(matrix.c1.z, 7)
        self.assertEqual(matrix.c2.x, 2)
        self.assertEqual(matrix.c2.y, 5)
        self.assertEqual(matrix.c2.z, 8)
        self.assertEqual(matrix.c3.x, 3)
        self.assertEqual(matrix.c3.y, 6)
        self.assertEqual(matrix.c3.z, 9)

    def test_eye(self):
        matrix = m3d.Matrix3x3.eye()
        self.assertEqual(matrix.c1.x, 1)
        self.assertEqual(matrix.c1.y, 0)
        self.assertEqual(matrix.c1.z, 0)
        self.assertEqual(matrix.c2.x, 0)
        self.assertEqual(matrix.c2.y, 1)
        self.assertEqual(matrix.c2.z, 0)
        self.assertEqual(matrix.c3.x, 0)
        self.assertEqual(matrix.c3.y, 0)
        self.assertEqual(matrix.c3.z, 1)

    # test for Pose3D
    def test_pose3d_init(self):
        test_pose = m3d.Pose3D()
        self.assertEqual(test_pose.rotation.c1.x, 1)
        self.assertEqual(test_pose.rotation.c1.y, 0)
        self.assertEqual(test_pose.rotation.c1.z, 0)
        self.assertEqual(test_pose.rotation.c2.x, 0)
        self.assertEqual(test_pose.rotation.c2.y, 1)
        self.assertEqual(test_pose.rotation.c2.z, 0)
        self.assertEqual(test_pose.rotation.c3.x, 0)
        self.assertEqual(test_pose.rotation.c3.y, 0)
        self.assertEqual(test_pose.rotation.c3.z, 1)
        self.assertEqual(test_pose.translation.x, 0)
        self.assertEqual(test_pose.translation.y, 0)
        self.assertEqual(test_pose.translation.z, 0)

    def test_pose3d_mul(self):
        # the multiplication transforms a vector in local coordinates to global coordinates.
        # syntax pose2d(robot) in global coordinates * a vector in the robots coordinate system
        glob_robot_pose = m3d.Pose3D()
        glob_robot_pose.translation = m3d.Vector3(100, 0, 0)
        glob_robot_pose.rotation = m3d.Matrix3x3.eye()

        rel_ball_position = m3d.Vector3(100, 10, 0)
        glob_ball_pos = glob_robot_pose * rel_ball_position
        self.assertEqual(glob_ball_pos.x, 200)
        self.assertEqual(glob_ball_pos.y, 10)
        self.assertEqual(glob_ball_pos.z, 0)

    def test_pose3d_invert(self):
        # rotation matrix has to be orthogonal
        col1 = m3d.Vector3(1, 0, 0)
        col2 = m3d.Vector3(0, math.cos(2), math.sin(2))
        col3 = m3d.Vector3(0, -math.sin(2), math.cos(2))
        rotation = m3d.Matrix3x3(col1, col2, col3)
        translation = m3d.Vector3(1234, 123, 798.1)
        pose = m3d.Pose3D()
        pose.translation = translation
        pose.rotation = rotation

        inverted_pose = ~pose
        original = ~inverted_pose

        self.assertEqual(pose.rotation.c1.x, original.rotation.c1.x)
        self.assertEqual(pose.rotation.c1.y, original.rotation.c1.y)
        self.assertEqual(pose.rotation.c1.z, original.rotation.c1.z)
        self.assertEqual(pose.rotation.c2.x, original.rotation.c2.x)
        self.assertEqual(pose.rotation.c2.y, original.rotation.c2.y)
        self.assertEqual(pose.rotation.c2.z, original.rotation.c2.z)
        self.assertEqual(pose.rotation.c3.x, original.rotation.c3.x)
        self.assertEqual(pose.rotation.c3.y, original.rotation.c3.y)
        self.assertEqual(pose.rotation.c3.z, original.rotation.c3.z)

        self.assertAlmostEqual(pose.translation.x, original.translation.x, places=9)
        self.assertAlmostEqual(pose.translation.y, original.translation.y, places=9)
        self.assertAlmostEqual(pose.translation.z, original.translation.z, places=9)



if __name__ == '__main__':
    unittest.main()
