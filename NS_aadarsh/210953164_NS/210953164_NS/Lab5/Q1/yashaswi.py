import os
os.system('echo "Geeks 4 Geeks"')
delay=[5,10,15,20,25,25]
datarate=[0.5,1,4,10,24,100]
for d in delay:
    for x in datarate:
        os.system("./waf --run \"scratch/q1.cc --delay="+str(d)+"ms --datarate="+str(x)+"Mbps\"")