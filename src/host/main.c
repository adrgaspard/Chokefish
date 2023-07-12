#include "../modules/core/precomputed_board_data.h"
#include "../modules/core/shared_random.h"
#include "../modules/core/zobrist.h"

int main()
{
    initialize_shared_random(42);
    initialize_zobrist();
    initialize_precomputed_board_data();
    return 0;
}