import unittest
import math2d as m2d
import math


class test_m2d(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        pass

    @classmethod
    def tearDownClass(cls):
        pass

    def setUp(self):
        pass

    def tearDown(self):
        pass

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

        ab = a * b
        ac = a * c
        ad = a * d

        self.assertEqual(ab.x, 50)
        self.assertEqual(ab.y, 0)

        self.assertEqual(ac.x, 0)
        self.assertEqual(ac.y, -50)

        self.assertEqual(ad.x, 25)
        self.assertEqual(ad.y, -350)

        e = 0.5
        f = 0
        g = -2

        ae = a * e
        fa = f * a
        ga = g * a

        self.assertEqual(ae.x, 25)
        self.assertEqual(ae.y, -25)

        self.assertEqual(fa.x, 0)
        self.assertEqual(fa.y, 0)

        self.assertEqual(ga.x, -100)
        self.assertEqual(ga.y, 100)

    def test_div(self):
        a = m2d.Vector2(20,-20)
        b = m2d.Vector2(-10,-5.)

        self.assertEqual(a/5, m2d.Vector2(4,-4))
        self.assertEqual(a/-2.5, m2d.Vector(4.,2.))

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

    def test_mul(self):
        pass

    def test_invert(self):
        pass

    def test_div(self):
        pass

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

    def test_project(self):
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

        self.assertEqual(line_point1.x, 1) # equal to end point
        self.assertEqual(line_point1.y, 1)
        self.assertEqual(line_point2.x, 1)
        self.assertEqual(line_point2.y, 0.5)
        self.assertAlmostEquals(line_point3.x, 3/math.sqrt(2)-1)
        self.assertAlmostEquals(line_point3.y, 3/math.sqrt(2)-1)
        self.assertEqual(line_point4.x, 0)
        self.assertEqual(line_point4.y, 1)

    def test_projection(self):
        """
        #TODO: change method!! Does not work in any case
        begin1 = m2d.Vector2(1, 0)
        end1 = m2d.Vector2(1, 1)
        begin2 = m2d.Vector2(-1, -1)
        end2 = m2d.Vector2(2, 2)
        begin3 = m2d.Vector2(1, 1)
        end3 = m2d.Vector2(-1, 1)

        line1 = m2d.LineSegment(begin1, end1)
        line2 = m2d.LineSegment(begin2, end2)
        line3 = m2d.LineSegment(begin3, end3)

        project1 = line1.project(1)
        project2 = line2.project(0)
        """
        pass

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

        intersect1 = line1.intersect(line2)
        intersect2 = line2.intersect(line3)
        intersect3 = line3.intersect(line1)

        self.assertEqual(intersect1, 1)
        self.assertEqual(intersect2, 1)
        self.assertEqual(intersect3, 1)


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
        self.assertAlmostEquals(line_intersection2, math.sqrt(8))
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
        pass

    def test_min(self):
        pass

    def test_max(self):
        pass

if __name__ == '__main__':
    unittest.main()
