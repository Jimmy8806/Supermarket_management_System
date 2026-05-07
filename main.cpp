#include <bits/stdc++.h>
#include <fstream>
#include <limits>
#include "customer.h"
#include "products.h"
#include "sales.h"
#include "staff.h"

using namespace std;
int main() {
    int choice;
    do {
        cout << "       MANAGEMENT SYSTEM - MAIN MENU \n";
        cout<< "-------------------------------------------\n";
        cout << "   1. Staff Management                 \n";
        cout << "   2. Sales Management                 \n";
        cout << "   3. Customer Management              \n";
        cout << "   4. Products Management              \n";
        cout << "   0. Exit                             \n";
        cout << "Enter Choice: ";
        cin >> choice;

        switch (choice) {
        case 1: StaffMenu();    break;
        case 2: SalesMenu();    break;
        case 3: CustomersMenu(); break;
        case 4: ProductsMenu(); break;
        case 0: cout << "Goodbye!\n"; break;
        default: cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}