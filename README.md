![my data structure](/struct1.png)

# Coursework 1

## Task 1:
Write a C / C++ function that prints in the command prompt window the contents of all the items from the current data structure. Prototype(text printed in bluedepends on the type of data structure, specify it yourself): <br />`void PrintDataStructure(pointer_to_data_structure);`
<br />The output should be similar to the following:

## Task 2:
Write a C / C++ function that inserts a new item into the current data structure. Prototype (text printed in bluedepends on the type of data structure, specify it yourself): 
<br />`pointer_to_data_structure=InsertItem(pointer_to_data_structure, char *pNewItemID = 0);` 
<br />The new item must be constructed by function GetItem()from ICS0017DataSource.dll. 
<br />The user may set the ID itself or set the pointer to it to zero. In the last case the ID is selected by GetItem().
You may freely select the position of new item in the linked list of items.
<br />The return value is the pointer to the modified data structure.
<br />The function must keep the current contents of data structure andthrow an exception if:
<br />• An item with same ID is already in the data structure.
<br />• he ID presented by user does not follow the formatting rules

## Task 3:
Write a C / C++ function that inserts a new item into the current data structure. Prototype (text printed in bluedepends on the typeof data structure, specify it yourself):
<br /> `pointer_to_data_structure=RemoveItem(pointer_to_data_structure, char *pItemID);`
<br />The memory fields occupied by the item to be removed must be released (use operator delete).
<br />Do not forget that if a header has lost all the items or other headers associated with it, this header itself must also disappear.
<br />The return value is the pointer to the modified data structure.
<br />The function must keep the current contents of data structure and throw an exception if:
<br />• Item with the specified ID does not exist.
<br />• The ID presented by user does not follow the formatting rules

## Task 4:
Put the implemented function into C / C++ application and write a simple command promptwindow menu to control its work

# Coursework 2
## Task
Coursework2  is  actually  the  continuation  of  Coursework1.  
<br />It  uses  the  same  data  structure,  item  and ICS0017DataSource.dll.Implement class DataStructurecontaining the following public functions(text printed in bluedepends on the type of item, specify it yourself):
<br />1. `DataStructure();`
<br />Constructor that creates empty data structure.
<br />2. `DataStructure(char *pFilename);`
<br />Constructor that reads data from a binary file. The file was created by function Write(see below).Fails ifthere occur problems with file handling.
<br />3. `~DataStructure();`
<br />Destructor that deletes all the items, vectors of pointers and headers.
<br />4. `DataStructure(const DataStructure &Original);`
<br />Copy constructor.
<br />5. `int GetItemsNumber();`
<br />Returns the current number of items indata structure.
<br />6. `pointer_to_item GetItem(char *pID);`
<br />Returns pointer to item with the specified ID. If the item was not found, returns 0.
<br />7. `pointer_to_item GetItem(long int code);`
<br />Returns pointer to item with the specified code. If the item was not found, returns 0.
<br />8. `pointer_to_item GetItem1(date_or_time);`
<br />Returns pointer to item with the specified date or time. 
<br />For example, if the item type is ITEM5,  the prototype should be ITEM5* GetItem(DATE1 d). If the item was not found, returns 0.
<br />9. `void operator+=(pointer_to_Item);`
<br />Operator function to add a new item into data structure. Fails if the data structure already contains an item with the specified ID. 
<br />Usage example:
<br />`DataStructure *pds = new DataStructure;`
<br />`ITEM5 *p = (ITEM5 *)GetItem(5);`
<br />`*pds +=p;`
<br />10. `void operator-=(char *pID);`
<br />Operator function to remove and destroy item with the specified ID. Fails if there is no item with the specified ID. 
<br />Usage example:
<br />`*pds-= (char *)”Dark Khaki”;`
<br />11. `DataStructure &operator=(const DataStructure &Right);`
<br />Operator function for assignment. Do not forget that before the assignment you have to destroy all the existing contents. Usage example:
<br />`DataStructure ds;`
<br />`ds = *pds;`
<br />12. `Int operator==(DataStructure &Other);`
<br />Operator function for comparison. Two data structures are equal if they contain the same number of items and for each item in the first structure there is a 100% matching item in the second structure. 
<br />The order in the linked lists may be different. Returs 0 (not equal) or 1 (equal). Usage example:
<br />`cout << (ds == *pds) << endl;`
<br />13. `void Write(char *pFilename);`
<br />Serializes the data structure and writes into the specified file.Fails if there occur problems with file handling or if the data structure is empty.
<br />14.`friend std::ostream &operator<<(std::ostream &ostr, const DataStructure &str);`
<br />Prints all the items into command prompt window. Usage example: 
<br />`cout << *pds << endl << endl;`
