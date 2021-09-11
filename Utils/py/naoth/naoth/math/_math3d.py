import math as _math


class Vector3:
    def __init__(self, x=0, y=0, z=0):
        self.x = x
        self.y = y
        self.z = z

    def __add__(self, other):
        return Vector3(self.x + other.x, self.y + other.y, self.z + other.z)

    def __sub__(self, other):
        return Vector3(self.x - other.x, self.y - other.y, self.z - other.z)

    def __neg__(self):
        return Vector3(-self.x, -self.y, -self.z)

    def abs(self):
        return _math.sqrt(self * self)

    def __mul__(self, other):
        if isinstance(other, Vector3):
            return self.x * other.x + self.y * other.y + self.z * other.z
        elif isinstance(other, (int, float)):
            return Vector3(self.x * other, self.y * other, self.z * other)
        else:
            return NotImplemented

    def __rmul__(self, other):
        if isinstance(other, Vector3):
            return self.x * other.x + self.y * other.y + self.z * other.z
        elif isinstance(other, (int, float)):
            return Vector3(self.x * other, self.y * other, self.z * other)
        else:
            return NotImplemented

    def __str__(self):
        return "({0},{1},{2})".format(self.x, self.y, self.z)

    def __repr__(self):
        return str(self)


class Matrix3x3:
    def __init__(self, c1=Vector3(), c2=Vector3(), c3=Vector3()):
        self.c1 = c1
        self.c2 = c2
        self.c3 = c3

    def __mul__(self, other):
        if isinstance(other, Matrix3x3):
            return Matrix3x3(self * other.c1, self * other.c2, self * other.c3)
        elif isinstance(other, Vector3):
            return self.c1 * other.x + self.c2 * other.y + self.c3 * other.z
        else:
            return NotImplemented

    def transpose(self):
        return Matrix3x3(
          Vector3(self.c1.x, self.c2.x, self.c3.x),
          Vector3(self.c1.y, self.c2.y, self.c3.y),
          Vector3(self.c1.z, self.c2.z, self.c3.z))

    @classmethod
    def eye(Matrix3x3):
        # the vectors are columnwise
        return Matrix3x3(
          Vector3(1, 0, 0),
          Vector3(0, 1, 0),
          Vector3(0, 0, 1)
        )

    def __str__(self):
        return "{0} {1} {2}\n{3} {4} {5}\n{6} {7} {8}".format(
          self.c1.x, self.c2.x, self.c3.x,
          self.c1.y, self.c2.y, self.c3.y,
          self.c1.z, self.c2.z, self.c3.z)

    def __repr__(self):
        return str(self)


class Pose3D:
    def __init__(self):
        self.translation = Vector3()
        self.rotation = Matrix3x3.eye()

    def __mul__(self, other):
        if isinstance(other, Vector3):
            return self.rotation * other + self.translation
        elif isinstance(other, Pose3D):
            p = Pose3D()
            p.translation = self * other.translation
            p.rotation = self.rotation * other.rotation
            return p
        else:
            return NotImplemented

    def __invert__(self):
        p = Pose3D()
        p.rotation = self.rotation.transpose()
        p.translation = p.rotation * (Vector3() - self.translation)
        return p

    def __str__(self):
        return "(translation = {0}, rotation = {1})".format(self.translation, self.rotation)

    def __repr__(self):
        return str(self)
