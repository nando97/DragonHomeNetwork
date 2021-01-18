#!/usr/bin/env python
"""
This is a python script that handles and store message received by Gateway.
"""
import sys, os, serial, signal, time, json
from datetime import datetime

from config_file import get_conf

conf_file_path = "serial-to-data.conf"

# callback functions
def monitor(gateway_ser):
   data_point={'0':0,'1':0}
   point_set=set()
   start = time.time()
   # loop to read incoming data unless terminated
   while (1):
        # read in incoming message in serial port
        if(gateway_ser.in_waiting > 0):
            line = gateway_ser.readline()
            line_str = str(line)
            print(line_str)
            # Check if any data is read
            if (line_str != ""):
            #print line[:-1]         # strip \n
                fields = line_str[2:-3] .split('/')
                # Check if the data received is sensors data
                if len(fields) > 1:
                    start = time.time() 
                    server_ID = fields[1]
                    sensor_ID = fields[2]
                    data_point[sensor_ID] = data_point[sensor_ID] + 1
                    point_set.add(data_point[sensor_ID])
                    data_point[sensor_ID] = len(point_set) 
                    # generate string of data 

                    # get current time at the time of receiving data
                    now = datetime.now()
                    cur_time = now. strftime("%D %H:%M:%S")

                    # get run number
                    with open(os.getcwd() + "/run_config.json") as json_file:
                        data = json.load(json_file)
                        run_num = data['run_num']
                    msg = "DataPoint " + str(data_point[sensor_ID]) + " Time "\
                        + cur_time + " Run " + str(run_num) + " SensorID " + sensor_ID
                    print("device ID: ", sensor_ID)
                    text_file = open(os.getcwd() + "/gateway_data.txt", "a")

                    # read in all measurements sent from the sensor
                    for idx in range(3,len(fields),2):
                        para = fields[idx]
                        measure = fields[idx+1]     
                        msg = msg + " " + para + " " + measure

                    text_file.write(msg + "\n")
                    text_file.close()
        
        else:
            end = time.time()
            duration = end - start
            if duration >= 10.00:
                print(duration)
                print("Ending Program.....")
                update_json()
                sys.exit(0)
       # do some other things here

   print("Stop Monitoring")

# handling json file by reading then writing data
def update_json():
    with open(os.getcwd() + "/run_config.json") as json_file:
        data = json.load(json_file)
        run_num = data['run_num']
        print("Run number %d ended" %run_num)
        run_num = run_num + 1
        data['run_num'] = run_num

    with open(os.getcwd() + "/run_config.json", "w") as json_file:   
        json.dump(data, json_file)

# signal handler for ctrl-c    
def signal_handler(signal, frame):
    print("Ending Program.....")
    update_json()
    sys.exit(0)

""" -------------------------------------------
MAIN APPLICATION
"""  
def main():
    # exit gracefully if sigint is received
    signal.signal(signal.SIGINT, signal_handler)

    # get configuration from config file
    config = {}
    try:
        conf_file = open(conf_file_path, 'r')
        config = get_conf(conf_file)
        conf_file.close()
    except FileNotFoundError:
        print("Cannot find configuration file - using default settings.")

    # setup the serial port based on the config file/or assignment
    gateway_ser = serial.Serial()
    gateway_ser.baudrate = config['baud rate'] if 'baud rate' in config else 9600
    gateway_ser.timeout = None
    if 'time out' in config and config['time out'].lower() != 'none':
        gateway_ser.timeout = config['time out']

    # check to see if the user inputed a serial port
    if len(sys.argv) == 2:
        gateway_ser.port = sys.argv[1]
    else:
        gateway_ser.port = config['serial port'] if 'serial port' in config else 'COM6' 

    # attempt to open the serial port
    try:
        gateway_ser.open()
    except serial.SerialException:
        print('could not find serial port: ' + gateway_ser.port)
        sys.exit(1)

    print("Starting Serial Monitor\n")

    # Function call to log data coming to serial port
    monitor(gateway_ser)

if __name__ == '__main__':
    main()