import os
import sys
import numpy as np

print('about to import', file=sys.stderr)
print('python is', sys.version_info)
print('pid is', os.getpid())

import my_pb_mod
from my_pb_mod import NoCopyWrapper

import psutil
process = psutil.Process(os.getpid())

N, M = 1000, 1000
print('imported, about to call', file=sys.stderr)

print(f"before: {process.memory_info().rss}")
arr = np.random.rand(N, M).astype(np.float32)

print(f"init: {process.memory_info().rss}")
ncw = NoCopyWrapper(arr.data, N, M)
#ncw = NoCopyWrapper()
print(f"pass: {process.memory_info().rss}")
arr1 = ncw.get_raw()
print(f"numpy: {process.memory_info().rss}")
#mat = ncw.get_mat()
#print(f"bufprotocal: {process.memory_info().rss}")

print(f"before vec: {process.memory_info().rss}")

l = []
for i in range(M*N):
    l.append(i)

print(f"init vec: {process.memory_info().rss}")
ncw1 = NoCopyWrapper(l)
#ncw = NoCopyWrapper()
print(f"pass pybind: {process.memory_info().rss}")
l1 = ncw1.get_vec()
print(f"py list: {process.memory_info().rss}")

print(f"before mat: {process.memory_info().rss}")
ncw1.create_mat()
print(f"create mat: {process.memory_info().rss}")
abc = ncw1.get_mat()
print(f"after return: {process.memory_info().rss}")

result = my_pb_mod.add(2, 3)
print(result)
assert result == 5

print('done!', file=sys.stderr)
