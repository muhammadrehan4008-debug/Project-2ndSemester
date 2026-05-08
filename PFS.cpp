#include <iostream>
#include <string>

using namespace std;


class Transaction
{
public:

    string category;
    float amount;
    string date;

    Transaction()
    {
        category = "";
        amount = 0;
        date = "";
    }

    Transaction(string cat, float amt, string dt)
    {
        category = cat;
        amount = amt;
        date = dt;
    }
};


class TransactionManager
{
private:

    string filename;

    bool containsSubstring(string text, string sub)
    {
        int tLen = text.length();
        int sLen = sub.length();

        if (sLen > tLen)
            return false;

        for (int i = 0; i <= tLen - sLen; i++)
        {
            bool match = true;

            for (int j = 0; j < sLen; j++)
            {
                if (text[i + j] != sub[j])
                {
                    match = false;
                    break;
                }
            }

            if (match)
                return true;
        }

        return false;
    }

    string floatToString(float val)
    {
        if (val < 0)
            return "-" + floatToString(-val);

        int intPart = (int)val;
        int decPart = (int)((val - intPart) * 100 + 0.5);

        string result = "";
        int temp = intPart;

        if (temp == 0)
        {
            result = "0";
        }
        else
        {
            while (temp > 0)
            {
                result = (char)('0' + temp % 10) + result;
                temp /= 10;
            }
        }

        result += ".";

        if (decPart < 10)
            result += "0";

        result += (char)('0' + decPart / 10);
        result += (char)('0' + decPart % 10);

        return result;
    }

    float stringToFloat(string s)
    {
        float result = 0;
        float decimal = 0;
        bool afterDot = false;
        float divisor = 10;

        for (int i = 0; i < (int)s.length(); i++)
        {
            if (s[i] == '.')
            {
                afterDot = true;
                continue;
            }

            if (!afterDot)
                result = result * 10 + (s[i] - '0');
            else
            {
                decimal += (s[i] - '0') / divisor;
                divisor *= 10;
            }
        }

        return result + decimal;
    }

    void printPadded(string text, int width)
    {
        cout << text;
        int spaces = width - (int)text.length();
        for (int i = 0; i < spaces; i++)
            cout << " ";
    }

    void printPaddedNum(int num, int width)
    {
        string s = "";
        int temp = num;

        if (temp == 0)
        {
            s = "0";
        }
        else
        {
            while (temp > 0)
            {
                s = (char)('0' + temp % 10) + s;
                temp /= 10;
            }
        }

        printPadded(s, width);
    }

public:

    TransactionManager(string file)
    {
        filename = file;
    }

    void addTransaction(Transaction t)
    {
        FILE* fp = fopen(filename.c_str(), "a");

        if (!fp)
        {
            cout << "Error opening file!\n";
            return;
        }

        string line = t.category + " " + floatToString(t.amount) + " " + t.date + "\n";

        for (int i = 0; i < (int)line.length(); i++)
            fputc(line[i], fp);

        fclose(fp);
    }

    int getCount()
    {
        FILE* fp = fopen(filename.c_str(), "r");

        if (!fp)
            return 0;

        int count = 0;
        int c;
        bool inLine = false;

        while ((c = fgetc(fp)) != EOF)
        {
            if (c == '\n' && inLine)
            {
                count++;
                inLine = false;
            }
            else if (c != '\n')
            {
                inLine = true;
            }
        }

        if (inLine)
            count++;

        fclose(fp);
        return count;
    }

    Transaction getByIndex(int index)
    {
        FILE* fp = fopen(filename.c_str(), "r");
        Transaction t;

        if (!fp)
            return t;

        int current = 0;
        int c;

        while (true)
        {
            string cat = "", amtStr = "", dt = "";
            int field = 0;
            bool gotData = false;

            while ((c = fgetc(fp)) != EOF && c != '\n')
            {
                gotData = true;

                if (c == ' ')
                    field++;
                else if (field == 0)
                    cat += (char)c;
                else if (field == 1)
                    amtStr += (char)c;
                else if (field == 2)
                    dt += (char)c;
            }

            if (!gotData && c == EOF)
                break;

            if (current == index)
            {
                t.category = cat;
                t.amount = stringToFloat(amtStr);
                t.date = dt;
                fclose(fp);
                return t;
            }

            current++;

            if (c == EOF)
                break;
        }

        fclose(fp);
        return t;
    }

    void viewAll(string label)
    {
        int total = getCount();

        if (total == 0)
        {
            cout << "No records found.\n";
            return;
        }

        cout << "\n--- " << label << " ---\n";
        printPadded("No", 5);
        printPadded("Category", 18);
        printPadded("Amount", 12);
        printPadded("Date", 14);
        cout << "\n";
        cout << "---------------------------------------------------\n";

        for (int i = 0; i < total; i++)
        {
            Transaction t = getByIndex(i);
            printPaddedNum(i + 1, 5);
            printPadded(t.category, 18);
            printPadded(floatToString(t.amount), 12);
            printPadded(t.date, 14);
            cout << "\n";
        }
    }

    float getTotal()
    {
        int total = getCount();
        float sum = 0;

        for (int i = 0; i < total; i++)
            sum += getByIndex(i).amount;

        return sum;
    }

    void deleteAll()
    {
        FILE* fp = fopen(filename.c_str(), "w");
        fclose(fp);
        cout << "All records deleted successfully!\n";
    }

    void deleteByNumber(int num)
    {
        int total = getCount();

        if (num < 1 || num > total)
        {
            cout << "Invalid record number!\n";
            return;
        }

        string tempFile = "temp_del.txt";
        FILE* out = fopen(tempFile.c_str(), "w");

        for (int i = 0; i < total; i++)
        {
            if (i == num - 1)
                continue;

            Transaction t = getByIndex(i);
            string line = t.category + " " + floatToString(t.amount) + " " + t.date + "\n";

            for (int j = 0; j < (int)line.length(); j++)
                fputc(line[j], out);
        }

        fclose(out);
        remove(filename.c_str());
        rename(tempFile.c_str(), filename.c_str());

        cout << "Record deleted successfully!\n";
    }

    float getTotalByMonth(string month, string year)
    {
        int total = getCount();
        float sum = 0;

        for (int i = 0; i < total; i++)
        {
            Transaction t = getByIndex(i);

            if (containsSubstring(t.date, month) && containsSubstring(t.date, year))
                sum += t.amount;
        }

        return sum;
    }

    float getTotalByYear(string year)
    {
        int total = getCount();
        float sum = 0;

        for (int i = 0; i < total; i++)
        {
            Transaction t = getByIndex(i);

            if (containsSubstring(t.date, year))
                sum += t.amount;
        }

        return sum;
    }

    float getTotalByCategory(string cat)
    {
        int total = getCount();
        float sum = 0;

        for (int i = 0; i < total; i++)
        {
            Transaction t = getByIndex(i);

            if (t.category == cat)
                sum += t.amount;
        }

        return sum;
    }

    string floatToStr(float val)
    {
        return floatToString(val);
    }
};


class UserManager
{
private:

    string userFile;

    void readLine(FILE* fp, string &u, string &p, bool &gotData, int &lastChar)
    {
        u = "";
        p = "";
        gotData = false;
        int field = 0;
        int c;

        while ((c = fgetc(fp)) != EOF && c != '\n')
        {
            gotData = true;

            if (c == ' ')
                field++;
            else if (field == 0)
                u += (char)c;
            else if (field == 1)
                p += (char)c;
        }

        lastChar = c;
    }

public:

    UserManager()
    {
        userFile = "users.txt";
    }

    bool usernameExists(string username)
    {
        FILE* fp = fopen(userFile.c_str(), "r");

        if (!fp)
            return false;

        string u, p;
        bool gotData;
        int lastChar;

        while (true)
        {
            readLine(fp, u, p, gotData, lastChar);

            if (!gotData && lastChar == EOF)
                break;

            if (u == username)
            {
                fclose(fp);
                return true;
            }

            if (lastChar == EOF)
                break;
        }

        fclose(fp);
        return false;
    }

    bool registerUser(string username, string password)
    {
        if (usernameExists(username))
        {
            cout << "Username already exists! Try a different one.\n";
            return false;
        }

        FILE* fp = fopen(userFile.c_str(), "a");

        if (!fp)
        {
            cout << "Error creating user database!\n";
            return false;
        }

        string line = username + " " + password + "\n";

        for (int i = 0; i < (int)line.length(); i++)
            fputc(line[i], fp);

        fclose(fp);
        cout << "Registration successful! Please login.\n";
        return true;
    }

    bool loginUser(string username, string password, string &currentUser)
    {
        FILE* fp = fopen(userFile.c_str(), "r");

        if (!fp)
        {
            cout << "No users registered yet. Please register first.\n";
            return false;
        }

        string u, p;
        bool gotData;
        int lastChar;

        while (true)
        {
            readLine(fp, u, p, gotData, lastChar);

            if (!gotData && lastChar == EOF)
                break;

            if (u == username && p == password)
            {
                fclose(fp);
                currentUser = username;
                cout << "Login successful! Welcome back, " << username << ".\n";
                return true;
            }

            if (lastChar == EOF)
                break;
        }

        fclose(fp);
        cout << "Invalid username or password!\n";
        return false;
    }

    bool changePassword(string username, string oldPass, string newPass)
    {
        FILE* fp = fopen(userFile.c_str(), "r");

        if (!fp)
        {
            cout << "Error accessing user database!\n";
            return false;
        }

        string users[100], passes[100];
        int count = 0;
        bool found = false;
        string u, p;
        bool gotData;
        int lastChar;

        while (true)
        {
            readLine(fp, u, p, gotData, lastChar);

            if (!gotData && lastChar == EOF)
                break;

            users[count] = u;

            if (u == username && p == oldPass)
            {
                passes[count] = newPass;
                found = true;
            }
            else
            {
                passes[count] = p;
            }

            count++;

            if (lastChar == EOF)
                break;
        }

        fclose(fp);

        if (!found)
        {
            cout << "Invalid old password!\n";
            return false;
        }

        FILE* out = fopen(userFile.c_str(), "w");

        for (int i = 0; i < count; i++)
        {
            string line = users[i] + " " + passes[i] + "\n";

            for (int j = 0; j < (int)line.length(); j++)
                fputc(line[j], out);
        }

        fclose(out);
        cout << "Password changed successfully!\n";
        return true;
    }
};


class FinanceApp
{
private:

    string currentUser;
    bool loggedIn;
    UserManager userManager;

    void clearScreen()
    {
        system("cls");
    }

    void printFloat(float val)
    {
        if (val < 0)
        {
            cout << "-";
            val = -val;
        }

        int intPart = (int)val;
        int decPart = (int)((val - intPart) * 100 + 0.5);

        cout << intPart << ".";

        if (decPart < 10)
            cout << "0";

        cout << decPart;
    }

    void printPadded(string text, int width)
    {
        cout << text;
        int spaces = width - (int)text.length();
        for (int i = 0; i < spaces; i++)
            cout << " ";
    }

    void showCategoryBreakdown(string type, TransactionManager &tm)
    {
        int total = tm.getCount();

        if (total == 0)
        {
            cout << "No records found.\n";
            return;
        }

        string cats[100];
        int catCount = 0;

        for (int i = 0; i < total; i++)
        {
            Transaction t = tm.getByIndex(i);
            bool found = false;

            for (int j = 0; j < catCount; j++)
            {
                if (cats[j] == t.category)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                cats[catCount] = t.category;
                catCount++;
            }
        }

        float grandTotal = tm.getTotal();

        cout << "\n--- " << type << " Category Breakdown ---\n";
        printPadded("Category", 20);
        printPadded("Amount", 14);
        cout << "%\n";
        cout << "------------------------------------------\n";

        for (int i = 0; i < catCount; i++)
        {
            float catTotal = tm.getTotalByCategory(cats[i]);
            int percent = (grandTotal > 0) ? (int)(catTotal / grandTotal * 100) : 0;

            printPadded(cats[i], 20);
            printPadded(tm.floatToStr(catTotal), 14);
            cout << percent << "%\n";
        }

        cout << "------------------------------------------\n";
        printPadded("TOTAL", 20);
        printFloat(grandTotal);
        cout << "\n";
    }

    void expenseMenu()
    {
        clearScreen();
        int ch;

        while (1)
        {
            TransactionManager em(currentUser + "_expenses.txt");

            cout << "\n--- Expense Management ---\n";
            cout << "1. Add New Expense\n";
            cout << "2. View All Expenses\n";
            cout << "3. Show Total Expenses\n";
            cout << "4. Delete All Expenses\n";
            cout << "5. Delete Specific Expense\n";
            cout << "6. Category Breakdown\n";
            cout << "7. Back to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> ch;

            if (ch == 1)
            {
                string cat, dt;
                float amt;

                cout << "Enter expense category: ";
                cin >> cat;
                cout << "Enter amount: ";
                cin >> amt;
                cout << "Enter date (DD/MM/YYYY): ";
                cin >> dt;

                em.addTransaction(Transaction(cat, amt, dt));
                cout << "Expense saved successfully!\n";
            }
            else if (ch == 2)
            {
                em.viewAll("Expense List");
            }
            else if (ch == 3)
            {
                cout << "Total Expenses: ";
                printFloat(em.getTotal());
                cout << "\n";
            }
            else if (ch == 4)
            {
                em.deleteAll();
            }
            else if (ch == 5)
            {
                em.viewAll("Expense List");
                int num;
                cout << "\nEnter record number to delete: ";
                cin >> num;
                em.deleteByNumber(num);
            }
            else if (ch == 6)
            {
                showCategoryBreakdown("Expense", em);
            }
            else if (ch == 7)
            {
                clearScreen();
                return;
            }
            else
            {
                cout << "Invalid option! Try again.\n";
            }
        }
    }

    void incomeMenu()
    {
        clearScreen();
        int ch;

        while (1)
        {
            TransactionManager im(currentUser + "_income.txt");

            cout << "\n--- Income Management ---\n";
            cout << "1. Add Income\n";
            cout << "2. View Income Records\n";
            cout << "3. Show Total Income\n";
            cout << "4. Delete All Income\n";
            cout << "5. Delete Specific Income\n";
            cout << "6. Category Breakdown\n";
            cout << "7. Back to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> ch;

            if (ch == 1)
            {
                string src, dt;
                float amt;

                cout << "Enter income source: ";
                cin >> src;
                cout << "Enter amount: ";
                cin >> amt;
                cout << "Enter date (DD/MM/YYYY): ";
                cin >> dt;

                im.addTransaction(Transaction(src, amt, dt));
                cout << "Income saved successfully!\n";
            }
            else if (ch == 2)
            {
                im.viewAll("Income Records");
            }
            else if (ch == 3)
            {
                cout << "Total Income: ";
                printFloat(im.getTotal());
                cout << "\n";
            }
            else if (ch == 4)
            {
                im.deleteAll();
            }
            else if (ch == 5)
            {
                im.viewAll("Income Records");
                int num;
                cout << "\nEnter record number to delete: ";
                cin >> num;
                im.deleteByNumber(num);
            }
            else if (ch == 6)
            {
                showCategoryBreakdown("Income", im);
            }
            else if (ch == 7)
            {
                clearScreen();
                return;
            }
            else
            {
                cout << "Invalid option! Try again.\n";
            }
        }
    }

    void monthlySummary()
    {
        string month, year;

        cout << "Enter month (MM): ";
        cin >> month;
        cout << "Enter year (YYYY): ";
        cin >> year;

        TransactionManager em(currentUser + "_expenses.txt");
        TransactionManager im(currentUser + "_income.txt");

        float tExp = em.getTotalByMonth(month, year);
        float tInc = im.getTotalByMonth(month, year);
        float net = tInc - tExp;

        cout << "\n--- Monthly Summary (" << month << "/" << year << ") ---\n";
        cout << "Total Income : "; printFloat(tInc); cout << "\n";
        cout << "Total Expense: "; printFloat(tExp); cout << "\n";
        cout << "Net Balance  : "; printFloat(net); cout << "\n";

        if (tInc > 0)
        {
            int savingsRate = (int)(net / tInc * 100);
            cout << "Savings Rate : " << savingsRate << "%\n";
        }
    }

    void yearlySummary()
    {
        string year;

        cout << "Enter year (YYYY): ";
        cin >> year;

        TransactionManager em(currentUser + "_expenses.txt");
        TransactionManager im(currentUser + "_income.txt");

        float tExp = em.getTotalByYear(year);
        float tInc = im.getTotalByYear(year);
        float net = tInc - tExp;

        cout << "\n--- Yearly Summary (" << year << ") ---\n";
        cout << "Total Income : "; printFloat(tInc); cout << "\n";
        cout << "Total Expense: "; printFloat(tExp); cout << "\n";
        cout << "Net Balance  : "; printFloat(net); cout << "\n";

        if (tInc > 0)
        {
            int savingsRate = (int)(net / tInc * 100);
            cout << "Savings Rate : " << savingsRate << "%\n";
        }
    }

    void overallBalance()
    {
        TransactionManager em(currentUser + "_expenses.txt");
        TransactionManager im(currentUser + "_income.txt");

        float tExp = em.getTotal();
        float tInc = im.getTotal();
        float balance = tInc - tExp;

        cout << "\n--- Overall Balance ---\n";
        cout << "Total Income : "; printFloat(tInc); cout << "\n";
        cout << "Total Expense: "; printFloat(tExp); cout << "\n";
        cout << "Final Balance: "; printFloat(balance); cout << "\n";

        if (balance > 0)
            cout << "Status: You are in SURPLUS!\n";
        else if (balance < 0)
            cout << "Status: You are in DEFICIT!\n";
        else
            cout << "Status: You are BREAKING EVEN.\n";
    }

    void savingsGoalTracker()
    {
        float goal;

        cout << "\n--- Savings Goal Tracker ---\n";
        cout << "Enter your savings goal amount: ";
        cin >> goal;

        TransactionManager em(currentUser + "_expenses.txt");
        TransactionManager im(currentUser + "_income.txt");

        float currentSavings = im.getTotal() - em.getTotal();

        cout << "\nSavings Goal   : "; printFloat(goal); cout << "\n";
        cout << "Current Savings: "; printFloat(currentSavings); cout << "\n";

        if (currentSavings >= goal)
        {
            cout << "Progress       : 100%\n";
            cout << "Congratulations! You have reached your savings goal!\n";
        }
        else if (currentSavings <= 0)
        {
            cout << "Progress       : 0%\n";
            cout << "You need to increase income or cut expenses to start saving.\n";
        }
        else
        {
            int progress = (int)(currentSavings / goal * 100);
            float remaining = goal - currentSavings;

            cout << "Progress       : " << progress << "%\n";
            cout << "Remaining      : "; printFloat(remaining); cout << "\n";
            cout << "Progress Bar   : [";

            int filled = progress / 5;

            for (int i = 0; i < 20; i++)
            {
                if (i < filled)
                    cout << "=";
                else
                    cout << " ";
            }

            cout << "]\n";
        }
    }

    void reportMenu()
    {
        clearScreen();
        int ch;

        while (1)
        {
            cout << "\n--- Financial Reports ---\n";
            cout << "1. Monthly Summary\n";
            cout << "2. Yearly Summary\n";
            cout << "3. Overall Balance\n";
            cout << "4. Savings Goal Tracker\n";
            cout << "5. Back to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> ch;

            if (ch == 1)
                monthlySummary();
            else if (ch == 2)
                yearlySummary();
            else if (ch == 3)
                overallBalance();
            else if (ch == 4)
                savingsGoalTracker();
            else if (ch == 5)
            {
                clearScreen();
                return;
            }
            else
                cout << "Invalid option! Try again.\n";
        }
    }

public:

    FinanceApp()
    {
        currentUser = "";
        loggedIn = false;
    }

    void run()
    {
        int choice;

        while (1)
        {
            if (!loggedIn)
            {
                cout << "\n******* PERSONAL FINANCE MANAGEMENT SYSTEM *******\n";
                cout << "1. Login\n";
                cout << "2. Register\n";
                cout << "3. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice;

                if (choice == 1)
                {
                    string u, p;
                    clearScreen();
                    cout << "\n********** LOGIN **********\n";
                    cout << "Enter username: ";
                    cin >> u;
                    cout << "Enter password: ";
                    cin >> p;
                    loggedIn = userManager.loginUser(u, p, currentUser);
                }
                else if (choice == 2)
                {
                    string u, p;
                    clearScreen();
                    cout << "\n********** REGISTRATION **********\n";
                    cout << "Enter username: ";
                    cin >> u;
                    cout << "Enter password: ";
                    cin >> p;
                    userManager.registerUser(u, p);
                }
                else if (choice == 3)
                {
                    cout << "Thank you for using the system!\n";
                    return;
                }
                else
                {
                    cout << "Invalid choice! Try again.\n";
                }
            }
            else
            {
                clearScreen();
                cout << "\n******* PERSONAL FINANCE MANAGEMENT SYSTEM *******\n";
                cout << "Welcome, " << currentUser << "!\n";
                cout << "1. Expense Management\n";
                cout << "2. Income Management\n";
                cout << "3. Financial Reports\n";
                cout << "4. Change Password\n";
                cout << "5. Logout\n";
                cout << "6. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice;

                if (choice == 1)
                {
                    expenseMenu();
                }
                else if (choice == 2)
                {
                    incomeMenu();
                }
                else if (choice == 3)
                {
                    reportMenu();
                }
                else if (choice == 4)
                {
                    string oldP, newP, confirmP;
                    clearScreen();
                    cout << "\n********** CHANGE PASSWORD **********\n";
                    cout << "Enter old password: ";
                    cin >> oldP;
                    cout << "Enter new password: ";
                    cin >> newP;
                    cout << "Confirm new password: ";
                    cin >> confirmP;

                    if (newP != confirmP)
                        cout << "Passwords do not match!\n";
                    else
                        userManager.changePassword(currentUser, oldP, newP);
                }
                else if (choice == 5)
                {
                    loggedIn = false;
                    currentUser = "";
                    cout << "Logged out successfully!\n";
                }
                else if (choice == 6)
                {
                    cout << "Thank you for using the system!\n";
                    return;
                }
                else
                {
                    cout << "Invalid choice! Try again.\n";
                }
            }
        }
    }
};


int main()
{
    FinanceApp app;
    app.run();
    return 0;
}
