#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100

// --- 1. STRUCT DEFINITIONS ---
typedef struct {
    int id;
    char username[30];
    char password[30];
    char name[50];
    char contact[20];
} User;

typedef struct {
    int id;
    int posterId;    // The user who created the task
    int assigneeId;  // The user who accepted the task (0 means unassigned)
    char description[100];
    float fee;
} Task;

// --- 2. GLOBAL VARIABLES ---
User users[MAX];
Task tasks[MAX];
int userCount = 0;
int taskCount = 0;
int loggedInUserId = 0; // 0 means no one is logged in

// --- 3. SUBPROGRAM PROTOTYPES ---
void loadFiles();
void saveFiles();
void authMenu();
void userDashboard();
void registerUser();
int loginUser();
void postTask();
void findTask();
void viewAssignedTasks();
void viewPostedTasks();
void editProfile();
void deleteProfile();
void clearBuffer();

// --- 4. MAIN FUNCTION ---
int main() {
    loadFiles();
    authMenu();
    return 0;
}

// --- 5. AUTHENTICATION & MENUS ---
void authMenu() {
    int choice;
    do {
        printf("\n=== CAMPUS ERRAND SYSTEM ===\n");
        printf("1. Login\n");
        printf("2. Register New Account\n");
        printf("0. Exit & Save\n");
        printf("Choice: ");
        scanf("%d", &choice);
        clearBuffer();

        if (choice == 1) {
            loggedInUserId = loginUser();
            if (loggedInUserId != 0) {
                userDashboard(); // Go to main app if login success
            }
        } else if (choice == 2) {
            registerUser();
        } else if (choice == 0) {
            saveFiles();
            printf("Data saved. Goodbye!\n");
        }
    } while (choice != 0);
}

void userDashboard() {
    int choice;
    do {
        printf("\n=== USER DASHBOARD (ID: %d) ===\n", loggedInUserId);
        printf("1. Post a New Task\n");
        printf("2. Find Tasks (Accept a job)\n");
        printf("3. View My Assigned Tasks (Jobs I accepted)\n");
        printf("4. View My Posted Tasks (Jobs I created)\n");
        printf("5. Edit My Profile\n");
        printf("6. Delete My Profile\n");
        printf("0. Logout\n");
        printf("Choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch(choice) {
            case 1: postTask(); break;
            case 2: findTask(); break;
            case 3: viewAssignedTasks(); break;
            case 4: viewPostedTasks(); break;
            case 5: editProfile(); break;
            case 6: deleteProfile(); break;
            case 0: 
                loggedInUserId = 0; 
                printf("Logged out successfully.\n"); 
                break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0 && loggedInUserId != 0);
}

// --- 6. USER OPERATIONS ---
void registerUser() {
    if (userCount >= MAX) return;
    
    users[userCount].id = userCount + 1; // Auto-generate ID starting from 1
    printf("Enter Username: ");
    fgets(users[userCount].username, 30, stdin);
    users[userCount].username[strcspn(users[userCount].username, "\n")] = 0;
    
    printf("Enter Password: ");
    fgets(users[userCount].password, 30, stdin);
    users[userCount].password[strcspn(users[userCount].password, "\n")] = 0;
    
    printf("Enter Full Name: ");
    fgets(users[userCount].name, 50, stdin);
    users[userCount].name[strcspn(users[userCount].name, "\n")] = 0;

    printf("Enter Contact Number: ");
    fgets(users[userCount].contact, 20, stdin);
    users[userCount].contact[strcspn(users[userCount].contact, "\n")] = 0;

    userCount++;
    printf("Registration successful! You can now login.\n");
}

int loginUser() {
    char user[30], pass[30];
    printf("Username: ");
    fgets(user, 30, stdin); user[strcspn(user, "\n")] = 0;
    printf("Password: ");
    fgets(pass, 30, stdin); pass[strcspn(pass, "\n")] = 0;

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, user) == 0 && strcmp(users[i].password, pass) == 0) {
            printf("Login successful! Welcome, %s.\n", users[i].name);
            return users[i].id;
        }
    }
    printf("Invalid username or password!\n");
    return 0; // 0 means failed
}

void editProfile() {
    for (int i = 0; i < userCount; i++) {
        if (users[i].id == loggedInUserId) {
            printf("Current Name: %s\nEnter New Name: ", users[i].name);
            fgets(users[i].name, 50, stdin);
            users[i].name[strcspn(users[i].name, "\n")] = 0;
            
            printf("Current Contact: %s\nEnter New Contact: ", users[i].contact);
            fgets(users[i].contact, 20, stdin);
            users[i].contact[strcspn(users[i].contact, "\n")] = 0;
            printf("Profile updated!\n");
            return;
        }
    }
}

void deleteProfile() {
    char confirm;
    printf("Are you sure you want to delete your profile? (y/n): ");
    scanf("%c", &confirm);
    clearBuffer();

    if (confirm == 'y' || confirm == 'Y') {
        for (int i = 0; i < userCount; i++) {
            if (users[i].id == loggedInUserId) {
                // Shift remaining users to the left to overwrite the deleted user
                for (int j = i; j < userCount - 1; j++) {
                    users[j] = users[j + 1];
                }
                userCount--;        // Decrease total user count
                loggedInUserId = 0; // Log the user out
                printf("Profile successfully deleted.\n");
                return;
            }
        }
    } else {
        printf("Profile deletion cancelled.\n");
    }
}

// --- 7. TASK OPERATIONS ---
void postTask() {
    tasks[taskCount].id = taskCount + 101; // Auto-generate ID starting from 101
    tasks[taskCount].posterId = loggedInUserId;
    tasks[taskCount].assigneeId = 0; // 0 means unassigned
    
    printf("Enter Task Description: ");
    fgets(tasks[taskCount].description, 100, stdin);
    tasks[taskCount].description[strcspn(tasks[taskCount].description, "\n")] = 0;
    
    printf("Enter Task Fee (RM): ");
    scanf("%f", &tasks[taskCount].fee);
    clearBuffer();
    
    taskCount++;
    printf("Task posted successfully!\n");
}

void findTask() {
    printf("\n--- Available Tasks ---\n");
    int found = 0;
    for (int i = 0; i < taskCount; i++) {
        // Show tasks that are unassigned (0) AND not created by the logged in user
        if (tasks[i].assigneeId == 0 && tasks[i].posterId != loggedInUserId) {
            printf("Task ID: %d | Desc: %s | Fee: RM%.2f\n", tasks[i].id, tasks[i].description, tasks[i].fee);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No tasks available right now.\n");
        return;
    }

    int acceptId;
    printf("\nEnter Task ID to accept (or 0 to cancel): ");
    scanf("%d", &acceptId);
    clearBuffer();
    
    if (acceptId != 0) {
        for (int i = 0; i < taskCount; i++) {
            if (tasks[i].id == acceptId && tasks[i].assigneeId == 0) {
                tasks[i].assigneeId = loggedInUserId; // Assign to current user
                printf("Task accepted successfully!\n");
                return;
            }
        }
        printf("Invalid Task ID.\n");
    }
}

void viewAssignedTasks() {
    printf("\n--- My Assigned Tasks (To Do) ---\n");
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].assigneeId == loggedInUserId) {
            printf("Task ID: %d | Desc: %s | Fee: RM%.2f\n", tasks[i].id, tasks[i].description, tasks[i].fee);
        }
    }
}

void viewPostedTasks() {
    printf("\n--- My Posted Tasks ---\n");
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].posterId == loggedInUserId) {
            printf("Task ID: %d | Desc: %s | Fee: RM%.2f | ", tasks[i].id, tasks[i].description, tasks[i].fee);
            
            if (tasks[i].assigneeId == 0) {
                printf("Status: Waiting for someone to accept\n");
            } else {
                // Find who accepted it to show their info
                for (int j = 0; j < userCount; j++) {
                    if (users[j].id == tasks[i].assigneeId) {
                        printf("Accepted By: %s (Contact: %s)\n", users[j].name, users[j].contact);
                        break;
                    }
                }
            }
        }
    }
}

// --- 8. HELPER & FILE I/O ---
void clearBuffer() {
    int c; while ((c = getchar()) != '\n' && c != EOF);
}

void loadFiles() {
    FILE *uf = fopen("users.txt", "r");
    if (uf != NULL) {
        while (fscanf(uf, "%d,%29[^,],%29[^,],%49[^,],%19[^\n]\n", &users[userCount].id, users[userCount].username, users[userCount].password, users[userCount].name, users[userCount].contact) != EOF) userCount++;
        fclose(uf);
    }
    FILE *tf = fopen("tasks.txt", "r");
    if (tf != NULL) {
        while (fscanf(tf, "%d,%d,%d,%f,%99[^\n]\n", &tasks[taskCount].id, &tasks[taskCount].posterId, &tasks[taskCount].assigneeId, &tasks[taskCount].fee, tasks[taskCount].description) != EOF) taskCount++;
        fclose(tf);
    }
}

void saveFiles() {
    FILE *uf = fopen("users.txt", "w");
    for (int i = 0; i < userCount; i++) fprintf(uf, "%d,%s,%s,%s,%s\n", users[i].id, users[i].username, users[i].password, users[i].name, users[i].contact);
    fclose(uf);
    
    FILE *tf = fopen("tasks.txt", "w");
    for (int i = 0; i < taskCount; i++) fprintf(tf, "%d,%d,%d,%.2f,%s\n", tasks[i].id, tasks[i].posterId, tasks[i].assigneeId, tasks[i].fee, tasks[i].description);
    fclose(tf);
}