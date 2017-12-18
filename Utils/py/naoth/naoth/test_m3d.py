import unittest
import math3d as m3d


class TestMath3d(unittest.TestCase):

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


if __name__ == '__main__':
    unittest.main()
