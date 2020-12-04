import enum as _enum


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


class XABSLAction(_enum.Enum):
    XABSLSymbol = _enum.auto()
    XABSLOption = _enum.auto()
