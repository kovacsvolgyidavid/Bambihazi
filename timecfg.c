int time(unsigned int* date, unsigned char* hour, unsigned char* ampm, unsigned char* min, unsigned char* sec, unsigned char* tenth)
{
	if(*tenth == 10){
		*sec++;
		*tenth=0;
	}
	
	
}