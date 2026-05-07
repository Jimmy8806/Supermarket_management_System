#include <bits/stdc++.h>
#include <fstream>
#include <limits>
using namespace std;
struct SalesSecondaryIndex {
    char customerName[50]; int id;
    bool operator<(const SalesSecondaryIndex& si) const {
        return string(customerName) < string(si.customerName);
    }
};
vector<SalesSecondaryIndex> SalesSecondaryIndexList;

void Sales_writeSecondaryIndexFile() {
    ofstream outFile("Sales_SecondaryIndex.txt", ios::binary | ios::out | ios::trunc);
    for (int i = 0; i < (int)SalesSecondaryIndexList.size(); i++)
        outFile.write((char*)&SalesSecondaryIndexList[i], sizeof(SalesSecondaryIndex));
    outFile.close();
}
void Sales_ReadSecondaryIndexFile() {
    ifstream inFile("Sales_SecondaryIndex.txt", ios::binary | ios::in);
    if (!inFile) return;
    SalesSecondaryIndex si; SalesSecondaryIndexList.clear();
    while (inFile.read((char*)&si, sizeof(si))) SalesSecondaryIndexList.push_back(si);
    inFile.close();
}
void Sales_AddToSecondaryIndexList(SalesSecondaryIndex si) {
    SalesSecondaryIndexList.push_back(si);
    sort(SalesSecondaryIndexList.begin(), SalesSecondaryIndexList.end());
}
void Sales_DeleteFromSecondaryIndexList(int id) {
    for (int i = 0; i < (int)SalesSecondaryIndexList.size(); i++)
        if (SalesSecondaryIndexList[i].id == id) { SalesSecondaryIndexList.erase(SalesSecondaryIndexList.begin() + i); return; }
}
vector<int> Sales_searchInSecondaryIndexList(char name[]) {
    vector<int> foundIds;
    for (int i = 0; i < (int)SalesSecondaryIndexList.size(); i++)
        if (string(SalesSecondaryIndexList[i].customerName) == string(name))
            foundIds.push_back(SalesSecondaryIndexList[i].id);
    return foundIds;
}

struct SalesPrimaryIndex {
    int id, offset;
    bool operator<(const SalesPrimaryIndex& pi) const { return id < pi.id; }
};
vector<SalesPrimaryIndex> SalesPrimaryIndexList;

void Sales_writePrimaryIndexFile() {
    ofstream outFile("Sales_PrimaryIndex.txt", ios::binary | ios::trunc);
    for (auto pi : SalesPrimaryIndexList) outFile.write((char*)&pi, sizeof(pi));
    outFile.close();
}
void Sales_ReadPrimaryIndexFile() {
    ifstream inFile("Sales_PrimaryIndex.txt", ios::binary);
    if (!inFile) return;
    SalesPrimaryIndex pi;
    while (inFile.read((char*)&pi, sizeof(pi))) SalesPrimaryIndexList.push_back(pi);
}
void Sales_AddToPrimaryIndexList(SalesPrimaryIndex pi) {
    SalesPrimaryIndexList.push_back(pi);
    sort(SalesPrimaryIndexList.begin(), SalesPrimaryIndexList.end());
}
void Sales_deleteFromPrimaryIndexList(int id) {
    for (int i = 0; i < (int)SalesPrimaryIndexList.size(); i++)
        if (SalesPrimaryIndexList[i].id == id) { SalesPrimaryIndexList.erase(SalesPrimaryIndexList.begin() + i); return; }
}
int Sales_searchInPrimaryIndexList(int id) {
    int low = 0, high = (int)SalesPrimaryIndexList.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (SalesPrimaryIndexList[mid].id == id) return SalesPrimaryIndexList[mid].offset;
        else if (SalesPrimaryIndexList[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

class Sales {
public:
    int saleID; char customerName[50]; char productName[50]; float totalAmount; int quantitySold;

    void FixInput() { if (cin.peek() == '\n') cin.ignore(); }

    void ReadSale(bool isUpdate = false) {
        if (!isUpdate) {
            while (true) {
                int temp; cout << "Enter Sale ID: "; cin >> temp;
                if (Sales_searchInPrimaryIndexList(temp) != -1) cout << "Error: ID exists\n";
                else { saleID = temp; break; }
            }
        }
        do {
            cout << "Enter Customer Name (min 5 chars): ";
            FixInput(); cin.getline(customerName, 50);
        } while (strlen(customerName) < 5);
        cout << "Enter Product Name: "; FixInput(); cin.getline(productName, 50);
        cout << "Enter Amount: "; cin >> totalAmount;
        cout << "Enter Quantity: "; cin >> quantitySold;
    }
    void DisplaySale() {
        cout << "ID: " << saleID << "\nCustomer: " << customerName
             << "\nProduct: " << productName << "\nAmount: " << totalAmount
             << "\nQuantity: " << quantitySold << "\n";
    }
    short CalcLength() {
        return sizeof(int) + sizeof(float) + sizeof(int) + strlen(customerName) + strlen(productName) + 2 * sizeof(short);
    }
};

void Sales_WriteToFile(fstream& file, Sales s) {
    file.write((char*)&s.saleID, sizeof(s.saleID));
    short len = strlen(s.customerName); file.write((char*)&len, sizeof(short)); file.write(s.customerName, len);
    len = strlen(s.productName); file.write((char*)&len, sizeof(short)); file.write(s.productName, len);
    file.write((char*)&s.totalAmount, sizeof(float)); file.write((char*)&s.quantitySold, sizeof(int));
}
void Sales_ReadFromFile(fstream& file, Sales& s) {
    file.read((char*)&s.saleID, sizeof(s.saleID));
    short len; file.read((char*)&len, sizeof(short)); file.read(s.customerName, len); s.customerName[len] = '\0';
    file.read((char*)&len, sizeof(short)); file.read(s.productName, len); s.productName[len] = '\0';
    file.read((char*)&s.totalAmount, sizeof(float)); file.read((char*)&s.quantitySold, sizeof(int));
}

void Sales_FirstFit(fstream& file, Sales& s) {
    file.seekg(0); short header; file.read((char*)&header, sizeof(header));
    short needed = s.CalcLength();
    if (header == -1) {
        file.seekp(0, ios::end); int pos = file.tellp();
        Sales_WriteToFile(file, s); Sales_AddToPrimaryIndexList({ s.saleID, pos }); return;
    }
    short curr = header, prev = -1;
    while (curr != -1) {
        file.seekg(curr + sizeof(int) + sizeof(short) + sizeof(char));
        short next, size; file.read((char*)&next, sizeof(short)); file.read((char*)&size, sizeof(short));
        if (size >= needed) {
            file.seekp(curr); int pos = file.tellp();
            Sales_WriteToFile(file, s); Sales_AddToPrimaryIndexList({ s.saleID, pos });
            short padding = size - needed; while (padding--) file.put('~');
            if (prev == -1) { file.seekp(0); file.write((char*)&next, sizeof(next)); }
            else { file.seekp(prev + sizeof(int) + sizeof(short) + sizeof(char)); file.write((char*)&next, sizeof(next)); }
            return;
        }
        prev = curr; curr = next;
    }
    file.seekp(0, ios::end); int pos = file.tellp();
    Sales_WriteToFile(file, s); Sales_AddToPrimaryIndexList({ s.saleID, pos });
}

void Sales_Insert(fstream& file, Sales& s) {
    Sales_FirstFit(file, s);
    SalesSecondaryIndex si; strcpy(si.customerName, s.customerName); si.id = s.saleID;
    Sales_AddToSecondaryIndexList(si); cout << "Sale inserted successfully.\n";
}
void Sales_SearchByID(fstream& file, int id) {
    int offset = Sales_searchInPrimaryIndexList(id);
    if (offset == -1) { cout << "Not Found.\n"; return; }
    Sales s; file.seekg(offset); Sales_ReadFromFile(file, s);
    cout << "Sale Found.\n"; s.DisplaySale(); cout << "------------------\n";
}
void Sales_DeleteByID(fstream& file, int id) {
    file.seekg(0); short header; file.read((char*)&header, sizeof(header));
    int offset = Sales_searchInPrimaryIndexList(id);
    if (offset == -1) { cout << "Not Found.\n"; return; }
    Sales s; file.seekg(offset); Sales_ReadFromFile(file, s);
    file.seekp(offset + sizeof(int) + sizeof(short)); file.put('*');
    file.write((char*)&header, sizeof(header)); short size = s.CalcLength(); file.write((char*)&size, sizeof(size));
    file.seekp(0); file.write((char*)&offset, sizeof(offset));
    Sales_deleteFromPrimaryIndexList(id); Sales_DeleteFromSecondaryIndexList(id);
    cout << "Sale deleted successfully.\n";
}
void Sales_UpdateByID(fstream& file, int id) {
    file.seekg(0); short header; file.read((char*)&header, sizeof(header));
    int offset = Sales_searchInPrimaryIndexList(id);
    if (offset == -1) { cout << "Not Found.\n"; return; }
    Sales s; file.seekg(offset); Sales_ReadFromFile(file, s);
    cout << "Old Data:\n"; s.DisplaySale(); short oldLen = s.CalcLength();
    Sales_DeleteFromSecondaryIndexList(id);
    cout << "New Data:\n"; s.ReadSale(true); short newLen = s.CalcLength();
    SalesSecondaryIndex si; strcpy(si.customerName, s.customerName); si.id = s.saleID;
    Sales_AddToSecondaryIndexList(si);
    if (oldLen >= newLen) {
        file.seekp(offset); Sales_WriteToFile(file, s);
        short padding = oldLen - newLen; while (padding--) file.put('~');
    } else {
        file.seekp(offset + sizeof(int) + sizeof(short)); file.put('*');
        file.write((char*)&header, sizeof(header)); file.write((char*)&oldLen, sizeof(oldLen));
        file.seekp(0); file.write((char*)&offset, sizeof(offset));
        Sales_deleteFromPrimaryIndexList(id);
        file.seekp(0, ios::end); int newOffset = file.tellp();
        Sales_WriteToFile(file, s); Sales_AddToPrimaryIndexList({ s.saleID, newOffset });
    }
    cout << "Sale updated successfully.\n";
}
void Sales_SearchByCustomerName(fstream& file) {
    char name[50]; cout << "Enter Customer Name: ";
    cin.ignore(); cin.getline(name, 50);
    vector<int> ids = Sales_searchInSecondaryIndexList(name);
    if (ids.empty()) { cout << "Not Found.\n"; return; }
    for (int id : ids) Sales_SearchByID(file, id);
}
void Sales_UpdateByCustomerName(fstream& file) {
    char name[50]; cout << "Enter Customer Name: ";
    cin.ignore(); cin.getline(name, 50);
    vector<int> ids = Sales_searchInSecondaryIndexList(name);
    if (ids.empty()) { cout << "Not Found.\n"; }
    else if (ids.size() == 1) { Sales_UpdateByID(file, ids[0]); }
    else {
        cout << "Multiple found:\n";
        for (int id : ids) cout << "ID: " << id << "\n";
        int target; cin >> target; Sales_UpdateByID(file, target);
    }
}
void Sales_DeleteByCustomerName(fstream& file) {
    char name[50]; cout << "Enter Customer Name: ";
    cin.ignore(); cin.getline(name, 50);
    vector<int> ids = Sales_searchInSecondaryIndexList(name);
    if (ids.empty()) { cout << "Not Found.\n"; }
    else if (ids.size() == 1) { Sales_DeleteByID(file, ids[0]); }
    else {
        cout << "Multiple found:\n";
        for (int id : ids) cout << "ID: " << id << "\n";
        int target; cout << "Enter ID: "; cin >> target; Sales_DeleteByID(file, target);
    }
}
void Sales_DisplayAll(fstream& file) {
    cout << "========= All Sales =========\n";
    Sales s;
    for (int i = 0; i < (int)SalesPrimaryIndexList.size(); i++) {
        file.seekg(SalesPrimaryIndexList[i].offset); Sales_ReadFromFile(file, s);
        s.DisplaySale(); cout << "------------------\n";
    }
}
void Sales_DisplayAvailList(fstream& file) {
    file.seekg(0); short header; file.read((char*)&header, sizeof(header));
    cout << "\n----- Sales Avail List -----\n";
    if (header == -1) { cout << "Empty\n"; return; }
    short curr = header;
    while (curr != -1) {
        cout << "Offset: " << curr;
        file.seekg(curr + sizeof(int) + sizeof(short) + sizeof(char));
        short next, size; file.read((char*)&next, sizeof(short)); file.read((char*)&size, sizeof(short));
        cout << " | Size: " << size << " | Next: " << next << "\n"; curr = next;
    }
    cout << "----------------------------\n";
}
void Sales_initFile() {
    fstream file("sales.txt", ios::binary | ios::in);
    if (!file) {
        ofstream out("sales.txt", ios::binary);
        short header = -1; out.write((char*)&header, sizeof(header)); out.close();
    }
}

void SalesMenu() {
    Sales_initFile();
    fstream file("sales.txt", ios::binary | ios::in | ios::out);
    Sales_ReadPrimaryIndexFile(); Sales_ReadSecondaryIndexFile();
    int choice; Sales s;
    do {
        cout << "\n========= SALES MENU =========\n";
        cout << "1.  Add New Sale\n";
        cout << "2.  Display All Sales\n";
        cout << "3.  Search By Sale ID\n";
        cout << "4.  Search By Customer Name\n";
        cout << "5.  Update By Sale ID\n";
        cout << "6.  Update By Customer Name\n";
        cout << "7.  Delete By Sale ID\n";
        cout << "8.  Delete By Customer Name\n";
        cout << "9.  Display Avail List\n";
        cout << "0.  Back to Main Menu\n";
        cout << "Enter Choice: "; cin >> choice;
        switch (choice) {
        case 1: s.ReadSale(); Sales_Insert(file, s); break;
        case 2: Sales_DisplayAll(file); break;
        case 3: { int id; cout << "Enter ID: "; cin >> id; Sales_SearchByID(file, id); break; }
        case 4: Sales_SearchByCustomerName(file); break;
        case 5: { int id; cout << "Enter ID: "; cin >> id; Sales_UpdateByID(file, id); break; }
        case 6: Sales_UpdateByCustomerName(file); break;
        case 7: { int id; cout << "Enter ID: "; cin >> id; Sales_DeleteByID(file, id); break; }
        case 8: Sales_DeleteByCustomerName(file); break;
        case 9: Sales_DisplayAvailList(file); break;
        case 0: break;
        default: cout << "Invalid choice!\n";
        }
        file.clear();
    } while (choice != 0);
    Sales_writePrimaryIndexFile(); Sales_writeSecondaryIndexFile(); file.close();
}