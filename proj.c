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

#define ERROR(O) printf(O); return

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

const Activity TO_DO = {"TO DO"}, IN_PROGRESS = {"IN_PROGRESS"}, DONE = {"DONE"};

Manager manager = {0};
int user_count = 0, activity_count = 3, task_count = 0;

int isNumerical(char* string) {
    int i = ZERO;
    char c;
    while((c = string[i++]) != NULL_CHARACTER)
        if(c < '0' || c > '9')
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

void addTask(char* input) {
    Task task;
    int duration, i = ZERO, j = ZERO;
    char description[TASK_DESCRIPTION];
    char* token, c;

    token = nextToken(input);
    if(strlen(token) == ZERO || isNumerical(token) == FALSE) {
        ERROR("invalid duration");
    }
    duration = atoi(token);
    free(token);

    while(strlen(token = nextToken(input)) != ZERO) {
        while((c = token[j++]) != NULL_CHARACTER)
            description[i++] = c;
        description[i++] = WHITESPACE;
        j = ZERO;
    }
    description[i] = NULL_CHARACTER;
    free(token);

    if(task_count == TASK_MAX) {
        ERROR("too many tasks");
    }

    task.id = task_count + 1;
    task.duration = duration;
    task.start = manager.clock;
    strcpy(task.description, description);
    task.activity = TO_DO;

    printf("task %d", task.id);

    manager.tasks[task_count++] = task;
}

void printTasks(int* ids, int id_count) {
    Task task;
    int i = 0;

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
                printf("%s: no such task", token);
                return;
            }
            ids[i++] = id - 1;
        }
        printTasks(ids, i);
    }
}

void clockStep(char* input) {
    char* token;
    int delta;

    if(isNumerical(token = nextToken(input)) == FALSE || strlen(input) != ZERO) {
        ERROR("invalid time");
    }
    delta = atoi(token);
    if(delta != ZERO)
        manager.clock += delta;
    printf("%d", manager.clock);
}

void addUser(char* input) {
    User task;
    int duration, i = ZERO, j = ZERO;
    char name[USER_NAME];
    char* token, c;

    while(strlen(token = nextToken(input)) != ZERO) {
        while((c = token[j++]) != NULL_CHARACTER)
            name[i++] = c;
        name[i++] = WHITESPACE;
        j = ZERO;
    }
    name[i] = NULL_CHARACTER;
    free(token);

    if(task_count == TASK_MAX) {
        ERROR("too many tasks");
    }

    strcpy(task.description, description);

    printf("task %d", task.id);

    manager.tasks[task_count++] = task;
}

int parseInput(char* input) {
    Command command;
    fgets(input, COMMAND_SIZE, stdin);
    command = nextToken(input)[0];

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
        /*case M:
            moveTask(input);
            break;
        case D:
            listTasksInActivity(input);
            break;
        case A:
            addActivity(input);
            break;*/
    }

    return FALSE;
}

int main() {
    char input[COMMAND_SIZE];
    while(parseInput(input) != QUIT)
        putchar(NEW_LINE);
    
    return 0;
}