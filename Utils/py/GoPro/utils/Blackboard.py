import threading

class Blackboard:

    def __init__(self):
        # using lock explicitly, don't want to rely on GIL or other python stuff
        self.__data_lock = threading.Lock()
        self.__data = {}

    def get(self, key):
        with self.__data_lock:
            if key not in self.__data:
                return None
            return self.__data[key]

    def set(self, key, value):
        with self.__data_lock:
            self.__data[key] = value

    def __getitem__(self, key):
        return self.get(key)

    def __setitem__(self, key, value):
        self.set(key, value)

    def __repr__(self):
        return str(self.__data)

blackboard = Blackboard()