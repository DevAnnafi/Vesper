#include <stdlib.h>
#include "terminal.h"
#include "editor.h"


int main(int argc, char *argv[])
{
	// save original settings
        atexit(disableRawMode);
 
        enableRawMode();

	if (argc > 1)
	{

		editorLoop(argv[1]);

	}

	else 
	{
		editorLoop(NULL);
	}
 
        return 0;

}
