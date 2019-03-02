const int pt_pin = A5;
const int laser_pin = 10;

const int pt_thres = 1020;

// in ms
const int send_timeout = 1;

const int buf_len = 256;
char buf[buf_len];

void setup()
{
	Serial.begin(9600);
}


void loop()
{
	if (Serial.available() > 0) {
		Serial.write(analogRead(pt_pin));
		Serial.read();
	}
}
