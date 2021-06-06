#!/bin/python
import subprocess
import re
import tqdm
print("Testing ./test-trans -M 32 -N 32")

res = []
res2 = []
res3 = []

for num in tqdm.tqdm(range(1,64)):
    p = subprocess.run("make clean",shell = True,stdout=subprocess.DEVNULL)
    p = subprocess.run("CFLAGS=-D__BLOCK__={} make".format(str(num)),shell = True,stdout=subprocess.DEVNULL)
    p = subprocess.run("/bin/gcc -g -Wall -Werror -std=c99 -m64 -lm -O0 test-trans.o trans.o cachelab.o -o test-trans",shell = True)
    if (num < 32):
        p = subprocess.Popen("./test-trans -M 32 -N 32 | grep TEST_TRANS_RESULTS", 
                                shell=True, stdout=subprocess.PIPE)
        stdout_data = p.communicate()[0].decode("UTF-8")
        result32 = re.findall(r'(\d+)', stdout_data)
        rc = p.returncode
        #print("32:{}: {}".format(num, int(result32[1])))
        res.append((num, int(result32[1])))

    p = subprocess.Popen("./test-trans -M 64 -N 64 | grep TEST_TRANS_RESULTS", 
                            shell=True, stdout=subprocess.PIPE)
    stdout_data = p.communicate()[0].decode("UTF-8")
    result32 = re.findall(r'(\d+)', stdout_data)
    rc = p.returncode
    #print("64:{}: {}".format(num, int(result32[1])))
    res2.append((num, int(result32[1])))

    p = subprocess.Popen("./test-trans -M 61 -N 67 | grep TEST_TRANS_RESULTS", 
                            shell=True, stdout=subprocess.PIPE)
    stdout_data = p.communicate()[0].decode("UTF-8")
    result32 = re.findall(r'(\d+)', stdout_data)
    rc = p.returncode
    #print("61:{}: {}".format(num, int(result32[1])))

    res3.append((num, int(result32[1])))
    

print(sorted(res))
print(sorted(res2))
print(sorted(res3))
