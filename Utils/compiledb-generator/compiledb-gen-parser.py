#!/usr/bin/env python2
#
#   compiledb-generator: Tool for generating LLVM Compilation Database
#   files for make-based build systems.
#
#   Copyright (c) 2017 Nick Diego Yamane <nick.diego@gmail.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
from __future__ import print_function
import sys
import os
import os.path
import re
import argparse
import json


def main():
    if(sys.platform.startswith("win32")):
        msg("Error: Windows is not supported")

    # parse command-line args
    parser = argparse.ArgumentParser(description="Process build output and automatically generates compilation database file for it.")
    parser.add_argument("-v", "--verbose", default=False, action="store_true", help="Show output from build process")
    parser.add_argument("-f", "--force", action="store_true", help="Overwrite the file if it exists.")
    parser.add_argument("-o", "--output", help="Save the config file as OUTPUT. Default: .ycm_extra_conf.py, or .color_coded if --format=cc.")
    parser.add_argument("-i", "--input", help="File path to be used as input. It must contain the make output. Default: stdin.")
    parser.add_argument("-p", "--include-prefix", help="Prefix path to be concatenated to each include path flag. Default: $PWD")
    parser.add_argument("PROJ_DIR", nargs='?', default=os.getcwd(), help="The root directory of the project.")
    args = vars(parser.parse_args())

    include_path_prefix = args["include_prefix"]
    output_path = args["output"]
    input_path = args["input"]
    verbose = args["verbose"]
    pretty_output = True

    proj_dir = os.path.abspath(args["PROJ_DIR"])
    if not os.path.isdir(proj_dir):
        msg("Error: Project dir '{}' does not exists!".format(proj_dir))
        return 1

    with input_file(input_path) as build_log:
        msg("## Processing build commands from '{}'".format('std input' if input_path is None else input_path))
        (count, skipped, compile_db) = parse_flags(build_log, proj_dir, include_path_prefix, verbose)
        # to generate relative include paths (as is)
        # (count, skipped, compile_db) = parse_flags(build_log, include_path_prefix, None)
        msg("## Writing compilation database with {} entries to {}".format(len(compile_db), 'std output' if output_path is None else output_path))
        generate_compile_db_file(compile_db, output_path, pretty_output)
        msg("## Done.")

    return 0


def parse_flags(build_log, proj_dir, inc_prefix, verbose):
    skip_count = 0
    cc_compile_regex = re.compile("(.*-?g?cc )|(.*-?clang )")
    cpp_compile_regex = re.compile("(.*-?[gc]\+\+ )|(.*-?clang\+\+ )")

    # Leverage make --print-directory option
    make_enter_dir = re.compile("^\s*make\[\d+\]: Entering directory [`\'\"](?P<dir>.*)[`\'\"]\s*$")
    make_leave_dir = re.compile("^\s*make\[\d+\]: Leaving directory .*$")

    # Flags we want:
    # -includes (-i, -I)
    # -warnings (-Werror), but no assembler, etc. flags (-Wa,-option)
    # -language (-std=gnu99) and standard library (-nostdlib)
    # -defines (-D)
    # -m32 -m64
    flags_whitelist = [
        "-c",
        "-m.+",
        #"-W[^,]*",
        "-[iIDF].*",
        "-std=[a-z0-9+]+",
        "-(no)?std(lib|inc)",
        "-D([a-zA-Z0-9_]+)=(.*)",
        "--sysroot=(.*)"
    ]
    flags_whitelist = re.compile("|".join(map("^{}$".format, flags_whitelist)))

    # Used to only bundle filenames with applicable arguments
    filename_flags = ["-o", "-I", "-isystem", "-iquote", "-include", "-imacros", "-isysroot"]
    invalid_include_regex = re.compile("(^.*out/.+_intermediates.*$)|(.+/proguard.flags$)")

    file_regex = re.compile("(^.+\.c$)|(^.+\.cc$)|(^.+\.cpp$)|(^.+\.cxx$)")

    compile_db = []
    line_count = 0
    compiler = None

    dir_stack = [proj_dir]
    working_dir = proj_dir

    # Process build log
    for line in build_log:
        # Concatenate line if need
        accumulate_line = line
        while (line.endswith('\\\n')):
            accumulate_line = accumulate_line[:-2]
            line = next(build_log, '')
            accumulate_line += line
        line = accumulate_line

        # Parse directory that make entering/leaving
        enter_dir = make_enter_dir.match(line)
        if (make_enter_dir.match(line)):
            working_dir = enter_dir.group('dir')
            dir_stack.append(working_dir)
        elif (make_leave_dir.match(line)):
            dir_stack.pop()
            working_dir = dir_stack[-1]

        if (cc_compile_regex.match(line)):
            compiler = 'cc'
        elif (cpp_compile_regex.match(line)):
            compiler = 'c++'
        else:
            continue

        cmds = split_cmd_line(line)
        compiler = cmds[0]

        arguments = [compiler]

        words = cmds[1:]
        filepath = None
        line_count += 1


        for (i, word) in enumerate(words):
            word = word.strip('"')
            if (file_regex.match(word)):
                filepath = word

            if(word[0] != '-' or not flags_whitelist.match(word)):
                continue

            # include arguments for this option, if there are any, as a tuple
            if(i != len(words) - 1 and word in filename_flags and words[i + 1][0] != '-'):
                w = words[i + 1]
                p = w if inc_prefix is None else os.path.join(inc_prefix, w)
                if not invalid_include_regex.match(p):
                    arguments.extend([word, p])
            else:
                if word.startswith("-I"):
                    opt = word[0:2]
                    val = word[2:]
                    p = val if inc_prefix is None else os.path.join(inc_prefix, val)
                    if not invalid_include_regex.match(p):
                        arguments.append(opt + p)
                else:
                    arguments.append(word)

        if filepath is None:
            # msg("Empty file name. Ignoring: {}".format(line))
            skip_count += 1
            continue

        # add entry to database
        # TODO performance: serialize to json file here?
        if (verbose):
            msg("args={} --> {}".format(len(arguments), filepath))
        arguments.append(filepath)
        compile_db.append({
            'directory': working_dir,
            'file': filepath,
            'arguments': arguments
        })

    return (line_count, skip_count, compile_db)


def generate_compile_db_file(compile_db, output_path, indent=False):

    with output_file(output_path) as output:
        json.dump(compile_db, output, indent=indent)
        output.write(os.linesep)


def split_cmd_line(line):
    # Pass 1: split line using whitespace
    words = line.strip().split()
    # Pass 2: merge words so that the no. of quotes is balanced
    res = []
    for w in words:
        if(len(res) > 0 and unbalanced_quotes(res[-1])):
            res[-1] += " " + w
        else:
            res.append(w)
    return res


def unbalanced_quotes(s):
    single = 0
    double = 0
    for c in s:
        if(c == "'"):
            single += 1
        elif(c == '"'):
            double += 1
    return (single % 2 == 1 or double % 2 == 1)


def input_file(path):
    return sys.stdin if path is None else open(path, "r")


def output_file(path):
    return sys.stdout if path is None else open(path, "w")


def msg(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


if(__name__ == "__main__"):
    sys.exit(main())

# ex: ts=2 sw=4 et filetype=python
