#include "../modules/core/precomputed_board_data.h"
#include "../modules/core/shared_random.h"
#include "../modules/core/zobrist.h"
#include "../modules/uci/debug_printer.h"
#include "../modules/uci/uci.h"

int main()
{
    initialize_shared_random((uint32_t)time(NULL));
    initialize_zobrist();
    initialize_precomputed_board_data();
    initialize_debug_printer();
    handle_commands();
    return 0;
}