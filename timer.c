#include "mcu_avr_atmega128_api.h" 	// MCU API   
#include "dpy_trm_s01.h"			// DPY API
/*********************************Constant*************************************/
#define MOD_CLOCK 1
#define MOD_STOPPER 2
#define MOD_DATE 3
#define STOPPER_ON 1
#define STOPPER_OFF 0
#define SETTINGS_ON_1 1
#define SETTINGS_ON_2 2
#define SETTINGS_OFF 0
#define BUTTON_ON 0
#define BUTTON_OFF 1
#define BUTTON_TIME 100
/******************************Valtozok**************************************/
int				time;
volatile unsigned char	tenth=0;
unsigned char	sec=0;
unsigned int	cnt=0;
unsigned char	min=0;
unsigned char	time_hours=0;
unsigned char	time_days=1;
unsigned char	time_month=1;
unsigned char	stopper_tenth=0;
unsigned char	stopper_sec=0;
unsigned char	stopper_min=0;
volatile unsigned char	disp_flag=0;
unsigned char	err;
unsigned char	but1=BUTTON_OFF;
unsigned char	but2=BUTTON_OFF;
unsigned char	but3=BUTTON_OFF;
unsigned char	but1_buffer=0;
unsigned char	but2_buffer=0;
unsigned char	but3_buffer=0;
unsigned char	mod=MOD_CLOCK;
unsigned char	stopper_state=STOPPER_OFF;
unsigned char 	settings_state=SETTINGS_OFF;
unsigned char * display_1;
unsigned char * display_2;
unsigned char	flash=0;
unsigned char	ampm=0;

/********  function prototypes  ***************************/
void Timer0_Init(void);
void set_stopper_state(void);
ISR(SIG_OVERFLOW0);
int main(void);

/********  Timer0 overflow IT Service Routine  ***************************/
ISR(SIG_OUTPUT_COMPARE0) // Timer0 overflow
{	
	cnt++;
	if(cnt==100){
		tenth++;
   		disp_flag=1;
		cnt=0;
		if(stopper_state)stopper_tenth++;
	}
}
/*****************************   Functions  *****************************/

unsigned char set_time(void) /*unsigned char* month, unsigned char* day, unsigned char* hour, unsigned char* ampm, unsigned char* min, unsigned char* sec, unsigned char* tenth*/
{
	unsigned char dom [12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	/*unsigned char flash = 0;*/
	
	
	if(tenth == 10){
		
		sec++;
		tenth = 0;
		
	}
	
	if(sec == 60){
		
		min++;
		sec = 0;
		
		if( flash ) flash = 0;
		else flash++;
		
	}
	
	if(min == 60){
		
		time_hours++;
		min = 0;
		
	}
	
	if(time_hours == 13){
		
		if(ampm){
			
			time_days++;
			ampm = 0;
			
		}
		
		else ampm++;
		
		time_hours = 1;
	}
	
	if(time_days > dom [ time_month - 1 ]){
		
		time_month++;
		time_days = 1;
		
	}
	
	if(time_month > 12) time_month = 1;
	
	return flash;
}


/*****************************  Display  *************************************/
void to_display(unsigned char var1, unsigned char var2, unsigned char dec_point)
{
	if(dec_point == 1) dpy_trm_s01__7seq_write_number( ( 10*var1 + var2 ), 1 );
	else dpy_trm_s01__7seq_write_number( ( 100*var1 + var2 ), dec_point );
}

void display(void)
{	unsigned char fraction=0;
	switch(mod){
	case MOD_CLOCK:
		display_1=&time_hours;
		display_2=&min;
		fraction=2;
	break;
	case MOD_DATE:
		display_1=&time_month;
		display_2=&time_days;
		fraction=2;
	break;
	case MOD_STOPPER:
		if(stopper_min){
		display_1=&stopper_sec;
		display_2=&stopper_tenth;
		fraction=1;
		}
		else{
		display_1=&stopper_min;
		display_2=&stopper_sec;
		fraction=2;
		
		}
}
to_display(*display_1, *display_2, fraction);
}
/******************************************  State modification  ****************************************/
void set_mod(void){
	if(mod==MOD_DATE){
		mod=MOD_CLOCK;
		}
	else{
		mod++;
		}
}
void set_settings(void){
	if(settings_state==SETTINGS_ON_2){
		settings_state=SETTINGS_OFF;
		}
	else{
		settings_state++;
		}
}
void set_stopper_state(void){
	stopper_state=!stopper_state;
	
}

void stopper_reset(void){
	stopper_min=0;
	stopper_sec=0;
	stopper_tenth=0;
}
/*************************************   Button pushed   ********************************************************/
void button1_pushed(void)
{
if(settings_state){
	switch(mod){
		case MOD_CLOCK:
			if(settings_state==SETTINGS_ON_1){
				time_hours--;
			}
			if(settings_state==SETTINGS_ON_2){
				min--;
			}
			break;
		case MOD_DATE:
			if(settings_state==SETTINGS_ON_1){
				time_month--;
			}
			if(settings_state==SETTINGS_ON_2){
				time_days--;
			}
			break;
		case MOD_STOPPER:
			settings_state=SETTINGS_OFF;
			set_mod();
	}
}
else {
	set_mod();
}
}

void button2_pushed(void)
{
switch(mod){
		case MOD_CLOCK:
			if(settings_state==SETTINGS_ON_1){
				time_hours++;
			}
			if(settings_state==SETTINGS_ON_2){
				min++;
			}
			break;
		case MOD_DATE:
			if(settings_state==SETTINGS_ON_1){
				time_month++;
			}
			if(settings_state==SETTINGS_ON_2){
				time_days++;
			}
			break;
		case MOD_STOPPER:
			set_stopper_state();
	}
}

void button3_pushed(void)
{
switch(mod){
	case MOD_CLOCK:
		set_settings();
	break;
	case MOD_DATE:
		set_settings();
	break;
	case MOD_STOPPER:
		stopper_reset();
		set_stopper_state();
		
}
}

void set_buttons(void)
{	
	but1=DPY_TRM_S01__BUTTON_1_GET_STATE();
	if(but1==BUTTON_ON){
		but1_buffer++;	
	}
	else{
		but1_buffer=0;
	}
	but2=DPY_TRM_S01__BUTTON_2_GET_STATE();
	if(but2==BUTTON_ON){
		but2_buffer++;	
	}
	else{
		but2_buffer=0;
	}
	but3=DPY_TRM_S01__BUTTON_3_GET_STATE();
	if(but3==BUTTON_ON){
		but3_buffer++;	
	}
	else{
		but3_buffer=0;
	}
	if (butbuffer1>BUTTON_TIME) button1_pushed();
	if (butbuffer2>BUTTON_TIME) button2_pushed();
	if (butbuffer3>BUTTON_TIME) button3_pushed();
	
	
}
/*****************  stopper  *************************/
void stopper_time(void)
{
	if(stopper_tenth==10){
		stopper_sec++;
	}
	if(stopper_sec==60){
		stopper_min++;
	}
}
/********  main program  ***************************/
int main (void)
{
   dpy_trm_s01__Init();		// Initialize the DPY dysplay card
   Timer0_Init();			// Initialize timer0
   dpy_trm_s01__Temp_Init(TMP75_JUMPER_OFF,TMP75_JUMPER_OFF,TMP75_JUMPER_OFF);	
							/* Initialisation of temp. sensor */ 
   SYS_LED_DIR_OUTPUT();	// Set the pin driving the system led to output
   SYS_LED_ON();			// Switch on system led
   sei();					// enable interrupts

   while(1)
   {
   
    
	set_buttons();
	flash=set_time();
	stopper_time();
	if (disp_flag==1){
	display(); 
	}
	
	
	
   }

}


/********  Timer0 initialisation  ***************************/
void Timer0_Init(void)
{
   TCCR0=0x0B;			// Set TIMER0 prescaler to CLK/32
   TCNT0=0;				// Set the counter initial value
   OCR0=0xF9;
   TIMSK=_BV(OCIE0);	// Enable TIMER0 overflow interrupt                 
}
