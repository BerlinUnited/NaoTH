import inspect as _inspect
import sys
from .. import pb as _pb
from ._xabsl import XABSLSymbols

# get all protobuf classes
_proto = {}
for _m in _inspect.getmembers(_pb, _inspect.ismodule):
    for _c in _inspect.getmembers(_m[1], _inspect.isclass):
        if not _c[0].startswith('_'):
            _proto[_c[0]] = _c[1]


class Parser:
    def __init__(self):
        self.nameToType = {}

    def register(self, name, type):
        self.nameToType[name] = type

    def parse(self, name, data):
        message = None

        if name in self.nameToType:
            name = self.nameToType[name]

        if name in _proto:
            message = _proto[name]()

        if message is not None:
            message.ParseFromString(data)

        return message


class BehaviorParser:
    def __init__(self, data=None):

        self.parser = Parser()
        self.symbols = XABSLSymbols()
        self.__options = None
        self.__current_options = None

        self.init(data)

    def init(self, data):
        if data is not None:
            # check given data
            if data == b'unknown representation\n':
                raise Exception('Unknown representation')

            message = self.parser.parse('BehaviorStateComplete', data)

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

            return message

    def __check_initialized(self):
        if self.__options is None:
            # print(" BehaviorParser must be initialized with 'BehaviorStateComplete' data! -> Running Init")
            return False

        return True

    def parse(self, name, data):
        if not self.__check_initialized() and name == "BehaviorStateComplete":
            message = self.init(data)

            return (message, self.__current_options, self.symbols)
        elif name == "BehaviorStateSparse":
            self.__current_options = {}

            message = self.parser.parse("BehaviorStateSparse", data)

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

            return (message, self.__current_options, self.symbols)
        else:
            return(self.parser.parse(name, data))


    def __parseOption(self, o):
        if o.type == 0:  # Option
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
        return [s['state'].name for s in self.__current_options.values()]

    def getActiveOptionState(self, option):
        if option in self.__current_options:
            return self.__current_options[option]['state'].name

    def __str__(self):
        # TODO: how to represent the options?!
        result = str(self.symbols) + ',\n[\n'
        if self.__current_options:
            for o in self.__current_options:
                result += '\t{} [{} ms] - {} [{} ms]\n'.format(o, self.__current_options[o]['time'],
                                                               self.__current_options[o]['state'].name,
                                                               self.__current_options[o]['stateTime'])
        result += ']'

        return result
