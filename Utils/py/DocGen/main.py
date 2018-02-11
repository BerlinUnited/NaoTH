import argparse
import os, mmap, re, markdown

from file_info import FileInfo


include_pattern = re.compile(rb'#include [<\"\'](.+)[>\"\']', re.IGNORECASE)  # | re.MULTILINE re.DOTALL |
class_pattern = re.compile(rb'^\s*class\s+(\S+)\s*[\n:{]', re.IGNORECASE | re.MULTILINE)  # | re.MULTILINE re.DOTALL |
module_pattern = re.compile(rb"^\s*BEGIN_DECLARE_MODULE\((?P<name>\S+)\)(?P<def>.+)END_DECLARE_MODULE\((?P=name)\)",
                            re.DOTALL | re.IGNORECASE | re.MULTILINE)  # | re.MULTILINE
require_pattern = re.compile(rb'^\s+REQUIRE\((\S+)\)', re.IGNORECASE | re.MULTILINE)
provide_pattern = re.compile(rb'^\s+PROVIDE\((\S+)\)', re.IGNORECASE | re.MULTILINE)

naoth_doc_pattern = re.compile(rb'/\*\*!(.*?)\*\*/', re.DOTALL)

empty_pattern = re.compile(rb'^\s*$')
clear_pattern = re.compile(rb'^\s*\*', re.MULTILINE)

def parseFileMM(file):
    info = FileInfo(file)
    with open(file, 'r') as f:
        with mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ) as m:
            docs = []
            for i in naoth_doc_pattern.finditer(m):
                str = clearDocString(i[1]).decode('utf-8').strip()
                docs.append({'start':i.start(), 'end':i.end(), 'value': str})
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
                module_repr = match[2] #.decode('utf-8')
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
                html = '<h2 id="{}">{}</h2>'.format(idx.replace('/','-'),idx)
                if data[idx].hasDocs():
                    html += markdown.markdown(data[idx].docs)
                if data[idx].hasIncludes():
                    html += "<h3>Includes</h3>"
                    html += "<ul>"
                    for i in data[idx].includes:
                        html += "<li>"
                        if i in data:
                            html += '<a href="#' + i.replace('/','-') + '">' + i + '</a>'
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
                #print(idx, str(data[idx]))
                f.write(html)
        f.write("</body></html>")

def parseArgs():
    parser = argparse.ArgumentParser(description='Generate documentation for the NaoTH project.')
    parser.add_argument('-s','--src', type=str, default="../../../NaoTHSoccer/Source", help='Location of the source files')
    parser.add_argument('-f','--file', type=str, default="info.html", help='Location of the source files')
    return parser.parse_args()


if __name__ == "__main__":
    args = parseArgs()
    #print(args)

    source_directory = args.src
    html_file = args.file

    code_structure = {}
    for root, dirs, files in os.walk(source_directory):
        path = root.split(os.sep)
        for file in files:
            if file.endswith(".h"):
                file_idx = os.path.join(root, file).replace(source_directory+'/', '')
                code_structure[file_idx] = parseFileMM(os.path.join(root, file))

    #lambda f: f.hasDocs()
    #lambda f: f.hasModules()
    writeHtmlFile(html_file, code_structure)
