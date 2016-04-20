int to_display(unsigned char var1, unsigned char var2, unsigned char dec_point)
{
	if(dec_point == 1) dpy_trm_s01__7seq_write_number( ( 10*var1 + var2 ), 1 );
	else dpy_trm_s01__7seq_write_number( ( 100*var1 + var2 ), dec_point );
	
	
}