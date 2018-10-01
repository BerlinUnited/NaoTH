import logging
import logging.handlers
import os

LOGGER_NAME = "pyGoPro"
LOGGER_FMT  = '%(levelname)s: %(message)s'
LOGGER_FMT_CHILD = '%(levelname)s[%(name)s]: %(message)s'


lh = logging.StreamHandler()
sh = None
fh = None
logger = logging.getLogger(LOGGER_NAME)
__children__ = []

def setupLogger(quiet:bool = False, verbose:bool = False, syslog:bool = False, directory:str = None):
    lh.setFormatter(LogFormatter())

    lvl = logging.ERROR if quiet else (logging.DEBUG if verbose else logging.INFO)
    logging.basicConfig(level=lvl, format=LOGGER_FMT)

    logger.propagate = False
    logger.addHandler(lh)

    if syslog:
        sh = logging.handlers.SysLogHandler(address='/dev/log')
        sh.setFormatter(logging.Formatter('GoPro:'+LOGGER_FMT))
        logger.addHandler(sh)
        # add the syslog handler to all child loggers
        for c in __children__: __addHandler(c, sh)

    if directory and os.path.isdir(directory):
        # log to files by day and only warning and above
        fh = logging.handlers.TimedRotatingFileHandler(os.path.join(directory, LOGGER_NAME+'.log'), 'D', backupCount=5)
        fh.setLevel(logging.WARNING)
        fh.setFormatter(LogFormatter(True))
        logger.addHandler(fh)
        # add the file handler to all child loggers
        for c in __children__: __addHandler(c, fh)

def __addHandler(l:logging.Logger, h:logging.Handler):
    # check before adding handler
    if h not in l.handlers:
        l.addHandler(h)

def getLogger(name):
    l = logger.getChild(name) #logging.getLogger(name)
    # add handlers
    l.addHandler(lh)
    if sh: l.addHandler(sh)
    if fh: l.addHandler(fh)
    # dont propagate to parent
    l.propagate = False
    # remember children
    __children__.append(l)

    return l

class LogFormatter(logging.Formatter):

    def __init__(self, include_date:bool=False):
        super().__init__()
        self.include_date = include_date

    def format(self, record):
        prefix = self.formatTime(record) + ' ' if self.include_date else ''
        if record.name != LOGGER_NAME:
            self._style._fmt = prefix + LOGGER_FMT_CHILD
        else:
            self._style._fmt = prefix + LOGGER_FMT

        return logging.Formatter.format(self, record)  # super(LogFormatter, self).format(format)

# make methods public
error = logger.error
warning = logger.warning
info = logger.info
debug = logger.debug
log = logger.log