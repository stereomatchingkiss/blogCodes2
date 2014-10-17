import os,sys

file = open("foo.txt", "r")
sum = [0, 0, 0, 0, 0, 0, 0, 0, 0]
total = 0
for line in file:
    total = total + 1
    print line,
    info = line.split()
    for i in range(0, 8):        
        sum[i] += float(info[i])

print '\n'		
		
for data in sum:
    print data / total,