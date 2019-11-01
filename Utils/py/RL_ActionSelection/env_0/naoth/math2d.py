import math


def clamp(x, minimum, maximum):
    return max(minimum, min(x, maximum))


class Vector2:
    def __init__(self, x=0.0, y=0.0):
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
            math.cos(a) * self.x - math.sin(a) * self.y,
            math.sin(a) * self.x + math.cos(a) * self.y)

    def abs(self):
        return math.sqrt(self.x * self.x + self.y * self.y)

    def __mul__(self, other):
        if isinstance(other, Vector2):
            return self.x * other.x + self.y * other.y
        elif isinstance(other, (int, float, long)):
            return Vector2(self.x * other, self.y * other)
        else:
            return NotImplemented

    # TODO python3: use __truediv__
    # https://docs.python.org/3/library/operator.html
    def __div__(self, divisor):
        return Vector2(self.x / divisor, self.y / divisor)

    def __str__(self):
        return "({0},{1})".format(self.x, self.y)

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
        return math.atan2(self.y, self.x)

    def copy(self):
        return Vector2(self.x, self.y)


class Pose2D:
    def __init__(self, translation=Vector2(), rotation=0):
        self.translation = translation.copy()
        self.rotation = rotation

    def __mul__(self, other):
        if isinstance(other, Vector2):
            return other.rotate(self.rotation) + self.translation
        elif isinstance(other, Pose2D):
            p = Pose2D()
            p.translation = self * other.translation
            p.rotation = (self.rotation + other.rotation + math.pi) % (2 * math.pi) - math.pi
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

    # TODO python3: use __truediv__
    # https://docs.python.org/3/library/operator.html
    def __div__(self, other):
        if isinstance(other, Vector2):
            return (other - self.translation).rotate(-self.rotation)
        else:
            return NotImplemented

    def __str__(self):
        return "(translation = {0}, rotation = {1})".format(self.translation, self.rotation)

    def rotate(self, a):
        self.rotation += a

    def translate(self, x, y):
        self.translation = self * Vector2(x, y)

    def copy(self):
        return Pose2D(self.translation.copy(), self.rotation)


class LineSegment(object):
    def __init__(self, begin, end):
        self.base = begin
        self.direction = end - self.base
        self.length = Vector2.abs(self.direction)
        if self.direction.abs() != 0:
            self.direction /= self.direction.abs()

    def __str__(self):
        return str("Begin: " + str(self.base)) + " End: " + str(
            self.base + self.direction * self.length)

    def begin(self):
        return self.base

    def end(self):
        return self.base + self.direction * self.length

    def point(self, t):
        t = clamp(t, 0.0, self.length)
        return self.base + self.direction * t

    def project(self, p):
        return self.direction * p - self.direction * self.base

    def projection(self, p):
        t = self.direction * p - self.direction * self.base
        return self.point(t)

    def intersection(self, other):
        normal = Vector2(-other.direction.y, other.direction.x)
        t = normal * self.direction
        if t == 0:
            return float('Inf')

        t = normal * (other.base - self.base) / t
        t = clamp(t, 0.0, self.length)

        return t

    def line_intersection(self, other):
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
