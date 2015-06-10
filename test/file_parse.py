import os
import sys

if len(sys.argv) == 3:
    target_dir = sys.argv[1]
    header_name = sys.argv[2]
else:
    target_dir = '.'
    header_name = 'AllTests.h'

list_of_tests = []

for root, dirs, files in os.walk(target_dir):
    for f in files:
        if '_test.c' in f:
            cur_file = open(os.path.join(root, f), 'r')
            lines = cur_file.readlines()
            for line in lines:
                if 'TEST' in line and '#define' not in line and 'IGNORE_TEST' not in line:
                    l_list = line.split(',')
                    list_of_tests.append(l_list[1][1:-2])
            cur_file.close()

f = open(header_name, 'w');
f.write('#ifndef ALL_TESTS_H_INCLUDED\n')
f.write('#define ALL_TESTS_H_INCLUDED\n')
f.write('#include "test_functions.h"\n')
for l in list_of_tests:
    f.write('void ' + l + '(void);\n');
f.write('test_functions_t test_functions[] = {\n')
for l in list_of_tests:
    f.write('    ' + l + ',\n');
f.write('};\n')
f.write('#endif\n')
f.close()
