const int pt_pin = A5;
const int laser_pin = 10;

const int pt_thres = 200;

// in ms
const int send_timeout = 1;

const int buf_len = 256;
char buf[buf_len];

// we don't need to keep track of the bytes to send because
// we're always going to send a full packet (coz i'm lazy)

int bytes_from_serial;


void get_data_from_serial()
{
	int c = Serial.read();
	int n = 0;
	int i;

	switch (c) {
		case 's':
			n = Serial.readBytesUntil('\0', buf, min(bytes_from_serial, buf_len));
			break;
		case 'b':
		default:
			n = Serial.readBytes(buf, min(bytes_from_serial, buf_len));
			break;
	}

	for (i = n; i < buf_len; i++)
		buf[i] = 0;
}

void send_data()
{
	int i, j;	

	digitalWrite(laser_pin, HIGH);

	for (i = 0; i < buf_len; i++)
		for (j = 0; j < 8; j++)
			if ((buf[i] >> j) & 1)
				digitalWrite(laser_pin, HIGH);
			else
				digitalWrite(laser_pin, LOW);
}

void setup()
{
	pinMode(laser_pin, OUTPUT);
	Serial.setTimeout(1);
	Serial.begin(9600);
}

void loop()
{
	if ((bytes_from_serial = Serial.available()) > 0) {
		// send send request
		digitalWrite(laser_pin, HIGH);
		while (analogRead(pt_pin) <= pt_thres) {}
		digitalWrite(laser_pin, LOW);
		while (analogRead(pt_pin) > pt_thres) {}

		// send data
		get_data_from_serial();
		send_data();

		// get ack for data
		while (analogRead(pt_pin) <= pt_thres) {}
		digitalWrite(laser_pin, HIGH);
		while (analogRead(pt_pin) > pt_thres) {}
		digitalWrite(laser_pin, LOW);
	}
}
