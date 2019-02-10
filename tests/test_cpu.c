#include <stdio.h>
#include <libtime.h>
#include <inttypes.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	uint64_t c_s, c_e;
	uint64_t w_s, w_e;
	libtime_init();

	/* Ensure that measurement works properly between wall and cpu times */

	w_s = libtime_wall();
	c_s = libtime_cpu();
	usleep(137);
	c_e = libtime_cpu();
	w_e = libtime_wall();

	printf("nsec: %" PRIu64 " %" PRIu64 "\n", w_e - w_s, libtime_cpu_to_wall(c_e - c_s));
	printf("cycles: %" PRIu64 " %" PRIu64 "\n", libtime_wall_to_cpu(w_e - w_s), c_e - c_s);

	return 0;
}
