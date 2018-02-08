
class Vector2(object):
    def __init__(self, x, y):
        self.__dict__ = {'x':x, 'y':y}
    def __repr__(self):
        return str(self.__dict__)

class Vector3(object):
    def __init__(self, x, y, z):
        self.__dict__ = {'x':x, 'y':y, 'z':z}
    def __repr__(self):
        return str(self.__dict__)

class Pose2D(object):
    def __init__(self, x, y, r):
        self.__dict__ = {'x':x, 'y':y, 'r':r}
    def __repr__(self):
        return str(self.__dict__)
