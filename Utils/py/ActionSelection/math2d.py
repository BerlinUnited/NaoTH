import math


def clamp(x, minimum, maximum):
    return max(minimum, min(x, maximum))


class Vector2:
    def __init__(self, x=0, y=0):
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
          math.cos(a)*self.x - math.sin(a)*self.y,
          math.sin(a)*self.x + math.cos(a)*self.y)

    def abs(self):
        return math.sqrt(self.x*self.x + self.y*self.y)

    def __mul__(self, other):
        if isinstance(other, Vector2):
            return self.x*other.x + self.y*other.y
        elif isinstance(other, (int, float, long)):
            return Vector2(self.x*other, self.y*other)
        else:
            return NotImplemented

    def __div__(self, fraction):
        return Vector2(self.x/fraction, self.y/fraction)

    def __str__(self):
        return str(self.x) + " " + str(self.y)


class Pose2D:

    def __init__(self):
        self.translation = Vector2()
        self.rotation = 0

    def __mul__(self, other):
        return other.rotate(self.rotation) + self.translation

    def __invert__(self):
        p = Pose2D()
        p.rotation = -self.rotation
        p.translation = (Vector2() - self.translation).rotate(p.rotation)
        return p


class LineSegment(object):
    def __init__(self, begin, end):
        self.base = begin
        self.direction = end-self.base
        self.length = Vector2.abs(self.direction)
        self.direction /= Vector2.abs(self.direction)

    def __str__(self):
        return str("Begin: " + str(self.base)) + " End: " + str(self.base+self.direction*self.length)

    def begin(self):
        return self.base

    def end(self):
        return self.base+self.direction*self.length

    def point(self, t):
        return self.base + self.direction*t

    def project(self, p):
        return self.direction*p - self.direction*self.base

    def projection(self, p):
        return self.point(self.project(p))

    def intersection(self, other):
        normal = Vector2(-other.direction.y, other.direction.x)
        t = normal*self.direction
        if t == 0:
            return float('Inf')
        t = clamp(t, 0.0, self.length)
        return normal*(other.base-self.base)/t

    def intersect(self, other):
        normal = Vector2(-other.direction.y, other.direction.x)
        t = normal*self.direction
        return 0.0 <= t <= self.length
