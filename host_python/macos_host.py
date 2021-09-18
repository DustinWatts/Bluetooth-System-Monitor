# Install the following Python modules:
# pip install pyserial
# pip install psutil

import serial
import os
import time
import psutil

obj_Disk = psutil.disk_usage('/')
updateTime = 4 #number of seconds between each update

def CPU_Temp():
    cpu_temp = [each.strip() for each in (os.popen('sudo powermetrics --samplers smc -i1 -n1')).read().split('\n') if each != '']
    for line in cpu_temp:
        if 'CPU die temperature' in line:
            return line.strip('CPU die temperature: ').rstrip(' C')
    return 'n/a'

def GPU_Temp():
    # Will not work with Integrated Intel GPU
    gpu_temp = [each.strip() for each in (os.popen('sudo powermetrics --samplers smc -i1 -n1')).read().split('\n') if each != '']
    for line in gpu_temp:
        if 'GPU die temperature' in line:
            return line.strip('GPU die temperature: ').rstrip(' C')
    return 'n/a'

def FAN_Speed():
    gpu_temp = [each.strip() for each in (os.popen('sudo powermetrics --samplers smc -i1 -n1')).read().split('\n') if each != '']
    for line in gpu_temp:
        if 'Fan' in line:
            return line.strip('Fan: ').rstrip(' rpm')
    return 'n/a'

def sendData(temp, rpm, gpu, free_disk, mem_use, procs):
    try:
        connection = serial.Serial('/dev/tty.ESP32-ESP32SPP')
        data = temp + ',' + rpm + ',' + str(mem_use) + ',' + str(free_disk) + ',' + gpu + ',' + str(procs)
        connection.write(data)
        print("Data written", temp, rpm, mem_use, free_disk, gpu, procs)
        connection.close  
    except Exception as e:
        print(e)


while(1):
    temp = CPU_Temp()
    rpm = FAN_Speed()
    free_disk = int(obj_Disk.used / (1024.0 ** 3))
    mem_use = int(psutil.virtual_memory().used / (1024 * 1024)) 
    proc_counter = 0
    for proc in psutil.process_iter():
        proc_counter += 1
    sendData(temp, rpm, temp, free_disk, mem_use, proc_counter)
    time.sleep(updateTime)
    
