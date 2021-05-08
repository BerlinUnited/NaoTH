r"""Command-line tool to start a debug proxy.

usage: python -m naoth.utils.DebugProxy host [port] [--target TARGET] [--print]
"""
import argparse

from . import DebugProxy


if __name__ == '__main__':
    prog = 'python -m naoth.utils.DebugProxy'
    description = 'A simple command line tool to start a naoth debug proxy.'
    parser = argparse.ArgumentParser(prog=prog, description=description)
    parser.add_argument('host',
                        help='The host of the naoth agent (eg. "localhost")')
    parser.add_argument('port', nargs='?', type=int, default=5401,
                        help='The debug port of the naoth agent (default: 5401)')
    parser.add_argument('--target', type=int, default=7777,
                        help='The proxy port where other applications can connect to (default: 7777)')
    parser.add_argument('--print', action='store_true', default=False,
                        help='Print all commands and their responses going through the proxy.')
    args = parser.parse_args()

    p = DebugProxy(args.host, args.port, args.target, args.print)
    print('DebugProxy active at {}:{} <-> localhost:{}'.format(args.host, args.port, args.target))
    print('Ctrl+C to stop the proxy ...')
    try:
        p.join()
    except KeyboardInterrupt:
        pass
    finally:
        print('\nBye')
        p.stop()
