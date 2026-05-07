#include <bits/stdc++.h>
#include <fstream>
#include <limits>
using namespace std;
struct StaffSecondaryIndex {
    char name[30];
    int id;
    bool operator<(const StaffSecondaryIndex& si) const {
        return string(name) < string(si.name);
    }
};
vector<StaffSecondaryIndex> StaffSecondaryIndexList;

void Staff_writeSecondaryIndexFile() {
    ofstream outFile("Staff_SecondaryIndex.txt", ios::binary | ios::out | ios::trunc);
    for (int i = 0; i < (int)StaffSecondaryIndexList.size(); i++)
        outFile.write((char*)&StaffSecondaryIndexList[i], sizeof(StaffSecondaryIndex));
    outFile.close();
}
void Staff_ReadSecondaryIndexFile() {
    ifstream inFile("Staff_SecondaryIndex.txt", ios::binary | ios::in);
    if (!inFile) return;
    StaffSecondaryIndex si;
    StaffSecondaryIndexList.clear();
    while (inFile.read((char*)&si, sizeof(si)))
        StaffSecondaryIndexList.push_back(si);
    inFile.close();
}
void Staff_AddToSecondaryIndexList(StaffSecondaryIndex si) {
    StaffSecondaryIndexList.push_back(si);
    sort(StaffSecondaryIndexList.begin(), StaffSecondaryIndexList.end());
}
void Staff_DeleteFromSecondaryIndexList(int id) {
    for (int i = 0; i < (int)StaffSecondaryIndexList.size(); i++) {
        if (StaffSecondaryIndexList[i].id == id) {
            StaffSecondaryIndexList.erase(StaffSecondaryIndexList.begin() + i);
            return;
        }
    }
}
vector<int> Staff_searchInSecondaryIndexList(char name[]) {
    vector<int> foundIds;
    for (int i = 0; i < (int)StaffSecondaryIndexList.size(); i++)
        if (string(StaffSecondaryIndexList[i].name) == string(name))
            foundIds.push_back(StaffSecondaryIndexList[i].id);
    return foundIds;
}

struct StaffPrimaryIndex {
    int id, offset;
    bool operator<(const StaffPrimaryIndex& pi) const { return id < pi.id; }
};
vector<StaffPrimaryIndex> StaffPrimaryIndexList;

void Staff_writePrimaryIndexFile() {
    ofstream outFile("Staff_PrimaryIndex.txt", ios::binary | ios::out | ios::trunc);
    for (int i = 0; i < (int)StaffPrimaryIndexList.size(); i++)
        outFile.write((char*)&StaffPrimaryIndexList[i], sizeof(StaffPrimaryIndex));
    outFile.close();
}
void Staff_ReadPrimaryIndexFile() {
    ifstream inFile("Staff_PrimaryIndex.txt", ios::binary | ios::in);
    if (!inFile) return;
    StaffPrimaryIndex pi;
    while (inFile.read((char*)&pi, sizeof(pi)))
        StaffPrimaryIndexList.push_back(pi);
    inFile.close();
}
void Staff_AddToPrimaryIndexList(StaffPrimaryIndex pi) {
    StaffPrimaryIndexList.push_back(pi);
    sort(StaffPrimaryIndexList.begin(), StaffPrimaryIndexList.end());
}
int Staff_searchInPrimaryIndexList(const int& id) {
    int low = 0, high = (int)StaffPrimaryIndexList.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (StaffPrimaryIndexList[mid].id == id) return StaffPrimaryIndexList[mid].offset;
        else if (StaffPrimaryIndexList[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}
void Staff_deleteFromPrimaryIndexList(const int& id) {
    int low = 0, high = (int)StaffPrimaryIndexList.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (StaffPrimaryIndexList[mid].id == id) { StaffPrimaryIndexList.erase(StaffPrimaryIndexList.begin() + mid); return; }
        else if (StaffPrimaryIndexList[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
}

class Staff {
public:
    int id;
    char name[30];
    char jobTitle[20];

    void FixInput() { if (cin.peek() == '\n') cin.ignore(); }

    void ReadStaff(bool isUpdate = false) {
        if (!isUpdate) {
            while (true) {
                cout << "Enter Staff ID: ";
                int tempId; cin >> tempId;
                if (Staff_searchInPrimaryIndexList(tempId) != -1)
                    cout << "Error: ID " << tempId << " already exists.\n";
                else { id = tempId; break; }
            }
        }
        do {
            cout << "Enter Staff Name (min 3 chars): ";
            FixInput(); cin.getline(name, 30);
            if (strlen(name) < 3) cout << "Error: Name too short!\n";
        } while (strlen(name) < 3);

        cout << "Enter Staff Job Title: ";
        FixInput(); cin.getline(jobTitle, 20);
    }

    void DisplayStaff() {
        cout << "ID        : " << id << "\n";
        cout << "Name      : " << name << "\n";
        cout << "Job Title : " << jobTitle << "\n";
    }

    short CalcLength() {
        return sizeof(int) + strlen(name) + strlen(jobTitle) + 2 * sizeof(short);
    }
};

void Staff_WriteToFile(fstream& file, Staff& s) {
    file.write((char*)&s.id, sizeof(s.id));
    short len = strlen(s.name);
    file.write((char*)&len, sizeof(short)); file.write(s.name, len);
    len = strlen(s.jobTitle);
    file.write((char*)&len, sizeof(short)); file.write(s.jobTitle, len);
}
void Staff_ReadFromFile(fstream& file, Staff& s) {
    file.read((char*)&s.id, sizeof(s.id));
    short len;
    file.read((char*)&len, sizeof(short)); file.read(s.name, len); s.name[len] = '\0';
    file.read((char*)&len, sizeof(short)); file.read(s.jobTitle, len); s.jobTitle[len] = '\0';
}

void Staff_FirstFit(fstream& file, Staff& s) {
    file.seekg(0); short header; file.read((char*)&header, sizeof(header));
    if (file.fail()) file.clear();
    short needed = s.CalcLength();
    if (header == -1) {
        file.seekp(0, ios::end); int pos = (int)file.tellp();
        Staff_WriteToFile(file, s); Staff_AddToPrimaryIndexList({ s.id, pos }); return;
    }
    short curr = header, prev = -1;
    while (curr != -1) {
        file.seekg(curr + sizeof(int) + sizeof(short) + sizeof(char));
        short next, size; file.read((char*)&next, sizeof(short)); file.read((char*)&size, sizeof(short));
        if (size >= needed) {
            file.seekp(curr); int pos = (int)file.tellp();
            Staff_WriteToFile(file, s); Staff_AddToPrimaryIndexList({ s.id, pos });
            short padding = size - needed; while (padding--) file.put('~');
            if (prev == -1) { file.seekp(0); file.write((char*)&next, sizeof(next)); }
            else { file.seekp(prev + sizeof(int) + sizeof(short) + sizeof(char)); file.write((char*)&next, sizeof(next)); }
            return;
        }
        prev = curr; curr = next;
    }
    file.seekp(0, ios::end); int pos = (int)file.tellp();
    Staff_WriteToFile(file, s); Staff_AddToPrimaryIndexList({ s.id, pos });
}

void Staff_Insert(fstream& file, Staff& s) {
    Staff_FirstFit(file, s);
    StaffSecondaryIndex si; strncpy(si.name, s.name, 30); si.name[29] = '\0'; si.id = s.id;
    Staff_AddToSecondaryIndexList(si);
    cout << "Staff added successfully!\n";
}
void Staff_DisplayAll(fstream& file) {
    cout << "========= All Staff =========\n";
    Staff s;
    for (int i = 0; i < (int)StaffPrimaryIndexList.size(); i++) {
        file.seekg(StaffPrimaryIndexList[i].offset); Staff_ReadFromFile(file, s);
        if (s.name[0] != '*') { s.DisplayStaff(); cout << "-----------------------------\n"; }
    }
}
void Staff_SearchByID(fstream& file, int id) {
    int offset = Staff_searchInPrimaryIndexList(id);
    if (offset == -1) { cout << "Not Found.\n"; return; }
    Staff s; file.seekg(offset); Staff_ReadFromFile(file, s);
    cout << "Staff Found.\n"; s.DisplayStaff(); cout << "-----------------------------\n";
}
void Staff_DeleteByID(fstream& file, int id) {
    file.seekg(0); short header; file.read((char*)&header, sizeof(header));
    int offset = Staff_searchInPrimaryIndexList(id);
    if (offset == -1) { cout << "Not Found.\n"; return; }
    Staff s; file.seekg(offset); Staff_ReadFromFile(file, s); s.DisplayStaff();
    file.seekp(offset + sizeof(int) + sizeof(short)); file.put('*');
    file.write((char*)&header, sizeof(header)); short ds = s.CalcLength(); file.write((char*)&ds, sizeof(ds));
    file.seekp(0); short so = (short)offset; file.write((char*)&so, sizeof(so));
    Staff_DeleteFromSecondaryIndexList(id); Staff_deleteFromPrimaryIndexList(id);
    cout << "Staff deleted successfully.\n";
}
void Staff_UpdateByID(fstream& file, int id) {
    file.seekg(0); short header; file.read((char*)&header, sizeof(header));
    int offset = Staff_searchInPrimaryIndexList(id);
    if (offset == -1) { cout << "Not Found.\n"; return; }
    Staff s; file.seekg(offset); Staff_ReadFromFile(file, s);
    cout << "Current Data:\n"; s.DisplayStaff(); short oldLen = s.CalcLength();
    cout << "Enter New Data:\n"; s.ReadStaff(true); short newLen = s.CalcLength();
    Staff_DeleteFromSecondaryIndexList(id);
    StaffSecondaryIndex newSi; newSi.id = s.id; strncpy(newSi.name, s.name, 30); newSi.name[29] = '\0';
    Staff_AddToSecondaryIndexList(newSi);
    if (oldLen >= newLen) {
        file.seekp(offset); Staff_WriteToFile(file, s);
        short padding = oldLen - newLen; while (padding--) file.put('~');
    } else {
        file.seekp(offset + sizeof(int) + sizeof(short)); file.put('*');
        file.write((char*)&header, sizeof(header)); file.write((char*)&oldLen, sizeof(oldLen));
        file.seekp(0); short so = (short)offset; file.write((char*)&so, sizeof(so));
        Staff_deleteFromPrimaryIndexList(id);
        file.seekp(0, ios::end); StaffPrimaryIndex pi = { s.id, (int)file.tellp() };
        Staff_AddToPrimaryIndexList(pi); Staff_WriteToFile(file, s);
    }
    cout << "Staff updated successfully.\n";
}
void Staff_SearchByName(fstream& file, char name[]) {
    vector<int> ids = Staff_searchInSecondaryIndexList(name);
    if (ids.empty()) { cout << "No staff found with name: " << name << "\n"; return; }
    cout << "Found " << ids.size() << " staff member(s).\n";
    for (int i = 0; i < (int)ids.size(); i++) Staff_SearchByID(file, ids[i]);
}
void Staff_DeleteByName(fstream& file) {
    char name[30]; cout << "Enter Staff Name to Delete: ";
    Staff dummy; dummy.FixInput(); cin.getline(name, 30);
    vector<int> ids = Staff_searchInSecondaryIndexList(name);
    if (ids.empty()) { cout << "No staff found with name '" << name << "'.\n"; }
    else if (ids.size() == 1) { Staff_DeleteByID(file, ids[0]); }
    else {
        cout << "Multiple staff found. Enter ID to delete:\n";
        for (int i = 0; i < (int)ids.size(); i++) cout << " >> ID: " << ids[i] << "\n";
        int targetId; cout << "Enter ID: "; cin >> targetId;
        Staff_DeleteByID(file, targetId);
    }
}
void Staff_UpdateByName(fstream& file) {
    char name[30]; cout << "Enter Staff Name to Update: ";
    Staff dummy; dummy.FixInput(); cin.getline(name, 30);
    vector<int> ids = Staff_searchInSecondaryIndexList(name);
    if (ids.empty()) { cout << "Staff name not found.\n"; }
    else if (ids.size() == 1) { Staff_UpdateByID(file, ids[0]); }
    else {
        cout << "Multiple staff found. Select ID:\n";
        for (int i = 0; i < (int)ids.size(); i++) cout << " >> ID: " << ids[i] << "\n";
        int targetId; cout << "Enter ID: "; cin >> targetId;
        bool valid = false;
        for (int i = 0; i < (int)ids.size(); i++) if (ids[i] == targetId) { valid = true; break; }
        if (valid) Staff_UpdateByID(file, targetId);
        else cout << "Invalid ID.\n";
    }
}
void Staff_DisplayAvailList(fstream& file) {
    file.seekg(0); short header; file.read((char*)&header, sizeof(header));
    cout << "\n----- Staff Avail List -----\n";
    if (header == -1) { cout << "Empty\n"; return; }
    short curr = header;
    while (curr != -1) {
        cout << "Offset: " << curr;
        file.seekg(curr + sizeof(int) + sizeof(short) + sizeof(char));
        short next, size; file.read((char*)&next, sizeof(short)); file.read((char*)&size, sizeof(short));
        cout << " | Size: " << size << " | Next: " << next << "\n";
        curr = next;
    }
    cout << "----------------------------\n";
}
void Staff_DisplayTop5(fstream& file) {
    vector<Staff> temp; Staff s;
    for (int i = 0; i < (int)StaffPrimaryIndexList.size(); i++) {
        file.seekg(StaffPrimaryIndexList[i].offset); Staff_ReadFromFile(file, s);
        if (s.name[0] != '*') temp.push_back(s);
    }
    for (int i = 0; i < (int)temp.size(); i++)
        for (int j = 0; j < (int)temp.size() - 1; j++)
            if (temp[j].id < temp[j + 1].id) swap(temp[j], temp[j + 1]);
    cout << "\n--- Top 5 Staff by ID (Descending) ---\n";
    int limit = min((int)temp.size(), 5);
    for (int i = 0; i < limit; i++) { temp[i].DisplayStaff(); cout << "-----------------------------\n"; }
}
void Staff_initFile() {
    fstream file("staff.txt", ios::binary | ios::in);
    if (!file) {
        ofstream out("staff.txt", ios::binary);
        short header = -1; out.write((char*)&header, sizeof(header)); out.close();
    }
}

void StaffMenu() {
    Staff_initFile();
    fstream file("staff.txt", ios::binary | ios::in | ios::out);
    Staff_ReadPrimaryIndexFile(); Staff_ReadSecondaryIndexFile();
    int choice; Staff s;
    do {
        cout << "\n========= STAFF MENU =========\n";
        cout << "1.  Add New Staff\n";
        cout << "2.  Display All Staff\n";
        cout << "3.  Search By ID\n";
        cout << "4.  Search By Name\n";
        cout << "5.  Update By ID\n";
        cout << "6.  Update By Name\n";
        cout << "7.  Delete By ID\n";
        cout << "8.  Delete By Name\n";
        cout << "9.  Display Avail List\n";
        cout << "10. Display Top 5 Staff\n";
        cout << "0.  Back to Main Menu\n";
        cout << "Enter Choice: "; cin >> choice;
        switch (choice) {
        case 1: s.ReadStaff(); Staff_Insert(file, s); break;
        case 2: Staff_DisplayAll(file); break;
        case 3: { int id; cout << "Enter ID: "; cin >> id; Staff_SearchByID(file, id); break; }
        case 4: { char name[30]; cout << "Enter Name: "; Staff s2; s2.FixInput(); cin.getline(name, 30); Staff_SearchByName(file, name); break; }
        case 5: { int id; cout << "Enter ID: "; cin >> id; Staff_UpdateByID(file, id); break; }
        case 6: Staff_UpdateByName(file); break;
        case 7: { int id; cout << "Enter ID: "; cin >> id; Staff_DeleteByID(file, id); break; }
        case 8: Staff_DeleteByName(file); break;
        case 9: Staff_DisplayAvailList(file); break;
        case 10: Staff_DisplayTop5(file); break;
        case 0: break;
        default: cout << "Invalid choice!\n";
        }
        file.clear();
    } while (choice != 0);
    Staff_writePrimaryIndexFile(); Staff_writeSecondaryIndexFile(); file.close();
}