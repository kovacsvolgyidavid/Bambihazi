unsigned char time() /*unsigned char* month, unsigned char* day, unsigned char* hour, unsigned char* ampm, unsigned char* min, unsigned char* sec, unsigned char* tenth*/
{
	unsigned char dom [12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	unsigned char flash = 0;
	
	if(*tenth == 10){
		
		*sec++;
		*tenth = 0;
		
	}
	
	if(*sec == 60){
		
		*min++;
		*sec = 0;
		
		if( flash ) flash = 0;
		else flash++;
		
	}
	
	if(*min == 60){
		
		*hour++;
		*min = 0;
		
	}
	
	if(*hour == 13){
		
		if(*ampm){
			
			*day++;
			*ampm = 0;
			
		}
		
		else ampm++;
		
		*hour = 1;
	}
	
	if(*day > dom [ *month - 1 ]){
		
		*month++;
		*day = 1;
		
	}
	
	if(*month > 12) month = 1;
	
	return flash;
}