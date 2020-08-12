import math
import unittest

from naoth import math as m2d
from naoth import math as m3d


class TestMath2D(unittest.TestCase):
    # Vector2 methods

    def test_add(self):
        a = m2d.Vector2(100, 100)
        b = m2d.Vector2(1000, 10)
        c = m2d.Vector2(-100, -100)
        d = m2d.Vector2(-5000, 5.3)
        e = m2d.Vector2(0.245, -32.28)

        ab = a + b
        self.assertEqual(ab.x, 1100)
        self.assertEqual(ab.y, 110)

        ac = a + c
        self.assertEqual(ac.x, 0)
        self.assertEqual(ac.y, 0)

        de = d + e
        self.assertEqual(de.x, -4999.755)
        self.assertEqual(de.y, -26.98)

        cc = c + c
        self.assertEqual(cc.x, -200)
        self.assertEqual(cc.y, -200)

    def test_sub(self):
        a = m2d.Vector2(100, 100)
        b = m2d.Vector2(1000, 10)
        c = b - a
        self.assertEqual(c.x, 900)
        self.assertEqual(c.y, -90)

    def test_neg(self):
        a = m2d.Vector2(100, 100)
        b = -a
        self.assertEqual(b.x, -100)
        self.assertEqual(b.y, -100)

    def test_abs(self):
        a = m2d.Vector2(100, 0)
        b = a.abs()
        self.assertEqual(b, 100)

    def test_rotate(self):
        a = m2d.Vector2(1, 0)
        b = m2d.Vector2(1, 1)

        a90 = a.rotate(1.5708)
        self.assertAlmostEqual(a90.x, 0, places=4)
        self.assertAlmostEqual(a90.y, 1, places=4)
        a180 = a90.rotate(1.5708)
        self.assertAlmostEqual(a180.x, -1, places=4)
        self.assertAlmostEqual(a180.y, 0, places=4)
        a270 = a180.rotate(1.5708)
        self.assertAlmostEqual(a270.x, 0, places=4)
        self.assertAlmostEqual(a270.y, -1, places=4)
        a360 = a270.rotate(1.5708)
        self.assertAlmostEqual(a360.x, 1, places=4)
        self.assertAlmostEqual(a360.y, 0, places=4)

        b90 = b.rotate(1.5708)
        self.assertAlmostEqual(b90.x, -1, places=4)
        self.assertAlmostEqual(b90.y, 1, places=4)
        b180 = b90.rotate(1.5708)
        self.assertAlmostEqual(b180.x, -1, places=4)
        self.assertAlmostEqual(b180.y, -1, places=4)
        b270 = b180.rotate(1.5708)
        self.assertAlmostEqual(b270.x, 1, places=4)
        self.assertAlmostEqual(b270.y, -1, places=4)
        b360 = b270.rotate(1.5708)
        self.assertAlmostEqual(b360.x, 1, places=4)
        self.assertAlmostEqual(b360.y, 1, places=4)

    def test_mul(self):
        a = m2d.Vector2(50, -50)
        b = m2d.Vector2(1, 0)
        c = m2d.Vector2(0, 1)
        d = m2d.Vector2(0.5, 7)

        # Scalarproduct
        ab = a * b
        ac = a * c
        ad = a * d

        self.assertEqual(ab, 50)
        self.assertEqual(ac, -50)
        self.assertEqual(ad, -325)

        e = 0.5
        f = 0
        g = -2

        ea = e * a  # Test for __rmul__
        ae = a * e
        fa = a * f
        ga = a * g

        self.assertEqual(ea.x, 25)
        self.assertEqual(ea.y, -25)

        self.assertEqual(ae.x, 25)
        self.assertEqual(ae.y, -25)

        self.assertEqual(fa.x, 0)
        self.assertEqual(fa.y, 0)

        self.assertEqual(ga.x, -100)
        self.assertEqual(ga.y, 100)

        # Negative Test
        with self.assertRaises(TypeError):
            t = m2d.Vector2(50, -50)
            z = m3d.Vector3(1, 0, 1)
            t * z

    def test_div(self):
        a = m2d.Vector2(20, -20)
        b = m2d.Vector2(4, -4)
        c = m2d.Vector2(-8., 8.)

        d = a/5
        e = a/-2.5

        self.assertEqual(d.x, b.x)
        self.assertEqual(d.y, b.y)

        self.assertEqual(e.x, c.x)
        self.assertEqual(e.y, c.y)

    def test_normalize(self):
        a = m2d.Vector2(1, 0)
        b = m2d.Vector2(0, -1.)
        c = m2d.Vector2(math.sqrt(2), math.sqrt(2))
        d = m2d.Vector2(0, 0)

        a = a.normalize()
        b = b.normalize()
        c = c.normalize()
        d = d.normalize()

        self.assertEqual(a.x, 1)
        self.assertEqual(a.y, 0)
        self.assertEqual(b.x, 0)
        self.assertEqual(b.y, -1.)
        self.assertAlmostEqual(c.x, math.sqrt(2)/2)
        self.assertAlmostEqual(c.y, math.sqrt(2)/2)
        self.assertEqual(d.x, 0)
        self.assertEqual(d.y, 0)

        c_n = c.normalize()

        self.assertEqual(c_n.x, c.x)
        self.assertEqual(c_n.y, c.y)

    def test_normalize_length(self):
        a = m2d.Vector2(1, 0)
        b = m2d.Vector2(0, -1.)
        c = m2d.Vector2(math.sqrt(2), math.sqrt(2))

        a = a.normalize_length(2.6)
        b = b.normalize_length(-15)
        c = c.normalize_length(0)

        self.assertEqual(a.x, 2.6)
        self.assertEqual(a.y, 0)
        self.assertEqual(b.x, 0)
        self.assertEqual(b.y, 15)
        self.assertEqual(c.x, 0)
        self.assertEqual(c.y, 0)

    def test_rotate_right(self):
        a = m2d.Vector2(1, 0)
        b = m2d.Vector2(0, -1.)
        c = m2d.Vector2(math.sqrt(2), math.sqrt(2))

        a = a.rotate_right()
        b = b.rotate_right()
        c = c.rotate_right()

        self.assertEqual(a.x, 0)
        self.assertEqual(a.y, -1)
        self.assertEqual(b.x, -1)
        self.assertEqual(b.y, 0)
        self.assertEqual(c.x, math.sqrt(2))
        self.assertEqual(c.y, -math.sqrt(2))

        a = a.rotate_right().rotate_right()

        self.assertEqual(a.x, 0)
        self.assertEqual(a.y, 1)

    def test_angle(self):
        a = m2d.Vector2(1, 0)
        b = m2d.Vector2(0, -1.)
        c = m2d.Vector2(math.sqrt(2), math.sqrt(2))
        d = m2d.Vector2(-1, 0)

        a_angle = a.angle()
        b_angle = b.angle()
        c_angle = c.angle()
        d_angle = d.angle()

        self.assertEqual(a_angle, math.radians(0))
        self.assertEqual(b_angle, math.radians(-90))
        self.assertEqual(c_angle, math.radians(45))
        self.assertEqual(d_angle, math.radians(180))

    # Pose2D methods

    def test_global_transformation(self):
        # the multiplication transforms a vector in local coordinates to global coordinates.
        # syntax pose2d(robot) in global coordinates * a vector in the robots coordinate system
        glob_robot_pose = m2d.Pose2D(m2d.Vector2(-1000, -1000), 0)

        # test the init method
        self.assertEqual(glob_robot_pose.translation.x, -1000)
        self.assertEqual(glob_robot_pose.translation.y, -1000)
        self.assertEqual(glob_robot_pose.rotation, 0)

        rel_ball_position = m2d.Vector2(100, 0)

        glob_ball_pos = glob_robot_pose * rel_ball_position
        self.assertEqual(glob_ball_pos.x, -900)
        self.assertEqual(glob_ball_pos.y, -1000)

    def test_invert(self):
        glob_robot_pose = m2d.Pose2D(m2d.Vector2(-1000, -1000), 20)
        test = ~glob_robot_pose
        testtest = ~test

        self.assertAlmostEqual(glob_robot_pose.translation.x, testtest.translation.x, places=9)
        self.assertAlmostEqual(glob_robot_pose.translation.y, testtest.translation.y, places=9)
        self.assertAlmostEqual(glob_robot_pose.rotation, testtest.rotation, places=9)

    def test_local_transformation(self):
        # the division transforms a vector in global coordinates to local coordinates.
        # vector in local coord = pose3d(robot) in global coordinates / global vector
        glob_robot_pose = m2d.Pose2D(m2d.Vector2(1000, 0), 0)
        glob_ball_pose = m2d.Vector2(1200, 0)

        test = glob_robot_pose / glob_ball_pose
        self.assertEqual(test.x, 200)
        self.assertEqual(test.y, 0)

    # LineSegment

    def test_end(self):
        begin1 = m2d.Vector2(1, 0)
        end1 = m2d.Vector2(1, 1)
        begin2 = m2d.Vector2(-1, -1)
        end2 = m2d.Vector2(2, 2)
        begin3 = m2d.Vector2(1, 1)
        end3 = m2d.Vector2(-1, 1)

        line1 = m2d.LineSegment(begin1, end1)
        line2 = m2d.LineSegment(begin2, end2)
        line3 = m2d.LineSegment(begin3, end3)

        line_end1 = line1.end()
        line_end2 = line2.end()
        line_end3 = line3.end()

        self.assertEqual(line_end1.x, 1)
        self.assertEqual(line_end1.y, 1)
        self.assertEqual(line_end2.x, 2)
        self.assertEqual(line_end2.y, 2)
        self.assertEqual(line_end3.x, -1)
        self.assertEqual(line_end3.y, 1)

    def test_point(self):
        begin1 = m2d.Vector2(1, 0)
        end1 = m2d.Vector2(1, 1)
        begin2 = m2d.Vector2(-1, -1)
        end2 = m2d.Vector2(2, 2)
        begin3 = m2d.Vector2(1, 1)
        end3 = m2d.Vector2(-1, 1)

        line1 = m2d.LineSegment(begin1, end1)
        line2 = m2d.LineSegment(begin2, end2)
        line3 = m2d.LineSegment(begin3, end3)

        line_point1 = line1.point(20)
        line_point2 = line1.point(0.5)
        line_point3 = line2.point(3)
        line_point4 = line3.point(1)

        self.assertEqual(line_point1.x, 1)  # equal to end point
        self.assertEqual(line_point1.y, 1)
        self.assertEqual(line_point2.x, 1)
        self.assertEqual(line_point2.y, 0.5)
        self.assertAlmostEqual(line_point3.x, 3/math.sqrt(2)-1)
        self.assertAlmostEqual(line_point3.y, 3/math.sqrt(2)-1)
        self.assertEqual(line_point4.x, 0)
        self.assertEqual(line_point4.y, 1)

    def test_projection(self):

        begin1 = m2d.Vector2(-1, 0)
        end1 = m2d.Vector2(1, 0)

        line1 = m2d.LineSegment(begin1, end1)

        project1 = line1.projection(m2d.Vector2(0, 1))
        project2 = line1.projection(m2d.Vector2(5, 1))
        self.assertEqual(project1.x, 0)
        self.assertEqual(project1.y, 0)
        self.assertEqual(project2.x, 1)
        self.assertEqual(project2.y, 0)

    def test_intersection(self):
        begin1 = m2d.Vector2(1, 0)
        end1 = m2d.Vector2(1, 1)
        begin2 = m2d.Vector2(-1, -1)
        end2 = m2d.Vector2(2, 2)
        begin3 = m2d.Vector2(1, 1)
        end3 = m2d.Vector2(-1, 1)

        line1 = m2d.LineSegment(begin1, end1)
        line2 = m2d.LineSegment(begin2, end2)
        line3 = m2d.LineSegment(begin3, end3)

        intersection1 = line1.intersection(line2)
        intersection2 = line2.intersection(line3)
        intersection3 = line3.intersection(line1)

        self.assertEqual(intersection1, 1)
        self.assertAlmostEqual(intersection2, math.sqrt(8))
        self.assertEqual(intersection3, 0)

    def test_line_intersection(self):
        begin1 = m2d.Vector2(1, 0)
        end1 = m2d.Vector2(1, 1)
        begin2 = m2d.Vector2(-1, -1)
        end2 = m2d.Vector2(2, 2)
        begin3 = m2d.Vector2(1, 1)
        end3 = m2d.Vector2(-1, 1)

        line1 = m2d.LineSegment(begin1, end1)
        line2 = m2d.LineSegment(begin2, end2)
        line3 = m2d.LineSegment(begin3, end3)

        line_intersection1 = line1.line_intersection(line2)
        line_intersection2 = line2.line_intersection(line3)
        line_intersection3 = line3.line_intersection(line1)

        self.assertEqual(line_intersection1, 1)
        self.assertAlmostEqual(line_intersection2, math.sqrt(8))
        self.assertEqual(line_intersection3, 0)

    def test_intersect(self):
        begin1 = m2d.Vector2(1, 0)
        end1 = m2d.Vector2(1, 1)
        begin2 = m2d.Vector2(-1, -1)
        end2 = m2d.Vector2(2, 2)
        begin3 = m2d.Vector2(1, 1)
        end3 = m2d.Vector2(-1, 1)
        begin4 = m2d.Vector2(-1, 1)
        end4 = m2d.Vector2(-1, -1)

        line1 = m2d.LineSegment(begin1, end1)
        line2 = m2d.LineSegment(begin2, end2)
        line3 = m2d.LineSegment(begin3, end3)
        line4 = m2d.LineSegment(begin4, end4)

        intersect1 = line1.intersect(line2)
        intersect2 = line2.intersect(line3)
        intersect3 = line3.intersect(line1)
        intersect4 = line4.intersect(line1)

        self.assertEqual(intersect1, 1)
        self.assertEqual(intersect2, 1)
        self.assertEqual(intersect3, 1)
        self.assertEqual(intersect4, 0)

    # Rect2d methods

    def test_inside(self):
        a1 = m2d.Vector2(0, 0)
        a2 = m2d.Vector2(1, 1)
        a = m2d.Rect2d(a1, a2)

        b1 = m2d.Vector2(-1, -1)
        b2 = m2d.Vector2(1, 1)
        b = m2d.Rect2d(b1, b2)

        c1 = m2d.Vector2(1, 1)
        c2 = m2d.Vector2(1, -1)
        c = m2d.Rect2d(c1, c2)

        self.assertEqual(a.inside(m2d.Vector2(0, 0)), 1)
        self.assertEqual(b.inside(m2d.Vector2(0, 0)), 1)
        self.assertEqual(c.inside(m2d.Vector2(0, 0)), 0)

        self.assertEqual(a.inside(m2d.Vector2(1, 0.5)), 1)
        self.assertEqual(b.inside(m2d.Vector2(1, 0.5)), 1)
        self.assertEqual(c.inside(m2d.Vector2(1, 0.5)), 1)

        self.assertEqual(a.inside(m2d.Vector2(2, -0.5)), 0)
        self.assertEqual(b.inside(m2d.Vector2(0, -1.1)), 0)
        self.assertEqual(c.inside(m2d.Vector2(1, -1)), 1)

    def test_min(self):
        a1 = m2d.Vector2(0, 0)
        a2 = m2d.Vector2(1, 1)
        a = m2d.Rect2d(a1, a2)
        a_min = a.min()

        b1 = m2d.Vector2(-1, -1)
        b2 = m2d.Vector2(1, 1)
        b = m2d.Rect2d(b1, b2)
        b_min = b.min()

        c1 = m2d.Vector2(1, 1)
        c2 = m2d.Vector2(1, -1)
        c = m2d.Rect2d(c1, c2)
        c_min = c.min()

        d1 = m2d.Vector2(-1, 1)
        d2 = m2d.Vector2(1, -1)
        d = m2d.Rect2d(d1, d2)
        d_min = d.min()

        self.assertEqual(a_min.x, 0)
        self.assertEqual(a_min.y, 0)
        self.assertEqual(b_min.x, -1)
        self.assertEqual(b_min.y, -1)
        self.assertEqual(c_min.x, 1)
        self.assertEqual(c_min.y, -1)
        self.assertEqual(d_min.x, -1)
        self.assertEqual(d_min.y, -1)

    def test_max(self):
        a1 = m2d.Vector2(0, 0)
        a2 = m2d.Vector2(1, 1)
        a = m2d.Rect2d(a1, a2)
        a_max = a.max()

        b1 = m2d.Vector2(-1, -1)
        b2 = m2d.Vector2(1, 1)
        b = m2d.Rect2d(b1, b2)
        b_max = b.max()

        c1 = m2d.Vector2(1, 1)
        c2 = m2d.Vector2(1, -1)
        c = m2d.Rect2d(c1, c2)
        c_max = c.max()

        d1 = m2d.Vector2(-1, 1)
        d2 = m2d.Vector2(1, -1)
        d = m2d.Rect2d(d1, d2)
        d_max = d.max()

        self.assertEqual(a_max.x, 1)
        self.assertEqual(a_max.y, 1)
        self.assertEqual(b_max.x, 1)
        self.assertEqual(b_max.y, 1)
        self.assertEqual(c_max.x, 1)
        self.assertEqual(c_max.y, 1)
        self.assertEqual(d_max.x, 1)
        self.assertEqual(d_max.y, 1)


if __name__ == '__main__':
    unittest.main()
