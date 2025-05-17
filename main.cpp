#include <assert.h>
#include <stdlib.h>

#include "FrontEnd.h"

#include "general.h" // FIXME: #include "utility_lib.h"


// GENERAL
const char HIGH_LEVEL_CODE_PATH[] = "./code.mcc";
const char AST_DOTCODE_OUTPATH[] = "./AST_tree.txt";
const char AST_IMG_OUTPATH[] = "./AST_img.png";

int main() {
    if (!FrontEnd_make_AST(HIGH_LEVEL_CODE_PATH, AST_DOTCODE_OUTPATH, AST_IMG_OUTPATH)) {
        debug("FrontEnd_make_AST failed\n");
        return EXIT_FAILURE;
    }

    return 0;
}
