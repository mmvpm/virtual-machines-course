import re
import os
import time

target = '../main'
lamac = '../../src/lamac'
regression_dir = '../../regression'

broken = set(['test111'])
pattern = re.compile(r'^test\d{3}$')
test_files = sorted(
    filename
    for filename in os.listdir(regression_dir)
    if pattern.match(filename) and filename not in broken
)

if not os.path.exists('temp'):
    os.makedirs('temp')

for test_filename in test_files:
    os.system(f'{lamac} -b {regression_dir}/{test_filename}.lama')
    os.system(f'touch temp/{test_filename}.log')
    os.system(f'./{target} {test_filename}.bc < {regression_dir}/{test_filename}.input > temp/{test_filename}.log')
    fail = os.system(f'diff {regression_dir}/{test_filename}.log temp/{test_filename}.log')
    if fail:
        print(f'{test_filename} failed')
        break
    else:
        print(f'{test_filename} passed')
else:
    print('Regression tests passed!')

start = time.time()
os.system(f'./{target} Sort.bc')
print(f'Elapsed time on "Sort.bc": {time.time() - start:.03f} seconds')
