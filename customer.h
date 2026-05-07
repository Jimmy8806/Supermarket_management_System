#include <bits/stdc++.h>
#include <fstream>
#include <limits>
using namespace std;
struct CustPrimaryIndex {
    short id; int pos;
    bool operator<(const CustPrimaryIndex& pi) const { return id < pi.id; }
};
vector<CustPrimaryIndex> CustPrimaryIndexList;

void C_WritePrimaryIndexFile() {
    ofstream outFile("Customers_Primary_Index_List.txt", ios::binary | ios::out | ios::trunc);
    for (int i = 0; i < (int)CustPrimaryIndexList.size(); i++) {
        outFile.write((char*)&CustPrimaryIndexList[i].id, sizeof(CustPrimaryIndexList[i].id));
        outFile.write((char*)&CustPrimaryIndexList[i].pos, sizeof(CustPrimaryIndexList[i].pos));
    }
    outFile.close();
}
void C_ReadPrimaryIndexFile() {
    ifstream inFile("Customers_Primary_Index_List.txt", ios::binary | ios::in);
    if (!inFile) return;
    CustPrimaryIndex pi;
    while (true) {
        inFile.read((char*)&pi.id, sizeof(pi.id)); if (inFile.eof()) break;
        inFile.read((char*)&pi.pos, sizeof(pi.pos)); CustPrimaryIndexList.push_back(pi);
    }
    inFile.close();
}
void C_AddToPrimaryIndexList(CustPrimaryIndex pi) {
    CustPrimaryIndexList.push_back(pi);
    sort(CustPrimaryIndexList.begin(), CustPrimaryIndexList.end());
}
void C_DeleteFromPrimaryIndexList(const short& id) {
    int low = 0, high = (int)CustPrimaryIndexList.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (CustPrimaryIndexList[mid].id == id) { CustPrimaryIndexList.erase(CustPrimaryIndexList.begin() + mid); return; }
        else if (CustPrimaryIndexList[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
}
int C_SearchInPrimaryIndexList(const short& id) {
    int low = 0, high = (int)CustPrimaryIndexList.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (CustPrimaryIndexList[mid].id == id) return CustPrimaryIndexList[mid].pos;
        else if (CustPrimaryIndexList[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

struct CustSecondaryIndex {
    char name[30]; vector<short> ids;
    bool operator<(const CustSecondaryIndex& si) const { return strcmp(name, si.name) < 0; }
};
vector<CustSecondaryIndex> CustSecondaryIndexList;

void C_WriteSecondaryIndexFile() {
    ofstream outFile("Customers_Secondary_Index_List.txt", ios::binary | ios::out | ios::trunc);
    for (int i = 0; i < (int)CustSecondaryIndexList.size(); i++) {
        outFile.write(CustSecondaryIndexList[i].name, sizeof(CustSecondaryIndexList[i].name));
        int idCount = CustSecondaryIndexList[i].ids.size();
        outFile.write((char*)&idCount, sizeof(idCount));
        for (int j = 0; j < idCount; j++) outFile.write((char*)&CustSecondaryIndexList[i].ids[j], sizeof(CustSecondaryIndexList[i].ids[j]));
    }
    outFile.close();
}
void C_ReadSecondaryIndexFile() {
    ifstream inFile("Customers_Secondary_Index_List.txt", ios::binary | ios::in);
    if (!inFile) return;
    CustSecondaryIndexList.clear();
    while (true) {
        CustSecondaryIndex si; inFile.read(si.name, sizeof(si.name)); if (inFile.eof()) break;
        int idCount; inFile.read((char*)&idCount, sizeof(idCount));
        for (int j = 0; j < idCount; j++) { short tempId; inFile.read((char*)&tempId, sizeof(tempId)); si.ids.push_back(tempId); }
        CustSecondaryIndexList.push_back(si);
    }
    inFile.close();
}
void C_AddToSecondaryIndexList(char name[], short id) {
    for (int i = 0; i < (int)CustSecondaryIndexList.size(); i++) {
        if (strcmp(CustSecondaryIndexList[i].name, name) == 0) { CustSecondaryIndexList[i].ids.push_back(id); return; }
    }
    CustSecondaryIndex newSi; strncpy(newSi.name, name, 30); newSi.name[29] = '\0';
    newSi.ids.push_back(id); CustSecondaryIndexList.push_back(newSi);
    sort(CustSecondaryIndexList.begin(), CustSecondaryIndexList.end());
}
void C_DeleteFromSecondaryIndexList(char name[], short id) {
    int low = 0, high = (int)CustSecondaryIndexList.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2; int cmp = strcmp(CustSecondaryIndexList[mid].name, name);
        if (cmp == 0) {
            for (int i = 0; i < (int)CustSecondaryIndexList[mid].ids.size(); i++)
                if (CustSecondaryIndexList[mid].ids[i] == id) { CustSecondaryIndexList[mid].ids.erase(CustSecondaryIndexList[mid].ids.begin() + i); break; }
            return;
        } else if (cmp < 0) low = mid + 1; else high = mid - 1;
    }
}
vector<short> C_SearchInSecondaryIndexList(char name[]) {
    int low = 0, high = (int)CustSecondaryIndexList.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2; int cmp = strcmp(CustSecondaryIndexList[mid].name, name);
        if (cmp == 0) return CustSecondaryIndexList[mid].ids;
        else if (cmp < 0) low = mid + 1; else high = mid - 1;
    }
    return {};
}

class Customer {
public:
    short id; char name[30]; char address[20]; int phone;

    void fixInput() { if (cin.peek() == '\n') cin.ignore(1, '\n'); }

    void inputCustomer(bool isupdate = false) {
        if (!isupdate) {
            while (true) {
                short tempId; cout << "Enter Customer ID: "; cin >> tempId;
                if (C_SearchInPrimaryIndexList(tempId) != -1) cout << "Error: ID already exists.\n";
                else { id = tempId; break; }
            }
        }
        do {
            cout << "Enter Customer Name (min 5 chars): ";
            fixInput(); cin.getline(name, 30);
            if (strlen(name) < 5) cout << "Error: Name too short!\n";
        } while (strlen(name) < 5);
        cout << "Enter Customer Address: "; fixInput(); cin.getline(address, 20);
        cout << "Enter Customer Phone: "; cin >> phone;
    }
    void DisplayCustomer() {
        cout << "ID: " << id << " | Name: " << name << " | Address: " << address << " | Phone: " << phone << "\n";
    }
    short CalcLength() {
        return sizeof(id) + sizeof(short) + strlen(name) + sizeof(short) + strlen(address) + sizeof(phone);
    }
};

void C_WriteToFile(fstream& iofile, Customer c) {
    iofile.write((char*)&c.id, sizeof(c.id));
    short nameLen = strlen(c.name); iofile.write((char*)&nameLen, sizeof(nameLen)); iofile.write(c.name, nameLen);
    short addressLen = strlen(c.address); iofile.write((char*)&addressLen, sizeof(addressLen)); iofile.write(c.address, addressLen);
    iofile.write((char*)&c.phone, sizeof(c.phone));
}
void C_ReadFromFile(fstream& iofile, Customer& c) {
    iofile.read((char*)&c.id, sizeof(c.id));
    short nameLen; iofile.read((char*)&nameLen, sizeof(nameLen)); iofile.read(c.name, nameLen); c.name[nameLen] = '\0';
    short addressLen; iofile.read((char*)&addressLen, sizeof(addressLen)); iofile.read(c.address, addressLen); c.address[addressLen] = '\0';
    iofile.read((char*)&c.phone, sizeof(c.phone));
}

void C_FirstFit(fstream& iofile, Customer& c) {
    iofile.seekg(0); int header; iofile.read((char*)&header, sizeof(header)); iofile.clear();
    if (header == -1) {
        iofile.seekp(0, ios::end); C_AddToPrimaryIndexList({ c.id, (int)iofile.tellp() });
        C_AddToSecondaryIndexList(c.name, c.id); C_WriteToFile(iofile, c);
    } else {
        int currentPos = header, prevPos = -1; short neededSize = c.CalcLength();
        while (currentPos != -1) {
            iofile.seekg(currentPos + sizeof(c.id) + sizeof(char) + sizeof(short));
            int nextDeleted; short deletedSize;
            iofile.read((char*)&nextDeleted, sizeof(nextDeleted)); iofile.read((char*)&deletedSize, sizeof(deletedSize));
            if (deletedSize >= neededSize) {
                iofile.seekp(currentPos); C_AddToPrimaryIndexList({ c.id, currentPos });
                C_AddToSecondaryIndexList(c.name, c.id); C_WriteToFile(iofile, c);
                short padding = deletedSize - neededSize; while (padding--) iofile.put('~');
                if (prevPos == -1) { iofile.seekp(0); iofile.write((char*)&nextDeleted, sizeof(nextDeleted)); }
                else { iofile.seekp(prevPos + sizeof(c.id) + sizeof(char) + sizeof(short)); iofile.write((char*)&nextDeleted, sizeof(nextDeleted)); }
                return;
            }
            prevPos = currentPos; currentPos = nextDeleted;
        }
        iofile.seekp(0, ios::end); C_AddToPrimaryIndexList({ c.id, (int)iofile.tellp() });
        C_AddToSecondaryIndexList(c.name, c.id); C_WriteToFile(iofile, c);
    }
}

void C_AddNewCustomer(fstream& ioFile, Customer& c) { C_FirstFit(ioFile, c); cout << "Customer added successfully.\n"; }

void C_SearchByID(fstream& ioFile, const short id) {
    int pos = C_SearchInPrimaryIndexList(id);
    if (pos == -1) { cout << "Not Found.\n"; return; }
    Customer c; ioFile.seekg(pos); C_ReadFromFile(ioFile, c);
    cout << "Customer Found.\n"; c.DisplayCustomer(); cout << "--------------------------\n";
}

short C_SearchByName(fstream& ioFile, char name[]) {
    vector<short> matchingIds = C_SearchInSecondaryIndexList(name);
    if (matchingIds.empty()) { cout << "Name Not Found.\n"; return -1; }
    cout << "Found " << matchingIds.size() << " Customer(s) with name '" << name << "'.\n";
    cout << "Available IDs: ";
    for (int i = 0; i < (int)matchingIds.size(); i++) cout << matchingIds[i] << " ";
    cout << "\nEnter exact ID: ";
    short targetId;
    while (true) {
        cin >> targetId; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        for (int i = 0; i < (int)matchingIds.size(); i++)
            if (matchingIds[i] == targetId) { C_SearchByID(ioFile, matchingIds[i]); return targetId; }
        cout << "Invalid ID. Try again: ";
    }
}

void C_DeleteByID(fstream& ioFile, const short id) {
    ioFile.seekg(0); int header; ioFile.read((char*)&header, sizeof(header));
    int pos = C_SearchInPrimaryIndexList(id);
    if (pos == -1) { cout << "Not Found.\n"; return; }
    Customer c; ioFile.seekg(pos); C_ReadFromFile(ioFile, c); c.DisplayCustomer();
    ioFile.seekp(pos + sizeof(c.id) + sizeof(short)); ioFile.put('*');
    ioFile.write((char*)&header, sizeof(header)); short ds = c.CalcLength(); ioFile.write((char*)&ds, sizeof(ds));
    ioFile.seekp(0); int newHead = pos; ioFile.write((char*)&newHead, sizeof(newHead));
    C_DeleteFromPrimaryIndexList(id); C_DeleteFromSecondaryIndexList(c.name, id);
    cout << "Customer deleted successfully.\n";
}

void C_DeleteByName(fstream& ioFile, char name[]) {
    short targetId = C_SearchByName(ioFile, name);
    if (targetId != -1) C_DeleteByID(ioFile, targetId);
}

void C_UpdateByID(fstream& ioFile, const short id) {
    ioFile.seekg(0); int header; ioFile.read((char*)&header, sizeof(header));
    int pos = C_SearchInPrimaryIndexList(id);
    if (pos == -1) { cout << "Not Found.\n"; return; }
    Customer c; ioFile.seekg(pos); C_ReadFromFile(ioFile, c);
    cout << "Current Data:\n"; c.DisplayCustomer(); short oldLen = c.CalcLength();
    char oldName[30]; strncpy(oldName, c.name, 30);
    cout << "Edit Data:\n"; c.inputCustomer(true); short newLen = c.CalcLength();
    if (oldLen >= newLen) {
        ioFile.seekp(pos); C_WriteToFile(ioFile, c);
        short padding = oldLen - newLen; while (padding--) ioFile.put('~');
        if (strcmp(oldName, c.name) != 0) { C_DeleteFromSecondaryIndexList(oldName, id); C_AddToSecondaryIndexList(c.name, id); }
    } else {
        ioFile.seekp(pos + sizeof(c.id) + sizeof(short)); ioFile.put('*');
        ioFile.write((char*)&header, sizeof(header)); ioFile.write((char*)&oldLen, sizeof(oldLen));
        ioFile.seekp(0); int newHead = pos; ioFile.write((char*)&newHead, sizeof(newHead));
        C_DeleteFromPrimaryIndexList(id); C_DeleteFromSecondaryIndexList(oldName, id);
        C_AddNewCustomer(ioFile, c);
    }
    cout << "Customer updated successfully.\n";
}

void C_UpdateByName(fstream& ioFile, char name[]) {
    short targetId = C_SearchByName(ioFile, name);
    if (targetId != -1) C_UpdateByID(ioFile, targetId);
}

void C_DisplayAll(fstream& iofile) {
    Customer c;
    cout << "================== Customers ==================\n";
    for (int i = 0; i < (int)CustPrimaryIndexList.size(); i++) {
        iofile.seekg(CustPrimaryIndexList[i].pos); C_ReadFromFile(iofile, c); c.DisplayCustomer();
        cout << "-----------------------------------------------\n";
    }
}
void C_DisplayTop5(fstream& iofile) {
    Customer c;
    cout << "================== Top 5 Customers ==================\n";
    for (int i = 0; i < 5 && i < (int)CustPrimaryIndexList.size(); i++) {
        iofile.seekg(CustPrimaryIndexList[i].pos); C_ReadFromFile(iofile, c); c.DisplayCustomer();
        cout << "-----------------------------------------------\n";
    }
}
void C_DisplayAvailList(fstream& ioFile) {
    ioFile.seekg(0); int header; ioFile.read((char*)&header, sizeof(header));
    cout << "\n----- Customer Avail List -----\n";
    if (header == -1) { cout << "Empty\n"; return; }
    int currentPos = header;
    while (currentPos != -1) {
        cout << "Offset: " << currentPos;
        ioFile.seekg(currentPos + sizeof(short) + sizeof(char) + sizeof(short));
        int nextDeleted; ioFile.read((char*)&nextDeleted, sizeof(nextDeleted));
        short deletedSize; ioFile.read((char*)&deletedSize, sizeof(deletedSize));
        cout << " | Size: " << deletedSize << " | Next: " << nextDeleted << "\n"; currentPos = nextDeleted;
    }
    cout << "-------------------------------\n";
}
void C_initFile() {
    fstream ioFile("Customers_File.txt", ios::binary | ios::in | ios::out);
    if (!ioFile) {
        ofstream out("Customers_File.txt", ios::binary | ios::out);
        int header = -1; out.write((char*)&header, sizeof(header)); out.close();
    }
}

void CustomersMenu() {
    C_initFile();
    fstream ioFile("Customers_File.txt", ios::binary | ios::in | ios::out);
    C_ReadPrimaryIndexFile(); C_ReadSecondaryIndexFile();
    short choice; short id; Customer c;
    do {
        cout << "\n========= CUSTOMERS MENU =========\n";
        cout << "1. Add New Customer\n";
        cout << "2. Display All Customers\n";
        cout << "3. Display Top 5 Customers\n";
        cout << "4. Search By ID\n";
        cout << "5. Search By Name\n";
        cout << "6. Delete By ID\n";
        cout << "7. Delete By Name\n";
        cout << "8. Update By ID\n";
        cout << "9. Update By Name\n";
        cout << "10. Display Avail List\n";
        cout << "0.  Back to Main Menu\n";
        cout << "Enter Choice: "; cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (choice) {
        case 1: c.inputCustomer(); C_AddNewCustomer(ioFile, c); break;
        case 2: C_DisplayAll(ioFile); break;
        case 3: C_DisplayTop5(ioFile); break;
        case 4: { cout << "Enter ID: "; cin >> id; cin.ignore(numeric_limits<streamsize>::max(), '\n'); C_SearchByID(ioFile, id); break; }
        case 5: { char name[30]; cout << "Enter Name: "; cin.getline(name, 30); C_SearchByName(ioFile, name); break; }
        case 6: { cout << "Enter ID: "; cin >> id; cin.ignore(numeric_limits<streamsize>::max(), '\n'); C_DeleteByID(ioFile, id); break; }
        case 7: { char name[30]; cout << "Enter Name: "; cin.getline(name, 30); C_DeleteByName(ioFile, name); break; }
        case 8: { cout << "Enter ID: "; cin >> id; cin.ignore(numeric_limits<streamsize>::max(), '\n'); C_UpdateByID(ioFile, id); break; }
        case 9: { char name[30]; cout << "Enter Name: "; cin.getline(name, 30); C_UpdateByName(ioFile, name); break; }
        case 10: C_DisplayAvailList(ioFile); break;
        case 0: break;
        default: cout << "Invalid choice!\n";
        }
        ioFile.clear();
    } while (choice != 0);
    C_WritePrimaryIndexFile(); C_WriteSecondaryIndexFile(); ioFile.close();
}