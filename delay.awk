BEGIN{
	##getting required flow
        printf("Enter source IP : ")
        getline sourceip < "-"
	printf("Enter source port no : ")
	getline sourceport < "-"
	printf("Enter destination IP : ")
        getline destip < "-"
	printf("Enter destination port no : ")
        getline destport < "-"
		
	#printf("Enter ")
#	printf("%s\n",destip)
}

{
         stime = $13
         rtime = $16
         source=$2
         destination=$7
         sport=$5
         dport=$10
	 filename= "./results/gfc-dctcp/20-08-2018-07-06-10/delay/flow-S2-20.plotme"
	# printf(source);
	 if(source==sourceip && destination==destip && sport==sourceport && dport=destport){
             printf("%f %f\n",rtime/1000000,(rtime-stime)/1000)>filename;
         }
}
END{
}
