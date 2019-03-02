const int pt_pin = A5;
const int laser_pin = 10;

const int pt_thres = 200;

// in ms
const int send_timeout = 1;

const int buf_len = 256;
char buf[buf_len];

void setup()
{
	pinMode(laser_pin, OUTPUT);
	Serial.begin(9600);
}

void send_data_to_serial()
{
	int i;

	for (i = buf_len-1; i >= 0; i--)
		if (buf[i])
			break;

	Serial.write(buf, i+1);
}

void recv_data()
{
	int i, j;

	// wait for start of data (high)
	while (analogRead(pt_pin) <= pt_thres) {}

	for (i = 0; i < buf_len; i++)
		for (j = 0; j < 8; j++)
			if (analogRead(pt_pin) > pt_thres)
				buf[i] |= (1 << j);
			else
				buf[i] &= ~(1 << j);
}

void loop()
{
	// recv send reqest
	if (analogRead(pt_pin) > pt_thres) {
		// ack send request
		digitalWrite(laser_pin, HIGH);
		while (analogRead(pt_pin) > pt_thres) {}
		digitalWrite(laser_pin, LOW);

		// receive data
		recv_data();
		send_data_to_serial();

		// send ack for data
		digitalWrite(laser_pin, HIGH);
		while (analogRead(pt_pin) <= pt_thres) {}
		digitalWrite(laser_pin, LOW);
		while (analogRead(pt_pin) > pt_thres) {}
	}
}
