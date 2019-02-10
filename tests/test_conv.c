#include <stdio.h>
#include <libtime.h>
#include <inttypes.h>

int main(int argc, char **argv)
{
	uint64_t v;
	libtime_init();

	/* Basic cpu->wall and wall->cpu conversion tests */

	for(v = 1; v < (1ULL << 60ULL); v *= 10) {
		printf("libtime_cpu_to_wall(%" PRIu64 ") = %" PRIu64 "\n",
				v, libtime_cpu_to_wall(v));
		printf("libtime_wall_to_cpu(%" PRIu64 ") = %" PRIu64 "\n",
				v, libtime_wall_to_cpu(v));
	}

	return 0;
}
