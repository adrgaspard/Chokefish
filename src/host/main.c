#include "../modules/core/board.h"
#include "../modules/core/shared_random.h"
#include "../modules/core/zobrist.h"

int main()
{
    initialize_shared_random_component(42);
    initialize_zobrist_component();
    initialize_board_component();
    return 0;
}