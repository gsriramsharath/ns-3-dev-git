mkdir -p Graphs
#Cwnd plot
gnuplot gnuplotscriptQ
cp queue.png Graphs/

cd cwndTraces
gnuplot gnuplotscriptCwnd
cp Cwnd*.png ../Graphs/
