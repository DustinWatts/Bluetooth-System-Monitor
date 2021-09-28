# Install the following Python modules:
# pip install pyserial
# pip install psutil

# On Mac's with the CPU and GPU in the same dye, only the CPU temperature is available
# so pass the CPU temp to GPU temp

import serial
import os
import time
import psutil

updateTime = 4 #number of seconds between each update

def sendData(temp, rpm, gpu, free_disk, free_mem, procs):
    try:
        connection = serial.Serial('/dev/tty.MyDisplay-ESP32SPP')
        data = temp + ',' + rpm + ',' + str(free_mem) + ',' + str(free_disk) + ',' + gpu + ',' + str(procs) + '/'
        connection.write(data.encode())
        print("Data written", data.encode())
        connection.close  
    except Exception as e:
        print(e)

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

while(1):
    temp = CPU_Temp()
    rpm = FAN_Speed()
    obj_Disk = psutil.disk_usage('/')
    free_disk = int(obj_Disk.free / (1000.0 ** 3))
    free_mem = (int(psutil.virtual_memory().total - psutil.virtual_memory().used)/ (1024 * 1024)) 
    proc_counter = 0
    for proc in psutil.process_iter():
        proc_counter += 1
    sendData(temp, rpm, temp, free_disk, free_mem, proc_counter)
    time.sleep(updateTime)
    
