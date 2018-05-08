
import scipy.io as spio
import numpy as np

# https://pyhogs.github.io/reading-mat-files.html
def loadmat(filename):
  '''
  this function should be called instead of direct spio.loadmat
  as it cures the problem of not properly recovering python dictionaries
  from mat files. It calls the function check keys to cure all entries
  which are still mat-objects

  from: `StackOverflow <http://stackoverflow.com/questions/7008608/scipy-io-loadmat-nested-structures-i-e-dictionaries>`_
  '''
  # , matlab_compatible=True
  data = spio.loadmat(filename, struct_as_record=False, squeeze_me=True)
  return _check_keys(data)


def _check_keys(dict):
  '''
  checks if entries in dictionary are mat-objects. If yes
  todict is called to change them to nested dictionaries
  '''
  for key in dict:
    if isinstance(dict[key], spio.matlab.mio5_params.mat_struct):
      dict[key] = _todict(dict[key])
  return dict


def _todict(matobj):
  '''
  A recursive function which constructs from matobjects nested dictionaries
  '''
  dict = {}
  for strg in matobj._fieldnames:
    elem = matobj.__dict__[strg]
    if isinstance(elem, spio.matlab.mio5_params.mat_struct):
      dict[strg] = _todict(elem)
    elif isinstance(elem, np.ndarray) and elem.dtype.type == np.dtype(spio.matlab.mio5_params.mat_struct).type:
      dict[strg] = [_todict(e) for e in elem]
    else:
      dict[strg] = elem
  return dict
  
  
def savemat(filename, dict):
  spio.savemat(filename, dict)
