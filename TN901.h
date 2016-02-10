#ifndef TH901_H
#define TH901_H

#include <Arduino.h>
#include <inttypes.h>

#ifdef __AVR_ARCH__
#include <avr/wdt.h>
typedef uint32_t time_t;
#endif

#define TN901_OTADDRESS 0x4c
#define TN901_ETADDRESS 0x66
#define TN901_ENDADDRESS 0x0d

#define MODE_OT 0x01
#define MODE_ET 0x02

class TN901
{
private:
	mutable float _tempEnvironment;
	mutable float _tempObject;
	mutable float _tempWhat;
	uint8_t _dataPin;
	uint8_t _clkPin;
	uint8_t _ackPin;
	mutable uint8_t _data[5];

	void init()
	{
		pinMode(_dataPin, INPUT);
		pinMode(_clkPin, INPUT);
		pinMode(_ackPin, OUTPUT);
		digitalWrite(_ackPin, HIGH);
	}

public:
	TN901() {}

	TN901(uint8_t pinData, uint8_t pinClk, uint8_t pinAck):
		_dataPin(pinData), _clkPin(pinClk), _ackPin(pinAck)
	{
		init();
	}
	
	void init(uint8_t pinData, uint8_t pinClk, uint8_t pinAck)
	{
		_dataPin = pinData;
		_clkPin = pinClk;
		_ackPin = pinAck;

		init();
	}

	void read(uint8_t mode = MODE_ET | MODE_OT) const
	{
		uint8_t flag = 0x00;

		digitalWrite(_ackPin, LOW); // start conversion

		uint8_t k;
		for(k = 0; k < 10; ++k) // try 10 frames
		{
			#ifdef TN901_DEBUG
			Serial.print("TN901: data: ");
			#endif

			for(uint8_t j = 0; j < 5; ++j) // 5 byte per frame
			{
				for(uint8_t i = 0; i < 8; ++i) // 8 bits per byte
				{
					 // wait for clkPin go HIGH
					while (!digitalRead(_clkPin))
					{
						wdt_reset();
					}
					// wait for clkPin go LOW
					while(digitalRead(_clkPin))
					{
						wdt_reset();
					}
					_data[j]= (_data[j] << 1) | ((digitalRead(_dataPin) == HIGH) ? 0x01 : 0x00);
				}
				#ifdef TN901_DEBUG
				Serial.print(_data[j], HEX);
				Serial.print(" : ");
				#endif
			}

			#ifdef TN901_DEBUG
			Serial.print(", crc = ");
			Serial.print((_data[0] + _data[1] + _data[2]) & 0xff, HEX);
			Serial.print(" (");
			Serial.print(_data[3], HEX);
			if (((_data[0] + _data[1] + _data[2]) & 0xff) == _data[3])
				Serial.println(") Good! ^______^");
			else
				Serial.println(") Bad /_____\\");
			#endif
			if (_data[4] == TN901_ENDADDRESS && ((_data[0] + _data[1] + _data[2]) & 0xff) == _data[3])
			{
				if (_data[0] == TN901_OTADDRESS)
				{
					_tempObject = ((_data[1] * 256.0 + _data[2]) / 16.0) - 273.15;
					flag |= MODE_OT;
					#ifdef TN901_DEBUG
					Serial.print("TN901: Got object temp = ");
					Serial.println(_tempObject);
					#endif
				}
				else if (_data[0] == TN901_ETADDRESS)
				{
					_tempEnvironment = ((_data[1] * 256.0 + _data[2]) / 16.0) - 273.15;
					flag |= MODE_ET;
					#ifdef TN901_DEBUG
					Serial.print("TN901: Got environment temp = ");
					Serial.println(_tempObject);
					#endif
				}
			}

			#ifdef TN901_DEBUG
			Serial.print("TN901: mode = ");
			Serial.print(mode, HEX);
			Serial.print(", flag = ");
			Serial.println(flag, HEX);
			#endif
			if ((mode & flag) == mode)
			{
				#ifdef TN901_DEBUG
				Serial.println("TN901: got all temp we want, return...");
				#endif
				break;
			}
		}

		digitalWrite(_ackPin, HIGH); // end conversion

		#ifdef TN901_DEBUG
		Serial.print("TN901: conversions = ");
		Serial.println(k);
		#endif

		return;
	}

	float getObjectTemperature() const { return _tempObject; }
	float getEnvironmentTemperature() const { return _tempEnvironment; }
};

#endif