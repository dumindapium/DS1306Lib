// DS max clocksped is 2MHz(2000000)
// Clock polarity 1or0. 1 is selected (SPI_MODE2)
// Use MSB first (MSBFIRST)


//note - fix the crystal. attach an LED to 1Hz pin to get an output by control register. chipenable pin bug fixed.

#include <SPI.h>
#include "DS1306Lib.h"

//MOSI: pin 11    maps DS-SDI pin 12
//MISO: pin 12    maps DS-SDO pin 13
//SCK: pin 13    maps DS-SCLK pin 11

	DS1306::DS1306(byte CE){
		PIN_DS_CE = CE;
		
	}

	// String DS1306::printDigits(byte digits)
	// {
	//   String str;
	//   // utility function for digital clock display: prints preceding colon and leading 0
	//   if(digits < 10)
	//     str = "0" + String(digits);
	//   else
	//     str = String(digits);
	 
	//   return str;
	// }

	byte DS1306::readAddress(byte address) {
		byte result = 0;
		// chip select 1
		digitalWrite(PIN_DS_CE, HIGH);
		SPI.transfer(address);
		result = SPI.transfer(0x00);

		digitalWrite(PIN_DS_CE, LOW);  // chip select 0

		return (result);
	}

	void DS1306::writeAddress(byte address, byte data) {
		//begin - write enable
		
		// chip select 1
		digitalWrite(PIN_DS_CE, HIGH);
		SPI.transfer(ADDRESS_CONTROL_WP_R | WRITE);
		SPI.transfer(BIT_WP);
		digitalWrite(PIN_DS_CE, LOW);   // chip select 0
		//end - write enable
		delay(1);
		//begin write data
		// chip select 1
		digitalWrite(PIN_DS_CE, HIGH);

		SPI.transfer(address);
		SPI.transfer(data);

		digitalWrite(PIN_DS_CE, LOW);   // chip select 0
		//end write data
		delay(1);
		//begin - write disable
		byte currentControl = readAddress(ADDRESS_CONTROL_WP_R);
		
		//Serial.print("Control bits");Serial.println(String(currentControl,BIN));   // for debugging
		
		byte valueWPDisable = 0x40 ^ currentControl;
		// chip select 1
		digitalWrite(PIN_DS_CE, HIGH);
		SPI.transfer(ADDRESS_CONTROL_WP_R | WRITE);
		SPI.transfer(valueWPDisable);
		digitalWrite(PIN_DS_CE, LOW);   // chip select 0
		//end - write disable
		delay(1);
	}




	void DS1306::init() {
		pinMode(PIN_DS_CE, OUTPUT);     //set chip select output
		digitalWrite(PIN_DS_CE, LOW);   // chip select 0

		SPI.begin();
		SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE1);

		//writeAddress(ADDRESS_SEC_R | WRITE, 0);
		//writeAddress(ADDRESS_MIN_R | WRITE, 0);
	}

	void DS1306::setSeconds(byte val){
		val = dec_bcd(val);
		//Serial.print("Set sec-");Serial.println(val);   // for debugging
		writeAddress(ADDRESS_SEC_R | WRITE, val);
	}

	void DS1306::setMinutes(byte val){
		val = dec_bcd(val);
		//Serial.print("Set min-");Serial.println(val);   // for debugging
		writeAddress(ADDRESS_MIN_R | WRITE, val);
	}

	void DS1306::setHours(boolean is12,boolean isPM, byte val){
		val = dec_bcd(val);
		//Serial.print("Set bcd hh-");Serial.println(val);   // for debugging
		val |= (is12 ? 0x40 : 0x00);
		val |= (isPM ? 0x20 : 0x00);
		//Serial.print("Set byte hh-");Serial.println(val);   // for debugging
		writeAddress(ADDRESS_HH_R | WRITE, val);
	}

	void DS1306::setDate(byte val){
		writeAddress(ADDRESS_DATE_R | WRITE, dec_bcd(val));
	}

	void DS1306::setMonth(byte val){
		writeAddress(ADDRESS_MONTH_R | WRITE, dec_bcd(val));
	}

	void DS1306::setYear(byte val){
		writeAddress(ADDRESS_YEAR_R | WRITE, dec_bcd(val));
	}

	void DS1306::enableAlarm(boolean en){
		byte currentControl = readAddress(ADDRESS_CONTROL_WP_R);
		if(en){
			currentControl |= BIT_AIE0;
		}else{
			currentControl &= (~BIT_AIE0);
		}
		
		writeAddress(ADDRESS_CONTROL_WP_R | WRITE, currentControl);
	}

	void DS1306::setALSeconds(byte val){
		writeAddress(ADDRESS_AL_SEC_R | WRITE, dec_bcd(val));
	}

	void DS1306::setALMinutes(byte val){
		writeAddress(ADDRESS_AL_MIN_R | WRITE, dec_bcd(val));
	}

	void DS1306::setALHours(boolean is12, boolean isPM, byte val){
		val = dec_bcd(val);
		val |= (is12 ? 0x40 : 0x00);
		val |= (isPM ? 0x20 : 0x00);
		writeAddress(ADDRESS_AL_HH_R | WRITE, val);
	}

	void DS1306::setALDay(byte val){
		val = dec_bcd(val);
		val |= (val==0 ? 0x81 : 0x00);
		writeAddress(ADDRESS_AL_DAY_R | WRITE, val);
	}


	///////////////////---------------------------- Getters and  Showing functions --------------------------------------------

	// String DS1306::showTime(char delim){
	// 	String strTime="";
	// 	byte hh = readAddress(ADDRESS_HH_R);
	// 	hh = hh & (bitRead(hh,6)==1 ? 0x1F : 0xFF);
	// 	hh = bcd_dec(hh);
	// 	byte mm = bcd_dec(readAddress(ADDRESS_MIN_R));
	// 	strTime = printDigits(hh) + String(delim) + printDigits(mm);
		
	// 	return strTime;
	// }

	// String DS1306::showDate(char delim){
	// 	String strDate="";
	// 	byte date = bcd_dec(readAddress(ADDRESS_DATE_R));
	// 	byte month = bcd_dec(readAddress(ADDRESS_MONTH_R));
	// 	byte year = bcd_dec(readAddress(ADDRESS_YEAR_R));
	// 	strDate = String(year) + String(delim) + printDigits(month) + String(delim) + printDigits(date);
		
	// 	return strDate;
	// }

	// String DS1306::showAlarm(char delim){
	// 	String strALTime="";
	// 	byte hh = bcd_dec(readAddress(ADDRESS_AL_HH_R));
	// 	byte mm = bcd_dec(readAddress(ADDRESS_AL_MIN_R));
	// 	byte day = bcd_dec(readAddress(ADDRESS_AL_DAY_R));
	// 	strALTime = printDigits(hh) + String(delim) + printDigits(mm) + "on "+printDigits(day);
		
	// 	return strALTime;
	// }

	void DS1306::getTime(byte* ret){
		
		byte hh = readAddress(ADDRESS_HH_R);
		boolean pm = (bitRead(hh,6)==1) & (bitRead(hh,5)==1);
		hh = hh & ( bitRead(hh,6)==1 ? 0x1F : 0xFF);
		hh = bcd_dec(hh);
		byte mm = bcd_dec(readAddress(ADDRESS_MIN_R));
		byte ss = bcd_dec(readAddress(ADDRESS_SEC_R));
		
		ret[0] = hh;ret[1] = mm;ret[2] = ss; ret[3]=(byte)(pm?'P':'A');
		//Serial.print(ret[0]);Serial.print("-");Serial.print(ret[1]);  Serial.print("-"); Serial.println(ret[2]);   // for debugging
	}

	void DS1306::getDate(byte* ret){
		ret[0] = bcd_dec(readAddress(ADDRESS_YEAR_R));
		ret[1] = bcd_dec(readAddress(ADDRESS_MONTH_R));
		ret[2] = bcd_dec(readAddress(ADDRESS_DATE_R));
		//Serial.print(ret[0]);Serial.print("-");Serial.print(ret[1]);  Serial.print("-"); Serial.println(ret[2]);   // for debugging	
	}

	void DS1306::getAlarm(byte* ret){
		byte hh = readAddress(ADDRESS_AL_HH_R);
		boolean pm = (bitRead(hh,6)==1) & (bitRead(hh,5)==1);
		hh = hh & ( bitRead(hh,6)==1 ? 0x1F : 0xFF);
		hh = bcd_dec(hh);
		byte mm = bcd_dec(readAddress(ADDRESS_AL_MIN_R));
		byte day = bcd_dec(readAddress(ADDRESS_AL_DAY_R));

		ret[0] = day; ret[1] = hh; ret[2] = mm;
	}