#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define BUFLEN 256

int main(int argc, char *argv[])
{
	int fd, n, i;
	char buf[BUFLEN];
	struct termios toptions;

	printf("starting reader\n");

	fd = open(argv[1], O_RDWR | O_NOCTTY);
	if (fd < 0) {
		perror("error opening serial");
		return 1;
	}
	printf("fd opened as %i\n", fd);

	/* wait for arduino to reboot */
	usleep(3500000);

	printf("setting serial port settings\n");

	/* get current serial port settings */
	tcgetattr(fd, &toptions);
	/* set 9600 baud both ways */
	cfsetispeed(&toptions, B9600);
	cfsetospeed(&toptions, B9600);
	/* 8 bits, no parity, no stop bits */
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;
	/* Canonical mode */
	toptions.c_lflag |= ICANON;
	/* commit the serial port settings */
	tcsetattr(fd, TCSANOW, &toptions);

	while (1) {
		printf("awaiting data from remote host\n")

		/* Receive string from Arduino */
		n = read(fd, buf, BUFLEN);
		buf[n] = 0;

		printf(buf);
		fflush(stdout);
	}

	return 0;
}
