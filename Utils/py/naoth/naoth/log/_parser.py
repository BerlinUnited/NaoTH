import logging as _logging
import inspect as _inspect
import enum as _enum
import re as _re

from .. import pb as _pb
from ._xabsl import XABSLOption, XABSLAction


# get all protobuf classes
_proto = {}
for _m in _inspect.getmembers(_pb, _inspect.ismodule):
    for _c in _inspect.getmembers(_m[1], _inspect.isclass):
        if not _c[0].startswith('_'):
            _proto[_c[0]] = _c[1]


class Parser:
    """
    Parses raw log file frame member bytes using protobuf.
    If a name of a frame member does not match a class defined in protobuf,
    the corresponding class name needs to be defined using the register function.

    Ex: parser.register("ImageTop", "Image") where Image is the corresponding name of the protobuf class.
    """

    def __init__(self):
        self.protobuf_classes = {}

        # load protobuf classes
        for module in [CommonTypes_pb2, Framework_Representations_pb2, Messages_pb2, Representations_pb2,
                       TeamMessage_pb2, AudioData_pb2]:
            self.protobuf_classes.update(LogParser._load_classes_from(module))

        self.name_to_type = {}

    @staticmethod
    def _load_classes_from(module):
        return {cls[0]: cls[1] for cls in inspect.getmembers(module, inspect.isclass)}

    def register(self, name: str, _type: str):
        """
        Register name corresponding protobuf type.
        :param name: frame member name
        :param _type: class in protobuf
        """
        if _type not in self.protobuf_classes:
            raise ValueError(f'Class {_type} is not part of the protobuf defined types.')

        self.name_to_type[name] = _type

    def parse(self, name, data):
        """
        Parse frame member bytes using protobuf.
        :param name: of frame member
        :param data: payload bytes
        :returns member class
        """
        if name in self.name_to_type:
            name = self.name_to_type[name]
        # protobuf
        if name in self.protobuf_classes:
            message = self.protobuf_classes[name]()
            message.ParseFromString(data)
            return message
        else:
            raise ValueError(f'Class {name} is not part of the protobuf defined types.')


class SymbolParser:
    """
    Maps symbol ids to the corresponding name in BehaviorStateComplete
    """
    def __init__(self, symbol_list, enum_types):
        self.decimal_id_to_name = {decimal.id: decimal.name for decimal in symbol_list.decimal}
        self.boolean_id_to_name = {boolean.id: boolean.name for boolean in symbol_list.boolean}

        self.enum_id_to_type = {enum.id: enum_types[enum.typeId] for enum in symbol_list.enumerated}

    def parse_symbols(self, symbol_list):
        for decimal in symbol_list.decimal:
            yield self.decimal_id_to_name[decimal.id], decimal.value

        for boolean in symbol_list.boolean:
            yield self.boolean_id_to_name[boolean.id], boolean.value

        for enum in symbol_list.enumerated:
            enum_type = self.enum_id_to_type[enum.id]
            yield enum_type.__name__, enum_type(enum.value)


class BehaviorFrame:
    def __init__(self):
        self.root_options = {}

        self.input_symbols = {}
        self.output_symbols = {}

    def __getitem__(self, option_name):
        return self.root_options[option_name]

    def __str__(self):
        return '\n'.join(XABSLOption.option_to_string(name, option) for name, option in self.root_options.items())


class BehaviorParser:
    def __init__(self, behavior_state=None):
        """
        :param behavior_state: BehaviorStateComplete member of the log file.
                               If not given, BehaviorParser must be initialized manually by calling initialize
        """
        self.agents = []

        self.enum_types = None
        self._input_symbol_parser = None
        self._output_symbol_parser = None
        self._options_complete = None

        if behavior_state is not None:
            self.initialize(behavior_state)

    def initialize(self, behavior_state):
        """
        Behavior parser must be initialized with BehaviorStateComplete member of the log file
        (usually contained in the first log file frame)
        :param behavior_state: BehaviorStateComplete
        """
        # parse agents
        self.agents = [agent for agent in behavior_state.agents]

        # initialize options
        self._options_complete = behavior_state.options

        # parse enum types
        self.enum_types = {
            typeId: self._parse_xabsl_enum(enumeration) for typeId, enumeration in enumerate(behavior_state.enumerations)
        }

        # initialize symbols
        self._input_symbol_parser = SymbolParser(behavior_state.inputSymbolList, self.enum_types)
        self._output_symbol_parser = SymbolParser(behavior_state.outputSymbolList, self.enum_types)

    @staticmethod
    def _parse_xabsl_enum(xabsl_enum):
        """
        Creates python enum from protobuf xabsl_enum
        """

        def convert_enum_value(value):
            # remove enum name prefix from enum value
            trimmed_value = value[len(xabsl_enum.name) + 1:]

            # replace _NAME_ values with __NAME__ since they can't be used in pythons enum class
            match = _re.match(r'_(?P<NAME>\S*)_', trimmed_value)
            if match:
                return trimmed_value.replace(f'_{match.group("NAME")}_', f'__{match.group("NAME")}__')

            return trimmed_value

        return _enum.Enum(xabsl_enum.name, [(convert_enum_value(elem.name), elem.value) for elem in xabsl_enum.elements])

    def _parse_options(self, option_sparse):
        if _pb.Messages_pb2.XABSLActionSparse.ActionType.Name(option_sparse.type) == 'Option':

            option_complete = self._options_complete[option_sparse.option.id]

            parsed_option = XABSLOption(option_sparse.option.timeOfExecution,
                                        option_complete.states[option_sparse.option.activeState],
                                        option_sparse.option.stateTime)

            decimals = iter(option_sparse.option.decimalParameters)
            booleans = iter(option_sparse.option.booleanParameters)
            enums = iter(option_sparse.option.enumeratedParameters)

            for parameter in option_complete.parameters:

                if _pb.Messages_pb2.XABSLSymbol.SymbolType.Name(parameter.type) == 'Decimal':
                    parsed_option.parameters[parameter.name] = next(decimals)
                elif _pb.Messages_pb2.XABSLSymbol.SymbolType.Name(parameter.type) == 'Boolean':
                    parsed_option.parameters[parameter.name] = next(booleans)
                elif _pb.Messages_pb2.XABSLSymbol.SymbolType.Name(parameter.type) == 'Enum':
                    type = self.enum_types[parameter.enumTypeId]
                    parsed_option.parameters[parameter.name] = type(next(enums))
                else:
                    raise ValueError(f'Parameter type {parameter.type} not implemented')

            for sub_action in option_sparse.option.activeSubActions:
                action, name, parsed_sub_action = self._parse_options(sub_action)
                if action == XABSLAction.XABSLOption:
                    parsed_option.sub_options[name] = parsed_sub_action
                elif action == XABSLAction.XABSLSymbol:
                    parsed_option.symbols[name] = parsed_sub_action
                else:
                    raise ValueError(f'Action {action} not implemented.')

            return XABSLAction.XABSLOption, option_complete.name, parsed_option

        elif _pb.Messages_pb2.XABSLActionSparse.ActionType.Name(option_sparse.type) == 'SymbolAssignement':
            symbol = option_sparse.symbol

            if _pb.Messages_pb2.XABSLActionSparse.SymbolType.Name(symbol.type) == 'Decimal':
                name = self._output_symbol_parser.decimal_id_to_name[symbol.id]
                value = symbol.decimalValue
            elif _pb.Messages_pb2.XABSLActionSparse.SymbolType.Name(symbol.type) == 'Boolean':
                name = self._output_symbol_parser.boolean_id_to_name[symbol.id]
                value = symbol.boolValue
            elif _pb.Messages_pb2.XABSLActionSparse.SymbolType.Name(symbol.type) == 'Enum':
                enum_type = self._output_symbol_parser.enum_id_to_type[symbol.id]
                name = enum_type.__name__
                value = enum_type(symbol.enumValue)
            else:
                raise ValueError(f'XABSLSymbol type {_pb.Messages_pb2.XABSLActionSparse.SymbolType.Name(symbol.type)} not implemented.')

            return XABSLAction.XABSLSymbol, name, value

    def parse(self, behavior_state):
        """
        Parses the XABSL Behavior based on the BehaviorStateSparse member of a log file frame
        :param behavior_state: BehaviorStateSparse
        :returns parsed BehaviorFrame
        """
        # check if initialized
        if self._options_complete is None:
            raise Exception('BehaviorParser must be initialized with "BehaviorStateComplete" (call initialize).')

        behavior_frame = BehaviorFrame()

        # parse input and output symbols
        for name, value in self._input_symbol_parser.parse_symbols(behavior_state.inputSymbolList):
            behavior_frame.input_symbols[name] = value
        for name, value in self._output_symbol_parser.parse_symbols(behavior_state.outputSymbolList):
            behavior_frame.output_symbols[name] = value

        # process active options
        for option_sparse in behavior_state.activeRootActions:
            action, name, option = self._parse_options(option_sparse)
            assert action == XABSLAction.XABSLOption
            behavior_frame.root_options[name] = option

        return behavior_frame
