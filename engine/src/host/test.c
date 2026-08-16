#include <stdlib.h>
#include <time.h>
#include "../modules/core/precomputed_board_data.h"
#include "../modules/core/shared_random.h"
#include "../modules/core/zobrist.h"
#include "../modules/test/test_suite_runner.h"

int main()
{
    bool success;
    initialize_shared_random((uint32_t)time(NULL));
    initialize_zobrist();
    initialize_precomputed_board_data();
    success = run_node_exploration_test_suite();
    run_performance_test_suite();
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
