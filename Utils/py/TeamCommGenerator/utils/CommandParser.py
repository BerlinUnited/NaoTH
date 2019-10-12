import cmd
import json


class CommandParser(cmd.Cmd):
    intro = 'TeamComm message generator.\n------------------------------\nType help or ? to list commands and use »tab« for completion.\n'
    prompt = '>>> '

    def __init__(self, tcg):
        super(CommandParser, self).__init__()
        self.thread = tcg

    def default(self, line):
        if line == 'EOF' or line == 'q':
            return self.do_quit(None)
        else:
            super(CommandParser, self).default(line)

    def __stop_thread(self):
        'Stops sending messages and exits.'
        self.thread.cancel.set()
        self.thread.join()

    def do_quit(self, args):
        self.__stop_thread()
        return True

    def do_exit(self, args):
        self.__stop_thread()
        return True

    def do_info(self, args):
        'Prints out the specified value or all values of the currently sending messages.'
        if args:
            value = self.thread.getMessageField(args)
            print(args+" = "+str(value) if value is not None else "*** Unknown message field")
        else:
            print(self.thread)

    def do_set(self, args):
        'Sets the value of a given message field. e.g.: set playerNumber 4\nNested values can be accessed via ».«, eg. set pose.x 3.4'
        params = args.split()
        if self.__check_parameter_number(params, 2):
            self.thread.setMessageField(*params)

    def complete_set(self, text, line, begidx, endidx):
        # only complete the "first" argument
        if len(line.split()) == 1 and len(text) == 0:
            return self.thread.getMessageFields()
        elif line.split().index(text) == 1:
            return [ i for i in self.thread.getMessageFields() if i.startswith(text) ]
        return []

    def complete_info(self, text, line, begidx, endidx):
        # only complete the "first" argument
        if len(line.split()) == 1 and len(text) == 0:
            return self.thread.getMessageFields()
        elif line.split().index(text) == 1:
            return [ i for i in self.thread.getMessageFields() if i.startswith(text) ]
        return []

    def do_timestamps(self, args):
        '(de-)activates the updating of the sent timestamp. (true/True/1, false/False/0)'
        params = args.split()
        if self.__check_parameter_number(params, 1):
            value = bool(json.loads(params[0].lower()))
            self.thread.updateTimestamp(value)

    def do_ntp(self, args):
        '(de-)activates the usage of the NTP protocol. (true/True/1, false/False/0)'
        params = args.split()
        if self.__check_parameter_number(params, 1):
            value = bool(json.loads(params[0].lower()))
            self.thread.useNTP(value)

    def __check_parameter_number(self, params, num:int):
        if len(params) != num:
            print("*** invalid number of arguments")
            return False
        return True
