#include <stdio.h>
#include "ijvm.h"
#include "util.h"
#include "gui.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        rungui();
        return 1;
    }

    if (init_ijvm(argv[1]) < 0)
    {
        fprintf(stderr, "Couldn't load binary %s\n", argv[1]);
        return 1;
    }

    run();

    destroy_ijvm();

    return 0;
}
