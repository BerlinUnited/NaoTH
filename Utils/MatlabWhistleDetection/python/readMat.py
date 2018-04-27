from naoth import matlab_tools as mt

"""
    Example for accessing structs inside of mat files.
"""


test = mt.loadmat('../MatDatabase/database.mat')
print(test)
print(type(test['database']))

print(test['database']['bhuman'][0])


