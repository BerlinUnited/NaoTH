import unittest
import math
from naoth import math2d as m2d


class TestVector2Methods(unittest.TestCase):
    def test_add(self):
        a = m2d.Vector2(100, 100)
        b = m2d.Vector2(1000, 10)
        c = a + b
        self.assertEqual(c.x, 1100)
        self.assertEqual(c.y, 110)

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

    def test_mul(self):
        a = m2d.Vector2(100, 100)
        b = m2d.Vector2(2, 3)
        c = a * b
        d = m2d.Vector2(100, 100) * 5
        self.assertEqual(c, 500)
        self.assertEqual(d.x, 500)
        self.assertEqual(d.y, 500)

    def test_rotate(self):
        a = m2d.Vector2(1, 0)
        a = a.rotate(math.radians(90))
        a.x = math.floor(a.x)
        a.y = math.floor(a.y)
        self.assertEqual(a.x, 0.0)
        self.assertEqual(a.y, 1.0)

    def test_divison(self):
        a = m2d.Vector2(100, 100)
        b = a / 2
        self.assertEqual(b.x, 50.0)
        self.assertEqual(b.y, 50.0)


if __name__ == '__main__':
    unittest.main()
