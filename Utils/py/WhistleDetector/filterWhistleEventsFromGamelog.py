from naoth.log import Reader as LogReader
from naoth.log import Parser


def get_whistle_percept(frame):
    percept = None
    try:
        percept = frame["WhistlePercept"]
    except:
        pass

    if percept:
        return [frame.number, percept]
    else:
        return None


if __name__ == "__main__":
    myParser = Parser()

    for msg in LogReader("\\\\nao\\rc17\\go18\\game_logs\\2018-04-28_GO_nomadz_half2\\090917-0100-Nao0225\\game.log",
                         myParser, get_whistle_percept):
        if msg:
            print(msg)
