import re
from enum import Enum, auto

from naoth.messages.Messages_pb2 import XABSLActionSparse
from naoth.messages.Messages_pb2 import XABSLSymbol as XABSLSymbol_pb2


def parse_xabsl_enum(xabsl_enum):
    """
    Creates python enum from protobuf xabsl_enum
    """
    def convert_enum_value(value):
        # remove enum name prefix from enum value
        trimmed_value = value[len(xabsl_enum.name)+1:]

        # replace _NAME_ values with __NAME__ since they can't be used in pythons enum class
        match = re.match(r'_(?P<NAME>\S*)_', trimmed_value)
        if match:
            return trimmed_value.replace(f'_{match.group("NAME")}_', f'__{match.group("NAME")}__')

        return trimmed_value

    return Enum(xabsl_enum.name, [(convert_enum_value(elem.name), elem.value) for elem in xabsl_enum.elements])


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


class XABSLState:
    def __init__(self, state):
        self.name = state.name
        self.target = state.target

    def __str__(self):
        return self.name


class XABSLOption:
    def __init__(self, time, state, state_time):
        self.time = time
        self.state = XABSLState(state)
        self.state_time = state_time

        self.symbols = {}
        self.parameters = {}

        self.sub_options = {}

    def __getitem__(self, option_name):
        return self.sub_options[option_name]

    def __str__(self):
        return f'[{self.time} ms] {self.state} [{self.state_time} ms]'

    @staticmethod
    def option_to_string(name, option, level=0):
        """
        Creates a BehaviorState string similar to the output of RobotControl
        :param name: root option name
        :param option: root option
        :return:
        """
        padding = level * '\t'

        string = f'{padding}{name} {option}'

        if option.parameters:
            string += f'\n{padding}\t' + f'\n{padding}\t'.join(f'@{name}={value}' for name, value in option.parameters.items())

        if option.symbols:
            string += f'\n{padding}\t' + f'\n{padding}\t'.join(f'{name}={value}' for name, value in option.symbols.items())

        if option.sub_options:
            string += f'\n' + '\n'.join(XABSLOption.option_to_string(sub_option_name, sub_option, level + 1)
                                        for sub_option_name, sub_option in option.sub_options.items())
        return string


class Action(Enum):
    XABSLSymbol = auto()
    XABSLOption = auto()


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
        self.enum_types = {typeId: parse_xabsl_enum(enumeration) for typeId, enumeration in enumerate(behavior_state.enumerations)}

        # initialize symbols
        self._input_symbol_parser = SymbolParser(behavior_state.inputSymbolList, self.enum_types)
        self._output_symbol_parser = SymbolParser(behavior_state.outputSymbolList, self.enum_types)

    def _parse_options(self, option_sparse):
        if XABSLActionSparse.ActionType.Name(option_sparse.type) == 'Option':

            option_complete = self._options_complete[option_sparse.option.id]

            parsed_option = XABSLOption(option_sparse.option.timeOfExecution,
                                        option_complete.states[option_sparse.option.activeState],
                                        option_sparse.option.stateTime)

            decimals = iter(option_sparse.option.decimalParameters)
            booleans = iter(option_sparse.option.booleanParameters)
            enums = iter(option_sparse.option.enumeratedParameters)

            for parameter in option_complete.parameters:
                if XABSLSymbol_pb2.SymbolType.Name(parameter.type) == 'Decimal':
                    parsed_option.parameters[parameter.name] = next(decimals)
                elif XABSLSymbol_pb2.SymbolType.Name(parameter.type) == 'Boolean':
                    parsed_option.parameters[parameter.name] = next(booleans)
                elif XABSLSymbol_pb2.SymbolType.Name(parameter.type) == 'Enum':
                    type = self.enum_types[parameter.enumTypeId]
                    parsed_option.parameters[parameter.name] = type(next(enums))
                else:
                    raise ValueError(f'Parameter type {parameter.type} not implemented')

            for sub_action in option_sparse.option.activeSubActions:
                action, name, parsed_sub_action = self._parse_options(sub_action)
                if action == Action.XABSLOption:
                    parsed_option.sub_options[name] = parsed_sub_action
                elif action == Action.XABSLSymbol:
                    parsed_option.symbols[name] = parsed_sub_action
                else:
                    raise ValueError(f'Action {action} not implemented.')

            return Action.XABSLOption, option_complete.name, parsed_option

        elif XABSLActionSparse.ActionType.Name(option_sparse.type) == 'SymbolAssignement':
            symbol = option_sparse.symbol

            if XABSLSymbol_pb2.SymbolType.Name(symbol.type) == 'Decimal':
                name = self._output_symbol_parser.decimal_id_to_name[symbol.id]
                value = symbol.decimalValue
            elif XABSLSymbol_pb2.SymbolType.Name(symbol.type) == 'Boolean':
                name = self._output_symbol_parser.boolean_id_to_name[symbol.id]
                value = symbol.boolValue
            elif XABSLSymbol_pb2.SymbolType.Name(symbol.type) == 'Enum':
                enum_type = self._output_symbol_parser.enum_id_to_type[symbol.id]
                name = enum_type.__name__
                value = enum_type(symbol.enumValue)
            else:
                raise ValueError(f'XABSLSymbol type {XABSLSymbol_pb2.SymbolType.Name(symbol.type)} not implemented.')

            return Action.XABSLSymbol, name, value

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
            assert action == Action.XABSLOption
            behavior_frame.root_options[name] = option

        return behavior_frame


if __name__ == '__main__':
    from naoth.logreader import LogReader
    import argparse

    arg_parser = argparse.ArgumentParser(description='Behavior parsing example.')
    arg_parser.add_argument('logfile', help='naoth log file')
    arg_parser.add_argument('-d', '--dump', action='store_true', help='dumps the behavior state to stdout,'
                                                                      'similar to the output of RobotControl')
    args = arg_parser.parse_args()

    with LogReader(args.logfile) as log_reader:
        behavior = BehaviorParser()

        for i, frame in enumerate(log_reader.read()):
            # Behavior parser must be initialized with a frame containing BehaviorStateComplete
            if 'BehaviorStateComplete' in frame:
                behavior.initialize(frame['BehaviorStateComplete'])

            # XABSL Behavior is parsed from "BehaviorStateSparse" member of the current log file frame
            if 'BehaviorStateSparse' in frame:
                behavior_frame = behavior.parse(frame['BehaviorStateSparse'])

                try:
                    # EXAMPLE: Tracking the value of the motion symbol
                    value = behavior_frame['soccer_agent']['roles_spl_play_soccer']['basic_play'].symbols['motion.type']
                    # motion type is an enumeration (e.g. stand, walk), the corresponding python enum class
                    # can be accessed with the __class__ attribute
                    assert value in value.__class__
                except KeyError:
                    pass

                if args.dump:
                    print(behavior_frame)
                    print()
