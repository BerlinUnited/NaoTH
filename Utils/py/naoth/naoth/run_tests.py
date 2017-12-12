import unittest
import math2d as m2d


class TestVectorMethods(unittest.TestCase):
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


if __name__ == '__main__':
    unittest.main()
