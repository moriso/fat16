#include "controller.cpp"


int main() {
	
	string input;
	std::vector<int> dirContent;
	std::vector<string> fullInput;

	if(!controller::checkIfDiscExists()) {
		print::discNotFormated();
		filemanip::format();
		controller::init();
		controller::currentROOTIndex = controller::registerEntryROOT("root", "", 1);
		controller::dirStack.push(controller::currentROOTIndex);
	} else {
		controller::init();
		controller::currentROOTIndex = controller::getEntryIndexROOT(0);	
		controller::dirStack.push(controller::currentROOTIndex);
	}

	print::welcome();

	while(1) {
		// cout << "===" << endl;
		// cout << fat.block[0] << endl;
		// cout << fat.block[1] << endl;
		// cout << fat.block[2] << endl;
		// cout << fat.block[3] << endl;
		// cout << fat.block[4] << endl;
		// cout << fat.block[5] << endl;
		// cout << fat.block[6] << endl;
		// cout << "===" << endl;
		// cout << root[0].name << " -> " << root[0].firstBlock << " -> " << root[0].type << endl;
		// cout << root[1].name << " -> " << root[1].firstBlock << " -> " << root[1].type <<endl;
		// cout << root[2].name << " -> " << root[2].firstBlock << " -> " << root[2].type <<endl;
		// cout << root[3].name << " -> " << root[3].firstBlock << " -> " << root[3].type <<endl;
		// cout << root[4].name << " -> " << root[4].firstBlock << " -> " << root[4].type <<endl;
		// cout << root[5].name << " -> " << root[5].firstBlock << " -> " << root[5].type <<endl;
		// cout << root[6].name << " -> " << root[6].firstBlock << " -> " << root[6].type <<endl;
		// cout << root[7].name << " -> " << root[7].firstBlock << " -> " << root[7].type <<endl;
		// cout << root[8].name << " -> " << root[8].firstBlock << " -> " << root[8].type <<endl;
		input = "";
		
		while(input.empty()) {

			cout << "fat16-computer:" << root[controller::currentROOTIndex].name << " ";
			getline(cin, input);
		}

		fullInput = controller::parseInput(input);
		input = fullInput.front();

		if (input.compare("man") == 0) {

			print::man();

		} else if (input.compare("dir") == 0) {

			filemanip::readSingleCluster(root[controller::currentROOTIndex].firstBlock);
			dirContent = controller::parseCurrentDirContent();
			controller::showDirContent(dirContent);

		} else if (input.compare("mkdir") == 0) {

			if (fullInput.size() != 2) 
				print::commandNotAvailable();
			else {
				controller::registerEntryROOT(fullInput[1], "", 1);
			}

			// controller::registerEntryROOT("danggggg", "", 1);
			// controller::registerEntryROOT("sup", "", 1);
			// controller::registerEntryROOT("bluh", "", 1);
			// controller::registerEntryROOT("amiga", "", 1);


			// cout << fat.block[0] << endl;
			// cout << fat.block[1] << endl;
			// cout << fat.block[2] << endl;
			// cout << fat.block[DISCSIZE -1] << endl;
			// cout << fat.block[DISCSIZE - 50000] << endl;

			// cout << root[0].name << endl;
			// cout << root[0].timeModified << endl;
			// cout << root[1].name << endl;
			// cout << root[1].timeModified << endl;
			// cout << root[2].name << endl;
			// cout << root[2].timeModified << endl;
			// cout << root[3].name << endl;
			// cout << root[3].timeModified << endl;

		} else if (input.compare("cd") == 0) {

			if (fullInput.size() != 2)
				print::commandNotAvailable();
			else {

				if (strcmp(fullInput[1].c_str(), "..") == 0) {
					if (controller::dirStack.top() != 0) {
						controller::dirStack.pop();
						controller::currentROOTIndex = controller::dirStack.top();
					}

				} else {
					filemanip::readSingleCluster(root[controller::currentROOTIndex].firstBlock);
					dirContent = controller::parseCurrentDirContent();
					int dirIndex = controller::isDirAvailable(dirContent, fullInput[1]);
					
					if (dirIndex >= 0) {
						controller::changeDir(dirIndex);
						filemanip::readSingleCluster(root[controller::currentROOTIndex].firstBlock);
						controller::dirStack.push(controller::currentROOTIndex);
					} else {
						print::commandNotAvailable();
					}
				}
			}

		} else if (input.compare("copy") == 0) {

			if (fullInput.size() != 2)
				print::commandNotAvailable();
			else {
				int fileSize = controller::checkFileSize(fullInput[1]);
				if (fileSize > 0) {
					int wasFileRecorded = controller::recordFile(fullInput[1], fileSize);
					
					if (wasFileRecorded < 0)
						print::errorRecordingFile();

				} else {
					print::errorLoadingFile();
				}
			}

		} else if (input.compare("type") == 0) {

			if (fullInput.size() != 2)
				print::commandNotAvailable();
			else {
				filemanip::readSingleCluster(root[controller::currentROOTIndex].firstBlock);
				dirContent = controller::parseCurrentDirContent();
				if (controller::readFile(fullInput[1], dirContent) == -1) {
					print::errorLoadingFile();
				}
			}

		} else if (input.compare("flush") == 0) {

			string yesOrNo = "";
			print::flushConfirmation();

			while(yesOrNo.empty()) {

				cout << "fat16-computer:" << root[controller::currentROOTIndex].name << " ";
				getline(cin, yesOrNo);

				if (yesOrNo.compare("N") == 0) {

					break;
				} else if (yesOrNo.compare("Y") == 0) {

					filemanip::format();
					controller::currentROOTIndex = controller::registerEntryROOT("root", "", 1);
					print::flushComplete();
					break;
				} else {

					print::flushConfirmation();
					yesOrNo = "";
				}
			}
		} else {

			print::commandNotAvailable();
		}
	}

}