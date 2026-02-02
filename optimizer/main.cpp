#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>

using namespace std;

int main() {
    int choice;
    ofstream outputFile("logs/log.txt", ios::app);
    if (!outputFile.is_open()) {
    cout << "Failed to open log file!" << endl;
    }
    const char* batchFileName1 = "cmd.exe /C scripts\\clean_temp.bat";
    const char* batchFileName2 = "cmd.exe /C scripts\\show_tasks.bat";

    string option1 = "1. Clean temp files";
    string option2 = "2. Show running processes";
    string option3 = "3. Exit";

    

    
    while (!(choice == 3)) {
        cout << option1 << endl
        << option2 << endl
        << option3 << endl;
        cin >> choice;

    switch (choice) {
        case 1: {
            cout << option1 << endl;
            int returnCode = system(batchFileName1);
            if (returnCode == 0) {
                cout << "Executed successfully." << endl;
            } else {
                cout << "Failed to execute." << endl;
            }
            outputFile << "Cleaned Temp." << endl;
            cout << "Logging to file..." << endl;
            outputFile.flush();
            break; }
        case 2: {
            cout << option2 << endl;
            int returnCode = system(batchFileName2);
            if (returnCode == 0) {
                cout << "Executed successfully." << endl;
            } else {
                cout << "Failed to execute." << endl;
            }
            outputFile << "Showed Tasks." << endl;
            cout << "Logging to file..." << endl;
            outputFile.flush();
            break; }
        case 3: {
            cout << option3 << endl;
            choice = 3;
            break; }
        default: {
            cout << "Invalid choice please try again" << endl; }
    }
}
    outputFile.close();

    return 0;
}