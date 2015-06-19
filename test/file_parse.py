import os
import sys

if len(sys.argv) == 3:
    target_dir = sys.argv[1]
    header_name = sys.argv[2]
else:
    target_dir = '.'
    header_name = 'AllTests.h'

list_of_test_groups = []
dict_of_tests = {}

for root, dirs, files in os.walk(target_dir):
    for f in files:
        if '_test.c' in f:
            cur_file = open(os.path.join(root, f), 'r')
            lines = cur_file.readlines()
            for line in lines:
                if 'TEST' in line and '#define' not in line:
                    group_name = line.split('(')[1].split(',')[0]
                    if group_name not in list_of_test_groups:
                        list_of_test_groups.append(group_name)
                        dict_of_tests[group_name] = []
                    if 'IGNORE_TEST' not in line:
                        dict_of_tests[group_name].append(line.split(',')[1][1:-2])
            cur_file.close()


f = open(header_name, 'w');
f.write('#ifndef ALL_TESTS_H_INCLUDED\n')
f.write('#define ALL_TESTS_H_INCLUDED\n')
f.write('#include "test_functions.h"\n')

for l in list_of_test_groups:
    f.write('void ' + l + '_setup(void);\n');
    f.write('void ' + l + '_cleanup(void);\n');
    for t in dict_of_tests[l]:
        f.write('void ' + t + '(void);\n');
    f.write('TestFunctions ' + l + '_tests[] = {\n')
    for t in dict_of_tests[l]:
        f.write('    ' + t + ',\n');
    f.write('};\n')
    f.write('struct TestsWrapper ' + l +'_wrapper = {\n')
    f.write('    ' + l + '_setup,\n')
    f.write('    ' + l + '_cleanup,\n')
    f.write('    ' + l + '_tests,\n')
    f.write('    ' + str(len(dict_of_tests[l])) + '\n')
    f.write('};\n')

f.write('struct TestsWrapper *wrappers[] = {\n')
for l in list_of_test_groups:
    f.write('    &' + l + '_wrapper,\n')
f.write('};\n')
f.write('#endif\n')
f.close()
