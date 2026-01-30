#include <stdlib.h>
#include "terminal.h"
#include "editor.h"


int main()
{
	// save original settings
        atexit(disableRawMode);
 
        enableRawMode();

	editorLoop();
 
        return 0;

}
