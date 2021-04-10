#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define ZERO 0
#define QUIT -1

#define COMMAND_SIZE 100
#define NEW_LINE '\n'
#define WHITESPACE ' '
#define NULL_CHARACTER '\0'

#define TASK_DESCRIPTION 50
#define TASK_MAX 10000

#define USER_NAME 20
#define USER_MAX 50

#define ACTIVITY_NAME 20
#define ACTIVITY_MAX 10

#define ERROR(S, O) printf(S, O); return

typedef struct {
    char name[USER_NAME];
} User;

typedef struct {
    char name[ACTIVITY_NAME];
} Activity;

typedef struct {
    int id, duration, start;
    char description[TASK_DESCRIPTION];
    User user;
    Activity activity;
} Task;

typedef struct {
    int clock;
    User users[USER_MAX];
    Activity activities[ACTIVITY_MAX];
    Task tasks[TASK_MAX];
} Manager;

enum Command {Q = 'q', T = 't', L = 'l', N = 'n', U = 'u', M = 'm', D = 'd', A = 'a'};
typedef enum Command Command;

const Activity TO_DO = {"TO DO"}, IN_PROGRESS = {"IN PROGRESS"}, DONE = {"DONE"};

Manager manager;
int user_count = ZERO, activity_count = ZERO, task_count = ZERO;

int isNumerical(char* string) {
    int i = ZERO;
    char c;
    
    while((c = string[i++]) != NULL_CHARACTER)
        if(c < '0' || c > '9')
            return FALSE;
    return TRUE;
}

int isUpperCase(char* string) {
    int i = ZERO;
    char c;

    while((c = string[i++]) != NULL_CHARACTER)
        if(c != ' ' && (c < 'A' || c > 'Z'))
            return FALSE;
    return TRUE;
}

char* nextToken(char* input) {
    char* token = (char*) malloc(COMMAND_SIZE * sizeof(char));
    char c;
    int i = ZERO, j = ZERO;

    while((c = input[i]) != WHITESPACE && c != NEW_LINE)
        token[i++] = c;
    i++;
    while((c = input[i++]) != NULL_CHARACTER)
        input[j++] = c;
    input[j] = NULL_CHARACTER;

    return token;
}

void readLine(char* input, char* output) {
    int i = ZERO;
    char* token;

    output[ZERO] = NULL_CHARACTER;
    while(strlen(token = nextToken(input)) != ZERO) {
        strcat(output, token);
        i += strlen(token);
        output[i++] = WHITESPACE;
        output[i] = NULL_CHARACTER;
    }
    output[i-1] = NULL_CHARACTER;

    free(token);
}

void addTask(char* input) {
    Task task;
    int duration, i = ZERO;
    char description[TASK_DESCRIPTION];
    char* token;

    token = nextToken(input);
    if(strlen(token) == ZERO || isNumerical(token) == FALSE) {
        ERROR("%s\n", "invalid duration");
    }
    duration = atoi(token);
    free(token);

    readLine(input, description);

    for(i = ZERO; i < task_count; i++) {
        if(strcmp(manager.tasks[i].description, description) == ZERO) {
            ERROR("%s\n", "duplicate description");
        }
    }

    if(task_count == TASK_MAX) {
        ERROR("%s\n", "too many tasks");
    }

    task.id = task_count + 1;
    task.duration = duration;
    task.start = ZERO;
    strcpy(task.description, description);
    task.activity = TO_DO;

    printf("task %d\n", task.id);

    manager.tasks[task_count++] = task;
}

void printTasks(int* ids, int id_count) {
    Task task;
    int i = ZERO;

    if(id_count == ZERO) {
        /*TODO: IMPLEMENT SORTING*/
    }
    else {
        while(i < id_count) {
            task = manager.tasks[ids[i++]];
            printf("%d %s #%d %s\n", task.id, task.activity.name, task.duration, task.description);
        }
    }
}

void listTasks(char* input) {
    int i = ZERO, id;
    char* token;
    int* ids;

    if(sizeof(input) == ZERO)
        printTasks(NULL, TASK_MAX);
    else {
        ids = (int*) malloc(TASK_MAX * sizeof(int));
        while(strlen(token = nextToken(input)) != ZERO) {
            if(isNumerical(token) == FALSE || (id = atoi(token)) > task_count) {
                ERROR("%s: no such task\n", token);
            }
            ids[i++] = id - 1;
        }
        printTasks(ids, i);
    }

    free(token);
    free(ids);
}

void clockStep(char* input) {
    char* token;
    int delta;

    if(isNumerical(token = nextToken(input)) == FALSE) {
        free(token);
        ERROR("%s\n", "invalid time");
    }

    delta = atoi(token);
    free(token);
    if(delta != ZERO)
        manager.clock += delta;
    
    printf("%d\n", manager.clock);
}

void printAllUsers() {
    int i;

    for(i = ZERO; i < user_count; i++)
        printf("%s\n", manager.users[i].name);
}

void printAllActivities() {
    int i;

    for(i = ZERO; i < activity_count; i++)
        printf("%s\n", manager.activities[i].name);
}

int userExists(char* name) {
    int i;

    for(i = ZERO; i < user_count; i++)
        if(strcmp(manager.users[i].name, name) == ZERO)
            return TRUE;
    return FALSE;
}

int activityExists(char* name) {
    int i;

    for(i = ZERO; i < activity_count; i++)
        if(strcmp(manager.activities[i].name, name) == ZERO)
            return TRUE;
    return FALSE;
}

int taskExists(int id) {
    int i;

    for(i = ZERO; i < task_count; i++)
        if(manager.tasks[i].id == id)
            return TRUE;
    return FALSE;
}

void addUser(char* input) {
    char* token;
    User user;

    if(strlen(input) == ZERO) {
        printAllUsers();
        return;
    }

    token = nextToken(input);
    strcpy(user.name, token);
    free(token);

    if(userExists(user.name) == TRUE) {
        ERROR("%s\n", "user already exists");
    }
        
    if(user_count == USER_MAX) {
        ERROR("%s\n", "too many users");
    }

    manager.users[user_count++] = user;
}

void moveTask(char* input) {
    int id, spent, slack, i;
    char* token;
    char user_name[USER_NAME], activity_name[ACTIVITY_NAME];

    if(isNumerical(token = nextToken(input)) == FALSE || taskExists(id = atoi(token)) == FALSE) {
        free(token);
        ERROR("%s\n", "no such task");
    }
    
    if(userExists(token = nextToken(input)) == FALSE) {
        free(token);
        ERROR("%s\n", "no such user");
    }
    strcpy(user_name, token);
    free(token);

    readLine(input, activity_name);
    if(activityExists(activity_name) == FALSE) {
        ERROR("%s\n", "no such activity");
    }
    if(strcmp(activity_name, TO_DO.name) == ZERO) {
        ERROR("%s\n", "task already started");
    }

    for(i = ZERO; i < task_count; i++) {
        if(manager.tasks[i].id == id) {
            if(strcmp(manager.tasks[i].activity.name, TO_DO.name) == ZERO)
                manager.tasks[i].start = manager.clock;
            if(strcmp(activity_name, DONE.name) == ZERO) {
                spent = manager.clock - manager.tasks[i].start;
                slack = spent - manager.tasks[i].duration;
                printf("duration=%d slack=%d\n", spent, slack);
            }
            strcpy(manager.tasks[i].user.name, user_name);
            strcpy(manager.tasks[i].activity.name, activity_name);
            break;
        }
    }
}

void listTasksInActivity(char* input) {
    int i;
    char activity_name[ACTIVITY_NAME];

    readLine(input, activity_name);
    if(activityExists(activity_name) == FALSE) {
        ERROR("%s\n", "no such activity");
    }

    /*TODO: SORT*/
    for(i = ZERO; i < task_count; i++)
        if(strcmp(manager.tasks[i].activity.name, activity_name) == ZERO)
            printf("%d %d %s\n", manager.tasks[i].id, manager.tasks[i].start, manager.tasks[i].description);
}

void addActivity(char* input) {
    char activity_name[ACTIVITY_NAME];

    if(strlen(input) == ZERO) {
        printAllActivities();
        return;
    }

    readLine(input, activity_name);

    if(activity_count == ACTIVITY_MAX) {
        ERROR("%s\n", "too many activities");   
    }

    if(activityExists(activity_name) == TRUE) {
        ERROR("%s\n", "duplicate activity");
    }

    if(isUpperCase(activity_name) == FALSE) {
        ERROR("%s\n", "invalid description");
    }

    strcpy(manager.activities[activity_count++].name, activity_name); 
}

int parseInput(char* input) {
    char* token;
    Command command;

    fgets(input, COMMAND_SIZE, stdin);
    token = nextToken(input);
    command = token[ZERO];
    free(token);

    switch(command){
        case Q:
            return QUIT;
        case T:
            addTask(input);
            break;
        case L:
            listTasks(input);
            break;
        case N:
            clockStep(input);
            break;
        case U:
            addUser(input);
            break;
        case M:
            moveTask(input);
            break;
        case D:
            listTasksInActivity(input);
            break;
        case A:
            addActivity(input);
            break;
    }

    return FALSE;
}

void init() {
    int i = ZERO;

    strcpy(manager.activities[i++].name, TO_DO.name);
    activity_count++;
    strcpy(manager.activities[i++].name, IN_PROGRESS.name);
    activity_count++;
    strcpy(manager.activities[i++].name, DONE.name);
    activity_count++;
}

int main() {
    char input[COMMAND_SIZE];
    
    init(); 
    while(parseInput(input) != QUIT)
        ;
    
    return ZERO;
}