import collections
import configparser
import glob
import os
import re
import tarfile
import zipfile


# directories to read and if they have subdirectories
DIRS = {
    'general': ('general/', False),
    'platform': ('platform/', True),
    'scheme': ('scheme/', True),
    'strategy': ('strategy/', True),
    'robots': ('robots/', True),
    'robots_bodies': ('robots_bodies/', True),
    'robot_heads': ('robot_heads/', True),
    'private': ('private/', False),
}


def getInstance(uri):
    if os.path.isdir(uri):
        return DirectoryConfig(uri + "/" if uri[-1] != "/" else "")
    elif tarfile.is_tarfile(uri):
        return TargzConfig(uri)
    elif zipfile.is_zipfile(uri):
        return ZipConfig(uri)

    return Config(uri)

def readConfig(uri):
    config = getInstance(uri)
    # read defined directories of config directory
    config.readConfig()
    return config


class Config:

    def __init__(self, uri):
        self._uri = uri
        self._config = {}

    def getName(self):
        return "ERROR: Invalid config file/directory"

    def readConfig(self):
        pass

    def getPlatforms(self):
        return self._config['platform'].keys() if 'platform' in self._config else []

    def getSchemes(self):
        return self._config['scheme'].keys() if 'scheme' in self._config else []

    def getStrategies(self):
        return self._config['strategy'].keys() if 'strategy' in self._config else []

    def getRobots(self):
        return self._config['robots'].keys() if 'robots' in self._config else []

    def getBodies(self):
        return self._config['robots_bodies'].keys() if 'robots_bodies' in self._config else []

    def getHeads(self):
        return self._config['robot_heads'].keys() if 'robot_heads' in self._config else []

    def getScheme(self):
        return None

    def getStrategy(self):
        return None

    def getConfigFor(self, platform=None, scheme=None, strategy=None, robot=None, body=None, head=None):

        config = {}

        self.__update_config_part(config, 'general')
        self.__update_config_part(config, 'platform', platform)
        self.__update_config_part(config, 'scheme', scheme)
        self.__update_config_part(config, 'strategy', strategy)
        self.__update_config_part(config, 'robots', robot)
        self.__update_config_part(config, 'robots_bodies', body)
        self.__update_config_part(config, 'robot_heads', head)
        self.__update_config_part(config, 'private')

        return config


    def __update_config_part(self, config, key, box = None):
        if key in self._config:
            if box is None and not DIRS[key][1]:
                self.update_dict(config, self._config[key], key if box is None else box)
            elif box in self._config[key]:
                self.update_dict(config, self._config[key][box], key if box is None else box)

    def update_dict(self, d, u, o):
        # recursive update dict
        for k, v in u.items():
            if isinstance(v, collections.Mapping):
                d[k] = self.update_dict(d.get(k, {}), v, o)
            else:
                d[k] = (v, o)  # value, origin
        return d


    def __contains__(self, item):
        return item in self._config

    def __getitem__(self, item):
        return self._config[item]

    def __cmp__(self, other):

        return self._config == other._config


class DirectoryConfig(Config):
    def __init__(self, uri):
        super().__init__(uri)

    def getName(self):
        return self._uri

    def getScheme(self):
        if os.path.isfile(self._uri + "scheme.cfg"):
            with open(self._uri + "scheme.cfg", "r") as f:
                return f.readline().strip()
        return None

    def getStrategy(self):
        if os.path.isfile(self._uri + "strategy.cfg"):
            with open(self._uri + "strategy.cfg", "r") as f:
                return f.readline().strip()
        return None

    def readConfig(self):
        for key in DIRS:
            self._config[key] = {}
            # does directory exist
            if os.path.isdir(self._uri + DIRS[key][0]):
                # read subdirectory
                if DIRS[key][1]:
                    for d in os.scandir(self._uri + DIRS[key][0]):
                        # read config files from subdirectories
                        if d.is_dir():
                            self._config[key][d.name] = self.__read_config_files(self._uri + DIRS[key][0] + d.name)
                else:
                    # read config files without traversing subdirectories
                    self._config[key] = self.__read_config_files(self._uri + DIRS[key][0])

    def __read_config_files(self,  dir):
        # only use '=' as assignment delimiter and be case-sensitive
        parser = configparser.ConfigParser(delimiters='=')
        parser.optionxform = str
        # read all config files in this directory
        for cfg in glob.glob(dir + "/*.cfg", recursive=True):
            try:
                parser.read(cfg)
            except configparser.DuplicateOptionError as e:
                # TODO: hint on duplicate option in the ui
                print(e)
        return parser._sections


class TargzConfig(Config):
    def __init__(self, uri):
        super().__init__(uri)
        self.file = tarfile.open(uri)

    def getName(self):
        return self.file.name

    def __getMember(self, name):
        try:
            return self.file.getmember(name)
        except:
            pass
        return None

    def getScheme(self):
        scheme_file = self.__getMember("Config/scheme.cfg")
        if scheme_file and scheme_file.isfile():
            # read scheme file
            return self.file.extractfile(scheme_file).readline().decode("utf-8").strip()
        return None

    def getStrategy(self):
        strategy_file = self.__getMember("Config/strategy.cfg")
        if strategy_file and strategy_file.isfile():
            # read scheme file
            return self.file.extractfile(strategy_file).readline().decode("utf-8").strip()
        return None

    def readConfig(self):
        for key in DIRS:
            self._config[key] = {}
            directory = self.__getMember("Config/"+DIRS[key][0].strip('/'))

            # does directory exist
            if directory and directory.isdir():
                # read subdirectory
                if DIRS[key][1]:
                    for d in self.file.getmembers():
                        # read config files from subdirectories
                        if d.isdir() and re.match("Config/"+DIRS[key][0]+"[^/]+$", d.name) is not None:
                            self._config[key][d.name.replace("Config/"+DIRS[key][0],"")] = self.__read_config_files(d.name)
                else:
                    # read config files without traversing subdirectories
                    self._config[key] = self.__read_config_files(directory.name)

    def __read_config_files(self,  prefix):
        # only use '=' as assignment delimiter and be case-sensitive
        parser = configparser.ConfigParser(delimiters='=')
        parser.optionxform = str

        for m in self.file.getmembers():
            if m.name.startswith(prefix + '/') and m.isfile() and m.name.endswith('.cfg'):
                try:
                    parser.read_string(self.file.extractfile(m).read().decode("utf-8"), m.name)
                except configparser.DuplicateOptionError as e:
                    # TODO: hint on duplicate option in the ui
                    print(e)

        return parser._sections


class ZipConfig(Config):
    def __init__(self, uri):
        super().__init__(uri)
        self.file = zipfile.ZipFile(uri)

    def getName(self):
        return self.file.filename

    def __getMember(self, name):
        try:
            return self.file.getinfo(name)
        except:
            pass
        return None

    def getScheme(self):
        scheme_file = self.__getMember("Config/scheme.cfg")
        #print(dir(scheme_file))
        if scheme_file and not scheme_file.is_dir():
            # read scheme file
            return self.file.read(scheme_file).decode("utf-8").strip()
        return None

    def getStrategy(self):
        strategy_file = self.__getMember("Config/strategy.cfg")
        #print(dir(scheme_file))
        if strategy_file and not strategy_file.is_dir():
            # read scheme file
            return self.file.read(strategy_file).decode("utf-8").strip()
        return None

    def readConfig(self):
        for key in DIRS:
            self._config[key] = {}
            directory = self.__getMember("Config/"+DIRS[key][0])
            # does directory exist
            if directory and directory.is_dir():
                # read subdirectory
                if DIRS[key][1]:
                    for d in self.file.infolist():
                        if d.is_dir():
                            sub = re.search("Config/"+DIRS[key][0]+"([^/]+)/$", d.filename)
                            if sub and sub[1]:
                                # read config files from subdirectories
                                self._config[key][sub[1]] = self.__read_config_files(d.filename)

                else:
                    # read config files without traversing subdirectories
                    self._config[key] = self.__read_config_files(directory.filename)

    def __read_config_files(self,  prefix):
        # only use '=' as assignment delimiter and be case-sensitive
        parser = configparser.ConfigParser(delimiters='=')
        parser.optionxform = str

        for m in self.file.infolist():
            if not m.is_dir() and m.filename.startswith(prefix) and m.filename.endswith('.cfg'):
                try:
                    parser.read_string(self.file.read(m).decode("utf-8"), m.filename)
                    pass
                except configparser.DuplicateOptionError as e:
                    # TODO: hint on duplicate option in the ui
                    print(e)

        return parser._sections

