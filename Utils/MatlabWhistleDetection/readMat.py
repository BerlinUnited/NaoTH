from naoth import matlab_tools as mt

test = mt.loadmat('MatDatabase/database.mat')
print(test)
print(type(test['database']))

print(test['database']['bhuman'][0])


