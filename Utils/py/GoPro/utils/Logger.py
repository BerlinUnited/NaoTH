import logging
import logging.handlers


LOGGER_NAME = "pyGoPro"
LOGGER_FMT  = '%(levelname)s: %(message)s'
LOGGER_FMT_CHILD = '%(levelname)s[%(name)s]: %(message)s'


lh = logging.StreamHandler()
sh = logging.handlers.SysLogHandler(address='/dev/log')
logger = logging.getLogger(LOGGER_NAME)


def setupLogger(quiet:bool = False, verbose:bool = False, syslog:bool = False):
    lh.setFormatter(LogFormatter())

    lvl = logging.ERROR if quiet else (logging.DEBUG if verbose else logging.INFO)
    logging.basicConfig(level=lvl, format=LOGGER_FMT)

    logger.propagate = False
    logger.addHandler(lh)

    if syslog:
        sh.setFormatter(logging.Formatter('GoPro:'+LOGGER_FMT))
        logger.addHandler(sh)


def getLogger(name):
    l = logger.getChild(name) #logging.getLogger(name)
    l.addHandler(lh)
    l.propagate = False

    return l

class LogFormatter(logging.Formatter):
    def format(self, record):
        if record.name != LOGGER_NAME:
            self._style._fmt = LOGGER_FMT_CHILD
        else:
            self._style._fmt = LOGGER_FMT

        return logging.Formatter.format(self, record)  # super(LogFormatter, self).format(format)

# make methods public
error = logger.error
warning = logger.warning
info = logger.info
debug = logger.debug
log = logger.log