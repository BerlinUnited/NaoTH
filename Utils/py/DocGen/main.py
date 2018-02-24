import argparse
import os
import shutil
import mmap
import re
import markdown
from file_info import FileInfo
from jinja2 import Environment, FileSystemLoader

PATH = os.path.dirname(os.path.abspath(__file__))
TEMPLATE_ENVIRONMENT = Environment(
  autoescape=False,
  loader=FileSystemLoader(os.path.join(PATH, 'templates')),
  trim_blocks=False)

include_pattern = re.compile(rb'#include [<\"\'](.+)[>\"\']', re.IGNORECASE)  # | re.MULTILINE re.DOTALL |
class_pattern = re.compile(rb'^\s*class\s+(\S+)\s*[\n:{]', re.IGNORECASE | re.MULTILINE)  # | re.MULTILINE re.DOTALL |
module_pattern = re.compile(rb"^\s*BEGIN_DECLARE_MODULE\((?P<name>\S+)\)(?P<def>.+)END_DECLARE_MODULE\((?P=name)\)",
                            re.DOTALL | re.IGNORECASE | re.MULTILINE)  # | re.MULTILINE
require_pattern = re.compile(rb'^\s+REQUIRE\((\S+)\)', re.IGNORECASE | re.MULTILINE)
provide_pattern = re.compile(rb'^\s+PROVIDE\((\S+)\)', re.IGNORECASE | re.MULTILINE)

naoth_doc_pattern = re.compile(rb'/\*!(.*?)\*/', re.DOTALL)

empty_pattern = re.compile(rb'^\s*$')
clear_pattern = re.compile(rb'^\s*\*', re.MULTILINE)


def parseFileMM(file):
    info = FileInfo(file)
    with open(file, 'r') as f:
        with mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ) as m:
            docs = []
            for i in naoth_doc_pattern.finditer(m):
                str = clearDocString(i[1]).decode('utf-8').strip()
                docs.append({'start': i.start(), 'end': i.end(), 'value': str})
                if empty_pattern.fullmatch(m[0:i.start()]):
                    info.setDocs(str)
            # find "includes"
            for match in include_pattern.finditer(m):
                info.addInclude(match[1].decode('utf-8'))
            # find class definitions
            for match in class_pattern.finditer(m):
                info.addClass(match[1].decode('utf-8'), checkDocs(docs, m, match.start()))

            for match in module_pattern.finditer(m):
                module_name = match[1].decode('utf-8')
                module_repr = match[2]  # .decode('utf-8')
                module_require = []
                module_provide = []

                for req in require_pattern.finditer(module_repr):
                    module_require.append(req[1].decode('utf-8'))

                for pro in provide_pattern.finditer(module_repr):
                    module_provide.append(pro[1].decode('utf-8'))

                info.addModule(module_name, module_require, module_provide, checkDocs(docs, m, match.start()))

    return info


def checkDocs(docs, data, start):
    for d in docs:
        if d['end'] <= start:
            if empty_pattern.fullmatch(data[d['end']:start]):
                return d['value']
    return None


def clearDocString(str):
    return clear_pattern.sub(b'', str)


def writeHtmlFile(file, data, filter=lambda x: True):
    with open(file, 'w') as f:
        f.write("<html><head><title>NaoTH-Docs</title></head><body>")
        for idx in data:
            if filter(data[idx]):
                html = '<h2 id="{}">{}</h2>'.format(idx.replace('/', '-'), idx)
                if data[idx].hasDocs():
                    html += markdown.markdown(data[idx].docs)
                if data[idx].hasIncludes():
                    html += "<h3>Includes</h3>"
                    html += "<ul>"
                    for i in data[idx].includes:
                        html += "<li>"
                        if i in data:
                            html += '<a href="#' + i.replace('/', '-') + '">' + i + '</a>'
                        else:
                            html += i
                        html += "</li>"
                    html += "</ul>"
                if data[idx].hasClasses():
                    html += "<h3>Classes</h3>"
                    html += "<ul>"
                    for c in data[idx].classes:
                        html += "<li>" + c.name
                        if c.hasDocs():
                            html += ":<br>" + markdown.markdown(c.docs)
                        html += "</li>"
                    html += "</ul>"
                if data[idx].hasModules():
                    html += "<h3>Modules</h3>"
                    html += "<ul>"
                    for m in data[idx].modules:
                        html += "<li>" + m
                        if data[idx].modules[m].hasDocs():
                            html += ":<br>" + markdown.markdown(data[idx].modules[m].docs)
                        html += "</li>"
                    html += "</ul>"
                # print(idx, str(data[idx]))
                f.write(html)
        f.write("</body></html>")


def parse_cognition(file, data, filter=lambda x: True):
    fname = file
    header = []
    behavior = []
    infrastructure = []
    modeling = []
    perception = []
    selfawareness = []
    visualcortex = []
    for idx in data:
        if filter(data[idx]):
            idx = os.path.normpath(idx)
            path_list = idx.split(os.sep)
            # remove prefix
            path_list = [e for e in path_list if e not in ('..', 'NaoTHSoccer', 'Source', 'Cognition')]
            if len(path_list) > 1:
                # print(path_list[1])  # these are the categories
                if path_list[1] == "Behavior":
                    # behavior.append(os.path.join(*path_list))
                    behavior.append(path_list[-1])
                if path_list[1] == "Infrastructure":
                    # infrastructure.append(os.path.join(*path_list))
                    infrastructure.append(path_list[-1])
                if path_list[1] == "Modeling":
                    # modeling.append(os.path.join(*path_list))
                    modeling.append(path_list[-1])
                if path_list[1] == "Perception":
                    # perception.append(os.path.join(*path_list))
                    perception.append(path_list[-1])
                if path_list[1] == "SelfAwareness":
                    # selfawareness.append(os.path.join(*path_list))
                    selfawareness.append(path_list[-1])
                if path_list[1] == "VisualCortex":
                    # visualcortex.append(os.path.join(*path_list))
                    visualcortex.append(path_list[-1])
            else:
                header.append(os.path.join(*path_list))
    context = {
      'header': header,
      'behavior': behavior,
      'infrastructure': infrastructure,
      'modeling': modeling,
      'perception': perception,
      'selfawareness': selfawareness,
      'visualcortex': visualcortex
    }

    with open(fname, 'w') as f:
        html = render_template('template.html', context)
        f.write(html)

    # TODO seperate doc files for each class
    # get a list of each filename without file ending

    # copy all static files to the doc folder
    src_files = os.listdir('static/')
    for file_name in src_files:
        full_file_name = os.path.join('static/', file_name)
        if os.path.isfile(full_file_name):
            shutil.copy(full_file_name, "../../../doc/")


def render_template(template_filename, context):
    return TEMPLATE_ENVIRONMENT.get_template(template_filename).render(context)


def parse_args():
    parser = argparse.ArgumentParser(description='Generate documentation for the NaoTH project.')
    parser.add_argument('-s', '--src', type=str, default="../../../NaoTHSoccer/Source/Cognition", help='Location of the source files')
    parser.add_argument('-f', '--file', type=str, default="../../../doc/index.html", help='Location of the output files')
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    # print(args)

    source_directory = args.src
    html_file = args.file

    code_structure = {}
    for root, dirs, files in os.walk(source_directory):
        path = root.split(os.sep)
        for file in files:
            if file.endswith(".h"):
                file_idx = os.path.join(root, file).replace(source_directory+'/', '')
                code_structure[file_idx] = parseFileMM(os.path.join(root, file))

    # lambda f: f.hasDocs()
    # lambda f: f.hasModules()
    # writeHtmlFile(html_file, code_structure)

    # write_html soll am ende parse_cognition, parse_motion, parse_xabsl etc aufrufen, das heißt parseFileMM muss mehrmals aufgerufen
    # werden

    parse_cognition(html_file, code_structure)
