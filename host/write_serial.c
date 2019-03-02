#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define BUFLEN 32

int main(int argc, char *argv[])
{
	int fd, n, i;
	char buf[BUFLEN];
	char user_in[BUFLEN];
	struct termios toptions;

	fd = open(argv[1], O_RDWR | O_NOCTTY);
	if (fd < 0) {
		perror("error opening serial");
		return 1;
	}
	printf("fd opened as %i\n", fd);

	/* wait for arduino to reboot */
	usleep(3500000);

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
		printf("> ");
		memset(user_in, 0, sizeof(user_in));
		if (!fgets(user_in, BUFLEN, stdin))
			continue;
		printf("user input = %s\n", user_in);
		write(fd, user_in, strlen(user_in));

		while (1) {
			n = read(fd, buf, BUFLEN);
			buf[n] = 0;
			printf(buf);
		}
	}

	return 0;
}
