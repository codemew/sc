// Program to illustrate the getopt()
// function in C

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	int opt;
	string str;

	// put ':' in the starting of the
	// string so that program can
	// distinguish between '?' and ':'
	while ((opt = getopt(argc, argv, "vp:o:s:")) != -1)
	{
		switch (opt)
		{
		case 'v':
			break;
		case 'p':
			str = optarg;
			printf("Path name: %s\n", str);
			break;
		case 'o':
			printf("option: %c\n", opt);
			break;
		case 's':
			printf("Stream name: %s\n", optarg);
			break;
		case ':':
			printf("option needs a value\n");
			break;
		case '?':
			printf("unknown option: %c\n", optopt);
			break;
		}
	}

	// optind is for the extra arguments
	// which are not parsed
	for (; optind < argc; optind++)
	{
		printf("extra arguments: %s\n", argv[optind]);
	}

	return 0;
}
