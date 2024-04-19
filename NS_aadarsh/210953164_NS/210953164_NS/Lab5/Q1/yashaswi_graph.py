import os
main_code="""set terminal png font 'Times-New-Roman,14'
set output "datarate_{}.png"
load "datarate_{}.png"
set xrange [0.0000:4]
set yrange [0.000:4]
set xlabel "-- Time (seconds) -->" offset 0, 0.5
set ylabel "--- Delay (seconds)-->" offset 1.0, 0
plot """
delay=[5,10,15,20,25,25]
datarate=[0.5,1,4,10,24,100]
for x in datarate:

    code = main_code.format(str(x),str(x))
    print(code)
    for i,d in enumerate(delay):

        filename="tcp-bulk-send_"+str(d)+"ms_"+str(x)+"Mbps.tr"
        output_file="delay_"+str(d)+"_"+str(x)
        code += '"{}" using ($1):($2) title \'Average-delay{}\' with linespoints ls {},\\\n'.format(output_file,str(i+1), str(i+1))
    code = code[:-2]
    code+='\nset output'
    with open('yashaswi_graph_gnu','w') as f:
        f.write(code)
    os.system("gnuplot yashaswi_graph_gnu")
    
