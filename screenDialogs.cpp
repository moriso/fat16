#include <iostream>
#include <vector>

using namespace std;

namespace print {
	void discNotFormated();
	void welcome();
	void man();
	void flushComplete();
	void flushConfirmation();
	void commandNotAvailable();
	void showDirContent(std::vector<string> v);
	void errorLoadingFile();
	void errorRecordingFile();
};

void print::discNotFormated() {
	cout << "================================================================================" << endl;
	cout << "=         The disc wasn't found. Creating a new one and formatting it.         =" << endl;
	cout << "================================================================================" << endl;
}

void print::welcome() {
	cout << "================================================================================" << endl;
	cout << "=        Welcome to the best computer running FAT16 in the whole world!        =" << endl;
	cout << "================================================================================" << endl;
	cout << "=           we recommend you to run this computer on a 80x24 screen.           =" << endl;
	cout << "=                in case you need some help, just type \"man\".                  =" << endl;
	cout << "================================================================================" << endl;
}

void print::man() {
	cout << "================================================================================" << endl;
	cout << "= - available commands:                                                        =" << endl;
	cout << "================================================================================" << endl;
	cout << "= 1: dir -> show what's inside the current directory.                          =" << endl;
	cout << "= 2: mkdir [dir_name] -> creates a directory named \"dir_name\".                 =" << endl;
	cout << "= 3: cd [dir_name] -> enter in \"dir_name\" directory.                           =" << endl;
	cout << "= 4: copy [src_file] -> copy a file located in \"src_file\" to the current di-   =" << endl;
	cout << "=     rectory.                                                                 =" << endl;
	cout << "= 5: type [file_name] -> show the content of a \"file_name\" file.               =" << endl;
	cout << "= 6: flush -> format the whole disc. [WARNING]                                 =" << endl;
	cout << "================================================================================" << endl;
	cout << "= you can turn the computer off anytime by pressing ctrl + c.                  =" << endl;
	cout << "================================================================================" << endl;
}

void print::flushComplete() {
	cout << "================================================================================" << endl;
	cout << "= your computer was successfully formated.                                     =" << endl;
	cout << "================================================================================" << endl;
}

void print::flushConfirmation() {
	cout << "================================================================================" << endl;
	cout << "= are you sure you want to format this computer? Y or N                        =" << endl;
	cout << "================================================================================" << endl;
}

void print::commandNotAvailable() {
	cout << "================================================================================" << endl;
	cout << "= command not available. type \"man\" to see all available commands.           =" << endl;
	cout << "================================================================================" << endl;
}

void print::showDirContent(std::vector<string> v) {
	cout << "================================================================================" << endl;
	for(int i = 0; i < v.size(); i++) {
		cout << v[i] << endl;
	}
	cout << "================================================================================" << endl;
}

void print::errorLoadingFile() {
	cout << "================================================================================" << endl;
	cout << "= an error ocurred while opening the file.                                     =" << endl;
	cout << "================================================================================" << endl;
}

void print::errorRecordingFile() {
	cout << "================================================================================" << endl;
	cout << "= an error ocurred while recording the file.                                   =" << endl;
	cout << "================================================================================" << endl;
}