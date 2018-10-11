#Receiving data from MEGA without using a ring buffer.
# Receives data and insert into a list. Which automatically prints out necessary data values into the csv file
#(or other method that softwareSquad prefers)
import serial
#import RingBuffer
import time
import sys
import operator
import numpy

#import threading (i will need this to manage tasks, need to read up on how to use threading and all)
#might not need for the time being.

class ReceiveData():
        def __init__(self, list, port):
                self.port = port
                self.list = list

        def run(self):
                #Handshaking, keep saying 'H' to Arduino unitl Arduino reply 'A'
                while(self.port.in_waiting == 0 or self.port.read() != 'A'):
                        print ('Connecting to Arduino')
                        self.port.write("H")
                        time.sleep(1)
                self.port.write("A");
                print ('Connected')
                self.readData()

        def readData(self):
                #receiving data from arduino
                if not self.buffer.isFull():
                        size = self.port.read(1) #ideally this should receive

                        for i in range(size):
                            data = self.port.read(1)
                            self.list.append(data)

                        checksum = self.port.read(1)
                        #assert isinstance(checksum, object)
                        StoreData(self.port, self.list, checksum)

                        #self.buffer.append(rcv)
                        #store = StoreData(self.port, self.buffer)
                        #store.run()

class StoreData():
        def __init__(self, port, list, checksum):
                self.port = port
                self.list =  list
                checksum = checksum

        def run(self):
                self.storeData()

        def storeData(self):
                if list:                                   #list is not empty
                        ack = False
                        #for data in dataList:
                        if reduce(operator.xor, list) == checksum: #pass checksum check
                        #if(numpy.bitwise_or.reduce(dataList)):
                                ack = True #ack current sample
                                sample = data
                                print(*list) #store the 24 reading data
                        else:
                                ack = False #if data has error, NAK
                                #break
                        if ack:
                                self.port.write('A')

                        else:
                                self.port.write('N')
                self.list.clear() #clears the list (once data has been stored) to prepare for next packet

class Raspberry():
        def __init__(self):
                self.list = [] #creating a list to store a packet received by the arduino
#dont need to use a ring buffer, as arduino side already  has one!

        def main(self):
            #set up port connection
            self.port=serial.Serial('/dev/ttyS0',115200) #or 38400 (TBC)
            receive = ReceiveData(self.buffer, self.port)
            receive.run()
            #not /dev/ttyAMA0

if __name__ == '__main__':
        pi = Raspberry()
        pi.main()