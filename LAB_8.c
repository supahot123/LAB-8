#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define ABSENT_TIME -1

#define START_TIME 480 // 8:00 AM
#define END_TIME 1020  // 5:00 PM

// structure for Employee data
typedef struct
{
    char name[50];
    char empID[10];

    int lateCount;
    int absences;

} Employee;

// structure for Employee data
typedef struct
{
    char empID[10];
    char name[50];

    int day;
    int month;
    int year;

    int timeIn;
    int timeOut;

    int late;
    int overtime;
    int undertime;

    int lateCount;
    int absences;

    char status[10];

} Attendance;

// DYNAMIC ARRAYS
Employee *emp = NULL;
Attendance *attendance = NULL;

// COUNTERS
int count = 0;
int logCount = 0;

// CAPACITY
int empCapacity = 2;
int logCapacity = 5;

// ============================
// FUNCTION PROTOTYPE
// ============================

// ============================
// FILE FUNCTIONS
// ============================

void saveEmployees();
void loadEmployees();

void saveLogs();
void loadLogs();

// ============================
// VALIDATION FUNCTIONS
// ============================

int isValidTime(int h, int m);
int isValidID(char id[]);
int isValidName(char name[]);

int isLeapYear(int year);
int daysInMonth(int month, int year);
int isValidDate(int day, int month, int year);

// ============================
// DATE FUNCTIONS
// ============================

void getCurrentDate(int *day,
                    int *month,
                    int *year);

void getDateChoice(int *day,
                   int *month,
                   int *year);

int toMinutes(int h, int m);

// ============================
// EMPLOYEE FUNCTIONS
// ============================

void addNewEmployee();

void modifyEmployeeInfo();

void updateEmployee();

int findEmployee(char id[]);

// ============================
// DISPLAY FUNCTIONS
// ============================

void displayEmployeeList();

void viewAllRecords();

void viewHistory();

void viewEmployees();

char *getStatus(int absences, int lateCount);

int findLogByDate(const char *empID,
                  int day,
                  int month,
                  int year);

// ============================
// MEMORY FUNCTIONS
// ============================

void resizeEmployee();

void resizeLogs();

// ============================
// UI FUNCTIONS
// ============================

void pauseScreen();

void printHeader();

void printLine();

void printTitle(char text[]);

// MAIN
int main()
{
    int choice;

    emp = malloc(empCapacity * sizeof(Employee));
    attendance = malloc(logCapacity * sizeof(Attendance));

    if (emp == NULL || attendance == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    FILE *fp;

    fp = fopen("employees.txt", "a");

    if (fp == NULL)
    {
        printf("Error Opening Employees File!\n");
        return 1;
    }

    fp = fopen("attendance.txt", "a");

    if (fp == NULL)
    {
        printf("Error Opening Attendance File!\n");
        return 1;
    }

    fclose(fp);

    loadEmployees();
    loadLogs();

    do
    {
        printHeader();

        printf("\n");
        printf("\033[1;32m");
        printf("[1]  Add New Employee\n");
        printf("[2]  Modify Employee Records\n");
        printf("[3]  View Employee List\n");
        printf("[4]  View Time Records\n");
        printf("[5]  View Late/Absent Record\n");
        printf("[6]  Exit\n");
        printf("\033[0m");

        printf("\nChoice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input!\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        switch (choice)
        {
        case 1:
            addNewEmployee();
            break;

        case 2:
        {
            int modifyChoice;

            if (count == 0)
            {
                printf("No records found!\n");
                break;
            }

            printTitle("MODIFY EMPLOYEE RECORDS");
            printf("[1] Modify Employee ID/Name\n");
            printf("[2] Modify Attendance Record\n");
            printf("Choice: ");

            scanf("%d", &modifyChoice);

            switch (modifyChoice)
            {
            case 1:
                modifyEmployeeInfo();
                break;

            case 2:
                updateEmployee();
                break;

            default:
                printf("Invalid Choice! Try Again!\n");
            }

            break;
        }

        case 3:
            displayEmployeeList();
            break;

        case 4:
        {
            int subChoice;

            if (logCount == 0)
            {
                printf("\nNo records found!\n");
                break;
            }

            printTitle("VIEW TIME RECORDS");
            printf("[1] View Records By Date\n");
            printf("[2] View All Records\n");
            printf("Choice: ");

            scanf("%d", &subChoice);

            switch (subChoice)
            {
            case 1:
                viewHistory();
                break;

            case 2:
                viewAllRecords();

                break;
            default:
                printf("Invalid Choice! Try Again!\n");
            }

            break;
        }

        case 5:
            viewEmployees();
            break;

        case 6:
            printf("\n===============================\n");
            printf("\n        End Of Program         \n");
            printf("\n===============================\n");
            break;

        default:
            printf("Invalid choice!\n");
        }

    } while (choice != 6);

    free(emp);
    free(attendance);

    return 0;
}

// ============================
// FUNCTION DEFINTION
// ============================

// ============================
// FILE FUNCTIONS
// ============================

void saveEmployees()
{
    FILE *fp = fopen("employees.txt", "w");

    if (fp == NULL)
    {
        printf("Error opening employee file!\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        fprintf(fp,
                "Employee ID: %s | Employee Name: %s | Late Count: %d | Absent Count: %d\n",
                emp[i].empID,
                emp[i].name,
                emp[i].lateCount,
                emp[i].absences);
    }

    fclose(fp);
}

void loadEmployees()
{
    FILE *fp = fopen("employees.txt", "r");

    if (fp == NULL)
        return;

    while (1)
    {
        if (count >= empCapacity)
            resizeEmployee();

        int result = fscanf(fp,
                            "Employee ID: %9[^|] | Employee Name: %49[^|] | Late Count: %d | Absent Count: %d\n",
                            emp[count].empID,
                            emp[count].name,
                            &emp[count].lateCount,
                            &emp[count].absences);

        if (result != 4)
            break;

        if (!isValidID(emp[count].empID) || !isValidName(emp[count].name))
        {
            continue;
        }

        if (emp[count].lateCount < 0 || emp[count].absences < 0)
            continue;

        count++;
    }

    fclose(fp);
}

void saveLogs()
{
    FILE *fp = fopen("attendance.txt", "w");

    if (fp == NULL)
    {
        printf("Error opening attendance file!\n");
        return;
    }

    for (int i = 0; i < logCount; i++)
    {
        int inH = attendance[i].timeIn / 60;
        int inM = attendance[i].timeIn % 60;

        int outH = attendance[i].timeOut / 60;
        int outM = attendance[i].timeOut % 60;

        int lateH = attendance[i].late / 60;
        int lateM = attendance[i].late % 60;

        int otH = attendance[i].overtime / 60;
        int otM = attendance[i].overtime % 60;

        int utH = attendance[i].undertime / 60;
        int utM = attendance[i].undertime % 60;

        int totalMinutes = attendance[i].timeOut - attendance[i].timeIn;
        int totalH = totalMinutes / 60;
        int totalM = totalMinutes % 60;

        fprintf(fp,
                "%02d/%02d/%04d | Employee ID: %s | Employee Name: %s | "
                "Time In: %02d:%02d | Time Out: %02d:%02d | "
                "Late: %02d:%02d | Overtime: %02d:%02d | Undertime: %02d:%02d | "
                "Total Time: %02d:%02d | "
                "Late Count: %d | Absent Count: %d | Status: %s\n",

                attendance[i].month,
                attendance[i].day,
                attendance[i].year,

                attendance[i].empID,
                attendance[i].name,

                inH, inM,
                outH, outM,

                lateH, lateM,
                otH, otM,
                utH, utM,

                totalH, totalM,

                attendance[i].lateCount,
                attendance[i].absences,
                attendance[i].status);
    }

    fclose(fp);
}

void loadLogs()
{
    FILE *fp = fopen("attendance.txt", "r");

    if (fp == NULL)
        return;

    int inH, inM, outH, outM;
    int lateH, lateM, otH, otM, utH, utM;
    int totalH, totalM;

    while (1)
    {
        if (logCount >= logCapacity)
            resizeLogs();

        int result = fscanf(fp,
                            "%d/%d/%d | Employee ID: %9[^|] | Employee Name: %49[^|] | "
                            "Time In: %d:%d | Time Out: %d:%d | "
                            "Late: %d:%d | Overtime: %d:%d | Undertime: %d:%d | "
                            "Total Time: %d:%d | "
                            "Late Count: %d | Absent Count: %d | Status: %9s\n",

                            &attendance[logCount].month,
                            &attendance[logCount].day,
                            &attendance[logCount].year,

                            attendance[logCount].empID,
                            attendance[logCount].name,

                            &inH, &inM,
                            &outH, &outM,

                            &lateH, &lateM,
                            &otH, &otM,
                            &utH, &utM,

                            &totalH, &totalM,

                            &attendance[logCount].lateCount,
                            &attendance[logCount].absences,
                            attendance[logCount].status);

        if (result != 19)
            break;

        if (!isValidDate(attendance[logCount].day, attendance[logCount].month, attendance[logCount].year))
            continue;

        if (!isValidID(attendance[logCount].empID) || !isValidName(attendance[logCount].name))
            continue;

        if (inH < 0 || inH > 23 || inM < 0 || inM > 59)
            continue;

        if (outH < 0 || outH > 23 || outM < 0 || outM > 59)
            continue;

        attendance[logCount].timeIn = toMinutes(inH, inM);
        attendance[logCount].timeOut = toMinutes(outH, outM);

        if (attendance[logCount].timeIn != ABSENT_TIME && attendance[logCount].timeOut <= attendance[logCount].timeIn)
            continue;

        attendance[logCount].late = toMinutes(lateH, lateM);
        attendance[logCount].overtime = toMinutes(otH, otM);
        attendance[logCount].undertime = toMinutes(utH, utM);

        if (attendance[logCount].lateCount < 0 || attendance[logCount].absences < 0)
            continue;

        logCount++;
    }

    fclose(fp);
}

// ============================
// VALIDATION FUNCTIONS
// ============================

int isValidTime(int h, int m)
{
    return (h >= 0 && h <= 23 &&
            m >= 0 && m <= 59);
}

int isValidID(char id[])
{
    if (strlen(id) == 0 || strlen(id) > 9)
        return 0;

    for (int i = 0; id[i]; i++)
        if (!isalnum(id[i]))
            return 0;

    return 1;
}

int isValidName(char name[])
{
    if (strlen(name) == 0)
        return 0;

    for (int i = 0; name[i]; i++)
        if (!isalpha(name[i]) && name[i] != ' ')
            return 0;

    return 1;
}

int isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) ||
           (year % 400 == 0);
}

int daysInMonth(int month, int year)
{
    int days[] = {31, 28, 31, 30, 31, 30,
                  31, 31, 30, 31, 30, 31};

    if (month == 2 && isLeapYear(year))
        return 29;

    return days[month - 1];
}

int isValidDate(int day, int month, int year)
{
    if (year < 1900 || year > 9999)
        return 0;

    if (month < 1 || month > 12)
        return 0;

    if (day < 1 || day > daysInMonth(month, year))
        return 0;

    return 1;
}

// ============================
// DATE FUNCTIONS
// ============================

void getCurrentDate(int *day,
                    int *month,
                    int *year)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    *day = tm.tm_mday;
    *month = tm.tm_mon + 1;
    *year = tm.tm_year + 1900;
}

void getDateChoice(int *day,
                   int *month,
                   int *year)
{
    int choice;

    while (1)
    {
        printf("\n--- SELECT DATE ---\n");
        printf("[1] Use Current Date\n");
        printf("[2] Enter Date Manually\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input!\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (choice == 1)
        {
            getCurrentDate(day, month, year);

            printf("Date set to: %02d/%02d/%04d\n",
                   *month, *day, *year);
            break;
        }
        else if (choice == 2)
        {
            while (1)
            {
                printf("Enter Date (MM DD YYYY): ");

                if (scanf("%d %d %d", month, day, year) != 3)
                {
                    printf("Invalid date input!\n");
                    while (getchar() != '\n')
                        ;
                    continue;
                }

                if (isValidDate(*day, *month, *year))
                    break;

                printf("Invalid date!\n");
            }
            break;
        }
        else
        {
            printf("\nInvalid choice! Try Again.\n");
        }
    }
}

int toMinutes(int h, int m)
{
    return h * 60 + m;
}

// ============================
// EMPLOYEE FUNCTIONS
// ============================

void addNewEmployee()
{
    printTitle("ADD NEW EMPLOYEE");

    char id[10];

    printf("\nEnter Employee ID: ");

    if (scanf("%9s", id) != 1)
    {
        printf("Invalid input!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    if (!isValidID(id))
    {
        printf("Invalid ID! Must be alphanumeric, max 9 characters.\n");
        return;
    }

    if (findEmployee(id) != -1)
    {
        printf("Employee already exists!\n");
        return;
    }

    if (count >= empCapacity)
        resizeEmployee();

    printf("Enter Employee Name: ");

    if (scanf(" %49[^\n]", emp[count].name) != 1)
    {
        printf("Invalid input!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    if (!isValidName(emp[count].name))
    {
        printf("Invalid name! Only letters and spaces allowed.\n");
        return;
    }

    strcpy(emp[count].empID, id);

    emp[count].lateCount = 0;
    emp[count].absences = 0;

    count++;

    saveEmployees();

    printf("Employee added successfully!\n");

    displayEmployeeList();
}

void modifyEmployeeInfo()
{
    char id[10];

    printf("\nEnter Current Employee ID: ");

    if (scanf("%9s", id) != 1)
    {
        printf("Invalid input!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    int index = findEmployee(id);

    if (index == -1)
    {
        printf("Employee not found!\n");
        return;
    }

    int choice;

    printf("\n--- MODIFY DATA ---\n");
    printf("[1] Modify Employee ID\n");
    printf("[2] Modify Employee Name\n");
    printf("Choice: ");

    if (scanf("%d", &choice) != 1)
    {
        printf("Invalid input!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    if (choice == 1)
    {
        char newID[10];

        printf("Enter New Employee ID: ");

        scanf("%9s", newID);

        if (!isValidID(newID))
        {
            printf("Invalid ID! Must be alphanumeric, max 9 characters.\n");
            return;
        }

        if (findEmployee(newID) != -1)
        {
            printf("ID already exists!\n");
            return;
        }

        // update employee list
        strcpy(emp[index].empID, newID);

        // update attendance records connected to old ID
        for (int i = 0; i < logCount; i++)
        {
            if (strcmp(attendance[i].empID, id) == 0)
            {
                strcpy(attendance[i].empID, newID);
            }
        }

        printf("Employee ID updated successfully!\n");
    }

    else if (choice == 2)
    {
        char newName[50];

        printf("Enter New Employee Name: ");

        scanf(" %49[^\n]", newName);

        if (!isValidName(newName))
        {
            printf("Invalid name! Only letters and spaces allowed.\n");
            return;
        }

        strcpy(emp[index].name, newName);

        // update attendance records connected to employee
        for (int i = 0; i < logCount; i++)
        {
            if (strcmp(attendance[i].empID, emp[index].empID) == 0)
            {
                strcpy(attendance[i].name, newName);
            }
        }

        printf("Employee name updated successfully!\n");
    }

    else
    {
        printf("Invalid choice!\n");
        return;
    }

    saveEmployees();
    saveLogs();

    displayEmployeeList();
}

void updateEmployee()
{
    char id[10];

    printf("\nEnter Employee ID: ");

    if (scanf("%9s", id) != 1)
    {
        printf("Invalid input!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    int index = findEmployee(id);

    if (index == -1)
    {
        printf("Employee not found!\n");
        return;
    }

    int day, month, year;
    getDateChoice(&day, &month, &year);

    int logIndex = findLogByDate(id, day, month, year);

    if (logIndex != -1)
    {
        printf("Record already exists for this date. Overwriting...\n");

        if (attendance[logIndex].timeIn == ABSENT_TIME)
            emp[index].absences--;
        else if (attendance[logIndex].late > 0)
            emp[index].lateCount--;
    }
    else
    {
        if (logCount >= logCapacity)
            resizeLogs();
        logIndex = logCount;
        logCount++;
    }

    int isAbsent;

    printf("Is the employee absent that day? (1 = Yes, 0 = No): ");

    if (scanf("%d", &isAbsent) != 1)
    {
        printf("Invalid input!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    if (isAbsent != 0 && isAbsent != 1)
    {
        printf("Invalid choice! Must be 0 or 1.\n");
        return;
    }

    if (isAbsent == 1)
    {
        emp[index].absences++;

        strcpy(attendance[logIndex].empID, emp[index].empID);
        strcpy(attendance[logIndex].name, emp[index].name);

        attendance[logIndex].day = day;
        attendance[logIndex].month = month;
        attendance[logIndex].year = year;

        attendance[logIndex].timeIn = ABSENT_TIME;
        attendance[logIndex].timeOut = ABSENT_TIME;

        attendance[logIndex].late = 0;
        attendance[logIndex].overtime = 0;
        attendance[logIndex].undertime = 0;

        attendance[logIndex].lateCount = emp[index].lateCount;
        attendance[logIndex].absences = emp[index].absences;

        strcpy(attendance[logIndex].status, getStatus(emp[index].absences, emp[index].lateCount));

        saveEmployees();
        saveLogs();

        printf("Absent recorded!\n");
        return;
    }

    int hIn, mIn, hOut, mOut;

    while (1)
    {
        printf("Enter Time In (HH MM): ");

        if (scanf("%d %d", &hIn, &mIn) != 2)
        {
            printf("Invalid time input!\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (!isValidTime(hIn, mIn))
        {
            printf("Invalid time! Hours: 0-23, Minutes: 0-59\n");
            continue;
        }

        break;
    }

    while (1)
    {
        printf("Enter Time Out (HH MM): ");

        if (scanf("%d %d", &hOut, &mOut) != 2)
        {
            printf("Invalid time input!\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (!isValidTime(hOut, mOut))
        {
            printf("Invalid time! Hours: 0-23, Minutes: 0-59\n");
            continue;
        }

        if (toMinutes(hOut, mOut) <= toMinutes(hIn, mIn))
        {
            printf("Invalid time range! Time out must be after time in.\n");
            continue;
        }

        break;
    }

    int late = (toMinutes(hIn, mIn) > START_TIME)
                   ? toMinutes(hIn, mIn) - START_TIME
                   : 0;

    int overtime = (toMinutes(hOut, mOut) > END_TIME)
                       ? toMinutes(hOut, mOut) - END_TIME
                       : 0;

    int undertime = (toMinutes(hOut, mOut) < END_TIME)
                        ? END_TIME - toMinutes(hOut, mOut)
                        : 0;

    if (late > 0)
        emp[index].lateCount++;

    strcpy(attendance[logIndex].empID, emp[index].empID);
    strcpy(attendance[logIndex].name, emp[index].name);

    attendance[logIndex].day = day;
    attendance[logIndex].month = month;
    attendance[logIndex].year = year;

    attendance[logIndex].timeIn = toMinutes(hIn, mIn);
    attendance[logIndex].timeOut = toMinutes(hOut, mOut);

    attendance[logIndex].late = late;
    attendance[logIndex].overtime = overtime;
    attendance[logIndex].undertime = undertime;

    attendance[logIndex].lateCount = emp[index].lateCount;
    attendance[logIndex].absences = emp[index].absences;

    strcpy(attendance[logIndex].status,
           getStatus(emp[index].absences, emp[index].lateCount));

    saveEmployees();
    saveLogs();

    printf("Attendance updated successfully!\n");
}

int findEmployee(char id[])
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(emp[i].empID, id) == 0)
            return i;
    }
    return -1;
}

// ============================
// DISPLAY FUNCTIONS
// ============================

void displayEmployeeList()
{
    if (count == 0)
    {
        printf("\nNo employees registered!\n");
        return;
    }

    printf("\n--- EMPLOYEE LIST ---\n");
    printf("%-10s %-20s\n", "ID", "Name");

    for (int i = 0; i < count; i++)
    {
        printf("%-10s %-20s\n",
               emp[i].empID,
               emp[i].name);
    }
}

void viewAllRecords()
{
    printf("\n--- ALL ATTENDANCE RECORDS ---\n");

    for (int i = 0; i < logCount; i++)
    {
        printf("%02d/%02d/%04d | %-10s | %-20s | ",
               attendance[i].month,
               attendance[i].day,
               attendance[i].year,
               attendance[i].empID,
               attendance[i].name);

        if (attendance[i].timeIn == ABSENT_TIME)
            printf("ABSENT\n");
        else
            printf("IN %02d:%02d OUT %02d:%02d\n",
                   attendance[i].timeIn / 60,
                   attendance[i].timeIn % 60,
                   attendance[i].timeOut / 60,
                   attendance[i].timeOut % 60);
    }
}

void viewHistory()
{
    if (logCount == 0)
    {
        printf("\nNo records yet!\n");
        return;
    }

    int day, month, year;
    getDateChoice(&day, &month, &year);

    int found = 0;

    printf("\n--- ATTENDANCE HISTORY ---\n");

    printf("%-12s %-10s %-20s %-10s %-10s %-8s %-8s %-8s %-8s\n",
           "Date", "ID", "Name", "TimeIn", "TimeOut", "Late", "OT", "UT", "Total");

    for (int i = 0; i < count; i++)
    {
        int idx = findLogByDate(emp[i].empID, day, month, year);

        if (idx == -1)
            continue;

        if (idx < 0 || idx >= logCount)
            continue;

        found = 1;

        printf("%02d/%02d/%04d   ",
               attendance[idx].month,
               attendance[idx].day,
               attendance[idx].year);

        printf("%-10s %-20s ",
               attendance[idx].empID,
               attendance[idx].name);

        if (attendance[idx].timeIn == ABSENT_TIME)
        {
            printf("%-10s %-10s %-8s %-8s %-8s %-8s\n",
                   "ABSENT", "ABSENT", "00:00", "00:00", "00:00", "00:00");
        }
        else
        {
            int total = attendance[idx].timeOut - attendance[idx].timeIn;

            printf("%02d:%02d     ", attendance[idx].timeIn / 60, attendance[idx].timeIn % 60);
            printf(" %02d:%02d     ", attendance[idx].timeOut / 60, attendance[idx].timeOut % 60);
            printf(" %02d:%02d   ", attendance[idx].late / 60, attendance[idx].late % 60);
            printf(" %02d:%02d   ", attendance[idx].overtime / 60, attendance[idx].overtime % 60);
            printf(" %02d:%02d   ", attendance[idx].undertime / 60, attendance[idx].undertime % 60);
            printf(" %02d:%02d\n", total / 60, total % 60);
        }
    }

    if (!found)
        printf("No records found for %02d/%02d/%04d.\n",
               month, day, year);
}

void viewEmployees()
{
    if (logCount == 0)
    {
        printf("\nNo records yet!\n");
        return;
    }

    printf("\n--- LATE/ABSENT RECORD ---\n");

    printf("%-10s %-20s %-10s %-10s %-10s\n",
           "ID", "Name", "LateCnt", "Absences", "Status");

    for (int i = 0; i < count; i++)
    {
        int late = 0, absent = 0;

        for (int j = 0; j < logCount; j++)
        {
            if (strcmp(emp[i].empID, attendance[j].empID) != 0)
                continue;

            int idx = findLogByDate(emp[i].empID,
                                    attendance[j].day,
                                    attendance[j].month,
                                    attendance[j].year);

            if (idx != j || idx == -1 || idx >= logCount)
                continue;

            if (attendance[idx].timeIn == ABSENT_TIME)
                absent++;
            else if (attendance[idx].late > 0)
                late++;
        }

        if (late > 0 || absent > 0)
        {
            char *status = getStatus(absent, late);

            if (strcmp(status, "GOOD") == 0)
                printf("\033[1;32m");

            else if (strcmp(status, "WARNING") == 0)
                printf("\033[1;33m");

            else
                printf("\033[1;31m");

            printf("%-10s %-20s %-10d %-10d %-10s",
                   emp[i].empID,
                   emp[i].name,
                   late,
                   absent,
                   status);

            printf("\033[0m\n");
        }
    }
}

char *getStatus(int absences, int lateCount)
{
    if (absences >= 8 || lateCount >= 8)
        return "CRITICAL";
    else if (absences >= 5 || lateCount >= 5)
        return "WARNING";
    else
        return "GOOD";
}

int findLogByDate(const char *empID, int day, int month, int year)
{
    for (int i = 0; i < logCount; i++)
    {
        if (strcmp(attendance[i].empID, empID) == 0 &&
            attendance[i].day == day &&
            attendance[i].month == month &&
            attendance[i].year == year)
        {
            return i;
        }
    }
    return -1;
}

// ============================
// MEMORY FUNCTIONS
// ============================

void resizeEmployee()
{
    empCapacity *= 2;

    Employee *temp = realloc(emp,
                             empCapacity * sizeof(Employee));

    if (!temp)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    emp = temp;
}

void resizeLogs()
{
    logCapacity *= 2;

    Attendance *temp = realloc(attendance,
                               logCapacity * sizeof(Attendance));

    if (!temp)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    attendance = temp;
}

// ============================
// UI FUNCTIONS
// ============================

void pauseScreen()
{
    printf("\nPress ENTER to continue...");

    while (getchar() != '\n')
        ;

    getchar();
}

void printHeader()
{
    int day, month, year;

    getCurrentDate(&day, &month, &year);

    printf("\033[1;36m");

    printLine();

    printf("              EMPLOYEE TIME TRACKER SYSTEM\n");

    printLine();

    printf("\033[1;33m");

    printf("\nEmployees Registered: %d", count);

    printf("\nAttendance Logs: %d", logCount);

    printf("\nToday's Date: %02d/%02d/%d\n",
           month,
           day,
           year);

    printf("\033[0m");

    printLine();
}

void printLine()
{
    printf("============================================================\n");
}

void printTitle(char text[])
{
    printf("\n\033[1;33m");
    printLine();
    printf("   %s\n", text);
    printLine();
    printf("\033[0m");
}
