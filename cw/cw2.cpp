#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include "DateTime.h"
#include "Headers.h"
#include "Items.h"
#include "ICS0017DataSource.h"
#include <map>
using namespace std;

class DataStructure {
private:
	HEADER_B* pB;
	HEADER_B* GetMyHeaderB(HEADER_B* pList, char pKey) {
		if (!pList || !pKey) return 0;
		HEADER_B* p;
		for (p = pList; p; p = p->pNext) {
			if (pKey == p->cBegin) break;
		}
		return p;
	}

	HEADER_A* GetMyHeaderA(HEADER_A* pList, char pKey) {
		if (!pList || !pKey) return 0;
		HEADER_A* p = pList;
		while (p) {
			if (pKey == p->cBegin) break;
			p = p->pNext;
		}
		return p;
	}

public:
	DataStructure();
	DataStructure(char* pFileName);
	void Write(char* pFilename);
	DataStructure(const DataStructure& Original);
	~DataStructure();
	int GetItemsNumber();
	ITEM1* GetItem(char* pID);
	ITEM1* GetItem(long int code);
	ITEM1* GetItemByTime(char* pTime);
	void operator +=(ITEM1* item);
	void operator -=(char* pID);
	DataStructure& operator=(const DataStructure& Right);
	int operator==(DataStructure& Other);
	friend std::ostream& operator<<(std::ostream& ostr, const DataStructure& str);
};

//1
DataStructure::DataStructure() {
	pB = nullptr;
}

//2
DataStructure::DataStructure(char* pFileName) {
	pB = nullptr;
	fstream File(pFileName, fstream::in | fstream::binary);
	
	while (File.peek() != EOF) {
		ITEM1* itm = new ITEM1;
		int total;
		unsigned long int code;
		//read the total length
		File.read((char *)&total, sizeof(int));

		//read the pid
		int lenID = (int)total - sizeof(int) - sizeof(unsigned long int) - 9 * sizeof(char); //9 is len of time
		itm->pID = (char*)malloc(lenID);
		for (int i = 0; i < lenID; i++) {
			*(itm->pID + i) = File.get();
		}

		//read the time
		itm->pTime = (char*)malloc(9 * sizeof(char));
		for (int i = 0; i < 9; i++) {
			*(itm->pTime + i) = File.get();
		}

		//read the code
		File.read((char*)&code, sizeof(unsigned long int));
		(*itm).Code = code;

		itm->pNext = nullptr;
		(*this).operator+=(itm);
	}
}

//3
DataStructure::~DataStructure() {
	for (HEADER_B* hB = pB; hB; ) {
		HEADER_B* pNextB = nullptr;
		for (HEADER_A* hA = hB->pHeaderA; hA; ) {
			HEADER_A* pNextA = nullptr;
			for (ITEM1* pI = (ITEM1*)hA->pItems; pI; ) {
				int len = strlen(pI->pID) + 1;
				char itm[100];
				char* pid = pI->pID;
				for (int i = 0; i < len; i++)
				{
					itm[i] = *pid;
					pid = pid + 1;
				}
				pI = pI->pNext;
				pNextA = hA->pNext;
				pNextB = hB->pNext;
				*this -= itm;
			}
			hA = pNextA;
		}
		hB = pNextB;
	}
}

//4
DataStructure::DataStructure(const DataStructure& original) {
	pB = nullptr;
	HEADER_B* tmpB = original.pB;
	for (HEADER_B** ppB = &pB; tmpB; tmpB = tmpB->pNext) {
		*ppB = new HEADER_B;
		(*ppB)->cBegin = tmpB->cBegin;
		HEADER_A* tmpA = tmpB->pHeaderA;
		for (HEADER_A** hA = &(*ppB)->pHeaderA; tmpA; tmpA = tmpA->pNext) {
			*hA = new HEADER_A;
			(*hA)->cBegin = tmpA->cBegin;
			ITEM1* tmpI = (ITEM1*)tmpA->pItems;
			for (ITEM1** hI = (ITEM1**) & (*hA)->pItems; tmpI; tmpI = tmpI->pNext) {
				*hI = new ITEM1;
				int n;
				(*hI)->pID = new char[n = strlen(tmpI->pID) + 1];
				strcpy_s((*hI)->pID, n, tmpI->pID);
				(*hI)->pTime = new char[n = strlen(tmpI->pTime) + 1];
				strcpy_s((*hI)->pTime, n, tmpI->pTime);
				(*hI)->Code = tmpI->Code;
				if (tmpI->pNext) {
					(*hI)->pNext = new ITEM1;
					hI = &(*hI)->pNext;
				}
				else (*hI)->pNext = nullptr;
			}
			if (tmpA->pNext) {
				(*hA)->pNext = new HEADER_A;
				hA = &(*hA)->pNext;
			}
			else (*hA)->pNext = nullptr;
		}
		if (tmpB->pNext) {
			(*ppB)->pNext = new HEADER_B;
			ppB = &(*ppB)->pNext;
		}
		else (*ppB)->pNext = nullptr;
	}
}
	
//5
int DataStructure::GetItemsNumber() {
	int j = 0;
	HEADER_B* hB = pB;
	while (hB) {
		HEADER_A* hA = hB->pHeaderA;
		while (hA) {
			ITEM1* pI = (ITEM1*)hA->pItems;
			while (pI) {
				++j;
				pI = pI->pNext;
			}
			hA = hA->pNext;
		}
		hB = hB->pNext;
	}
	return j;
}

//6
ITEM1* DataStructure::GetItem(char* pID) {
	char first = *pID;
	char second = *(strchr(pID, ' ') + 1);
	HEADER_B* p;
	for (p = pB; p; p = p->pNext) {
		if (first == p->cBegin) break;
	}
	if (!p) return nullptr;
	HEADER_A* pA = p->pHeaderA;
	while (pA) {
		if (second == pA->cBegin) break;
		pA = pA->pNext;
	}
	if (!pA) return nullptr;
	ITEM1* pI;
	for (pI = (ITEM1*)pA->pItems; pI && strcmp(pID, pI->pID); pI = pI->pNext);
	return pI;
}

//7
ITEM1* DataStructure::GetItem(long int code) {
	HEADER_B* hB = pB;
	while (hB) {
		HEADER_A* hA = hB->pHeaderA;
		while (hA) {
			ITEM1* pI = (ITEM1*)hA->pItems;
			while (pI) {
				if (pI->Code == code) return pI;
				pI = pI->pNext;
			}
			hA = hA->pNext;
		}
		hB = hB->pNext;
	}
	return nullptr;
}

//8
ITEM1* DataStructure::GetItemByTime(char* pTime) {
	for (HEADER_B* hB = pB; hB; hB = hB->pNext) {
		for (HEADER_A* hA = hB->pHeaderA; hA; hA = hA->pNext) {
			for (ITEM1* itm = (ITEM1*)hA->pItems; itm; itm = itm->pNext) {
				if (strcmp(itm->pTime, pTime) == 0) return itm;
			}
		}
	}
	return nullptr;
}

//9
void DataStructure::operator +=(ITEM1* pNewItem) {

	if (!pNewItem) return;
	char pFirstWord = *pNewItem->pID;
	char pSecondWord = *(strchr(pNewItem->pID, ' ') + 1);

	HEADER_B* hB = pB;
	HEADER_B* tmpB = pB;
	bool Bexist = false;
	char tmpWB = pFirstWord;
	while (tmpB) {
		if (tmpB->cBegin == tmpWB) {
			Bexist = true;
			break;
		}
		tmpB = tmpB->pNext;
	}
	if (!Bexist) {
		HEADER_B** tracer = &pB;
		HEADER_B* newHB = new HEADER_B;
		newHB->cBegin = tmpWB;
		while (*tracer && (*tracer)->cBegin < tmpWB) {
			tracer = &(*tracer)->pNext;
		}
		newHB->pNext = *tracer;
		*tracer = newHB;
		HEADER_A* newHA = new HEADER_A;
		newHB->pHeaderA = newHA;
		newHA->cBegin = pSecondWord;
		newHA->pItems = pNewItem;
		pNewItem->pNext = NULL;
		newHA->pNext = NULL;
		return;
	}
	HEADER_A* hA = tmpB->pHeaderA;
	HEADER_A* tmpA = tmpB->pHeaderA;
	bool Aexist = false;
	char tmpWA = pSecondWord;
	while (tmpA) {
		if (tmpA->cBegin == tmpWA) {
			Aexist = true;
			break;
		}
		tmpA = tmpA->pNext;
	}
	if (!Aexist) {
		HEADER_A** tracer = &tmpB->pHeaderA;
		HEADER_A* newHA = new HEADER_A;
		newHA->cBegin = tmpWA;
		while ((*tracer) && (*tracer)->cBegin < tmpWA) {
			tracer = &(*tracer)->pNext;
		}
		newHA->pNext = *tracer;
		*tracer = newHA;
		newHA->pItems = pNewItem;
		return;
	}
	ITEM1* tmpI = (ITEM1*)tmpA->pItems;
	while (tmpI) {
		if (GetItem(pNewItem->pID)) {
			throw std::exception("item ID exists\n");
			return;
		}
		tmpI = tmpI->pNext;
	}
	tmpI = (ITEM1*)tmpA->pItems;
	pNewItem->pNext = tmpI;
	tmpA->pItems = pNewItem;
}

//10
void DataStructure::operator-=(char* pID) {
	char pFirstWord = *pID;
	char pSecondWord = *(strchr(pID, ' ') + 1);
	HEADER_B* tmpB = GetMyHeaderB(pB, pFirstWord);
	if (!tmpB) {
		printf("no item exist\n");
		return;
	}
	HEADER_A* tmpA = GetMyHeaderA(tmpB->pHeaderA, pSecondWord);
	if (!tmpA) {
		printf("no item exist\n");
		return;
	}
	ITEM1* tmpI = (ITEM1*)tmpA->pItems;
	if (!tmpI) {
		printf("no item exist\n");
		return;
	}
	ITEM1* prevI,* pI;
	for (pI = (ITEM1*)tmpA->pItems, prevI = (ITEM1*)tmpA->pItems; pI; prevI = pI, pI = pI->pNext) {
		if (strcmp(pID, pI->pID) == 0) {
			if(prevI == pI) tmpA->pItems = pI->pNext;
			else prevI->pNext = pI->pNext;
			delete pI;
			break;
		}
	}
	if (!tmpA->pItems) {
		HEADER_A *pA, *preA;
		for (pA = tmpB->pHeaderA, preA = tmpB->pHeaderA; pA; preA = pA, pA = pA->pNext) {
			if (pA->cBegin == pSecondWord) {
				if (preA == pA) tmpB->pHeaderA = pA->pNext;
				else preA->pNext = pA->pNext; 
				delete pA;
				break;
			}
		}
	}
	if (!tmpB->pHeaderA) {
		HEADER_B* ptrB, *preB;
		for (ptrB = pB, preB = pB; ptrB; preB = ptrB, ptrB = ptrB->pNext) {
			if (ptrB->cBegin == pFirstWord) {
				if (preB == ptrB) pB = ptrB->pNext;
				else preB->pNext = ptrB->pNext;
				delete ptrB;
				break;
			}
		}
	}
}

//11
DataStructure& DataStructure::operator=(const DataStructure& right) {
	if (this != &right) {
		this->~DataStructure();
		HEADER_B* tmpB = right.pB;
		while (tmpB) {
			HEADER_A* tmpA = tmpB->pHeaderA;
			while (tmpA) {
				ITEM1* tmpI = (ITEM1*)tmpA->pItems;
				while (tmpI) {
					char* pid = tmpI->pID;
					this->operator+=((ITEM1*)::GetItem(1,tmpI->pID));
					tmpI = tmpI->pNext;
				}
				tmpA = tmpA->pNext;
			}
			tmpB = tmpB->pNext;
		}
	}
	return *this;
}


//12
int DataStructure::operator==(DataStructure& Other) {
	int num1 = (*this).GetItemsNumber();
	int num2 = Other.GetItemsNumber();
	if (num1 != num2) return 0;
	else {
		HEADER_B* tmpB = Other.pB;
		while (tmpB) {
			HEADER_A* tmpA = tmpB->pHeaderA;
			while (tmpA) {
				ITEM1* tmpI = (ITEM1*)tmpA->pItems;
				while (tmpI) {
					ITEM1* pThisItem = this->GetItem(tmpI->pID);
					if (!pThisItem || strcmp(pThisItem->pTime, tmpI->pTime) != 0 || pThisItem->Code != tmpI->Code) return  0;
					tmpI = tmpI->pNext;
				}
				tmpA = tmpA->pNext;
			}
			tmpB = tmpB->pNext;
		}
		return 1;
	}
}

//13
void DataStructure::Write(char* pFileName) {
	fstream File;
	File.open(pFileName, fstream::out | fstream::binary);
	HEADER_B* hB = pB;
	while (hB) {
		HEADER_A* hA = hB->pHeaderA;
		while (hA) {
			ITEM1* p = (ITEM1*)hA->pItems;
			while (p) {
				short int n1 = strlen(p->pID) + 1, n2 = strlen(p->pTime) + 1;
				int n = n1 + n2;
				char* pSer, * r;
				pSer = (char*)malloc(n += sizeof(ITEM1) + sizeof(int) - sizeof(ITEM1*) - 2 * sizeof(char*));
				memcpy(r = pSer, &n, sizeof(int));
				memcpy(r += sizeof(int), p->pID, n1);
				memcpy(r += n1, p->pTime, n2);
				memcpy(r += n2, &p->Code, sizeof(unsigned long int));
				File.write(pSer, n);
				p = p->pNext;
			}
			hA = hA->pNext;
		}
		hB = hB->pNext;
	}
	if (!File.good()) {
		cout << "Error failed to write!" << endl;
	}
}

//14
std::ostream& operator<<(std::ostream& ostr, const DataStructure& str) {
	int j = 0;
	HEADER_B* hB = str.pB;
	while (hB) {
		HEADER_A* hA = hB->pHeaderA;
		while (hA) {
			ITEM1* pI = (ITEM1*)hA->pItems;
			while (pI) {
				ostr << pI->pID << " " << pI->Code << " " << pI->pTime << endl;
				pI = pI->pNext;
			}
			hA = hA->pNext;
		}
		hB = hB->pNext;
	}
	return ostr;
}

int main() {	
	map<string, DataStructure> Structures;
	int choice;
	cout << "Welcome! Whatcha wanna do?" << endl
		<< "Type 1 to create a new data structure" << endl
		<< "Type 0 to exit" << endl;
	cin >> choice;
	if (choice == 0) {
		return 0;
	}
	if (choice == 1) {
		while (true) {
			int choice1;
			cout << "Type 1 for a blank one" << endl
				<< "Type 2 to import from a file" << endl
				<< "Type 3 to copy from an existing one" << endl
				<< "Type 0 to exit" << endl;
			cin >> choice1;
			if (choice1 == 0) exit;
			string name;
			cout << "Give your data structure a name:" << endl;
			cin >> name;
			map<string, DataStructure>::iterator it = Structures.find(name);
			if (it != Structures.end()) {
				cout << name << " already exists" << endl;
				continue;
			}
			else if (choice1 == 1) {
				Structures[name];
				cout << name << " is created" << endl;
			}
			else if (choice1 == 2) {
				string namestr;
				cout << "What is the file name?" << endl;
				cin >> namestr;
				char* fname = new char[namestr.length() + 1];
				strcpy_s(fname, namestr.length() + 1, namestr.c_str());
				Structures[name] = *new DataStructure(fname);
				cout << name << " is created" << endl;
			}
			else if (choice1 == 3) {
				string structname;
				cout << "What is the other structure's name?" << endl;
				cin >> structname;
				DataStructure s = Structures[structname];
				Structures.insert(std::pair<string, DataStructure>(name, s));
				cout << name << " is created" << endl;
			}
			while (true) {
				cout << "What do you want to do to " << name << " ?" << endl
					<< "Type 1 to print" << endl
					<< "Type 2 to add" << endl
					<< "Type 3 to remove" << endl
					<< "Type 4 to destroy" << endl
					<< "Type 5 to compare" << endl
					<< "Type 6 to assign" << endl
					<< "Type 7 to write to file" << endl
					<< "Type 8 to create new struct" << endl
					<< "Type 9 to see item numbers" << endl
					<< "Type 10 to get item by ID" << endl
					<< "Type 11 to get item by code" << endl
					<< "Type 12 to get item by time" << endl
					<< "Type 13 to change struct" << endl
					<< "Type 0 to exit" << endl;
				int choice2;
				cin >> choice2;
				if (choice2 == 0)
					return 0;
				else if (choice2 == 1) {
					cout << Structures[name];
				}
				else if (choice2 == 2) {
					string strid;
					cout << "What is the item ID?" << endl;
					cin.ignore();
					getline(cin, strid);
					char* id = new char[strid.length() + 1];
					strcpy_s(id, strid.length() + 1, strid.c_str());
					ITEM1* itm = (ITEM1*)::GetItem(1, id);
					Structures[name].operator+=(itm);
					cout << Structures[name] << endl;
				}
				else if (choice2 == 3) {
					string strid;
					cout << "What is the item ID?" << endl;
					cin.ignore();
					getline(cin, strid);
					char* id = new char[strid.length() + 1];
					strcpy_s(id, strid.length() + 1, strid.c_str());
					Structures[name].operator-=(id);
					cout << Structures[name] << endl;
				}
				else if (choice2 == 4) {
					Structures[name].~DataStructure();
					Structures.erase(name);
					cout << name << " is destroyed" << endl;
					break;
				}
				else if (choice2 == 5) {
					string other;
					cout << "What is the other structure's name?" << endl;
					cin >> other;
					if (Structures[name] == Structures[other]) {
						cout << "Yes, they are the same." << endl;
					}
					else cout << "No, they are not the same." << endl;
				}
				else if (choice2 == 6) {
					string other;
					cout << "What is the other structure's name?" << endl;
					cin >> other;
					Structures[name] = Structures[other];
					cout << name << " 's new items are: " << Structures[name] << endl;
				}
				else if (choice2 == 7) {
					string namestr;
					cout << "Provide file name: " << endl;
					cin >> namestr;
					char* fname = new char[namestr.length() + 1];
					strcpy_s(fname, namestr.length() + 1, namestr.c_str());
					Structures[name].Write(fname);
					cout << "structure is written to file " << namestr << endl;
				}
				else if (choice2 == 8) {
					break;
				}
				else if (choice2 == 9) {
					cout << "Number of items is " << Structures[name].GetItemsNumber() << endl;
				}
				else if (choice2 == 10) {
					string strid;
					cout << "What is the item ID?" << endl;
					cin.ignore();
					getline(cin, strid);
					char* id = new char[strid.length() + 1];
					strcpy_s(id, strid.length() + 1, strid.c_str());
					ITEM1* itm = Structures[name].GetItem(id);
					cout << itm->pID << " " << itm->Code << " " << itm->pTime << endl;
				}
				else if (choice2 == 11) {
					unsigned long int c;
					cout << "What is the code?" << endl;
					cin >> c;
					ITEM1* itm = Structures[name].GetItem(c);
					cout << itm->pID << " " << itm->Code << " " << itm->pTime << endl;
				}
				else if (choice2 == 12) {
					string strtime;
					cout << "What is the time?" << endl;
					cin >> strtime;
					char* time = new char[strtime.length() + 1];
					strcpy_s(time, strtime.length() + 1, strtime.c_str());
					ITEM1* itm = Structures[name].GetItemByTime(time);
					cout << itm->pID << " " << itm->Code << " " << itm->pTime << endl;
				}
				else if (choice2 == 13) {
					string s;
					cout << "What struct?" << endl;
					cin >> s;
					name = s;
				}
			}
			
		}
	}
	return 0;
}