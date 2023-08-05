#include <time.h>
#include "../modules/generation/generation_scheduler.h"
#include "../core/shared_random.h"

int main()
{
    initialize_shared_random((uint32_t)time(NULL));
    schedule_generation();
    return 0;
}