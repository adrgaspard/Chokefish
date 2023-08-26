#include "../serialization/consts.h"
#include "test_suite_runner.h"
#include "test_utils.h"

void run_performance_test_suite()
{
    run_performance_test(START_FEN_STR, 6);
    run_performance_test("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2", 6);
    run_performance_test("2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3 2", 5);
    run_performance_test("1Bb3BN/R2Pk2r/1Q5B/4q2R/2bN4/4Q1BK/1p6/1bq1R1rb w - - 0 1", 5);
}