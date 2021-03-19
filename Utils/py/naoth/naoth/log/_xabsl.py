

class XABSLSymbols:
    def __init__(self):
        self.values = {}
        self.decimalIdToName = {}
        self.booleanIdToName = {}
        self.enumIdToName = {}

    def __get_or_set(self, var, id, value):
        if value is None:
            if isinstance(id, str):
                return self.values[id]
            else:
                return self.values[var[id]]
        else:
            if isinstance(id, str):
                self.values[id] = value
            else:
                self.values[var[id]] = value

    def decimal(self, id, value=None):
        return self.__get_or_set(self.decimalIdToName, id, value)

    def boolean(self, id, value=None):
        return self.__get_or_set(self.booleanIdToName, id, value)

    def enum(self, id, value=None):
        return self.__get_or_set(self.enumIdToName, id, value)

    def __str_item(self, name, item):
        result = name + ' = {\n'
        for s in item:
            result += '\t{:3d}, {} = {}\n'.format(s, item[s], self.values[item[s]])
        result += '}'
        return result

    def __str__(self):
        result = self.__str_item('DecimalSymbols', self.decimalIdToName) \
               + ',\n' + self.__str_item('BooleanSymbols', self.booleanIdToName) \
               + ',\n' + self.__str_item('EnumSymbols', self.enumIdToName)
        return result
