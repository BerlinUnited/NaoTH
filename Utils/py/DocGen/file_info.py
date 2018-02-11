
class FileInfo:
    def __init__(self, name):
        self.name = name
        self.includes = []
        self.classes = []
        self.modules = {}
        self.docs = None

    def addInclude(self, inc):
        self.includes.append(inc)

    def hasIncludes(self):
        return len(self.includes) > 0

    def addClass(self, cls, doc=None):
        self.classes.append(FileClass(cls, doc))

    def hasClasses(self):
        return len(self.classes) > 0

    def addModule(self, module, require=[], provide=[], docs=None):
        self.modules[module] = FileModule(module, require, provide, docs)

    def hasModules(self):
        return len(self.modules) > 0

    def setDocs(self, docs):
        self.docs = docs

    def hasDocs(self):
        return True if self.docs else False

    def __str__(self):
        result = ""
        inc = "\n\t".join(self.includes)
        if inc:
            result += "includes:\n\t" + inc + "\n"

        cls = "\n\t".join([str(c) for c in self.classes])
        if cls:
            result += "classes:\n\t" + cls + "\n"

        mod = "\n\t".join([ str(x[1]) for x in self.modules.items() ])
        if cls:
            result += "modules:\n\t" + mod + "\n"

        return result

class FileClass:
    def __init__(self, name, docs):
        self.name = name
        self.docs = docs

    def __str__(self):
        return self.name

    def __repr__(self):
        return self.name

    def hasDocs(self):
        return True if self.docs else False

class FileModule:
    def __init__(self, name, require=[], provide=[], docs=None):
        self.name = name
        self.require = require
        self.provide = provide
        self.docs = docs

    def addRequired(self, req):
        self.require.append(req)

    def addProvided(self, pro):
        self.provide.append(pro)

    def addDocs(self, docs):
        self.docs = docs

    def hasDocs(self):
        return True if self.docs else False

    def __str__(self):
        return self.name + "\n\trequire:\n\t\t" + "\n\t\t".join(self.require) + "\n\tprovide:\n\t\t" + "\n\t\t".join(self.provide)