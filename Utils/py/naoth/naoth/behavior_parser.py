import logging

logger = logging.getLogger(__name__)


class XABSLEnum:
    def __init__(self, enumeration):
        self.name = enumeration.name

        # parse enumeration element names (cut the name prefix)
        self.elem_id_to_name = {elem.value: elem.name[len(self.name)+1:] for elem in enumeration.elements}

    def __getitem__(self, _id):
        return self.elem_id_to_name[_id]

    def __str__(self):
        return f"{self.name} {{{', '.join(value for value in self.elem_id_to_name.values())}}}"


class XABSLSymbols:
    def __init__(self):
        self.values = {}

        self.decimal_id_to_name = {}
        self.boolean_id_to_name = {}

        self.enum_id_to_name = {}
        self.enum_id_to_type = {}

    def set_decimal(self, _id, value):
        name = self.decimal_id_to_name[_id]
        self.values[name] = value

    def set_boolean(self, _id, value):
        name = self.boolean_id_to_name[_id]
        self.values[name] = value

    def set_enum(self, _id, value):
        name = self.enum_id_to_name[_id]
        enum = self.enum_id_to_type[_id]
        self.values[name] = enum[value]

    def __getitem__(self, name):
        return self.values[name]

    def __contains__(self, name):
        return name in self.values

    def _str_item(self, name, item):
        result = name + ' = {\n'
        for s in item:
            result += '\t{:3d}, {} = {}\n'.format(s, item[s], self.values[item[s]])
        result += '}'
        return result

    def __str__(self):
        result = self._str_item('DecimalSymbols', self.decimal_id_to_name) \
               + ',\n' + self._str_item('BooleanSymbols', self.boolean_id_to_name) \
               + ',\n' + self._str_item('EnumSymbols', self.enum_id_to_name)
        return result


class BehaviorParser:
    def __init__(self, init_frame=None):
        self.input_symbols = XABSLSymbols()
        self.output_symbols = XABSLSymbols()

        self._options = None
        self._current_options = {}

        if init_frame is not None:
            self.initialize(init_frame)

    @staticmethod
    def _initialize_symbols(enum_types, symbols, symbol_list):
        for decimal in symbol_list.decimal:
            symbols.decimal_id_to_name[decimal.id] = decimal.name

        for boolean in symbol_list.boolean:
            symbols.boolean_id_to_name[boolean.id] = boolean.name

        for enum in symbol_list.enumerated:
            enum_type = enum_types[enum.typeId]
            symbols.enum_id_to_type[enum.id] = enum_type
            symbols.enum_id_to_name[enum.id] = enum.name

    def initialize(self, init_frame):
        if 'BehaviorStateComplete' not in init_frame:
            raise ValueError('Frame does not contain BehaviorStateComplete.')

        behavior_state = init_frame['BehaviorStateComplete']

        # parse enum types
        enum_types = {typeId: XABSLEnum(enumeration) for typeId, enumeration in enumerate(behavior_state.enumerations)}

        # initialize options
        self._options = behavior_state.options

        # initialize input symbols
        self._initialize_symbols(enum_types, self.input_symbols, behavior_state.inputSymbolList)
        # initialize output symbols
        self._initialize_symbols(enum_types, self.output_symbols, behavior_state.outputSymbolList)

    @staticmethod
    def _set_symbols(symbols, symbol_list):
        for decimal in symbol_list.decimal:
            symbols.set_decimal(decimal.id, decimal.value)

        for boolean in symbol_list.boolean:
            symbols.set_boolean(boolean.id, boolean.value)

        for enum in symbol_list.enumerated:
            symbols.set_enum(enum.id, enum.value)

    def _parse_option(self, o):
        if o.type == 0:  # Option
            option_complete = self._options[o.option.id]
            self._current_options[option_complete.name] = {
              'time': o.option.timeOfExecution,
              'state': option_complete.states[o.option.activeState],
              'stateTime': o.option.stateTime
            }

            for so in o.option.activeSubActions:
                self._parse_option(so)

    def parse(self, frame):
        # check if initialized
        if self._options is None:
            raise Exception('BehaviorParser must be initialized with a frame containing '
                            '"BehaviorStateComplete" data! (call initialize)')

        if 'BehaviorStateSparse' not in frame:
            raise ValueError('Frame does not contain BehaviorStateSparse.')

        self._current_options = {}

        behavior_state = frame['BehaviorStateSparse']

        # process active options
        for o in behavior_state.activeRootActions:
            self._parse_option(o)

        # process symbols
        self._set_symbols(self.input_symbols, behavior_state.inputSymbolList)
        self._set_symbols(self.output_symbols, behavior_state.outputSymbolList)

    def get_active_options(self):
        return list(self._current_options.keys())

    def is_active_option(self, option):
        return option in self._current_options

    def get_active_states(self):
        return [s['state'].name for s in self._current_options.values()]

    def get_active_option_state(self, option):
        if option in self._current_options:
            return self._current_options[option]['state'].name

    def __str__(self):
        # TODO: how to represent the options?!
        result = str(self.symbols) + ',\n[\n'
        if self._current_options:
            for o in self._current_options:
                result += '\t{} [{} ms] - {} [{} ms]\n'.format(o,
                                                               self._current_options[o]['time'],
                                                               self._current_options[o]['state'].name,
                                                               self._current_options[o]['stateTime'])
        return result + ']'


if __name__ == '__main__':
    from .logreader import LogReader

    LOGFILE = 'game.log'

    with LogReader(LOGFILE) as log_reader:
        behavior = BehaviorParser()

        for _frame in log_reader.read():
            if 'BehaviorStateComplete' in _frame:
                behavior.initialize(_frame)

            if 'BehaviorStateSparse' in _frame:
                behavior.parse(_frame)

                if 'motion.type' in behavior.output_symbols and 'game.current_mode' in behavior.output_symbols:
                    print(_frame.number,
                          behavior.output_symbols['motion.type'], behavior.output_symbols['game.current_mode'])
