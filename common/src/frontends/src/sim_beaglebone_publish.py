#
# Sim publisher of driver data
# Binds PUB socket to tcp://*:5556
# Publishes currents and temperature
#

import zmq
import time
from random import uniform

context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.connect("tcp://localhost:5556")

while True:
    Ch1Current = 10.0 + uniform(-0.1,0.1)
    Ch1Temp = 25.0 + uniform(-1.0,1.0)
    Ch2Current = 20.0 + uniform(-0.1,0.1)
    Ch2Temp = 25.0 + uniform(-1.0,1.0)
    Ch3Current = 0.1 + uniform(-0.1,0.1)
    Ch3Temp = 25.0 + uniform(-1.0,1.0)
    Ch4Current = 0.2 + uniform(-0.1,0.1)
    Ch4Temp = 25.0 + uniform(-1.0,1.0)
    socket.send_string("%f %f %f %f" % (Ch1Current, Ch1Temp, Ch2Current, Ch2Temp))
    print "Sent a message"
    time.sleep(1)
