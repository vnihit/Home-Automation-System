//Home automation with bluetooth and GSM...PIR+MQ6
//I/O definition
#define F_CPU 1000000UL
#include<avr/io.h>
#include<util/delay.h>
#include"LCD.C"
#include"gsm.c"

#define mode ((PINB&(1<<PB0)))
#define pir  ((PINB&(1<<PB6)))
#define lpg  ((PINB&(1<<PB7)))

void d1opr(int i)
{
if(i)
{
PORTB|=1<<PB1;
print(0xc0,PSTR(" ON "));
}

else
{
PORTB&=~(1<<PB1);
print(0xc0,PSTR("OFF "));
}
}


void d2opr(int i)
{
//Device status on LCD
if(i)
{
PORTB|=1<<PB2;
print(0xc4,PSTR(" ON "));
}
else
{
PORTB&=~(1<<PB2);
print(0xc4,PSTR("OFF "));
}

}

void d3opr(int i)
{
if(i)
{
PORTB|=1<<PB3;
print(0xc8,PSTR(" ON "));
}

else
{
PORTB&=~(1<<PB3);
print(0xc8,PSTR("OFF "));
}
}

void d4opr(int i)
{

if(i)
{
PORTB|=1<<PB4;
print(0xcc,PSTR(" ON "));
}

else
{
PORTB&=~(1<<PB4);
print(0xcc,PSTR("OFF "));
}

}

void main(void)
{
int d1=0,d2=0,d3=0,d4=0;
int stpir=0, stlpg=0;
char fg=0;

//////////////////PORT CONFIGURATION//////////////
DDRC=0xff;
DDRB=0x3e;
PORTB|=(1<<PB0)|(1<<PB6)|(1<<PB7);
DDRD|=1<<PD2;
PORTD|=(1<<PD6)|(1<<PD7)|(1<<PD0)|(1<<PD1);

lcmd(0x02);
lcmd(0x28);
lcmd(0x0e);
lcmd(0x01);
UART_init();

///////////////// GSM CONFIGURATION /////////////////////

print(0x80,PSTR("CONFIG GSM....."));
print(0xc0,PSTR("AT:"));
/////////////// AT ///////////////////
AT:
fg=cmdrpl("AT\r\n","OK");
if(fg)
print(0xc3,PSTR("OK"));
else
{
print(0xc3,PSTR("ERROR"));
goto AT;
}

gsm;
rtxstr(PSTR("ATE0\r\n"));
_delay_ms(1000);
/////////////SIM CONFIGURATION/////////////////

lcmd(0x01);
print(0x80,PSTR("CHECKING SIM..."));

fg=cmdrpl("AT+CPIN?\r\n","READY");
if(fg)
print(0xc3,PSTR("PRESENT"));
else
{
print(0xc0,PSTR("NOT PRESENT!"));
}
_delay_ms(1000);
/////////////NETWORK CONFIGURATION////////////////////
lcmd(0x01);
print(0x80,PSTR("CHECK NETWORK..."));

fg=cmdrpl("AT+CREG?\r\n","+CREG: 0,1");
if(fg)
print(0xc3,PSTR("CONNECTED!"));
else
{
print(0xc0,PSTR("NOT CONNECTED!"));
}
_delay_ms(1000);

//////////////configuration///////////////
lcmd(0x01);
print(0x80,PSTR("CONFIG1 SIM...."));

fg=cmdrpl("AT+CMGF=1\r\n","OK");
if(fg)
print(0xc3,PSTR("DONE!"));
else
{
print(0xc3,PSTR("FAILED!"));
}

_delay_ms(500);
lcmd(0x01);
print(0x80,PSTR("CONFIG2 SIM...."));

fg=cmdrpl("AT+CMGD=1\r\n","OK");
if(fg)
print(0xc3,PSTR("DONE!"));
else
{
print(0xc3,PSTR("FAILED!"));
}
_delay_ms(500);

///////////////////////////PRIMARY/DEFAULT DISPLAY//////////////////////////////

if(mode)
{

lcmd(0x01);
print(0x80,PSTR("  VOICE MODE    "));
print(0xc0,PSTR("SPEAK UR COMMAND"));
_delay_ms(500);
lcmd(0x01);
print(0x80,PSTR(" D1  D2  D3  D4 "));

blt;
st=0;
bclr();


while(1)
{
	d1opr(d1);
	d2opr(d2);
	d3opr(d3);
	d4opr(d4);

    blt;
	if(st==1)
	{
	lcmd(0x01);
    bprint(0x80,buff);
	_delay_ms(1000);
	
	if(match(buff,"*light on#"))
	d1=1;

	else if(match(buff,"*light off#"))
	d1=0;
	 
     else if(match(buff,"*light of#"))
	d1=0;

	else if(match(buff,"*fan on#"))
	d2=1;

	else if(match(buff,"*fan off#"))
	d2=0;

	else if(match(buff,"*fan of#"))
	d2=0;

	else if(match(buff,"*motor on#"))
	d3=1;

	else if(match(buff,"*motor off#"))
	d3=0;

	else if(match(buff,"*motor of#"))
	d3=0;

	else if(match(buff,"*heater on#"))
	d4=1;

	else if(match(buff,"*heater off#"))
	d4=0;

	else if(match(buff,"*heater of#"))
	d4=0;

	st=0;
	bclr();
    print(0x80,PSTR(" D1  D2  D3  D4 "));

	}

if( (pir)&&(stpir==0) )
{
lcmd(0x01);
print(0x80,PSTR("SENDING MSG PIR!"));
fg=sendmsg("SOMEBODY ENTERD IN YOUR AREA");

if(fg)
print(0xc3,PSTR("SUCCESS!"));
else
print(0xc3,PSTR("FAILED!!"));

stpir=1;

_delay_ms(500);
lcmd(0x01);
print(0x80,PSTR(" D1  D2  D3  D4 "));

}

if( (!pir)&&(stpir==1) )
stpir=0;


if( (lpg)&&(stlpg==0) )
{
lcmd(0x01);
print(0x80,PSTR("SENDING MSG LPG!"));
fg=sendmsg("GAS LEAKAGE DETECTED");

if(fg)
print(0xc3,PSTR("SUCCESS!"));
else
print(0xc3,PSTR("FAILED!!"));

stlpg=1;
_delay_ms(500);
lcmd(0x01);
print(0x80,PSTR(" D1  D2  D3  D4 "));
}

if( (!lpg)&&(stlpg==1) )
stlpg=0;

}
}
/////////##############///DEFINING SMS MODE////////////////////////////////////////

else
{
print(0x80,PSTR("   SMS MODE    "));
print(0xc0,PSTR("SMS UR COMMANDS"));
_delay_ms(1000);
lcmd(0x01);
print(0x80,PSTR(" D1  D2  D3  D4 "));
while(1)
{
d1opr(d1);   //DEVICE 1 OPERATION
d2opr(d2);   //DEVICE 2 OPERATION
d3opr(d3);   //DEVICE 3 OPERATION
d4opr(d4);   //DEVICE 4 OPERATION


    gsm;
	if(st==1)
	{
    fg=match(buff,"+CMTI:");
	if(fg)
	{   print(0x8f,PSTR("*")); 
        fg=rdmsg();
        if(fg)
		{
		 print(0x8f,PSTR("#")); 

		ldmsg();

        if(match(buff,"MOB"))
	   svmob();
	   
		else if(match(buff,"light on"))
		d1=1;

		else if(match(buff,"light off"))
		d1=0;

		else if(match(buff,"fan on"))
		d2=1;

		else if(match(buff,"fan off"))
		d2=0;

		else if(match(buff,"motor on"))
		d3=1;

		else if(match(buff,"motor off"))
		d3=0;

		else if(match(buff,"heater on"))
		d4=1;

		else if(match(buff,"heater off"))
		d4=0;
		}
	}
   
    print(0x8f,PSTR(" ")); 
	st=0;
	bclr();
	}



if( (pir)&&(stpir==0) )
{
lcmd(0x01);
print(0x80,PSTR("SENDING MSG PIR!"));
fg=sendmsg("SOMEBODY ENTERD IN YOUR AREA");

if(fg)
print(0xc3,PSTR("SUCCESS!"));
else
print(0xc3,PSTR("FAILED!!"));

stpir=1;

_delay_ms(500);
lcmd(0x01);
print(0x80,PSTR(" D1  D2  D3  D4 "));

}

if( (!pir)&&(stpir==1) )
stpir=0;


if( (lpg)&&(stlpg==0) )
{
lcmd(0x01);
print(0x80,PSTR("SENDING MSG LPG!"));
fg=sendmsg("GAS LEAKAGE DETECTED");

if(fg)
print(0xc3,PSTR("SUCCESS!"));
else
print(0xc3,PSTR("FAILED!!"));

stlpg=1;
_delay_ms(500);
lcmd(0x01);
print(0x80,PSTR(" D1  D2  D3  D4 "));
}

if( (!lpg)&&(stlpg==1) )
stlpg=0;

}//while
}// else
}
