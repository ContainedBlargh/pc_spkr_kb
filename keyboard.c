#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>
#include <signal.h>
#include <time.h>

typedef struct _Note {
	uint16_t tone;
	uint16_t dur;
} Note;

Note *notes;
size_t notes_count = 0;
size_t notes_cap = 128;

int fp_cfd;

void freeplay_exit(int sig) {
	signal(sig, SIG_IGN);
	ioctl(fp_cfd, KIOCSOUND, 0);
	exit(0);
}

int freeplay() {
	fp_cfd = open("/dev/console", O_WRONLY | O_NONBLOCK);
	signal(SIGINT, freeplay_exit);
	printf("Didn't read anything, starting freeplay mode!\n");
	fclose(stdin);
	int fd = open("/dev/input/event3", O_RDONLY | O_NONBLOCK);
	if (fd == -1) {
		printf("Sorry, could not open keyboard :/\n");
		return -1;
	}
	struct input_event data;
	struct input_event old_data;
	int tone, bytes;
	int no_input = 0;
	
	while (1) {
		bytes = read (fd, &data, sizeof(data));
		printf("\r            ");
		fflush(stdout);
		if (data.code == old_data.code) {
			no_input++;
			if (no_input >= 1000000) {
				ioctl(fp_cfd, KIOCSOUND, 0);
			}
			continue;
		}
		old_data = data;
		if (bytes > 0 && data.value < 458000 && data.value > 0) {
			no_input = 0;
			tone = data.code * 100;
			ioctl(fp_cfd, KIOCSOUND, tone);
		}
		usleep(10 * 1000);
	}
}

int main(int argc, char** argv) {
	notes = malloc(sizeof(Note) * notes_cap);
	int res;
	while(1) {
		uint16_t *tone_ptr = &((notes + notes_count)->tone);
		uint16_t *dur_ptr = &((notes + notes_count)->dur);
		res = scanf("%hd,%hd\n", tone_ptr, dur_ptr);
		if (res == EOF) {
			break;
		}
		if (res == 2) {
			notes_count++;
		}
	}
	notes = realloc(notes, sizeof(Note) * notes_count);
	int cfd = open("/dev/console", O_RDWR);
	if (notes_count == 0) {
		return freeplay();
	}
	printf("Loaded %ld notes, playing...\n", notes_count);
	size_t i;
	printf("\n");
	while(1) {
		for (i = 0; i < notes_count; i++) {
			Note n = notes[i];
			ioctl(cfd, KIOCSOUND, n.tone);
			if (n.dur * 1000 < 1000000) {
				usleep(n.dur * 1000);
			} else {
				sleep(n.dur / 1000);
			}
			printf(".");
			fflush(stdout);
		}
		ioctl(cfd, KIOCSOUND, 0);
		printf("\n");
		sleep(3);
		printf("Beautiful, let's hear it again!\n");
	}
	return 0;
}
