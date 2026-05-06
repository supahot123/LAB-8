#include <stdio.h>
#include <string.h>

#define MAX 100
#define ABSENT_TIME -1

#define START_TIME 480 // 8:00 AM
#define END_TIME 1020  // 5:00 PM

// CURRENT EMPLOYEE
typedef struct
{
    char name[50];
    char empID[10];
    int lateCount;
    int absences;
} Employee;

Employee emp[MAX];
int count = 0;

// ATTENDANCE HISTORY
typedef struct
{
    char empID[10];
    char name[50];
    int timeIn;
    int timeOut;
    int late;
    int overtime;
    int undertime;

    int lateCount;
    int absences;
    char status[10];
} Attendance;

Attendance log[MAX * 5];
int logCount = 0;

// TIME VALIDATION
int isValidTime(int h, int m)
{
    return (h >= 0 && h <= 23 && m >= 0 && m <= 59);
}

int toMinutes(int h, int m)
{
    return h * 60 + m;
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

char *getStatus(int absences)
{
    if (absences >= 8 )
        return "CRITICAL";
    else if (absences >= 5)
        return "WARNING";
    else
        return "GOOD";
}

// ADD/UPDATE EMPLOYEE
void addEmployee()
{
    char id[10];
    int index;

    printf("\nEnter Employee ID: ");
    scanf("%s", id);

    index = findEmployee(id);

    printf("Enter Name: ");
    scanf(" %[^\n]", (index != -1) ? emp[index].name : emp[count].name);

    int isAbsent;
    printf("Is the employee absent today? (1 = Yes, 0 = No): ");
    scanf("%d", &isAbsent);

    // ABSENT CASE
    if (isAbsent == 1)
    {
        if (index == -1)
        {
            strcpy(emp[count].empID, id);
            emp[count].lateCount = 0;
            emp[count].absences = 0;
            index = count;
            count++;
        }

        emp[index].absences++;

        strcpy(log[logCount].empID, id);
        strcpy(log[logCount].name, emp[index].name);

        log[logCount].timeIn = ABSENT_TIME;
        log[logCount].timeOut = ABSENT_TIME;
        log[logCount].late = 0;
        log[logCount].overtime = 0;
        log[logCount].undertime = 0;

        log[logCount].lateCount = emp[index].lateCount;
        log[logCount].absences = emp[index].absences;
        strcpy(log[logCount].status, getStatus(emp[index].absences));

        logCount++;

        printf("Absence recorded.\n");
        return;
    }

    // PRESENT CASE
    int hIn, mIn, hOut, mOut;

    while (1)
    {
        printf("Enter Time In (24-HR, HH MM): ");
        scanf("%d %d", &hIn, &mIn);
        if (isValidTime(hIn, mIn))
            break;
        printf("Invalid time!\n");
    }

    while (1)
    {
        printf("Enter Time Out (24-HR, HH MM): ");
        scanf("%d %d", &hOut, &mOut);

        if (!isValidTime(hOut, mOut))
        {
            printf("Invalid time!\n");
            continue;
        }

        if (toMinutes(hOut, mOut) <= toMinutes(hIn, mIn))
        {
            printf("Time Out must be after Time In!\n");
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

    int totalWorked = toMinutes(hOut, mOut) - toMinutes(hIn, mIn);

    // UPDATE/CREATE
    if (index != -1)
    {
        if (late > 0)
            emp[index].lateCount++;

        while (emp[index].lateCount >= 3)
        {
            emp[index].absences++;
            emp[index].lateCount -= 3;
        }

        printf("Employee updated!\n");
    }
    else
    {
        strcpy(emp[count].empID, id);
        emp[count].lateCount = (late > 0) ? 1 : 0;
        emp[count].absences = 0;

        index = count;
        count++;

        printf("Employee added!\n");
    }

    // LOG RECORD
    strcpy(log[logCount].empID, id);
    strcpy(log[logCount].name, emp[index].name);

    log[logCount].timeIn = toMinutes(hIn, mIn);
    log[logCount].timeOut = toMinutes(hOut, mOut);

    log[logCount].late = late;
    log[logCount].overtime = overtime;
    log[logCount].undertime = undertime;

    log[logCount].lateCount = emp[index].lateCount;
    log[logCount].absences = emp[index].absences;
    strcpy(log[logCount].status, getStatus(emp[index].absences));

    logCount++;
}

// VIEW HISTORY
void viewHistory()
{
    if (logCount == 0)
    {
        printf("\nNo history records.\n");
        return;
    }

    printf("\n--- ATTENDANCE HISTORY ---\n");

    printf("%-10s %-20s %-10s %-10s %-8s %-8s %-8s %-8s\n",
           "ID", "Name", "TimeIn", "TimeOut",
           "Late", "OT", "UT", "Total");

    for (int i = 0; i < logCount; i++)
    {
        printf("%-10s %-20s ",
               log[i].empID,
               log[i].name);

        if (log[i].timeIn == ABSENT_TIME)
        {
            printf("%-10s %-10s %-8s\n", "ABSENT", "ABSENT", "0:00");
        }
        else
        {
            int totalWorked = log[i].timeOut - log[i].timeIn;

            printf("%02d:%02d     ", log[i].timeIn / 60, log[i].timeIn % 60);
            printf(" %02d:%02d     ", log[i].timeOut / 60, log[i].timeOut % 60);

            printf(" %02d:%02d   ", log[i].late / 60, log[i].late % 60);
            printf(" %02d:%02d   ", log[i].overtime / 60, log[i].overtime % 60);
            printf(" %02d:%02d   ", log[i].undertime / 60, log[i].undertime % 60);

            printf(" %02d:%02d\n", totalWorked / 60, totalWorked % 60);
        }
    }
}

// VIEW EMPLOYEES
void viewEmployees()
{
    if (count == 0)
    {
        printf("\nNo employees.\n");
        return;
    }

    printf("\n--- CURRENT EMPLOYEES ---\n");

    printf("%-10s %-20s %-10s %-10s %-10s\n",
           "ID", "Name", "LateCnt", "Absences", "Status");

    for (int i = 0; i < count; i++)
    {
        printf("%-10s %-20s %-10d %-10d %-10s\n",
               emp[i].empID,
               emp[i].name,
               emp[i].lateCount,
               emp[i].absences,
               getStatus(emp[i].absences));
    }
}

// MENU
int main()
{
    int choice;

    do
    {
        printf("\n===============================\n");
        printf("\n=    Employee Time Tracker    =\n");
        printf("\n===============================\n");
        printf("1. Add/Update Employee\n");
        printf("2. View Attendance History\n");
        printf("3. View Current Employees\n");
        printf("4. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            addEmployee();
            break;
        case 2:
            viewHistory();
            break;
        case 3:
            viewEmployees();
            break;
        case 4:
            printf("\n===============================\n");
            printf("\n=       End Of Program        =\n");
            printf("\n===============================\n");
            break;
        default:
            printf("Invalid choice.\n");
        }

    } while (choice != 4);

    return 0;
}
