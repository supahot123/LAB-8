#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ABSENT_TIME -1

#define START_TIME 480 // 8:00 AM
#define END_TIME 1020  // 5:00 PM

typedef struct
{
    char name[50];
    char empID[10];

    int lateCount;
    int absences;

} Employee;

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
Attendance *log = NULL;

// COUNTERS
int count = 0;
int logCount = 0;

// CAPACITY
int empCapacity = 2;
int logCapacity = 5;

// FUNCTION PROTOTYPES
void saveEmployees();
void loadEmployees();

void saveLogs();
void loadLogs();

// TIME VALIDATION
int isValidTime(int h, int m)
{
    return (h >= 0 && h <= 23 &&
            m >= 0 && m <= 59);
}

int toMinutes(int h, int m)
{
    return h * 60 + m;
}

// GET CURRENT DATE
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

// LEAP YEAR CHECK
int isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) ||
           (year % 400 == 0);
}

// DAYS IN MONTH
int daysInMonth(int month, int year)
{
    int days[] = {31, 28, 31, 30, 31, 30,
                  31, 31, 30, 31, 30, 31};

    if (month == 2 && isLeapYear(year))
        return 29;

    return days[month - 1];
}

// VALIDATE DATE
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

// DATE MENU
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
        scanf("%d", &choice);

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
                scanf("%d %d %d",
                      month, day, year);

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

// FIND EMPLOYEE
int findEmployee(char id[])
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(emp[i].empID, id) == 0)
            return i;
    }
    return -1;
}

// STATUS
char *getStatus(int absences)
{
    if (absences >= 8)
        return "CRITICAL";
    else if (absences >= 5)
        return "WARNING";
    else
        return "GOOD";
}

// FIND LOG BY DATE
int findLogByDate(const char *empID, int day, int month, int year)
{
    for (int i = 0; i < logCount; i++)
    {
        if (strcmp(log[i].empID, empID) == 0 &&
            log[i].day == day &&
            log[i].month == month &&
            log[i].year == year)
        {
            return i;
        }
    }
    return -1;
}

// RESIZE EMPLOYEE ARRAY
void resizeEmployee()
{
    empCapacity *= 2;

    emp = realloc(emp,
                  empCapacity * sizeof(Employee));

    if (emp == NULL)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }
}

// RESIZE LOG ARRAY
void resizeLogs()
{
    logCapacity *= 2;

    log = realloc(log,
                  logCapacity * sizeof(Attendance));

    if (log == NULL)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }
}

// SAVE EMPLOYEES
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

// LOAD EMPLOYEES
void loadEmployees()
{
    FILE *fp = fopen("employees.txt", "r");

    if (fp == NULL)
        return;

    while (fscanf(fp,
                  "Employee ID: %9[^|] | Employee Name: %49[^|] | Late Count: %d | Absent Count: %d\n",
                  emp[count].empID,
                  emp[count].name,
                  &emp[count].lateCount,
                  &emp[count].absences) == 4)
    {
        count++;

        if (count >= empCapacity)
            resizeEmployee();
    }

    fclose(fp);
}

// SAVE LOGS
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
        int inH = log[i].timeIn / 60;
        int inM = log[i].timeIn % 60;

        int outH = log[i].timeOut / 60;
        int outM = log[i].timeOut % 60;

        int lateH = log[i].late / 60;
        int lateM = log[i].late % 60;

        int otH = log[i].overtime / 60;
        int otM = log[i].overtime % 60;

        int utH = log[i].undertime / 60;
        int utM = log[i].undertime % 60;

        int totalMinutes = log[i].timeOut - log[i].timeIn;
        int totalH = totalMinutes / 60;
        int totalM = totalMinutes % 60;

        fprintf(fp,
                "%02d/%02d/%04d | Employee ID: %s | Employee Name: %s | "
                "Time In: %02d:%02d | Time Out: %02d:%02d | "
                "Late: %02d:%02d | Overtime: %02d:%02d | Undertime: %02d:%02d | "
                "Total Time: %02d:%02d | "
                "Late Count: %d | Absent Count: %d | Status: %s\n",

                log[i].month,
                log[i].day,
                log[i].year,

                log[i].empID,
                log[i].name,

                inH, inM,
                outH, outM,

                lateH, lateM,
                otH, otM,
                utH, utM,

                totalH, totalM,

                log[i].lateCount,
                log[i].absences,
                log[i].status);
    }

    fclose(fp);
}

// LOAD LOGS
void loadLogs()
{
    FILE *fp = fopen("attendance.txt", "r");

    if (fp == NULL)
        return;

    int inH, inM, outH, outM;
    int lateH, lateM, otH, otM, utH, utM;
    int totalH, totalM;

    while (fscanf(fp,
                  "%d/%d/%d | Employee ID: %9[^|] | Employee Name: %49[^|] | "
                  "Time In: %d:%d | Time Out: %d:%d | "
                  "Late: %d:%d | Overtime: %d:%d | Undertime: %d:%d | "
                  "Total Time: %d:%d | "
                  "Late Count: %d | Absent Count: %d | Status: %9s\n",

                  &log[logCount].month,
                  &log[logCount].day,
                  &log[logCount].year,

                  log[logCount].empID,
                  log[logCount].name,

                  &inH, &inM,
                  &outH, &outM,

                  &lateH, &lateM,
                  &otH, &otM,
                  &utH, &utM,

                  &totalH, &totalM,

                  &log[logCount].lateCount,
                  &log[logCount].absences,
                  log[logCount].status) == 19)
    {
        log[logCount].timeIn = toMinutes(inH, inM);
        log[logCount].timeOut = toMinutes(outH, outM);

        log[logCount].late = toMinutes(lateH, lateM);
        log[logCount].overtime = toMinutes(otH, otM);
        log[logCount].undertime = toMinutes(utH, utM);

        logCount++;

        if (logCount >= logCapacity)
            resizeLogs();
    }

    fclose(fp);
}

// ADD EMPLOYEE
void addNewEmployee()
{
    char id[10];

    printf("\nEnter Employee ID: ");
    scanf("%s", id);

    if (findEmployee(id) != -1)
    {
        printf("Employee already exists!\n");
        return;
    }

    if (count >= empCapacity)
        resizeEmployee();

    printf("Enter Employee Name: ");
    scanf(" %[^\n]", emp[count].name);

    strcpy(emp[count].empID, id);

    emp[count].lateCount = 0;
    emp[count].absences = 0;

    count++;

    saveEmployees();

    printf("Employee added successfully!\n");
}

// UPDATE EMPLOYEE
void updateEmployee()
{
    char id[10];

    printf("\nEnter Employee ID: ");
    scanf("%s", id);

    int index = findEmployee(id);

    if (index == -1)
    {
        printf("Employee not found!\n");
        return;
    }

    int day, month, year;
    getDateChoice(&day, &month, &year);

    if (logCount >= logCapacity)
        resizeLogs();

    int logIndex = findLogByDate(id, day, month, year);

    if (logIndex == -1)
    {
        logIndex = logCount;
        logCount++;
    }

    int isAbsent;

    printf("Is the employee absent today? (1 = Yes, 0 = No): ");
    scanf("%d", &isAbsent);

    if (isAbsent == 1)
    {
        emp[index].absences = 1;
        emp[index].lateCount = 0;

        strcpy(log[logIndex].empID, emp[index].empID);
        strcpy(log[logIndex].name, emp[index].name);

        log[logIndex].day = day;
        log[logIndex].month = month;
        log[logIndex].year = year;

        log[logIndex].timeIn = ABSENT_TIME;
        log[logIndex].timeOut = ABSENT_TIME;

        log[logIndex].late = 0;
        log[logIndex].overtime = 0;
        log[logIndex].undertime = 0;

        log[logIndex].lateCount = 0;
        log[logIndex].absences = 1;

        strcpy(log[logIndex].status, getStatus(emp[index].absences));

        saveEmployees();
        saveLogs();

        printf("Absent recorded!\n");
        return;
    }

    int hIn, mIn, hOut, mOut;

    while (1)
    {
        printf("Enter Time In (HH MM): ");
        scanf("%d %d", &hIn, &mIn);
        if (isValidTime(hIn, mIn))
            break;
        printf("Invalid time!\n");
    }

    while (1)
    {
        printf("Enter Time Out (HH MM): ");
        scanf("%d %d", &hOut, &mOut);
        if (isValidTime(hOut, mOut) &&
            toMinutes(hOut, mOut) > toMinutes(hIn, mIn))
            break;
        printf("Invalid time!\n");
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

    emp[index].lateCount = (late > 0) ? 1 : 0;
    emp[index].absences = 0;

    strcpy(log[logIndex].empID, emp[index].empID);
    strcpy(log[logIndex].name, emp[index].name);

    log[logIndex].day = day;
    log[logIndex].month = month;
    log[logIndex].year = year;

    log[logIndex].timeIn = toMinutes(hIn, mIn);
    log[logIndex].timeOut = toMinutes(hOut, mOut);

    log[logIndex].late = late;
    log[logIndex].overtime = overtime;
    log[logIndex].undertime = undertime;

    log[logIndex].lateCount = emp[index].lateCount;
    log[logIndex].absences = emp[index].absences;

    strcpy(log[logIndex].status,
           getStatus(emp[index].absences));

    saveEmployees();
    saveLogs();

    printf("Attendance updated successfully!\n");
}

// VIEW HISTORY
void viewHistory()
{
    if (logCount == 0)
    {
        printf("\nNo history records.\n");
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

        found = 1;

        printf("%02d/%02d/%04d   ",
               log[idx].month,
               log[idx].day,
               log[idx].year);

        printf("%-10s %-20s ",
               log[idx].empID,
               log[idx].name);

        if (log[idx].timeIn == ABSENT_TIME)
        {
            printf("%-10s %-10s %-8s %-8s %-8s %-8s\n",
                   "ABSENT", "ABSENT", "00:00", "00:00", "00:00", "00:00");
        }
        else
        {
            int total = log[idx].timeOut - log[idx].timeIn;

            printf("%02d:%02d     ", log[idx].timeIn / 60, log[idx].timeIn % 60);
            printf(" %02d:%02d     ", log[idx].timeOut / 60, log[idx].timeOut % 60);
            printf(" %02d:%02d   ", log[idx].late / 60, log[idx].late % 60);
            printf(" %02d:%02d   ", log[idx].overtime / 60, log[idx].overtime % 60);
            printf(" %02d:%02d   ", log[idx].undertime / 60, log[idx].undertime % 60);
            printf(" %02d:%02d\n", total / 60, total % 60);
        }
    }

    if (!found)
        printf("No records found for %02d/%02d/%04d.\n",
               month, day, year);
}

// VIEW EMPLOYEES
void viewEmployees()
{
    if (logCount == 0)
    {
        printf("\nNo records.\n");
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
            if (strcmp(emp[i].empID, log[j].empID) != 0)
                continue;

            int idx = findLogByDate(emp[i].empID,
                                    log[j].day,
                                    log[j].month,
                                    log[j].year);

            if (idx != j)
                continue;

            if (log[idx].timeIn == ABSENT_TIME)
                absent++;
            else if (log[idx].late > 0)
                late++;
        }

        printf("%-10s %-20s %-10d %-10d %-10s\n",
               emp[i].empID,
               emp[i].name,
               late,
               absent,
               getStatus(absent));
    }
}

// MAIN
int main()
{
    int choice;

    emp = malloc(empCapacity * sizeof(Employee));
    log = malloc(logCapacity * sizeof(Attendance));

    if (emp == NULL || log == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    loadEmployees();
    loadLogs();

    do
    {
        printf("\n===============================\n");
        printf("\n     Employee Time Tracker     \n");
        printf("\n===============================\n\n");

        printf("[1] Add New Employee\n");
        printf("[2] Update Existing Employee\n");
        printf("[3] View Time Record\n");
        printf("[4] View Late/Absent Record\n");
        printf("[5] Exit\n\n");

        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            addNewEmployee();
            break;
        case 2:
            updateEmployee();
            break;
        case 3:
            viewHistory();
            break;
        case 4:
            viewEmployees();
            break;
        case 5:
            printf("\n===============================\n");
            printf("\n        End Of Program         \n");
            printf("\n===============================\n");
            break;
        default:
            printf("Invalid choice!\n");
        }

    } while (choice != 5);

    free(emp);
    free(log);

    return 0;
}
