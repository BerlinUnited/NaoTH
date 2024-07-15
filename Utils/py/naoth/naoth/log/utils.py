from dataclasses import dataclass
from typing import Optional

from ._experimental_parser import Parser
from ._experimental_reader import Reader


def get_representations(path_to_file):
    my_parser = Parser()

    logged_representation = set()

    # iterate over the log and put the representation names in a set
    log = Reader(path_to_file, my_parser)
    for idx, frame in enumerate(log):
        names = frame.get_names()
        for name in names:
            logged_representation.add(name)

    return logged_representation


@dataclass
class Point2D:
    x: float
    y: float

    def __getitem__(self, index):
        assert index in (0, 1), "Index must be 0 or 1"
        return self.x if index == 0 else self.y

    def as_cv2_point(self):
        return int(self.x), int(self.y)

@dataclass
class BoundingBox:
    # FIXME this should go into our naoth python package
    top_left: Point2D
    bottom_right: Point2D

    @classmethod
    def from_coords(cls, top_left_x, top_left_y, bottom_right_x, bottom_right_y):
        return cls(
            Point2D(top_left_x, top_left_y), Point2D(bottom_right_x, bottom_right_y)
        )

    @classmethod
    def from_xywh(cls, top_left_x, top_left_y, width, height):
        return cls(
            Point2D(top_left_x, top_left_y), Point2D(top_left_x+width, top_left_y+height)
        )

    @property
    def width(self):
        return self.bottom_right.x - self.top_left.x

    @property
    def height(self):
        return self.bottom_right.y - self.top_left.y

    @property
    def area(self):
        return self.width * self.height

    @property
    def radius(self):
        width = round(self.width / 2)
        height = round(self.height / 2)

        # FIXME if the patch is on the image border it should be max
        return min(width, height)

    @property
    def center(self):
        """
        this will calculate the center of the rectangle in the coordinate frame the coordinates are in
        """
        # FIXME if the patch is on the image border it imagine that its a square based on the max

        x = round(self.top_left.x + self.width / 2)
        y = round(self.top_left.y + self.height / 2)

        return x, y

    def intersection(self, other: "BoundingBox") -> Optional["BoundingBox"]:
        """
        Calculates the intersection of this bounding box with another one.

        Returns:
            BoundingBox or None: A new BoundingBox representing the intersection,
            or None if there is no intersection.
        """
        intersect_top_left_x = max(self.top_left.x, other.top_left.x)
        intersect_top_left_y = max(self.top_left.y, other.top_left.y)
        intersect_bottom_right_x = min(self.bottom_right.x, other.bottom_right.x)
        intersect_bottom_right_y = min(self.bottom_right.y, other.bottom_right.y)

        # Check if the bounding boxes overlap
        if (
            intersect_top_left_x < intersect_bottom_right_x
            and intersect_top_left_y < intersect_bottom_right_y
        ):
            # If they do overlap, return a new BoundingBox object representing the intersection
            return BoundingBox.from_coords(
                intersect_top_left_x,
                intersect_top_left_y,
                intersect_bottom_right_x,
                intersect_bottom_right_y,
            )
        else:
            # If they don't overlap, return None
            return None