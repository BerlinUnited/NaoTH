import glob
import os
import pprint
import re

import Config


def compare_dict(d1: dict, d2: dict, verbose: bool, ignore=None):
    # storing the differences of each dict
    r = {}
    # compare the dict keys
    k1 = set(d1.keys())
    k2 = set(d2.keys())
    #
    if ignore is not None:
        for ign in ignore:
            if ign in k1:
                k1.remove(ign)
            if ign in k2:
                k2.remove(ign)
    # recursive traverse equal keys
    ke = k1.intersection(k2)
    for k in ke:
        # check the value type for differences
        if type(d1[k]) == type(d2[k]):
            # further compare dicts
            if isinstance(d1[k], dict):
                rs = compare_dict(d1[k], d2[k], verbose)
                # add the resulting (sub) difference dict only to this difference dict, if there actually are differences in the sub dict
                if rs:
                    r[k] = rs
            elif d1[k] == d2[k]:
                # values are equal, ignore
                pass
            else:
                # values are different, add them to the difference dict
                r[k] = (d1[k], d2[k])
                if verbose:
                    print(k, 'values', (d1[k], d2[k]))
        else:
            if verbose:
                print(k, 'types', (d1[k], d2[k]))
            r[k] = (d1[k], d2[k])
    # add unequal keys to the difference dict
    kd = k1.symmetric_difference(k2)
    if kd:
        if verbose:
            print('difference', kd)
        for k in kd:
            r[k] = d1[k] if k in d1 else d2[k]

    return r

def compare_configs(dir, verbose:bool=False):
    # check parameter
    if os.path.isdir(dir):
        print('Checking team configuration ...')
        # find configs
        regex = re.compile('\d{6}-\d{4}-(\S+)')
        regex_name = re.compile(r'^Name=(\S+)$', re.MULTILINE)
        configs = []
        for d in os.scandir(dir):
            if d.is_dir() and regex.fullmatch(d.name):
                config_uri = glob.glob(d.path + '/[Cc]onfig*')
                if config_uri:
                    nao = None
                    # check for additional infos
                    if os.path.isfile(d.path + '/nao.info'):
                        with open(d.path + '/nao.info', 'r') as f:
                            c = f.read()
                            m = re.search(regex_name, c)
                            if m:
                                nao = m.group(1)
                    configs.append((nao, Config.readConfig(config_uri[0])))
        # found configs
        if len(configs) > 1:
            # compare configs with each other
            for i in range(len(configs)):
                for j in range(i + 1, len(configs)):
                    # get the configuration for system set
                    c1 = configs[i][1].getConfigFor(platform='Nao', scheme=configs[i][1].getScheme(), robot=configs[i][0])
                    c2 = configs[j][1].getConfigFor(platform='Nao', scheme=configs[j][1].getScheme(), robot=configs[j][0])
                    # comparing dicts, but ignoring specific keys
                    r = compare_dict(c1, c2, False, ['CameraMatrixOffset'])

                    print('\n', configs[i][1].getName(), '\n', configs[j][1].getName(), sep='')
                    if r:
                        print('Found differences in ', list(r.keys()), sep='')
                        if verbose:
                            pp = pprint.PrettyPrinter(indent=4)
                            pp.pprint(r)
                    else:
                        print("OK")
        else:
            print('Found only 1 configuration!')
    else:
        print('The given URI isn\'t a directory!')


def compare_config_pair(config_1, config_2, verbose:bool=False, as_table:bool=True):
    # load configs
    c1 = Config.readConfig(config_1)
    c2 = Config.readConfig(config_2)
    c1_config = c1.getConfigFor(platform='Nao', scheme=c1.getScheme())
    c2_config = c2.getConfigFor(platform='Nao', scheme=c2.getScheme())
    # compare configs
    r = compare_dict(c1_config,
                     c2_config,
                     False,
                     ['CameraMatrixOffset'])
    # print out result
    print('\n', c1.getName(), '\n', c2.getName(), sep='')
    if r:
        print('Found differences in ', list(r.keys()), sep='')
        if verbose:
            if as_table:
                _print_as_table(r, c1_config, c2_config)
            else:
                pp = pprint.PrettyPrinter(indent=4)
                pp.pprint(r)
    else:
        print("OK")

def _print_as_table(r, c1_config, c2_config):
    for k in r:
        max_length = [0, 0, 0, 0]  # total, 1 col, 2 col, 3 col
        for param in r[k]:

            if max_length[1] < len(param):
                max_length[1] = len(param)

            if k in c1_config and param in c1_config[k] and max_length[2] < len(str(c1_config[k][param])):
                max_length[2] = len(str(c1_config[k][param]))

            if k in c2_config and param in c2_config[k] and max_length[3] < len(str(c2_config[k][param])):
                max_length[3] = len(str(c2_config[k][param]))

        max_length[0] = len(k) if sum(max_length) < len(k) else sum(max_length)

        print('|', '-' * (max_length[0] + 8), '|', sep='')
        print('|', k, ' ' * (max_length[0] - len(k) + 5), '|')
        print('|', '-' * (max_length[0] + 8), '|', sep='')
        for param in r[k]:
            left = str(c1_config[k][param]) if k in c1_config and param in c1_config[
                k] else ''
            right = str(c2_config[k][param]) if k in c2_config and param in c2_config[
                k] else ''
            first_col_size = max_length[0] - len(param) - sum(max_length[2:]) if sum(max_length[1:]) < max_length[
                0] else (max_length[1] - len(param))
            print('| ', param, ' ' * first_col_size,
                  ' | ', left, ' ' * (max_length[2] - len(left)),
                  ' | ', right, ' ' * (max_length[3] - len(right)), ' |',
                  sep='')
        print('|', '-' * (max_length[0] + 8), '|\n', sep='')
