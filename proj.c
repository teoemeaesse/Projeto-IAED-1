/*---INCLUDES---*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*---DEFINE MACROS---*/

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

/*---DEFINE MACROS---*/

#define ID_TO_INDEX(ID) ID - 1
#define IS_NUMERICAL(C) (C >= '0' && C <= '9')
#define IS_ALPHABETICAL(C) ((C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z'))
#define IS_UPPERCASE(C) (C >= 'A' && C <= 'Z')

#define ERROR(S, O) printf(S, O); return
#define MALLOC(S, T) (T*) malloc(S * sizeof(T))

/*---DEFINE STRUCTS---*/

/* 
data structure to represent a user in the system;
+1 to account for null character
*/  
typedef struct {
    char name[USER_NAME + 1];
} User;

/*
data structure to represent an activity in the system;
+1 to account for null character
*/
typedef struct {
    char name[ACTIVITY_NAME + 1];
} Activity;

/*
data structure to represent a task in the system;
+1 to account for null character
*/
typedef struct {
    int id, duration, start;
    char description[TASK_DESCRIPTION + 1];
    User user;
    Activity activity;
} Task;

/*
data structure to hold the system's state:
    internal time,
    user info,
    activity info,
    task info
*/
typedef struct {
    int clock;
    User users[USER_MAX];
    Activity activities[ACTIVITY_MAX];
    Task tasks[TASK_MAX];
} Manager;

/*
union type to hold comparable values i.e. strings and integers
*/
typedef union {
    char* string;
    int integer;
} Comparable;

/*
data structure consisting of a dynamic array of 
comparable elements
*/
typedef struct {
    Comparable* elements;
} Sortable;

/*
prototype for a function to compare two comparable items
*/
typedef int (*CompareFunction)(Comparable, Comparable);


/*---DEFINE ENUMS---*/

enum Command {Q = 'q', T = 't', L = 'l', N = 'n', U = 'u', M = 'm', D = 'd', A = 'a'};
typedef enum Command Command;

const Activity TO_DO = {"TO DO"}, IN_PROGRESS = {"IN PROGRESS"}, DONE = {"DONE"};


/*---GLOBAL VARIABLES---*/

Manager manager;
int user_count = ZERO, activity_count = ZERO, task_count = ZERO;


/*---AUXILIARY FUNCTIONS---*/

/*
isNumerical: char* -> int
    checks for non-numerical characters
    in the provided string and returns
    FALSE if any are found, TRUE otherwise.
*/
int isNumerical(char* string) {
    int i = ZERO;
    char c;
    
    /* look for non-numerical characters */
    while((c = string[i++]) != NULL_CHARACTER)
        if(!IS_NUMERICAL(c))
            return FALSE;
    return TRUE;
}

/*
isUpperCase: char* -> int
    checks for lower-case characters
    in the provided string and returns
    FALSE if any are found, TRUE otherwise.
*/
int isUpperCase(char* string) {
    int i = ZERO;
    char c;

    /* look for lower case alphabetical characters */
    while((c = string[i++]) != NULL_CHARACTER)
        if(IS_ALPHABETICAL(c) && !IS_UPPERCASE(c))
            return FALSE;
    return TRUE;
}

/*
nextToken: char* -> char*
    returns the first whitespace-separated
    token found in the provided string
    and removes it along with with any
    trailing whitespace from the provided string
*/
char* nextToken(char* input) {
    char* token = MALLOC(COMMAND_SIZE, char);
    char c;
    int i = ZERO, j = ZERO;

    while((c = input[i]) != WHITESPACE && c != NULL_CHARACTER)
        token[i++] = c;
    /* remove whitespace characters */
    while((c = input[i]) == WHITESPACE)
        i++;
    /* remove token from input string */
    while((c = input[i++]) != NULL_CHARACTER)
        input[j++] = c;
    input[j] = NULL_CHARACTER;

    return token;
}

/*
compareIntegers: Comparable, Comparable -> int
    auxiliary function for the mergeSort algorithm;
    compares two Comparables, assuming they are integers
*/
int compareIntegers(Comparable n1, Comparable n2) {
    return n1.integer < n2.integer;
}

/*
compareStrings: Comparable, Comparable -> int
    auxiliary function for the mergeSort algorithm;
    compares two Comparables, assuming they are strings
*/
int compareStrings(Comparable s1, Comparable s2) {
    return strcmp(s1.string, s2.string) < ZERO;
}

/*
merge: CompareFunction, Sortable, Sortable, int, int, int ->
    auxiliary function for the mergeSort algorithm;
    merges two ordered Sortable arrays
*/
void merge(CompareFunction compare, Sortable unsorted, Sortable aux, int left, int middle, int right) {
    int i = left, j = middle, k = ZERO;
    
    while(i < middle && j <= right)
        aux.elements[k++] = compare(unsorted.elements[i], unsorted.elements[j]) ? unsorted.elements[i++] : unsorted.elements[j++];
    
    if(i == middle)
        while(j <= right)
            aux.elements[k++] = unsorted.elements[j++];
    else
        while(i < middle)
            aux.elements[k++] = unsorted.elements[i++];

    for(k = left; k <= right; k++)
        unsorted.elements[k] = aux.elements[k - left];
}

/*
mergeSort: CompareFunction, Sortable, Sortable, int, int ->
    recursive merge sort algorithm to sort any Sortable arrays
*/
void mergeSort(CompareFunction compare, Sortable unsorted, Sortable aux, int left, int right) {
    int middle = (left + right) / 2;

    if(right <= left)
        return;

    mergeSort(compare, unsorted, aux, left, middle);
    mergeSort(compare, unsorted, aux, middle + 1, right);

    merge(compare, unsorted, aux, left, middle + 1, right);
}

/*
getSortedTasks: -> int*
    returns a sorted int array containing
    all the task ids, ordered alphabetically
*/
int* getSortedTasks() {
    int* ids = MALLOC(task_count, int);
    int i, j;
    Sortable descriptions, aux;

    /* allocate memory and initialize descriptions array */
    descriptions.elements = MALLOC(task_count, Comparable);
    aux.elements = MALLOC(task_count, Comparable);
    for(i = ZERO; i < task_count; i++){
        descriptions.elements[i].string = MALLOC(TASK_DESCRIPTION, char);
        aux.elements[i].string = MALLOC(TASK_DESCRIPTION, char);
        strcpy(descriptions.elements[i].string, manager.tasks[i].description);
    }

    /* sort descriptions alphabetically */
    mergeSort(*compareStrings, descriptions, aux, ZERO, task_count - 1);

    /* get sorted id list and free up memory */
    for(i = ZERO; i < task_count; i++) {
        for(j = ZERO; j < task_count; j++)
            if(strcmp(descriptions.elements[i].string, manager.tasks[j].description) == ZERO)
                ids[i] = manager.tasks[j].id;
        free(descriptions.elements[i].string);
    }
    free(descriptions.elements);

    return ids;
}

/*
printTasksInActivity: char* ->
    prints sorted tasks in a given activity;
    algorithm sorts task start time in
    ascending order, and does a secondary
    alphabetical sort
*/
void printTasksInActivity(char activity_name[]) {
    Sortable start_times, sorted;
    int *ids = getSortedTasks();
    int i, j, count = ZERO;

    /* get all task ids to sort */
    for(i = ZERO, j = ZERO; i < task_count; i++) {
        if(strcmp(manager.tasks[ID_TO_INDEX(ids[i])].activity.name, activity_name) == ZERO) {
            ids[j++] = ids[i];
            count++;
        }
    }

    /* allocate memory and initialize start_times array */
    start_times.elements = MALLOC(count, Comparable);
    sorted.elements = MALLOC(count, Comparable);
    for(i = ZERO; i < count; i++)
        start_times.elements[i].integer = manager.tasks[ID_TO_INDEX(ids[i])].start;

    /* sort by start time */
    mergeSort(*compareIntegers, start_times, sorted, ZERO, count - 1);

    /* get sorted start times */
    for(i = ZERO; i < count; i++) {
        for(j = ZERO; j < count; j++) {
            if(ids[j] != QUIT && start_times.elements[i].integer == manager.tasks[ID_TO_INDEX(ids[j])].start) {
                sorted.elements[i].integer = ids[j];
                ids[j] = QUIT;
                break;
            }
        }
    }
    
    for(i = ZERO; i < count; i++)
        printf("%d %d %s\n", sorted.elements[i].integer, start_times.elements[i].integer, manager.tasks[ID_TO_INDEX(sorted.elements[i].integer)].description);

    free(ids);
    free(start_times.elements);
    free(sorted.elements);
}

/*
printTasks: int*, int ->
    prints all tasks associated with
    the provided list of ids
*/
void printTasks(int* ids, int id_count) {
    Task task;
    int i = ZERO;

    if(id_count == ZERO) {
        printTasks(getSortedTasks(), task_count);
    }
    else {
        while(i < id_count) {
            task = manager.tasks[ID_TO_INDEX(ids[i++])];
            printf("%d %s #%d %s\n", task.id, task.activity.name, task.duration, task.description);
        }
    }
}

/*
printAllUsers: ->
    prints all registered users
*/
void printAllUsers() {
    int i;

    for(i = ZERO; i < user_count; i++)
        printf("%s\n", manager.users[i].name);
}

/*
printAllActivities: ->
    prints all registered activities
*/
void printAllActivities() {
    int i;

    for(i = ZERO; i < activity_count; i++)
        printf("%s\n", manager.activities[i].name);
}

/*
userExists: char* -> int
    returns TRUE if a user
    with the provided name exists,
    returns FALSE otherwise
*/
int userExists(char* name) {
    int i;

    for(i = ZERO; i < user_count; i++)
        if(strcmp(manager.users[i].name, name) == ZERO)
            return TRUE;
            
    return FALSE;
}

/*
activityExists: char* -> int
    returns TRUE if an activity
    with the provided name exists,
    returns FALSE otherwise
*/
int activityExists(char* name) {
    int i;

    for(i = ZERO; i < activity_count; i++)
        if(strcmp(manager.activities[i].name, name) == ZERO)
            return TRUE;
    return FALSE;
}

/*
taskExists: int -> int
    returns TRUE if a task
    corresponding to the provided
    id exists, returns FALSE otherwise
*/
int taskExists(int id) {
    int i;

    for(i = ZERO; i < task_count; i++)
        if(manager.tasks[i].id == id)
            return TRUE;
    return FALSE;
}


/*---MAIN FUNCTIONS---*/

/*
addTask: char* ->
    adds a new task to the system
*/
void addTask(char* input) {
    Task task;
    int duration, i = ZERO;
    char description[TASK_DESCRIPTION];
    char* token;

    token = nextToken(input);
    if(strlen(token) == ZERO || !isNumerical(token)) {
        ERROR("%s\n", "invalid duration");
    }
    duration = atoi(token);
    free(token);

    if(strlen(input) == ZERO || strlen(input) > TASK_DESCRIPTION)
        return;
    strcpy(description, input);

    for(i = ZERO; i < task_count; i++) {
        if(strcmp(manager.tasks[i].description, description) == ZERO) {
            ERROR("%s\n", "duplicate description");
        }
    }

    if(task_count == TASK_MAX) {
        ERROR("%s\n", "too many tasks");
    }

    /* construct the task, print its id and add it to the system */
    task.id = task_count + 1;
    task.duration = duration;
    task.start = ZERO;
    strcpy(task.description, description);
    task.activity = TO_DO;

    printf("task %d\n", task.id);

    manager.tasks[task_count++] = task;
}

/*
listTasks: char* ->
    lists all tasks OR
    lists all tasks specified by the user
*/
void listTasks(char* input) {
    int i = ZERO, id;
    char* token;
    int* ids;

    if(task_count == ZERO)
        return;

    if(sizeof(input) == ZERO)
        printTasks(NULL, TASK_MAX);
    else {
        ids = MALLOC(TASK_MAX, int);
        while(strlen(token = nextToken(input)) != ZERO) {
            if(!isNumerical(token) || (id = atoi(token)) > task_count) {
                ERROR("%s: no such task\n", token);
            }
            ids[i++] = id;
        }
        printTasks(ids, i);
    }
}

/*
clocksStep: char* ->
    prints current simulated time OR
    increases simulated time by a
    user-specified amount 
*/
void clockStep(char* input) {
    char* token;
    int delta;

    token = nextToken(input);
    if(!isNumerical(token)) {
        free(token);
        ERROR("%s\n", "invalid time");
    }

    delta = atoi(token);
    free(token);
    if(delta != ZERO)
        manager.clock += delta;
    
    printf("%d\n", manager.clock);
}

/*
addUser: char* -> 
    registers user OR
    lists all users
*/
void addUser(char* input) {
    char *token, *original = MALLOC(strlen(input), char);
    User user;

    if(strlen(input) == ZERO) {
        printAllUsers();
        return;
    }

    strcpy(original, input);

    token = nextToken(input);

    if(userExists(token)) {
        ERROR("%s\n", "user already exists");
    }
    if(user_count == USER_MAX) {
        ERROR("%s\n", "too many users");
    }

    if(strlen(original) > USER_NAME)
        return;

    strcpy(user.name, token);

    if((strlen(original) != ZERO && strcmp(user.name, original) != ZERO))
        return;
    free(token);

    manager.users[user_count++] = user;
}

/*
moveTask: char* ->
    moves a task from one
    activity to another
*/
void moveTask(char* input) {
    int id, spent, slack, i;
    char* token;
    char user_name[USER_NAME], activity_name[ACTIVITY_NAME];

    token = nextToken(input);
    if(!isNumerical(token) || !taskExists(id = atoi(token))) {
        ERROR("%s\n", "no such task");
    }
    
    token = nextToken(input);
    if(!userExists(token)) {
        ERROR("%s\n", "no such user");
    }
    strcpy(user_name, token);
    free(token);

    strcpy(activity_name, input);

    if(!activityExists(activity_name)) {
        ERROR("%s\n", "no such activity");
    }
    if(strcmp(activity_name, TO_DO.name) == ZERO) {
        ERROR("%s\n", "task already started");
    }

    /* search for the specified task by id */
    for(i = ZERO; i < task_count; i++) {
        if(manager.tasks[i].id == id) {
            if(strcmp(manager.tasks[i].activity.name, TO_DO.name) == ZERO)
                manager.tasks[i].start = manager.clock;

            if(strcmp(activity_name, DONE.name) == ZERO) {
                spent = manager.clock - manager.tasks[i].start;
                slack = spent - manager.tasks[i].duration;
                printf("duration=%d slack=%d\n", spent, slack);
            }

            /* change associated user and activity */
            strcpy(manager.tasks[i].user.name, user_name);
            strcpy(manager.tasks[i].activity.name, activity_name);
            break;
        }
    }
}

/*
listTasksInActivity: char* ->
    lists all tasks associated
    with a given activity
*/
void listTasksInActivity(char* input) {
    char activity_name[ACTIVITY_NAME];

    strcpy(activity_name, input);
    if(!activityExists(activity_name)) {
        ERROR("%s\n", "no such activity");
    }

    printTasksInActivity(activity_name);
}

/*
addActivity: char* ->
    creates a new user-specified activity
*/
void addActivity(char* input) {
    char activity_name[ACTIVITY_NAME];

    if(strlen(input) == ZERO) {
        printAllActivities();
        return;
    }

    if(strlen(input) > ACTIVITY_NAME)
        return;

    strcpy(activity_name, input);

    if(activity_count == ACTIVITY_MAX) {
        ERROR("%s\n", "too many activities");   
    }
    if(activityExists(activity_name)) {
        ERROR("%s\n", "duplicate activity");
    }
    if(!isUpperCase(activity_name)) {
        ERROR("%s\n", "invalid description");
    }

    strcpy(manager.activities[activity_count++].name, activity_name);
}

/*
parseInput: -> int
    parses all user input
*/
int parseInput() {
    char* token;
    char input[COMMAND_SIZE];
    Command command;

    fgets(input, COMMAND_SIZE, stdin);
    token = nextToken(input);
    command = token[ZERO];
    free(token);
    
    /* remove the trailing \n character  */
    input[strlen(input) - 1] = NULL_CHARACTER;

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

/*
init: ->
    initializes global variables
*/
void init() {
    int i = ZERO;

    strcpy(manager.activities[i++].name, TO_DO.name);
    activity_count++;
    strcpy(manager.activities[i++].name, IN_PROGRESS.name);
    activity_count++;
    strcpy(manager.activities[i++].name, DONE.name);
    activity_count++;
}

/*
main: -> int
    main function, begins execution and
    finalizes when user enters the QUIT command
*/
int main() {
    init(); 
    while(parseInput() != QUIT)
        ;
    
    return ZERO;
}