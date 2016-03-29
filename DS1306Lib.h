
#ifndef	DS1306Lib_h
#define	DS1306Lib_h



#define WRITE             0x80
#define ADDRESS_CONTROL_WP_R 0x0F //writeprotect
#define BIT_WP 0x00 //writeprotect
#define BIT_1HZ 0x04
#define BIT_AIE1 0x02
#define BIT_AIE0 0x01 //INT0 alarm
#define ADDRESS_SEC_R     0x00
#define ADDRESS_MIN_R     0x01
#define ADDRESS_HH_R     0x02
#define ADDRESS_DATE_R     0x04
#define ADDRESS_MONTH_R     0x05
#define ADDRESS_YEAR_R     0x06
#define ADDRESS_AL_SEC_R     0x07
#define ADDRESS_AL_MIN_R     0x08
#define ADDRESS_AL_HH_R     0x09
#define ADDRESS_AL_DAY_R     0x0A


#define dec_bcd(dec) (((dec / 10) << 4) + (dec % 10))
#define bcd_dec(bcd) (((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F))

class DS1306
{
	public:
		DS1306(byte CE);
		void init();
		void setSeconds(byte val);
		void setMinutes(byte val);
		void setHours(boolean is12,boolean isPM, byte val);
		void setDate(byte val);
		void setMonth(byte val);
		void setYear(byte val);

		void enableAlarm(boolean en);
		void setALSeconds(byte val);
		void setALMinutes(byte val);
		void setALHours(boolean is12,boolean isPM, byte val);
		void setALDay(byte val);

		// String showTime(char);
		// String showDate(char);
		// String showAlarm(char);
		void getTime(byte*);
		void getDate(byte*);
		void getAlarm(byte*);

	protected:
		//String printDigits(byte digits);
		byte readAddress(byte address);
		void writeAddress(byte address, byte data);
		

		byte PIN_DS_CE;

};

#endif