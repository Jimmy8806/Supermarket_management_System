#include <bits/stdc++.h>
#include <fstream>
#include <limits>
using namespace std;

struct ProdSecondaryIndex {
    char name[50]; int id;
    bool operator<(const ProdSecondaryIndex& si) const { return string(name) < string(si.name); }
};
vector<ProdSecondaryIndex> ProdSecondaryIndexList;

void Prod_writeSecondaryIndexFile() {
    ofstream outFile("Products_SecondaryIndex.txt", ios::binary | ios::out | ios::trunc);
    for (int i = 0; i < (int)ProdSecondaryIndexList.size(); i++)
        outFile.write((char*)&ProdSecondaryIndexList[i], sizeof(ProdSecondaryIndex));
    outFile.close();
}
void Prod_ReadSecondaryIndexFile() {
    ifstream inFile("Products_SecondaryIndex.txt", ios::binary | ios::in);
    if (!inFile) return;
    ProdSecondaryIndex si; ProdSecondaryIndexList.clear();
    while (inFile.read((char*)&si, sizeof(si))) ProdSecondaryIndexList.push_back(si);
    inFile.close();
}
void Prod_AddToSecondaryIndexList(ProdSecondaryIndex si) {
    ProdSecondaryIndexList.push_back(si);
    sort(ProdSecondaryIndexList.begin(), ProdSecondaryIndexList.end());
}
void Prod_DeleteFromSecondaryIndexList(int id) {
    for (int i = 0; i < (int)ProdSecondaryIndexList.size(); i++)
        if (ProdSecondaryIndexList[i].id == id) { ProdSecondaryIndexList.erase(ProdSecondaryIndexList.begin() + i); return; }
}
vector<int> Prod_searchInSecondaryIndexList(char name[]) {
    vector<int> foundIds;
    for (int i = 0; i < (int)ProdSecondaryIndexList.size(); i++)
        if (string(ProdSecondaryIndexList[i].name) == string(name))
            foundIds.push_back(ProdSecondaryIndexList[i].id);
    return foundIds;
}

struct ProdPrimaryIndex {
    int id, offset;
    bool operator<(const ProdPrimaryIndex& pi) const { return id < pi.id; }
};
vector<ProdPrimaryIndex> ProdPrimaryIndexList;

void Prod_writePrimaryIndexFile() {
    ofstream outFile("Products_PrimaryIndex.txt", ios::binary | ios::out | ios::trunc);
    for (int i = 0; i < (int)ProdPrimaryIndexList.size(); i++)
        outFile.write((char*)&ProdPrimaryIndexList[i], sizeof(ProdPrimaryIndex));
    outFile.close();
}
void Prod_ReadPrimaryIndexFile() {
    ifstream inFile("Products_PrimaryIndex.txt", ios::binary | ios::in);
    if (!inFile) return;
    ProdPrimaryIndex pi;
    while (inFile.read((char*)&pi, sizeof(pi))) ProdPrimaryIndexList.push_back(pi);
    inFile.close();
}
void Prod_AddToPrimaryIndexList(ProdPrimaryIndex pi) {
    ProdPrimaryIndexList.push_back(pi);
    sort(ProdPrimaryIndexList.begin(), ProdPrimaryIndexList.end());
}
void Prod_deleteFromPrimaryIndexList(const int& id) {
    int low = 0, high = (int)ProdPrimaryIndexList.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (ProdPrimaryIndexList[mid].id == id) { ProdPrimaryIndexList.erase(ProdPrimaryIndexList.begin() + mid); return; }
        else if (ProdPrimaryIndexList[mid].id < id) low = mid + 1; else high = mid - 1;
    }
}
int Prod_searchInPrimaryIndexList(const int& id) {
    int low = 0, high = (int)ProdPrimaryIndexList.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (ProdPrimaryIndexList[mid].id == id) return ProdPrimaryIndexList[mid].offset;
        else if (ProdPrimaryIndexList[mid].id < id) low = mid + 1; else high = mid - 1;
    }
    return -1;
}

class Products {
public:
    int id; char name[50]; char category[50]; float price; int quantity;

    void FixInput() { if (cin.peek() == '\n') cin.ignore(); }

    void ReadProduct(bool isupdate = false) {
        if (!isupdate) {
            while (true) {
                int tempId; cout << "Enter Product ID: "; cin >> tempId;
                if (Prod_searchInPrimaryIndexList(tempId) != -1) cout << "Error: ID already exists.\n";
                else { id = tempId; break; }
            }
        }
        do {
            cout << "Enter Product Name (min 5 chars): "; FixInput(); cin.getline(name, 50);
            if (strlen(name) < 5) cout << "Error: Name too short!\n";
        } while (strlen(name) < 5);
        cout << "Enter Category: "; FixInput(); cin.getline(category, 50);
        cout << "Enter Price: "; cin >> price;
        cout << "Enter Quantity: "; cin >> quantity;
    }
    void DisplayProduct() {
        cout << "ID: " << id << " | Name: " << name << " | Category: " << category
             << " | Price: " << price << " | Qty: " << quantity << "\n";
    }
    short CalcLength() {
        return sizeof(int) + sizeof(float) + sizeof(int) + strlen(name) + strlen(category) + 2 * sizeof(short);
    }
};

void Prod_WriteToFile(fstream& iofile, Products p) {
    iofile.write((char*)&p.id, sizeof(p.id));
    short len = strlen(p.name); iofile.write((char*)&len, sizeof(short)); iofile.write(p.name, len);
    len = strlen(p.category); iofile.write((char*)&len, sizeof(short)); iofile.write(p.category, len);
    iofile.write((char*)&p.price, sizeof(float)); iofile.write((char*)&p.quantity, sizeof(int));
}
void Prod_ReadFromFile(fstream& iofile, Products& p) {
    iofile.read((char*)&p.id, sizeof(p.id));
    short len; iofile.read((char*)&len, sizeof(short)); iofile.read(p.name, len); p.name[len] = '\0';
    iofile.read((char*)&len, sizeof(short)); iofile.read(p.category, len); p.category[len] = '\0';
    iofile.read((char*)&p.price, sizeof(float)); iofile.read((char*)&p.quantity, sizeof(int));
}

void Prod_FirstFit(fstream& iofile, Products& p) {
    iofile.seekg(0); short header; iofile.read((char*)&header, sizeof(header));
    if (iofile.fail()) iofile.clear();
    short needed = p.CalcLength();
    if (header == -1) {
        iofile.seekp(0, ios::end); int pos = iofile.tellp();
        Prod_WriteToFile(iofile, p); Prod_AddToPrimaryIndexList({ p.id, pos }); return;
    }
    short currPos = header, prevPos = -1;
    while (currPos != -1) {
        iofile.seekg(currPos + sizeof(int) + sizeof(short) + sizeof(char));
        short nextDeleted, deletedSize;
        iofile.read((char*)&nextDeleted, sizeof(short)); iofile.read((char*)&deletedSize, sizeof(short));
        if (deletedSize >= needed) {
            iofile.seekp(currPos); int pos = iofile.tellp();
            Prod_WriteToFile(iofile, p); Prod_AddToPrimaryIndexList({ p.id, pos });
            short padding = deletedSize - needed; while (padding--) iofile.put('~');
            if (prevPos == -1) { iofile.seekp(0); iofile.write((char*)&nextDeleted, sizeof(nextDeleted)); }
            else { iofile.seekp(prevPos + sizeof(int) + sizeof(short) + sizeof(char)); iofile.write((char*)&nextDeleted, sizeof(nextDeleted)); }
            return;
        }
        prevPos = currPos; currPos = nextDeleted;
    }
    iofile.seekp(0, ios::end); int pos = iofile.tellp();
    Prod_WriteToFile(iofile, p); Prod_AddToPrimaryIndexList({ p.id, pos });
}

void Prod_Insert(fstream& iofile, Products& p) {
    Prod_FirstFit(iofile, p);
    ProdSecondaryIndex si; strncpy(si.name, p.name, 50); si.name[49] = '\0'; si.id = p.id;
    Prod_AddToSecondaryIndexList(si); cout << "Product added successfully!\n";
}
void Prod_SearchByID(fstream& iofile, const int id) {
    int offset = Prod_searchInPrimaryIndexList(id);
    if (offset == -1) { cout << "Not Found.\n"; return; }
    Products p; iofile.seekg(offset); Prod_ReadFromFile(iofile, p);
    cout << "Product Found.\n"; p.DisplayProduct(); cout << "--------------------------\n";
}
void Prod_DeleteByID(fstream& ioFile, const int id) {
    ioFile.seekg(0); short header; ioFile.read((char*)&header, sizeof(header));
    short offset = Prod_searchInPrimaryIndexList(id);
    if (offset == -1) { cout << "Not Found.\n"; return; }
    Products p; ioFile.seekg(offset); Prod_ReadFromFile(ioFile, p); p.DisplayProduct();
    ioFile.seekp(offset + sizeof(int) + sizeof(short)); ioFile.put('*');
    ioFile.write((char*)&header, sizeof(header)); short ds = p.CalcLength(); ioFile.write((char*)&ds, sizeof(ds));
    ioFile.seekp(0); ioFile.write((char*)&offset, sizeof(offset));
    Prod_DeleteFromSecondaryIndexList(id); Prod_deleteFromPrimaryIndexList(id);
    cout << "Product deleted successfully.\n";
}
void Prod_UpdateByID(fstream& ioFile, const int id) {
    ioFile.seekg(0); short header; ioFile.read((char*)&header, sizeof(header));
    short offset = Prod_searchInPrimaryIndexList(id);
    if (offset == -1) { cout << "Not Found.\n"; return; }
    Products p; ioFile.seekg(offset); Prod_ReadFromFile(ioFile, p);
    cout << "Current Data:\n"; p.DisplayProduct(); short oldLen = p.CalcLength();
    cout << "Edit Data:\n"; p.ReadProduct(true); short newLen = p.CalcLength();
    Prod_DeleteFromSecondaryIndexList(id);
    ProdSecondaryIndex newSi; newSi.id = p.id; strncpy(newSi.name, p.name, 50); Prod_AddToSecondaryIndexList(newSi);
    if (oldLen >= newLen) {
        ioFile.seekp(offset); Prod_WriteToFile(ioFile, p);
        short padding = oldLen - newLen; while (padding--) ioFile.put('~');
    } else {
        ioFile.seekp(offset + sizeof(int) + sizeof(short)); ioFile.put('*');
        ioFile.write((char*)&header, sizeof(header)); ioFile.write((char*)&oldLen, sizeof(oldLen));
        ioFile.seekp(0); ioFile.write((char*)&offset, sizeof(offset));
        Prod_deleteFromPrimaryIndexList(id);
        ioFile.seekp(0, ios::end); ProdPrimaryIndex pi = { p.id, (int)ioFile.tellp() };
        Prod_AddToPrimaryIndexList(pi); Prod_WriteToFile(ioFile, p);
    }
    cout << "Product updated successfully.\n";
}
void Prod_SearchByName(fstream& iofile, char name[]) {
    vector<int> ids = Prod_searchInSecondaryIndexList(name);
    if (ids.empty()) { cout << "No products found with name: " << name << "\n"; return; }
    cout << "Found " << ids.size() << " product(s).\n";
    for (int i = 0; i < (int)ids.size(); i++) Prod_SearchByID(iofile, ids[i]);
}
void Prod_DeleteByName(fstream& iofile) {
    char name[50]; cout << "Enter Product Name to Delete: ";
    Products dummy; dummy.FixInput(); cin.getline(name, 50);
    vector<int> ids = Prod_searchInSecondaryIndexList(name);
    if (ids.empty()) { cout << "No products found with name '" << name << "'.\n"; }
    else if (ids.size() == 1) { Prod_DeleteByID(iofile, ids[0]); }
    else {
        cout << "Multiple found. Enter ID to delete:\n";
        for (int i = 0; i < (int)ids.size(); i++) cout << " >> ID: " << ids[i] << "\n";
        int targetId; cout << "Enter ID: "; cin >> targetId; Prod_DeleteByID(iofile, targetId);
    }
}
void Prod_UpdateByName(fstream& iofile) {
    char name[50]; cout << "Enter Product Name to Update: ";
    Products dummy; dummy.FixInput(); cin.getline(name, 50);
    vector<int> ids = Prod_searchInSecondaryIndexList(name);
    if (ids.empty()) { cout << "Product name not found.\n"; }
    else if (ids.size() == 1) { Prod_UpdateByID(iofile, ids[0]); }
    else {
        cout << "Multiple found. Select ID:\n";
        for (int i = 0; i < (int)ids.size(); i++) cout << " >> Option " << i+1 << " - ID: " << ids[i] << "\n";
        int targetId; cout << "Enter ID: "; cin >> targetId;
        bool valid = false;
        for (int i = 0; i < (int)ids.size(); i++) if (ids[i] == targetId) { valid = true; break; }
        if (valid) Prod_UpdateByID(iofile, targetId);
        else cout << "Invalid ID.\n";
    }
}
void Prod_DisplayAll(fstream& iofile) {
    cout << "================== Products ==================\n";
    Products p;
    for (int i = 0; i < (int)ProdPrimaryIndexList.size(); i++) {
        iofile.seekg(ProdPrimaryIndexList[i].offset); Prod_ReadFromFile(iofile, p);
        if (p.name[0] != '*') { p.DisplayProduct(); cout << "------------------------------\n"; }
    }
}
void Prod_DisplayTop5ByPrice(fstream& iofile) {
    vector<Products> temp; Products p;
    for (int i = 0; i < (int)ProdPrimaryIndexList.size(); i++) {
        iofile.seekg(ProdPrimaryIndexList[i].offset); Prod_ReadFromFile(iofile, p);
        if (p.name[0] != '*') temp.push_back(p);
    }
    for (int i = 0; i < (int)temp.size(); i++)
        for (int j = 0; j < (int)temp.size() - 1; j++)
            if (temp[j].price < temp[j+1].price) swap(temp[j], temp[j+1]);
    cout << "\n--- Top 5 Products by Price ---\n";
    int limit = min((int)temp.size(), 5);
    for (int i = 0; i < limit; i++) { temp[i].DisplayProduct(); cout << "--------------------------\n"; }
}
void Prod_DisplayAvailList(fstream& ioFile) {
    ioFile.seekg(0); short header; ioFile.read((char*)&header, sizeof(header));
    cout << "\n----- Products Avail List -----\n";
    if (header == -1) { cout << "Empty\n"; return; }
    short currentPos = header;
    while (currentPos != -1) {
        cout << "Offset: " << currentPos;
        ioFile.seekg(currentPos + sizeof(int) + sizeof(short) + sizeof(char));
        short nextDeleted, deletedSize;
        ioFile.read((char*)&nextDeleted, sizeof(short)); ioFile.read((char*)&deletedSize, sizeof(short));
        cout << " | Size: " << deletedSize << " | Next: " << nextDeleted << "\n"; currentPos = nextDeleted;
    }
    cout << "-------------------------------\n";
}
void Prod_initFile() {
    fstream ioFile("products.txt", ios::binary | ios::in | ios::out);
    if (!ioFile) {
        ofstream out("products.txt", ios::binary | ios::out);
        short header = -1; out.write((char*)&header, sizeof(header)); out.close();
    }
}

void ProductsMenu() {
    Prod_initFile();
    fstream iofile("products.txt", ios::binary | ios::in | ios::out);
    Prod_ReadPrimaryIndexFile(); Prod_ReadSecondaryIndexFile();
    int choice; int id; Products p;
    do {
        cout << "\n========= PRODUCTS MENU =========\n";
        cout << "1.  Add New Product\n";
        cout << "2.  Display All Products\n";
        cout << "3.  Search By ID\n";
        cout << "4.  Search By Name\n";
        cout << "5.  Update By ID\n";
        cout << "6.  Update By Name\n";
        cout << "7.  Delete By ID\n";
        cout << "8.  Delete By Name\n";
        cout << "9.  Display Avail List\n";
        cout << "10. Display Top 5 Products By Price\n";
        cout << "0.  Back to Main Menu\n";
        cout << "Enter Choice: "; cin >> choice;
        switch (choice) {
        case 1: p.ReadProduct(); Prod_Insert(iofile, p); break;
        case 2: Prod_DisplayAll(iofile); break;
        case 3: { cout << "Enter ID: "; cin >> id; Prod_SearchByID(iofile, id); break; }
        case 4: { char sName[50]; cout << "Enter Name: "; cin.ignore(); cin.getline(sName, 50); Prod_SearchByName(iofile, sName); break; }
        case 5: { cout << "Enter ID: "; cin >> id; Prod_UpdateByID(iofile, id); break; }
        case 6: Prod_UpdateByName(iofile); break;
        case 7: { cout << "Enter ID: "; cin >> id; Prod_DeleteByID(iofile, id); break; }
        case 8: Prod_DeleteByName(iofile); break;
        case 9: Prod_DisplayAvailList(iofile); break;
        case 10: Prod_DisplayTop5ByPrice(iofile); break;
        case 0: break;
        default: cout << "Invalid choice!\n";
        }
        iofile.clear();
    } while (choice != 0);
    Prod_writePrimaryIndexFile(); Prod_writeSecondaryIndexFile(); iofile.close();
}