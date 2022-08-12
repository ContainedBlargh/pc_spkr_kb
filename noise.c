#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {
	srand(time(NULL));
	if (argc < 2) {
		printf("Please provide an amount.\n");
		return 0;
	}
	int amount = atoi(argv[1]);
	while (amount > 0) {
		int tone = random() % 12000;
		int dur = (random() % 20) * 100;
		printf("%d,%d\n", tone, dur);
		amount--;
	}
	return 0;
}
