import math as _math
import numbers as _numbers


def clamp(x, minimum, maximum):
    # function f where f(x)=x if min<= x <= max, f(x) = minimum if x < minimum, f(x) = maximum if x > maximum
    return max(minimum, min(x, maximum))


class Vector2:
    def __init__(self, x=0.0, y=0.0, pb_vector=None):
        if pb_vector is not None:
            self.x = pb_vector.x
            self.y = pb_vector.y
        else:
            self.x = x
            self.y = y

    def __add__(self, other):
        return Vector2(self.x + other.x, self.y + other.y)

    def __sub__(self, other):
        return Vector2(self.x - other.x, self.y - other.y)

    def __neg__(self):
        return Vector2(-self.x, -self.y)

    def rotate(self, a):
        return Vector2(
            _math.cos(a) * self.x - _math.sin(a) * self.y,
            _math.sin(a) * self.x + _math.cos(a) * self.y)

    def abs(self):
        return _math.sqrt(self.x * self.x + self.y * self.y)

    def __mul__(self, other):
        if isinstance(other, Vector2):
            # interpret multiplication as other^t * self (scalar product of the vectors, where ^t means transpose)
            return self.x * other.x + self.y * other.y
        elif isinstance(other, _numbers.Number):
            return Vector2(self.x * other, self.y * other)
        else:
            return NotImplemented

    def __rmul__(self, other):
        if isinstance(other, Vector2):
            # interpret multiplication as other^t * self (scalar product of the vectors, where ^t means transpose)
            return self.x * other.x + self.y * other.y
        elif isinstance(other, _numbers.Number):
            return Vector2(self.x * other, self.y * other)
        else:
            return NotImplemented

    # TODO python3: use __truediv__
    # https://docs.python.org/3/library/operator.html
    def __div__(self, fraction):
        return Vector2(self.x / fraction, self.y / fraction)

    def __truediv__(self, fraction):
        return Vector2(self.x / fraction, self.y / fraction)

    def __str__(self):
        return "({0},{1})".format(self.x, self.y)

    def __repr__(self):
        return str(self)

    def __eq__(self, other):
        if self.x == other.x and self.y == other.y:
            return True
        else:
            return False

    def __getitem__(self, k):
        if k == 0:
            return self.x
        elif k == 1:
            return self.y
        else:
            raise IndexError()

    def normalize(self):
        if Vector2.abs(self) != 0:
            return self / Vector2.abs(self)
        else:
            return self

    def normalize_length(self, length):
        if self.x == 0 and self.y == 0:
            return Vector2(self.x, self.y)
        else:
            return (Vector2(self.x, self.y) * length) / self.abs()

    # TODO is this function really necessary?
    def rotate_right(self):
        x = self.x
        y = self.y
        return Vector2(y, -x)

    def angle(self):
        return _math.atan2(self.y, self.x)

    def copy(self):
        return Vector2(self.x, self.y)


class Pose2D:
    def __init__(self, translation=Vector2(), rotation=0, pb_pose=None):
        if pb_pose is not None:
            self.translation = Vector2(pb_vector=pb_pose.translation)
            self.rotation = pb_pose.rotation
        else:
            self.translation = translation.copy()
            self.rotation = rotation

    def __mul__(self, other):
        if isinstance(other, Vector2):
            return other.rotate(self.rotation) + self.translation
        elif isinstance(other, Pose2D):
            p = Pose2D()
            p.translation = self * other.translation
            p.rotation = (self.rotation + other.rotation + _math.pi) % (2 * _math.pi) - _math.pi
            return p
        else:
            return NotImplemented

    def __invert__(self):
        return self.invert()

    def invert(self):
        p = Pose2D()
        p.rotation = -self.rotation
        p.translation = (Vector2() - self.translation).rotate(p.rotation)
        return p

    # operator method for '/' in python2
    def __div__(self, point):
        if isinstance(point, Vector2):
            return (point - self.translation).rotate(-self.rotation)
        else:
            return NotImplemented

    # operator method for '/' in python3
    def __truediv__(self, point):
        if isinstance(point, Vector2):
            return (point - self.translation).rotate(-self.rotation)
        else:
            return NotImplemented

    def __str__(self):
        return "(translation = {0}, rotation = {1})".format(self.translation, self.rotation)

    def __repr__(self):
        return str(self)

    def rotate(self, a):
        self.rotation += a

    def translate(self, x, y):
        self.translation = self * Vector2(x, y)

    def copy(self):
        return Pose2D(self.translation.copy(), self.rotation)


class LineSegment(object):
    def __init__(self, begin=Vector2(), end=Vector2()):
        self.base = begin
        self.direction = end - self.base
        self.length = Vector2.abs(self.direction)
        if self.direction.abs() != 0:
            self.direction /= self.direction.abs()

    def __str__(self):
        return str("Begin: " + str(self.base)) + " End: " + str(self.base + self.direction * self.length)

    def begin(self):
        return self.base

    def end(self):
        return self.base + self.direction * self.length

    def point(self, t):
        # waere es nicht besser hier auch noch zu normieren, damit man t in [0,1] waehlen kann, mit t = 0 -> base point
        # t = 1 -> end point
        t = clamp(t, 0.0, self.length)
        return self.base + self.direction * t

    def project(self, p):
        return self.direction * p - self.direction * self.base

    def projection(self, p):
        t = self.direction * p - self.direction * self.base
        return self.point(t)

    def intersection(self, other):
        # Difference to line_intersection is the additional check if t is within the length of the linesegment.
        # HACK returns linesegment distance if intersection is not on linesegment

        normal = Vector2(-other.direction.y, other.direction.x)
        t = normal * self.direction
        if t == 0:
            return float('Inf')

        t = normal * (other.base - self.base) / t
        t = clamp(t, 0.0, self.length)

        return t

    def line_intersection(self, other):
        # maybe rename methods, this one and the one above; the return ist on how to scale the direction
        # vector to get the intersect point from the start (base) point. this is not derivable from the name
        normal = Vector2(-other.direction.y, other.direction.x)
        t = normal * self.direction
        if t == 0:
            return float('Inf')
        else:
            return normal * (other.base - self.base) / t

    def intersect(self, other):
        t = self.line_intersection(other)
        return 0.0 <= t <= self.length


class Rect2d:
    def __init__(self, a, b):
        self.minimum = Vector2(min(a.x, b.x), min(a.y, b.y))
        self.maximum = Vector2(max(a.x, b.x), max(a.y, b.y))

    def inside(self, p):
        return self.maximum.x >= p.x >= self.minimum.x and self.maximum.y >= p.y >= self.minimum.y

    def min(self):
        return self.minimum

    def max(self):
        return self.maximum
