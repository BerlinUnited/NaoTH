import cmd

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

    def do_quit(self, args):
        'Stops sending messages and exits.'
        self.thread.running = False
        self.thread.join()
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
        if len(params) != 2:
            print("*** invalid number of arguments")
            return
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
