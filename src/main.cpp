/** Some kind of big comment to document stuff probably goes here
* Authors, license, disclaimer, whatever
*/

#include "Application.h"
#include "Framework.h"
#include "stdafx.h"


// -----------------------------------------------------------------------------
int main(int argc, char* argv[]) {

	/* Parse cmd line arguments */
	appSettings settings;
	switch (argc) {
	case 0:
		Log::debug("No settings file provided, will load default settings");
		settings = defaultSettings();
		break;
	case 1:
		Log::debug("Settings file provided");
		settings = loadSettings(std::string(argv[0]));
		break;
	default:
		Log::error("Number of command line arguments (" + std::to_string(argc) + ") is not recognized ");
		return -1;
	}

	/* Apply settings */
	Log::debug("Settings successfully loaded, initializing application...");
	Application application(settings);
	Log::debug("Application successfully initialized, running application...");

	/* Enter main game loop */
	int ret = application.play();

	/* Shut down */
	if(ret){
		Log::error("Application unsuccessfully terminated");
		return -1;
	}
	Log::debug("Application successfully terminated");
	return 0;

}


