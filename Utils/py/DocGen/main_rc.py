import os, re, argparse, mmap
from bs4 import BeautifulSoup

dialog_pattern = re.compile(rb"<h(?P<h>[1-3])>(?P<title>\S+)</h(?P=h)>\s+(<img src=\"(?P<image>.+)\">)?(?P<doc>.+)", re.DOTALL | re.IGNORECASE | re.MULTILINE)
title_pattern = re.compile(rb"^<h(?P<h>[1-3])>(?P<title>.+)</h(?P=h)>$", re.IGNORECASE)
image_pattern = re.compile(rb"^<img src=\"(?P<image>[^\"]+)\".*>$", re.IGNORECASE)


def parseFileMM(file):
    info = { 'title': '', 'image': '', 'content': '' }
    with open(file, 'r') as f:
        with mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ) as m:
            for line in iter(m.readline, ""):
                if line == b'': break

                if title_pattern.match(line):
                    info['title'] = title_pattern.match(line).group('title').decode('utf-8')
                elif image_pattern.match(line):
                    info['image'] = image_pattern.match(line).group('image').decode('utf-8')
                else:
                    info['content'] += line.decode('utf-8').strip()
    return info

def writeLatexFile(file, data):
    with open(file, 'w') as f:
        for idx in data:
            if data[idx]['title'] and (data[idx]['image'] or data[idx]['content']):
                latex = "\subsection*{" + data[idx]['title'] + "}\n"
                if data[idx]['image']:
                    latex += "\\begin{wrapfigure}[11]{l}{0.65\\textwidth}\n"
                    latex += "\t\\vspace{-\intextsep}% needed to align at the top\n"
                    latex += "\t\includegraphics[width=0.65\\textwidth]{rc-dialogs/" + data[idx]['image'] + "}\n"
                    latex += "\end{wrapfigure}\n"
                #latex += data[idx]['content'] + "\n"
                #data[idx]['content']

                if data[idx]['content']:
                    bs = BeautifulSoup(data[idx]['content'], 'html.parser')
                    latex += bs.getText() + "\n"

                f.write(latex)

'''
\subsection*{Team Communication Viewer}
\begin{wrapfigure}[11]{l}{0.65\textwidth}
  \vspace{-\intextsep}% needed to align at the top
  \includegraphics[width=0.65\textwidth]{rc-dialogs/TeamCommViewer}
\end{wrapfigure}
aasdf
'''

def parseArgs():
    parser = argparse.ArgumentParser(description='Generate documentation for the NaoTH project.')
    parser.add_argument('-s','--src', type=str, default="../../../RobotControl/RobotControlGUI/src", help='Location of the source files')
    parser.add_argument('-f','--file', type=str, default="info.tex", help='Location of the source files')
    return parser.parse_args()

if __name__ == "__main__":
    args = parseArgs()
    #print(args)

    source_directory = args.src
    latex_file = args.file


    help_files = {}
    for root, dirs, files in os.walk(source_directory):
        path = root.split(os.sep)
        for file in files:
            if file.endswith(".html") and "/build/" not in root:
                file_idx = os.path.join(root, file).replace(source_directory+'/', '')
                print(os.path.join(root, file))
                help_files[file_idx] = parseFileMM(os.path.join(root, file))


    writeLatexFile(latex_file, help_files)
