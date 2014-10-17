import psutil
import os,sys

def get_process_id(name):
    for proc in psutil.process_iter():
        try:
            pinfo = proc.as_dict(attrs=['pid', 'name'])            
            item = pinfo.items()            
            #print item[0][1],
            #print item[1][1]
            if item[1][1] == name:
			    return item[0][1]
        except psutil.NoSuchProcess:
            pass
        #else:
            #print(pinfo)			

def print_cpu_usage(appName, fileName):
    fo = open(appName + '_' + fileName + '.txt', "w")
    while(1):
        list1 = psutil.cpu_percent(interval=1, percpu=True)    
        print list1
        fo.write(''.join(str(e) + ' ' for e in list1) + '\n')

if __name__ == "__main__":
    
    appName = "rtdb"
    fileName = ""
    option = 0
    if len(sys.argv) > 1:
        appName = sys.argv[1]
    if len(sys.argv) > 2:
        fileName = sys.argv[2]
    if len(sys.argv) > 3:
        option = sys.argv[3]	
			
fo = open(appName + '_' + fileName + '.txt', "w")
process_id = get_process_id(appName + '.exe')
process = psutil.Process(process_id)

if (option == 0):
    while(1):
        list1 = process.cpu_percent(interval=1)    
        print list1    
        fo.write(str(list1) + '\n')    
else:
    print_cpu_usage(appName, fileName)