#include <time.h>

namespace misc {
	long int getCurrentTime();
}

long int misc::getCurrentTime() {
	return static_cast<long int> (time(NULL));
}