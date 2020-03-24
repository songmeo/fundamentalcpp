#include <iostream>
#include <string>
#include "DateTime.h"
#include "Headers.h"
#include "Items.h"
#include <cstring>
#include "string.h"
#include "ICS0017DataSource.h"
#define SIZE 1
using namespace std;

void printStruct(HEADER_B * ps1);
HEADER_B* InsertItem(HEADER_B* p, char* pNewItemID = 0);
HEADER_B* RemoveItem(HEADER_B* p, char* pItemID);

int main()
{
	HEADER_B* ps1 = GetStruct1(1, 1);
	int choice;
	while (true) {
		printf("Type 0 to exit\n");
		printf("Type 1 to insert\n");
		printf("Type 2 to delete\n");
		printf("Type 3 to print\n");
		scanf_s("%d", &choice);
		if (choice == 0)
			return 0;
		if (choice == 1) {
			char itemID[100];
			printf("Type new item ID: \n");
			cin.get();
			cin.getline(itemID, sizeof(itemID));
			ps1 = InsertItem(ps1, itemID);
			printStruct(ps1);
		}
		if (choice == 2) {
			char itemID[100];
			printf("Type item ID: \n");
			cin.get();
			cin.getline(itemID, sizeof(itemID));
			ps1 = RemoveItem(ps1, itemID);
			printStruct(ps1);
		}
		if (choice == 3) {
			printStruct(ps1);
		}
	}
	return 0;
}

ITEM1* GetMyItem(ITEM1* pList, char* pKey) {
	if (!pList || !pKey) return 0;
	ITEM1* p;
	for (p = pList; p && strcmp(pKey, p->pID); p = p->pNext);
	return p;
}

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

//TASK 1
void printStruct(HEADER_B* p) {
	int j = 0;
	HEADER_B* hB = p;
	while (hB) {
		HEADER_A* hA = hB->pHeaderA;
		while (hA) {
			ITEM1* pI = (ITEM1*)hA->pItems;
			while (pI) {
				printf("%d) %s %lu %s\n", ++j, pI->pID, pI->Code, pI->pTime);
				pI = pI->pNext;
			}
			hA = hA->pNext;
		}
		hB = hB->pNext;
	}
}
//TASK 2
HEADER_B* InsertItem(HEADER_B* p, char* pNewItemID) {
	ITEM1* pNewItem = (ITEM1*)GetItem(1, pNewItemID);
	if (!pNewItem) return p;
	char* pFirstWord = pNewItemID;
	char* pSecondWord = strchr(pNewItemID, ' ') + 1;

	HEADER_B* hB = p;
	HEADER_B* tmpB = p;
	bool Bexist = false;
	char tmpWB = pFirstWord[0];
	while (tmpB) {
		if (tmpB->cBegin == tmpWB) {
			Bexist = true;
			break;
		}
		tmpB = tmpB->pNext;
	}
	if (!Bexist) {
		HEADER_B** tracer = &p;
		HEADER_B* newHB = new HEADER_B;
		newHB->cBegin = tmpWB;
		while (*tracer && (*tracer)->cBegin < tmpWB) {
			tracer = &(*tracer)->pNext;
		}
		newHB->pNext = *tracer;
		*tracer = newHB;
		HEADER_A* newHA = new HEADER_A;
		newHB->pHeaderA = newHA;
		newHA->cBegin = pSecondWord[0];
		newHA->pItems = pNewItem;
		pNewItem->pNext = NULL;
		newHA->pNext = NULL;
		return p;
	}
	HEADER_A* hA = tmpB->pHeaderA;
	HEADER_A* tmpA = tmpB->pHeaderA;
	bool Aexist = false;
	char tmpWA = pSecondWord[0];
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
		return p;
	}
	ITEM1* tmpI = (ITEM1*)tmpA->pItems;
	while (tmpI) {
		if (GetMyItem(tmpI, pNewItemID)) {
			printf("item ID exists\n");
			return p;
		}
		tmpI = tmpI->pNext;
	}
	tmpI = (ITEM1*)tmpA->pItems;
	pNewItem->pNext = tmpI;
	tmpA->pItems = pNewItem;
	return p;
}

//TASK 3
HEADER_B* RemoveItem(HEADER_B* p, char* pItemID) {
	char pFirstWord = *pItemID;
	char pSecondWord = *(strchr(pItemID, ' ') + 1);
	HEADER_B* tmpB = GetMyHeaderB(p, pFirstWord);
	if (!tmpB) {
		printf("no item exist\n");
		return p;
	}
	HEADER_A* tmpA = GetMyHeaderA(tmpB->pHeaderA, pSecondWord);
	if (!tmpA) {
		printf("no item exist\n");
		return p;
	}
	ITEM1* tmpI = (ITEM1*)tmpA->pItems;
	if (!tmpI) {
		printf("no item exist\n");
		return p;
	}
	/*
	ITEM1** tracerI = (ITEM1**)tmpA->pItems;
	bool exist = false;
	while (*tracerI && !(exist = (strcmp((*tracerI)->pID, pItemID) == 0))) {
		*tracerI = (*tracerI)->pNext;
	}
	if (exist) {
		ITEM1* old = *tracerI;
		*tracerI = (*tracerI)->pNext;
		delete old;
	}
	return p;
	*/
	ITEM1* prevI, * pI;
	prevI = NULL;
	for (pI = tmpI; pI; prevI = pI, pI = pI->pNext) {
		if (strcmp(pItemID, pI->pID) == 0) {
			if (prevI == NULL) tmpA->pItems = pI->pNext;
			else prevI->pNext = pI->pNext;
			delete pI;
			break;
		}
	}
	if (!tmpI) {
		HEADER_A* preA, * pA;
		preA = NULL;
		for (pA = tmpB->pHeaderA; pA; preA = pA, pA = pA->pNext) {
			if (strcmp((char*)pA->cBegin, (char*)tmpA->cBegin) == 0) {
				if (preA == NULL) tmpB->pHeaderA = pA->pNext;
				else preA->pNext = pA->pNext;
				delete pA;
				break;
			}
		}
	}
	if (!tmpA) {
		HEADER_B* preB, * pB;
		preB = NULL;
		for (pB = p; pB; preB = pB, pB = pB->pNext) {
			if (strcmp(&pB->cBegin, (char*)tmpB->cBegin) == 0) {
				if (preB == NULL) tmpB = pB->pNext;
				else preB->pNext = pB->pNext;
				delete pB;
				break;
			}	
		}
	}
	return p;
}
/*
HEADER_B* RemoveItem(HEADER_B* p, char* pItemID) {
	char pFirstWord = *pItemID;
	char pSecondWord = *(strchr(pItemID, ' ') + 1);
	HEADER_B* tmpB = GetMyHeaderB(p, pFirstWord);
	if (!tmpB) {
		printf("no headerB found.\n");
		return p;
	}
	HEADER_A* tmpA = GetMyHeaderA(tmpB->pHeaderA, pSecondWord);
	if (!tmpA) {
		printf("no headerA found.\n");
		return p;
	}
	ITEM1* item = GetMyItem((ITEM1 *)tmpA->pItems, pItemID);
	if (!item) {
		printf("no item found.\n");
		return p;
	}

	//delete item
	/*
	ITEM1* pList = (ITEM1 *)tmpA->pItems;
	ITEM1** tmp = &pList;
	ITEM1* pKey;
	ITEM1* pBefore = pList;
	while (*tmp && strcmp(pItemID,(char*)(*tmp)->pID)) {
		tmp = &(*tmp)->pNext;
	}
	pKey = *tmp;
	*tmp = (*tmp)->pNext;
	delete pKey;
	return p;
	ITEM1* pList = (ITEM1*)tmpA->pItems;
	ITEM1* pBefore = NULL;
	ITEM1* pI = pList;
	while(pI) {
		if (strcmp(pI->pID, pItemID) == 0) {
			if (!pBefore) {
				pList = pI->pNext;
				delete pI;
				/*
				if (!pList) {
					//delete tmpA
					HEADER_A** tmp = &tmpB->pHeaderA;
					while (strcmp((char*)(*tmp)->pNext->cBegin, (char*)tmpA->cBegin) != 0) {
						*tmp = (*tmp)->pNext;
					}
					HEADER_A* pBefore = *tmp;
					*tmp = (*tmp)->pNext;
					pBefore->pNext = (*tmp)->pNext;
					delete tmp;

					//delete tmpB
					HEADER_B** tmpp = &p;
					while (strcmp((char*)(*tmpp)->pNext->cBegin, (char*)tmpB->cBegin) != 0) {
						*tmpp = (*tmpp)->pNext;
					}
					HEADER_B* pBeforeB = *tmpp;
					*tmpp = (*tmpp)->pNext;
					pBeforeB->pNext = (*tmpp)->pNext;
					delete tmpp;
				}
				return p;
			}
			else {
				pBefore->pNext = pI->pNext;
			}
			delete pI;
			break;
		}
		else {
			pBefore = pI;
			pI = pI->pNext;
		}
	}
	return p;
}
*/

/*A
ITEM1* RemoveItem(ITEM1* pList, char* pItemID) {
	if (!pList) return pList;
	bool exist = false;
	ITEM1** tmp = &pList;
	ITEM1* pKey;
	while ((*tmp) && !(exist = (strcmp(pItemID, (*tmp)->pID) == 0) )) {
		tmp = &(*tmp)->pNext;
		if (exist) {
			pKey = *tmp;
			*tmp = (*tmp)->pNext;
			//free(pKey->pID);
			//free(pKey);
			delete pKey;
		}
	}
	return pList;
}
*/