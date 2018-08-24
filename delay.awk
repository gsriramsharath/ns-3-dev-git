BEGIN{
}

{
         stime = $13
         rtime = $16
         source=$2
         destination=$7
         sport=$5
         dport=$10
	 if(source==sourceip && destination==destip && sport==sourceport && dport=destport){
             printf("%f %f\n",rtime/1000000,(rtime-stime)/1000)>filename;
         }
}
END{
}
