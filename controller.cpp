#include "filemanip.cpp"
#include "miscFunctions.cpp"
#include "screenDialogs.cpp"
#include <vector>
#include <stack>
#include <math.h>


namespace controller {
	void init();
	int checkIfDiscExists();
	int seekFirstFreeEntryROOT();
	int seekFirstFreeBlockFAT();
	int registerEntryROOT(string name, string ext, int type);
	int getEntryIndexROOT(int firstBlock);
	void test();
	int currentROOTIndex;
	std::vector<int> parseCurrentDirContent();
	void showDirContent(std::vector<int> v);
	std::vector<string> parseInput(string input);
	int isDirAvailable(std::vector<int> currentDirContent, string dirName);
	void changeDir(int dirIndex);
	std::stack<int> dirStack;
	int checkFileSize(string filePath);
	int recordFile(string filePath, int fileSize);
	int readFile(string fileName, std::vector<int> dirContent);
}


void controller::init() {
	filemanip::init();
}

int controller::checkIfDiscExists() {
	ifstream disc("disc.bin");
	if (disc.good()) {
		disc.close();
		return 1;
	} else {
		disc.close();
		return 0;
	}
}


int controller::seekFirstFreeBlockFAT() {
	for(int i = 0; i < DISCSIZE; i++) {
		if(fat.block[i] == -1)
			return i;
	}
	return -1;
}

int controller::seekFirstFreeEntryROOT() {
	for(int i = 0; i < ROOTSIZE; i++) {
		if (root[i].timeModified == -1)
			return i;
	}
	return -1;
}

void controller::test() {
}

int controller::registerEntryROOT(string name, string ext, int type) {
	int rootIndex = controller::seekFirstFreeEntryROOT();
	if (rootIndex == -1) return rootIndex;

	strcpy(root[rootIndex].name, name.c_str());
	root[rootIndex].timeModified = misc::getCurrentTime();
	root[rootIndex].firstBlock = controller::seekFirstFreeBlockFAT();
	
	if (type == DIRECTORY_TYPE) {
		root[rootIndex].type = type;
		fat.block[root[rootIndex].firstBlock] = 0;
		filemanip::recordRootEntry(rootIndex, controller::currentROOTIndex);
	} else {
		strcpy(root[rootIndex].ext, ext.c_str());
		root[rootIndex].type = type;
		fat.block[root[rootIndex].firstBlock] = 0;
		filemanip::recordRootEntry(rootIndex, controller::currentROOTIndex);
	}

	// cout << root[rootIndex].name << endl;
	// cout << root[rootIndex].ext << endl;
	// cout << root[rootIndex].timeModified << endl;
	// cout << root[rootIndex].firstBlock << endl;
	// cout << root[rootIndex].type << endl;
	// cout << "===========" << endl;
	// cout << fat.block[0] << endl;
	// cout << fat.block[1] << endl;
	// cout << fat.block[2] << endl;
	// cout << fat.block[3] << endl;
	// cout << fat.block[4] << endl;
	// cout << fat.block[5] << endl;
	// cout << fat.block[6] << endl;

	
	return rootIndex;
} 

int controller::getEntryIndexROOT(int firstBlock) {
	for(int i = 0; i < ROOTSIZE; i++) {
		if (root[i].firstBlock == firstBlock)
			return i;
	}
	return -1;
}

std::vector<int> controller::parseCurrentDirContent() {
	std::vector<int> contents;
	string content = "";

	if(strcmp(cluster.data, "empty") != 0) {
		string clusterData(cluster.data);
		for(int i = 0; i < clusterData.length(); i++) {
			if (clusterData.at(i) == ',') {
				contents.push_back(stoi(content));
				content = "";
			} else {
				content += clusterData.at(i);
			}
		}
		if (content != "") contents.push_back(stoi(content));
	}
	return contents;
}

void controller::showDirContent(std::vector<int> v) {
	std::vector<string> names;
	string tmp;
	for(int i = 0; i < v.size(); i++) {
		if (root[v[i]].type == DIRECTORY_TYPE) {
			names.push_back(root[v[i]].name);
		} else {
			string name(root[v[i]].name);
			string ext(root[v[i]].ext);
			names.push_back(name + "." + ext);
		}
	}
	print::showDirContent(names);
}

std::vector<string> controller::parseInput(string input) {
	std::vector<string> tokens;
	string tmp = "";
	for(int i = 0; i < input.length(); i++) {
		if (input.at(i) == ' ') {
			tokens.push_back(tmp);
			tmp = "";
		} else {
			tmp += input.at(i);
		}
	}
	if (tmp != "") tokens.push_back(tmp);
	return tokens;
}

int controller::isDirAvailable(std::vector<int> currentDirContent, string dirName) {
	std::size_t foundDot = dirName.find('.');
	if(foundDot == std::string::npos) {
		for(int i = 0; i < currentDirContent.size(); i++) {
			if(strcmp(root[currentDirContent[i]].name, dirName.c_str()) == 0)
				return currentDirContent[i];
		}
	}
	return -1;
}

void controller::changeDir(int dirIndex) {
	controller::currentROOTIndex = dirIndex;
}

int controller::checkFileSize(string filePath) {
	return filemanip::checkFileSize(filePath);
}

int controller::recordFile(string filePath, int fileSize) {
	string fullFileName;
	std::vector<int> clustersPositions;
	size_t pos = filePath.find_last_of("/");

	if(pos != string::npos)
		fullFileName = filePath.substr(pos + 1);	
	else return -1;

	string fileName;
	string ext;

	pos = fullFileName.find_last_of(".");

	if(pos != string::npos) {
		fileName = fullFileName.substr(0, pos);
		ext = fullFileName.substr(pos + 1);
	} else return -1;

	int fileIndex = controller::registerEntryROOT(fileName, ext, 0);
	int totalClustersUsed = ceil(fileSize / CLUSTERSIZE) + 1;
	root[fileIndex].size = totalClustersUsed;
	
	clustersPositions.push_back(root[fileIndex].firstBlock);
	if (totalClustersUsed > 1) {
		int newFreeBlock = controller::seekFirstFreeBlockFAT();
		fat.block[root[fileIndex].firstBlock] = newFreeBlock;		
		for(int i = 1; i < totalClustersUsed; i++) {
			fat.block[newFreeBlock] = controller::seekFirstFreeBlockFAT() + 1;
			clustersPositions.push_back(newFreeBlock);
			if(i == totalClustersUsed - 1) {
				fat.block[newFreeBlock] = 0;
			}
			newFreeBlock = controller::seekFirstFreeBlockFAT();		
		}	
		
	}
	// for(int i = 0; i < clustersPositions.size(); i++) {
	// 	cout << clustersPositions[i] << endl;
	// }
	// cout << "===" << endl;
	// cout << fat.block[0] << endl;
	// cout << fat.block[1] << endl;
	// cout << fat.block[2] << endl;
	// cout << fat.block[3] << endl;
	// cout << fat.block[4] << endl;
	// cout << fat.block[5] << endl;
	// cout << fat.block[6] << endl;
	// cout << "===" << endl;
	
	filemanip::recordFile(filePath, clustersPositions);

	return 1;
}

int controller::readFile(string fileName, std::vector<int> dirContent) {
	string name;
	string ext;
	int currentFATValue;
	std::vector<int> clustersPositions;
	size_t pos = fileName.find_last_of(".");

	if(pos != string::npos) {
		name = fileName.substr(0, pos);
		ext = fileName.substr(pos + 1);
	} else return -1;

	// cout << fat.block[0] << endl;
	// cout << fat.block[1] << endl;
	// cout << fat.block[2] << endl;
	// cout << fat.block[3] << endl;
	// cout << fat.block[4] << endl;
	// cout << fat.block[5] << endl;
	// cout << fat.block[6] << endl;
	// cout << "===" << endl;

	for(int i = 0; i < dirContent.size(); i++) {
		if(strcmp(name.c_str(), root[dirContent[i]].name) == 0) {
			if(strcmp(ext.c_str(), root[dirContent[i]].ext) == 0) {
				clustersPositions.push_back(root[dirContent[i]].firstBlock);
				currentFATValue = fat.block[root[dirContent[i]].firstBlock];
				if(currentFATValue != 0) {
					do {
						clustersPositions.push_back(currentFATValue);
						currentFATValue = fat.block[currentFATValue];
					} while(currentFATValue != 0);
				}

				filemanip::readFile(clustersPositions);
				return 1;
			}
		}		
	}

	return -1;
}

// void readFile() {
// 	fstream disc("disc.bin", ios::binary | ios::out | ios::in);
// 	int totalRootSize = sizeof(root[0]) * ROOTSIZE;
// 	disc.seekp( 0, ios::beg);
// 	disc.seekp( sizeof(fat) + sizeof(cluster) + totalRootSize, ios::beg);

// 	for(int i = 0; i < 5; i++ ) {
// 		disc.read(cluster.data, sizeof(cluster));
// 		cout << cluster.data << endl;
// 	}
// }