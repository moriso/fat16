#include <iostream>
#include <fstream>
#include <sstream>
#include "structures.cpp"
#include <vector>

using namespace std;

namespace filemanip {
	void format();
	void readSingleCluster(int index);
	void init();
	void recordRootEntry(int index, int currentDirectoryIndex);
	void write();
	int checkFileSize(string fileName);
	void recordFile(string filePath, std::vector<int> clustersPositions);
	void readFile(std::vector<int> clustersPositions);
}

// this method formats the HDD, cleaning the structures
// and writing them to it.
// the final order is:
// - FAT
// - ROOT[ROOTSIZE]
// - CLUSTER[DISCSIZE]
void filemanip::format() {

	//our lil HDD
	ofstream disc("disc.bin", ios::binary);
	
	// clear FAT
	for(int i = 0; i < DISCSIZE; i++) {
		fat.block[i] = -1;
	}

	//clear ROOT
	for(int i = 0; i < ROOTSIZE; i++) {
		root[i].type 		 = -1;
		root[i].timeModified = -1;
		root[i].size 		 = -1;
		root[i].attr 		 = '0';
		root[i].firstBlock 	 = -1;
		strcpy(root[i].name, "");
		strcpy(root[i].ext,  "");
	}

	//clear CLUSTER
	strcpy(cluster.data, "empty");


	//write FAT
	disc.write((char *)&fat, sizeof(fat));

	//write ROOT
	for(int i = 0; i < ROOTSIZE; i++) {
		disc.write((char *)&root[i], sizeof(root[i]));	
	}

	//write CLUSTER
	for(int i = 0; i < DISCSIZE; i++) {
		disc.write((char *)&cluster, sizeof(cluster));	
	}

	disc.close();

}

void filemanip::write() {
	ofstream disc("disc.bin", ios::binary);
	
	disc.write((char *)&fat, sizeof(fat));

	for(int i = 0; i < ROOTSIZE; i++) {
		disc.write((char *)&root[i], sizeof(root[i]));
	}

	for(int i = 0; i < DISCSIZE; i++) {
		disc.write((char *)&cluster, sizeof(cluster));	
	}

	disc.close();
}

void filemanip::readSingleCluster(int index) {
	ifstream myfile("disc.bin", ios::binary);
	myfile.read((char *)&fat, sizeof(fat));
	myfile.read((char *)&root, sizeof(root));
	for(int i = 0; i <= index; i++) {
		myfile.read((char *)&cluster, sizeof(cluster));
	}
}

// this method loads the FAT and ROOT from the HDD
void filemanip::init() {
	ifstream disc("disc.bin", ios::binary);
	disc.read((char *)&fat, sizeof(fat));
	disc.read((char *)&root, sizeof(root));

	disc.close();
}

void filemanip::recordRootEntry(int index, int currentDirectoryIndex) {
	fstream disc("disc.bin", ios::binary | ios::out | ios::in);

	int totalRootSize = 0;
	for(int i = 0; i < index; i++) {
		totalRootSize += sizeof(root[i]);
	}

	//records FAT
	disc.seekp( 0, ios::beg);
	disc.write((char *)&fat, sizeof(fat));

	///records ROOT[index]
	disc.seekp( 0, ios::beg);
	disc.seekp( sizeof(fat) + totalRootSize, ios::beg);
	disc.write((char *)&root[index], sizeof(root[index]));

	//records CLUSTER
	strcpy(cluster.data, "empty");
	totalRootSize = sizeof(root[index]) * ROOTSIZE;
	int totalClusterSize = 0;
	disc.seekp( 0, ios::beg);
	for(int i = 0; i < root[index].firstBlock; i++) {
		totalClusterSize += sizeof(cluster);
	}
	disc.seekp( sizeof(fat) + totalRootSize + totalClusterSize, ios::beg);
	disc.write((char *)&cluster, sizeof(cluster));

	//records the current entry to the parent's directory
	if(index != 0 ) {
		totalClusterSize = 0;
		disc.seekp( 0, ios::beg);
		for(int i = 0; i < root[currentDirectoryIndex].firstBlock; i++) {
			totalClusterSize += sizeof(cluster);
		}
		disc.seekp( sizeof(fat) + totalRootSize + totalClusterSize, ios::beg);
		disc.read((char *)&cluster, sizeof(cluster));

		ostringstream data;
		if (strcmp(cluster.data, "empty") == 0) {
			data << index;
			strcpy(cluster.data, data.str().c_str());
		} else {
			string moreData = ",";
			data << index;
			moreData += data.str();
			strcat(cluster.data, moreData.c_str());
		}
		disc.seekp( sizeof(fat) + totalRootSize + totalClusterSize, ios::beg);
		disc.write((char *)&cluster, sizeof(cluster));
	}

	disc.close();
}

int filemanip::checkFileSize(string fileName) {
	ifstream file(fileName, ios::binary | ios::ate);
	return file.tellg();
}

void filemanip::recordFile(string filePath, std::vector<int> clustersPositions) {
	fstream disc("disc.bin", ios::binary | ios::out | ios::in);
	ifstream file(filePath);

	//records FAT
	disc.seekp( 0, ios::beg);
	disc.write((char *)&fat, sizeof(fat));

	///records ROOT
	disc.seekp( 0, ios::beg);
	disc.seekp( sizeof(fat), ios::beg);
	for(int i = 0; i < ROOTSIZE; i++) {
		disc.write((char *)&root[i], sizeof(root[i]));	
	}

	int totalRootSize = sizeof(root[0]) * ROOTSIZE;
	int totalClusterSize = 0;

	for(int i = 0; i < clustersPositions[0]; i++) {
		totalClusterSize += sizeof(cluster);
	}

	disc.seekp( 0, ios::beg);
	disc.seekp( sizeof(fat) + totalRootSize + totalClusterSize, ios::beg);

	char tmp[2000];
	for(int i = 0; i < clustersPositions.size(); i++) {
		file.read(tmp, sizeof(tmp) - 1);
		strcpy(cluster.data, tmp);
		disc.write((char *)&cluster, sizeof(cluster));
		memset(tmp, 0, 2000);
	}
}

void filemanip::readFile(std::vector<int> clustersPositions) {
	fstream disc("disc.bin", ios::binary | ios::out | ios::in);
	int totalRootSize = sizeof(root[0]) * ROOTSIZE;
	int totalClusterSize = 0;

	for(int i = 0; i < clustersPositions[0]; i++) {
		totalClusterSize += sizeof(cluster);
	}

	disc.seekp( 0, ios::beg);
	disc.seekp( sizeof(fat) + totalRootSize + totalClusterSize, ios::beg);

	for(int i = 0; i < clustersPositions.size(); i++ ) {
		disc.read(cluster.data, sizeof(cluster));
		cout << cluster.data << endl;
	}
}