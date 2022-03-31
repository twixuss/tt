#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "winmm")
#define streq(a, b) (!strcmp(a, b))
int main(int argc, char **argv) {
	int seconds = 0;
	int minutes = 0;
	int hours   = 0;
	char *message = 0;
	for (int i = 1; i < argc; ++i) {
		auto l = strlen(argv[i]);
		if (argv[i][l - 1] == 's') {
			argv[i][l - 1] = '\0';
			seconds = atoi(argv[i]);
		} else if (argv[i][l - 1] == 'm') {
			argv[i][l - 1] = '\0';
			minutes = atoi(argv[i]);
		} else if (argv[i][l - 1] == 'h') {
			argv[i][l - 1] = '\0';
			hours = atoi(argv[i]);
		} else if (streq(argv[i], "i")) {
			if ((i += 1) >= argc) {
				puts("No value provided after i");
				return 1;
			}
			message = argv[i];
		} else {
			printf("Invalid argument %s", argv[i]);
			return 1;
		}
	}
	auto norm = [&] {
		while (seconds < 0) { seconds += 60; minutes -= 1; }
		while (minutes < 0) { minutes += 60; hours -= 1; }
		while (seconds >= 60) { seconds -= 60; minutes += 1; }
		while (minutes >= 60) { minutes -= 60; hours += 1; }
	};

	timeBeginPeriod(1);

	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	int alarmCounter = 0;
	while (1) {
		if (hours == 0 && minutes == 0 && seconds == 0) {
			if (alarmCounter & 1) printf("\r\a*****ALARM***** %s", message);
			else                  printf("\r\a     ALARM      %s", message);
			alarmCounter += 1;
		} else {
			printf("\r%dh %dm %ds        ", hours, minutes, seconds);
			seconds--;
			norm();
		}

		LARGE_INTEGER now;

		QueryPerformanceCounter(&now);
		auto msElapsed = (now.QuadPart - start.QuadPart) * 1000 / freq.QuadPart;
		Sleep(1000 - msElapsed);

		start.QuadPart += freq.QuadPart;

		do {
			QueryPerformanceCounter(&now);
		} while (now.QuadPart < start.QuadPart);
	}

	timeEndPeriod(1);
}
