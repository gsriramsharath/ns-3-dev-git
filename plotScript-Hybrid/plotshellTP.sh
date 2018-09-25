#Usage startTime
printf "Flow Throughput\n\n" | tee -a overallTP.txt

#for R1
python tp-new.py --startTime=3 --destIp 10.7.0.1 --destPort 50000 N-59-0.pcap plotmeTP/TP-SR1-1.plotme plotmeTPP/TP-SR1-1.plotme S-R1-1 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.7.0.1 --destPort 50001 N-59-0.pcap plotmeTP/TP-SR1-2.plotme plotmeTPP/TP-SR1-2.plotme S-R1-2 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.7.0.1 --destPort 50002 N-59-0.pcap plotmeTP/TP-SR1-3.plotme plotmeTPP/TP-SR1-3.plotme S-R1-3 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.7.0.1 --destPort 50003 N-59-0.pcap plotmeTP/TP-SR1-4.plotme plotmeTPP/TP-SR1-4.plotme S-R1-4 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.7.0.1 --destPort 50004 N-59-0.pcap plotmeTP/TP-SR1-5.plotme plotmeTPP/TP-SR1-5.plotme S-R1-5 | tee -a overallTP.txt


# for R2
python tp-new.py --startTime=3 --destIp 10.4.1.1 --destPort 50000 N-49-0.pcap plotmeTP/TP-SR2-1.plotme plotmeTPP/TP-SR2-1.plotme S-R2-1 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.4.2.1 --destPort 50000 N-50-0.pcap plotmeTP/TP-SR2-2.plotme plotmeTPP/TP-SR2-2.plotme S-R2-2 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.4.3.1 --destPort 50000 N-51-0.pcap plotmeTP/TP-SR2-3.plotme plotmeTPP/TP-SR2-3.plotme S-R2-3 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.4.4.1 --destPort 50000 N-52-0.pcap plotmeTP/TP-SR2-4.plotme plotmeTPP/TP-SR2-4.plotme S-R2-4 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.4.5.1 --destPort 50000 N-53-0.pcap plotmeTP/TP-SR2-5.plotme plotmeTPP/TP-SR2-5.plotme S-R2-5 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.4.6.1 --destPort 50000 N-54-0.pcap plotmeTP/TP-SR2-6.plotme plotmeTPP/TP-SR2-6.plotme S-R2-6 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.4.7.1 --destPort 50000 N-55-0.pcap plotmeTP/TP-SR2-7.plotme plotmeTPP/TP-SR2-7.plotme S-R2-7 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.4.8.1 --destPort 50000 N-56-0.pcap plotmeTP/TP-SR2-8.plotme plotmeTPP/TP-SR2-8.plotme S-R2-8 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.4.9.1 --destPort 50000 N-57-0.pcap plotmeTP/TP-SR2-9.plotme plotmeTPP/TP-SR2-9.plotme S-R2-9 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.4.10.1 --destPort 50000 N-58-0.pcap plotmeTP/TP-SR2-10.plotme plotmeTPP/TP-SR2-10.plotme S-R2-10 | tee -a overallTP.txt

#for R3
python tp-new.py --startTime=3 --destIp 10.8.0.1 --destPort 50000 N-60-0.pcap plotmeTP/TP-SR3-1.plotme plotmeTPP/TP-SR3-1.plotme S-R3-1 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.8.0.1 --destPort 50001 N-60-0.pcap plotmeTP/TP-SR3-2.plotme plotmeTPP/TP-SR3-2.plotme S-R3-2 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.8.0.1 --destPort 50002 N-60-0.pcap plotmeTP/TP-SR3-3.plotme plotmeTPP/TP-SR3-3.plotme S-R3-3 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.8.0.1 --destPort 50003 N-60-0.pcap plotmeTP/TP-SR3-4.plotme plotmeTPP/TP-SR3-4.plotme S-R3-4 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.8.0.1 --destPort 50004 N-60-0.pcap plotmeTP/TP-SR3-5.plotme plotmeTPP/TP-SR3-5.plotme S-R3-5 | tee -a overallTP.txt


#for RGf1
python tp-new.py --startTime=3 --destIp 10.18.0.1 --destPort 50000 N-62-0.pcap plotmeTP/TP-SRGf1-1.plotme plotmeTPP/TP-SRGf1-1.plotme S-RGf1-1 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.18.0.1 --destPort 50001 N-62-0.pcap plotmeTP/TP-SRGf1-2.plotme plotmeTPP/TP-SRGf1-2.plotme S-RGf1-2 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.18.0.1 --destPort 50002 N-62-0.pcap plotmeTP/TP-SRGf1-3.plotme plotmeTPP/TP-SRGf1-3.plotme S-RGf1-3 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.18.0.1 --destPort 50003 N-62-0.pcap plotmeTP/TP-SRGf1-4.plotme plotmeTPP/TP-SRGf1-4.plotme S-RGf1-4 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.18.0.1 --destPort 50004 N-62-0.pcap plotmeTP/TP-SRGf1-5.plotme plotmeTPP/TP-SRGf1-5.plotme S-RGf1-5 | tee -a overallTP.txt


#for RGf2
python tp-new.py --startTime=3 --destIp 10.6.0.1 --destPort 50000 N-61-0.pcap plotmeTP/TP-SRGf2-1.plotme plotmeTPP/TP-SRGf2-1.plotme S-SRGf2-1 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.6.0.1 --destPort 50001 N-61-0.pcap plotmeTP/TP-SRGf2-2.plotme plotmeTPP/TP-SRGf2-2.plotme S-SRGf2-2 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.6.0.1 --destPort 50002 N-61-0.pcap plotmeTP/TP-SRGf2-3.plotme plotmeTPP/TP-SRGf2-3.plotme S-SRGf2-3 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.6.0.1 --destPort 50003 N-61-0.pcap plotmeTP/TP-SRGf2-4.plotme plotmeTPP/TP-SRGf2-4.plotme S-SRGf2-4 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.6.0.1 --destPort 50004 N-61-0.pcap plotmeTP/TP-SRGf2-5.plotme plotmeTPP/TP-SRGf2-5.plotme S-SRGf2-5 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.6.0.1 --destPort 50005 N-61-0.pcap plotmeTP/TP-SRGf2-6.plotme plotmeTPP/TP-SRGf2-6.plotme S-SRGf2-6 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.6.0.1 --destPort 50006 N-61-0.pcap plotmeTP/TP-SRGf2-7.plotme plotmeTPP/TP-SRGf2-7.plotme S-SRGf2-7 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.6.0.1 --destPort 50007 N-61-0.pcap plotmeTP/TP-SRGf2-8.plotme plotmeTPP/TP-SRGf2-8.plotme S-SRGf2-8 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.6.0.1 --destPort 50008 N-61-0.pcap plotmeTP/TP-SRGf2-9.plotme plotmeTPP/TP-SRGf2-9.plotme S-SRGf2-9 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.6.0.1 --destPort 50009 N-61-0.pcap plotmeTP/TP-SRGf2-10.plotme plotmeTPP/TP-SRGf2-10.plotme S-SRGf2-10 | tee -a overallTP.txt

#for RGf3
python tp-new.py --startTime=3 --destIp 10.15.0.1 --destPort 50000 N-63-0.pcap plotmeTP/TP-SRGf3-1.plotme plotmeTPP/TP-SRGf3-1.plotme S-SRGf3-1 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.15.0.1 --destPort 50001 N-63-0.pcap plotmeTP/TP-SRGf3-2.plotme plotmeTPP/TP-SRGf3-2.plotme S-SRGf3-2 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.15.0.1 --destPort 50002 N-63-0.pcap plotmeTP/TP-SRGf3-3.plotme plotmeTPP/TP-SRGf3-3.plotme S-SRGf3-3 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.15.0.1 --destPort 50003 N-63-0.pcap plotmeTP/TP-SRGf3-4.plotme plotmeTPP/TP-SRGf3-4.plotme S-SRGf3-4 | tee -a overallTP.txt
python tp-new.py --startTime=3 --destIp 10.15.0.1 --destPort 50004 N-63-0.pcap plotmeTP/TP-SRGf3-5.plotme plotmeTPP/TP-SRGf3-5.plotme S-SRGf3-5 | tee -a overallTP.txt

#for RB
python tp-new.py --startTime=3 --destIp 10.19.0.1   N-65-0.pcap plotmeTP/TP-SRB-1.plotme plotmeTPP/TP-SRB-1.plotme S-SRB-1 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.19.0.1 --destPort 50001 N-65-0.pcap plotmeTP/TP-SRB-2.plotme plotmeTPP/TP-SRB-2.plotme S-SRB-2 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.19.0.1 --destPort 50002 N-65-0.pcap plotmeTP/TP-SRB-3.plotme plotmeTPP/TP-SRB-3.plotme S-SRB-3 | tee -a overallTP.txt

#for RC
python tp-new.py --startTime=3 --destIp 10.16.0.1   N-67-0.pcap plotmeTP/TP-SRC-1.plotme plotmeTPP/TP-SRC-1.plotme S-SRC-1 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.16.0.1 --destPort 50001 N-67-0.pcap plotmeTP/TP-SRC-2.plotme plotmeTPP/TP-SRC-2.plotme S-SRC-2 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.16.0.1 --destPort 50002 N-67-0.pcap plotmeTP/TP-SRC-3.plotme plotmeTPP/TP-SRC-3.plotme S-SRC-3 | tee -a overallTP.txt

#for RD
python tp-new.py --startTime=3 --destIp 10.11.0.1   N-69-0.pcap plotmeTP/TP-SRD-1.plotme plotmeTPP/TP-SRD-1.plotme S-SRD-1 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.11.0.1 --destPort 50001 N-69-0.pcap plotmeTP/TP-SRD-2.plotme plotmeTPP/TP-SRD-2.plotme S-SRD-2 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.11.0.1 --destPort 50002 N-69-0.pcap plotmeTP/TP-SRD-3.plotme plotmeTPP/TP-SRD-3.plotme S-SRD-3 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.11.0.1 --destPort 50003 N-69-0.pcap plotmeTP/TP-SRD-4.plotme plotmeTPP/TP-SRD-4.plotme S-SRD-4 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.11.0.1 --destPort 50004 N-69-0.pcap plotmeTP/TP-SRD-5.plotme plotmeTPP/TP-SRD-5.plotme S-SRD-5 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.11.0.1 --destPort 50005 N-69-0.pcap plotmeTP/TP-SRD-6.plotme plotmeTPP/TP-SRD-6.plotme S-SRD-6 | tee -a overallTP.txt

#for RE
python tp-new.py --startTime=3 --destIp 10.20.0.1   N-71-0.pcap plotmeTP/TP-SRE-1.plotme plotmeTPP/TP-SRE-1.plotme S-SRE-1 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.20.0.1 --destPort 50001 N-71-0.pcap plotmeTP/TP-SRE-2.plotme plotmeTPP/TP-SRE-2.plotme S-SRE-2 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.20.0.1 --destPort 50002 N-71-0.pcap plotmeTP/TP-SRE-3.plotme plotmeTPP/TP-SRE-3.plotme S-SRE-3 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.20.0.1 --destPort 50003 N-71-0.pcap plotmeTP/TP-SRE-4.plotme plotmeTPP/TP-SRE-4.plotme S-SRE-4 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.20.0.1 --destPort 50004 N-71-0.pcap plotmeTP/TP-SRE-5.plotme plotmeTPP/TP-SRE-5.plotme S-SRE-5 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.20.0.1 --destPort 50005 N-71-0.pcap plotmeTP/TP-SRE-6.plotme plotmeTPP/TP-SRE-6.plotme S-SRE-6 | tee -a overallTP.txt

#for RF
python tp-new.py --startTime=3 --destIp 10.13.0.1   N-73-0.pcap plotmeTP/TP-SRF-1.plotme plotmeTPP/TP-SRF-1.plotme S-SRF-1 | tee -a overallTP.txt
#python tp-new.py --startTime=3 --destIp 10.13.0.1 --destPort 50001 N-73-0.pcap plotmeTP/TP-SRF-2.plotme plotmeTPP/TP-SRF-2.plotme S-SRF-2 | tee -a overallTP.txt



printf "\nRouter Throughput\n\n" | tee -a overallTP.txt

#G0--G1
python tp-new.py --startTime=3 N-0-0.pcap plotmeRouterTP/TP-T1.plotme plotmeRouterTPP/TP-T1.plotme T1 | tee -a overallTP.txt

#G1--G2
python tp-new.py --startTime=3 N-1-1.pcap plotmeRouterTP/TP-T2.plotme plotmeRouterTPP/TP-T2.plotme T2 | tee -a overallTP.txt

#G2--G3
python tp-new.py --startTime=3 N-2-1.pcap plotmeRouterTP/TP-T3.plotme plotmeRouterTPP/TP-T3.plotme T3 | tee -a overallTP.txt

#G2--R2
python tp-new.py --startTime=3 N-2-12.pcap plotmeRouterTP/TP-T3-R2-1.plotme plotmeRouterTPP/TP-T3-R2-1.plotme T3-R2-1 | tee -a overallTP.txt
python tp-new.py --startTime=3 N-2-13.pcap plotmeRouterTP/TP-T3-R2-2.plotme plotmeRouterTPP/TP-T3-R2-2.plotme T3-R2-2 | tee -a overallTP.txt
python tp-new.py --startTime=3 N-2-14.pcap plotmeRouterTP/TP-T3-R2-3.plotme plotmeRouterTPP/TP-T3-R2-3.plotme T3-R2-3 | tee -a overallTP.txt
python tp-new.py --startTime=3 N-2-15.pcap plotmeRouterTP/TP-T3-R2-4.plotme plotmeRouterTPP/TP-T3-R2-4.plotme T3-R2-4 | tee -a overallTP.txt
python tp-new.py --startTime=3 N-2-16.pcap plotmeRouterTP/TP-T3-R2-5.plotme plotmeRouterTPP/TP-T3-R2-5.plotme T3-R2-5 | tee -a overallTP.txt
python tp-new.py --startTime=3 N-2-17.pcap plotmeRouterTP/TP-T3-R2-6.plotme plotmeRouterTPP/TP-T3-R2-6.plotme T3-R2-6 | tee -a overallTP.txt
python tp-new.py --startTime=3 N-2-18.pcap plotmeRouterTP/TP-T3-R2-7.plotme plotmeRouterTPP/TP-T3-R2-7.plotme T3-R2-7 | tee -a overallTP.txt
python tp-new.py --startTime=3 N-2-19.pcap plotmeRouterTP/TP-T3-R2-8.plotme plotmeRouterTPP/TP-T3-R2-8.plotme T3-R2-8 | tee -a overallTP.txt
python tp-new.py --startTime=3 N-2-20.pcap plotmeRouterTP/TP-T3-R2-9.plotme plotmeRouterTPP/TP-T3-R2-9.plotme T3-R2-9 | tee -a overallTP.txt
python tp-new.py --startTime=3 N-2-21.pcap plotmeRouterTP/TP-T3-R2-10.plotme plotmeRouterTPP/TP-T3-R2-10.plotme T3-R2-10 | tee -a overallTP.txt

#G3--R1
python tp-new.py --startTime=3 N-3-3.pcap plotmeRouterTP/TP-T4-R1.plotme plotmeRouterTPP/TP-T4-R1.plotme T4-R1 | tee -a overallTP.txt

#G3--R3
python tp-new.py --startTime=3 N-3-2.pcap plotmeRouterTP/TP-T4-R3.plotme plotmeRouterTPP/TP-T4-R3.plotme T4-R3 | tee -a overallTP.txt

#G3--G4
python tp-new.py --startTime=3 N-3-1.pcap plotmeRouterTP/TP-T4.plotme plotmeRouterTPP/TP-T4.plotme T4 | tee -a overallTP.txt

#G4--R2
python tp-new.py --startTime=3 N-4-2.pcap plotmeRouterTP/TP-T5-R2.plotme plotmeRouterTPP/TP-T5-R2.plotme T5-R2 | tee -a overallTP.txt

#G4--G5
python tp-new.py --startTime=3 N-4-1.pcap plotmeRouterTP/TP-T5.plotme plotmeRouterTPP/TP-T5.plotme T5 | tee -a overallTP.txt

#G5--RD
python tp-new.py --startTime=3 N-5-3.pcap plotmeRouterTP/TP-T6-RD.plotme plotmeRouterTPP/TP-T6-RD.plotme T6-RD | tee -a overallTP.txt

#G5--G6
python tp-new.py --startTime=3 N-5-1.pcap plotmeRouterTP/TP-T6.plotme plotmeRouterTPP/TP-T6.plotme T6 | tee -a overallTP.txt

#G6--RF
python tp-new.py --startTime=3 N-6-2.pcap plotmeRouterTP/TP-T7-RF.plotme plotmeRouterTPP/TP-T7-RF.plotme T7-RF | tee -a overallTP.txt

#G6--G7
python tp-new.py --startTime=3 N-6-1.pcap plotmeRouterTP/TP-T7.plotme plotmeRouterTPP/TP-T7.plotme T7 | tee -a overallTP.txt


#G7--R3
python tp-new.py --startTime=3 N-7-2.pcap plotmeRouterTP/TP-T8-R3.plotme plotmeRouterTPP/TP-T8-R3.plotme T8-R3 | tee -a overallTP.txt


#G7--RC
python tp-new.py --startTime=3 N-7-3.pcap plotmeRouterTP/TP-T8-RC.plotme plotmeRouterTPP/TP-T8-RC.plotme T8-RC | tee -a overallTP.txt

#G7--G8
python tp-new.py --startTime=3 N-7-1.pcap plotmeRouterTP/TP-T8.plotme plotmeRouterTPP/TP-T8.plotme T8 | tee -a overallTP.txt


#G8--R1
python tp-new.py --startTime=3 N-8-1.pcap plotmeRouterTP/TP-T9-R1.plotme plotmeRouterTPP/TP-T9-R1.plotme T9-R1 | tee -a overallTP.txt


#G8--RE
python tp-new.py --startTime=3 N-8-3.pcap plotmeRouterTP/TP-T9-RE.plotme plotmeRouterTPP/TP-T9-RE.plotme T9-RE | tee -a overallTP.txt

#G8--RB
python tp-new.py --startTime=3 N-8-2.pcap plotmeRouterTP/TP-T9-RB.plotme plotmeRouterTPP/TP-T9-RB.plotme T9-RB | tee -a overallTP.txt


cp gnuplotscriptRouters plotmeRouterTP
cd plotmeRouterTP
gnuplot gnuplotscriptRouters
gnuplot gnuplotscriptRouters_Indiv
cd ..

#cp gnuplotscriptRoutersPackets plotmeRouterTPP
#cd plotmeRouterTPP
#gnuplot gnuplotscriptRoutersPackets
#cd ..


mkdir -p ../Graphs
cp plotmeRouterTP/*.png ../Graphs/
#cp plotmeRouterTPP/*.png ../Graphs/
#cp plotmeTP/*.png ../Graphs/
#cp plotmeTPP/*.png ../Graphs/
cp overallTP.txt ../Graphs/

#Cwnd plot
#cp gnuplotscriptQ ../
#cp gnuplotscriptCwnd ../cwndTraces/

#cd ../queueTraces/
#echo "Queue 0" > QueueStatsAfter3sec.txt
#drops=`awk '{if ($1 >= 3) print $0}' drop-0.plotme | wc -l`
#marks=`awk '{if ($1 >= 3) print $0}' mark-0.plotme | wc -l`
#echo -e  "$drops Drops, $marks Marks\n" >> QueueStatsAfter3sec.txt

#echo "Queue 1" >> QueueStatsAfter3sec.txt
#drops=`awk '{if ($1 >= 3) print $0}' drop-1.plotme | wc -l`
#marks=`awk '{if ($1 >= 3) print $0}' mark-1.plotme | wc -l`
#echo -e  "$drops Drops, $marks Marks\n" >> QueueStatsAfter3sec.txt

#echo "Queue 2" >> QueueStatsAfter3sec.txt
#drops=`awk '{if ($1 >= 3) print $0}' drop-2.plotme | wc -l`
#marks=`awk '{if ($1 >= 3) print $0}' mark-2.plotme | wc -l`
#echo -e  "$drops Drops, $marks Marks\n" >> QueueStatsAfter3sec.txt


cp ../queueStats.txt ../Graphs/


cd ../queueTraces
gnuplot gnuplotscriptQ
cp queue*.png ../Graphs/
cd ..
#cp queue-*.png Graphs/
#cp queueStats.txt Graphs/

cd cwndTraces
gnuplot gnuplotscriptCwnd
cp Cwnd*.png ../Graphs/
