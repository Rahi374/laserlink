const int pt_pin = A5;
const int laser_pin = 10;

const int pt_thres = 200;

// in us
const unsigned int timeout = 50000;

const int buf_len = 32;
char buf[buf_len];

// in us
unsigned long rtt;

// we don't need to keep track of the bytes to send because
// we're always going to send a full packet (coz i'm lazy)

int bytes_from_serial;

int wait_for_low()
{
	unsigned long start = micros();

	while (analogRead(pt_pin) > pt_thres)
		if ((micros() - start) >= timeout) {
			Serial.println("timed out!");
			return -1;
		}

	return 0;
}

int wait_for_high()
{
	unsigned long start = micros();

	while (analogRead(pt_pin) <= pt_thres)
		if ((micros() - start) >= timeout) {
			Serial.println("timed out!");
			return -1;
		}

	return 0;
}

int wait_for_low_indef()
{
	unsigned long start = micros();

	while (analogRead(pt_pin) > pt_thres)
		if ((micros() - start) >= 1000*timeout) {
			Serial.println("timed out!");
			return -1;
		}

	return 0;
}

int wait_for_high_indef()
{
	unsigned long start = micros();

	while (analogRead(pt_pin) <= pt_thres)
		if ((micros() - start) >= 1000*timeout) {
			Serial.println("timed out!");
			return -1;
		}

	return 0;
}


void get_data_from_serial()
{
	// char c;
	int n = 0;
	int i;
	String s;

	/*
	Serial.readBytes(&c, 1);

	switch (c) {
		case 's':
			n = Serial.readBytesUntil('\0', buf, min(bytes_from_serial, buf_len));
			break;
		case 'b':
		default:
			n = Serial.readBytes(buf, min(bytes_from_serial, buf_len));
			break;
	}
	*/

	//n = Serial.readBytesUntil('\0', buf, min(bytes_from_serial, buf_len));
	s = Serial.readString();
	n = s.length();
	s.getBytes(buf, buf_len);
	for (i = n; i < buf_len; i++)
		buf[i] = 0;

	Serial.println("going to send...");
	for(i = 0; i < buf_len; i++)
		Serial.print(buf[i], HEX);
}

int send_data()
{
	int i, j;
	char n = 0;
	//unsigned long time = micros();

	digitalWrite(laser_pin, HIGH);
	/*
	if (wait_for_high())
		return -1;
	*/
	//PORTB &= ~_BV(PB2);
	// while ((micros() - time) < 250) { delayMicroseconds(5); }

	for (i = 0; i < buf_len; i++) {
		for (j = 0; j < 8; j++) {
			// time = micros();
			if ((buf[i] >> j) & 1) {
				PORTB |= _BV(PB2);
				//digitalWrite(laser_pin, HIGH);
				//Serial.println("sending 1");
			} else {
				PORTB &= ~_BV(PB2);
				//digitalWrite(laser_pin, LOW);
				//Serial.println("sending 0");
			}

			if (n & 1)
				wait_for_high_indef();
			else
				wait_for_low_indef();
			n = ~n;

			//while ((micros() - time) < 220)
			//	delayMicroseconds(5);
		}
	}

	return 0;
}

void setup()
{
	pinMode(laser_pin, OUTPUT);
	Serial.setTimeout(1);
	Serial.begin(9600);
}

void loop()
{
	digitalWrite(laser_pin, LOW);

	if ((bytes_from_serial = Serial.available()) > 0) {
		// get data first
		Serial.println("getting data from serial");
		get_data_from_serial();
		Serial.println("data obtained");
		Serial.println(String(buf));

		// send send request
		Serial.println("sending send request");
		digitalWrite(laser_pin, HIGH);
		Serial.println("waiting for reply");
		if (wait_for_high())
			return;
		Serial.println("completing send request");
		digitalWrite(laser_pin, LOW);
		Serial.println("waiting to complete send request");
		if (wait_for_low())
			return;

		// send data
		Serial.println("sending data");
		if (send_data())
			return;

		Serial.println("acking data");
		// get ack for data
		if (wait_for_high())
			return;
		digitalWrite(laser_pin, HIGH);
		if (wait_for_low())
			return;
		digitalWrite(laser_pin, LOW);
	}
}
