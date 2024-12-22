import re
import os
import time

verifier = '../main'
interpreter = '../../interpreter/main'
interpreter_verified = '../../interpreter-verified/main'
lamac = '../../src/lamac'
regression_dir = '../../regression'

# collect test filenames
broken = set(['test111'])
pattern = re.compile(r'^test\d{3}$')
test_files = sorted(
    filename
    for filename in os.listdir(regression_dir)
    if pattern.match(filename) and filename not in broken
)

# create temp directory
if not os.path.exists('temp'):
    os.makedirs('temp')

failed_tests = 0
for test_filename in test_files:
    # compile
    os.system(f'{lamac} -b {regression_dir}/{test_filename}.lama')

    # run verifier
    fail = os.system(f'{verifier} {test_filename}.bc')
    if fail:
        print(f'{test_filename} failed on verifier')
        break

    # run interpreter-verified on testXXX_verified.bc
    os.system(f'touch temp/{test_filename}.log')
    os.system(f'{interpreter_verified} {test_filename}_verified.bc < {regression_dir}/{test_filename}.input > temp/{test_filename}.log')

    # compare output with expected 
    fail = os.system(f'diff {regression_dir}/{test_filename}.log temp/{test_filename}.log')
    if fail:
        print(f'{test_filename} failed on interpreter-verified')
        break

    print(f'{test_filename} passed')

else:
    print('Regression tests passed!')


# compare performance

start = time.time()
os.system(f'{verifier} Sort.bc')
print(f'[verifier] Elapsed time on "Sort.bc": {time.time() - start:.03f} seconds')

start = time.time()
os.system(f'{interpreter} Sort.bc')
print(f'[iterative] Elapsed time on "Sort.bc": {time.time() - start:.03f} seconds')

start = time.time()
os.system(f'{interpreter_verified} Sort.bc')
print(f'[iterative-verified] Elapsed time on "Sort.bc": {time.time() - start:.03f} seconds')

start = time.time()
os.system(f'echo "0" | {lamac} -i Sort.lama > /dev/null')
print(f'[recursive] Elapsed time on "Sort.lama": {time.time() - start:.03f} seconds')
