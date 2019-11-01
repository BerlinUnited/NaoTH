from naoth.math2d import Vector2 as Vec
from naoth.math2d import LineSegment


class Rectangle(object):
    def __init__(self, point_1, point_2, point_3, point_4):
        # construct a rectangle with four point clockwise starting at bottom left
        self.bottom = LineSegment(point_1, point_2)
        self.right = LineSegment(point_2, point_3)
        self.top = LineSegment(point_3, point_4)
        self.left = LineSegment(point_4, point_1)

        self.border = [self.bottom, self.right, self.top, self.left]

    def collision(self, Line):
        collision = False
        for element in self.border:
            if element.intersect(Line) and Line.intersect(element):
                collision = True
                break

        return collision
