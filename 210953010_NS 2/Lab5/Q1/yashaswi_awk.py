import os
delay=[5,10,15,20,25]
datarate=[0.5,1,4,10,24,100]
for x in datarate:
    for d in delay:
        filename="tcp-bulk-send_"+str(d)+"ms_"+str(x)+"Mbps.tr"
        output_file="delay_"+str(d)+"_"+str(x)
        command="awk -f delay-graph.awk "+filename+">"+output_file
        os.system(command)