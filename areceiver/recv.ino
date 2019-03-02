const int pt_pin = A5;
const int laser_pin = 10;

const int pt_thres = 200;

// in us
const unsigned int timeout = 50000;

const int buf_len = 32;
char buf[buf_len];

// in us
unsigned long rtt;

void setup()
{
	pinMode(laser_pin, OUTPUT);
	Serial.begin(9600);
}

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

int wait_for_high_indef()
{
	unsigned long start = micros();

	while (analogRead(pt_pin) <= pt_thres)
		if ((micros() - start) >= 10*timeout) {
			Serial.println("timed out!");
			return -1;
		}

	return 0;
}


void send_data_to_serial()
{
	int i;

	for (i = buf_len-1; i >= 0; i--)
		if (buf[i])
			break;

	Serial.println("GOT DATA!");
	Serial.println(i);
	Serial.println(String(buf));

	Serial.println("received...");
	for (i = 0; i < buf_len; i++)
		Serial.print(buf[i], HEX);
}

int recv_data()
{
	int i, j;
	char n = 0;
	// unsigned long time = micros(); 
	int val;

	// wait for start of data (high)
	if (wait_for_high())
		return -1;
	// rtt = micros() - rtt;
	// time = micros() - time;
	//digitalWrite(laser_pin, HIGH);
	// while ((micros() - time) < 250) { delayMicroseconds(5); }
	// delayMicroseconds((rtt - time)/2);

	for (i = 0; i < buf_len; i++) {
		for (j = 0; j < 8; j++) {
			// time = micros(); 
			if ((val = analogRead(pt_pin)) > pt_thres) {
				buf[i] |= (1 << j);
				Serial.println("received 1");
			} else {
				buf[i] &= ~(1 << j);
				Serial.println("received 0");
			}
			Serial.println(val);

			if (n & 1) {
				digitalWrite(laser_pin, HIGH);
			} else {
				digitalWrite(laser_pin, LOW);
				//delayMicroseconds(80);
			}
			n = ~n;

			//delayMicroseconds(40);
			// while ((micros() - time) < 220)
			// delayMicroseconds(120);
		}
	}

	return 0;
}

void loop()
{
	digitalWrite(laser_pin, LOW);

	// recv send reqest
	if (analogRead(pt_pin) > pt_thres) {
		// ack send request
		Serial.println("received send req");
		digitalWrite(laser_pin, HIGH);
		Serial.println("waiting for completion of send req");
		rtt = micros();
		if (wait_for_low())
			return;
		Serial.println("completing send req");
		digitalWrite(laser_pin, LOW);

		// receive data
		Serial.println("receiving data");
		if (recv_data())
			return;

		Serial.println("sending to serial");
		send_data_to_serial();

		// send ack for data
		Serial.println("sending ack");
		digitalWrite(laser_pin, HIGH);
		Serial.println("waiting for reply");
		if (wait_for_high())
			return;
		Serial.println("completing reply");
		digitalWrite(laser_pin, LOW);
		Serial.println("waiting for completion of ack");
		if (wait_for_low())
			return;
	}
}
