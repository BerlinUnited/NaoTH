import logging as _logging
import inspect as _inspect

from .. import pb
from ._xabsl import XABSLSymbols


# get all protobuf classes
proto = {}
for m in _inspect.getmembers(pb, _inspect.ismodule):
    for c in _inspect.getmembers(m[1], _inspect.isclass):
        if not c[0].startswith('_'):
            proto[c[0]] = c[1]


class Parser:
    def __init__(self):
        self.nameToType = {}

    def register(self, name, type):
        self.nameToType[name] = type

    def parse(self, name, data):
        message = None

        if name in self.nameToType:
            name = self.nameToType[name]

        if name in proto:
            message = proto[name]()

        if message is not None:
            message.ParseFromString(data)

        return message


class BehaviorParser:
    def __init__(self, data=None):

        self.__parser = Parser()
        self.symbols = XABSLSymbols()
        self.__options = None
        self.__current_options = None

        self.init(data)

    def init(self, data):
        if data is not None:
            # check given data
            if data == b'unknown representation\n':
                raise Exception('Unknown representation')

            message = self.__parser.parse('BehaviorStateComplete', data)

            # initialize options
            self.__options = message.options

            # initialize symbols
            for s in message.inputSymbolList.decimal:
                self.symbols.values[s.name] = s.value
                self.symbols.decimalIdToName[s.id] = s.name

            for s in message.inputSymbolList.boolean:
                self.symbols.values[s.name] = s.value
                self.symbols.booleanIdToName[s.id] = s.name

            for s in message.inputSymbolList.enumerated:
                self.symbols.values[s.name] = s.value
                self.symbols.enumIdToName[s.id] = s.name

    def __check_initialized(self):
        if self.__options is None:
            _logging.getLogger("BehaviorParser").error("BehaviorParser must be initialized with 'BehaviorStateComplete' data!")
            return False

        return True

    def parse(self, data):
        if self.__check_initialized():
            self.__current_options = {}

            message = self.__parser.parse('BehaviorStateSparse', data)

            # process active options
            for o in message.activeRootActions:
                self.__parseOption(o)

            # process symbols
            for s in message.inputSymbolList.decimal:
                self.symbols.decimal(s.id, s.value)

            for s in message.inputSymbolList.boolean:
                self.symbols.boolean(s.id, s.value)

            for s in message.inputSymbolList.enumerated:
                self.symbols.enum(s.id, s.value)

    def __parseOption(self, o):
        if o.type == 0: # Option
            optionComplete = self.__options[o.option.id]
            self.__current_options[optionComplete.name] = {
              'time': o.option.timeOfExecution,
              'state': optionComplete.states[o.option.activeState],
              'stateTime': o.option.stateTime
            }

            for so in o.option.activeSubActions:
                self.__parseOption(so)

    def getActiveOptions(self):
        return list(self.__current_options.keys())

    def isActiveOption(self, option):
        return option in self.__current_options

    def getActiveStates(self):
        return [ s['state'].name for s in self.__current_options.values() ]

    def getActiveOptionState(self, option):
        if option in self.__current_options:
            return self.__current_options[option]['state'].name

    def __str__(self):
        # TODO: how to represent the options?!
        result = str(self.symbols) + ',\n[\n'
        if self.__current_options:
            for o in self.__current_options:
                result += '\t{} [{} ms] - {} [{} ms]\n'.format(o, self.__current_options[o]['time'], self.__current_options[o]['state'].name, self.__current_options[o]['stateTime'])
        result += ']'

        return result
