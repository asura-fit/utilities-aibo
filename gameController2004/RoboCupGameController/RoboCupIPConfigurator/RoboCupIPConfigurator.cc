//
// Copyright 2002,2003 Sony Corporation
//
// Permission to use, copy, modify, and redistribute this software is
// hereby granted.
//
// This software is provided "as is" without warranty of any kind,
// either expressed or implied, including but not limited to the
// implied warranties of fitness for a particular purpose.
//

#include <gtk/gtk.h>
#include "Gui.h"
#include "TeamInfo.h"


int
main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    teamInfo.Load();
    InitGui();

    gtk_main();

    return 0;
}
