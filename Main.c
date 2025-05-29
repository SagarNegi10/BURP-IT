#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <windows.h>

#define MAX_NAME 50
#define MAX_PASS 20
#define MAX_CARD 16
#define MAX_LOCATIONS 150
#define INF 9999
#define MAX_NAME_LEN 50
#define ADMIN_REG_KEY "admin123"

// [ Data Files ]---------------------------------------------------------------------------------------------------------------

#define USERS_FILE "data/users.txt"
#define MENU_FILE "data/menu.txt"
#define ORDERS_FILE "data/orders.txt"
#define PAYMENT_FILE "data/payments.txt"
#define ROUTE_FILE "data/routes.txt"
#define HISTORY_FILE "data/history.txt"

// [ Core Functionalities & Features ]-----------------------------------------------------------------------------------------

void cls();
void br(int line);
void middle1();
void ccolor(int index);
void getConsoleSize(int* width, int* height);
void setCursorPosition(int x, int y);
void loadingbar();
void pwelcome();
void printCentered(const char* text, int newline);

void registerUser();
int loginUser(int *isAdmin);
void securePasswordInput(char *password);

void loadMenu();
void saveMenu();
void addMenuItem();
void removeMenuItem();
void updateMenuItem();
void displayMenu();
float getPriceFromMenu(int foodId);
const char* getFoodItemNameFromMenu(int foodId);
void reduceStock(char *itemName);

void loadOrders();
void saveOrders();
void placeOrder();
void processOrder();
void displayOrders();

void cancelOrder(int orderId);
void undoLastCancelledOrder();

void loadRoutes();
void saveRoutes();
int findLocationIndexByName(const char *name);
void addRoute();
void displayRoutes();
void findShortestRoute(int start, int destination);

void loadOrderHistory();
void saveOrderHistory();
void addOrderToHistory(int orderId, char *customer, char *food, int price, char *city);
void displayOrderHistory();

int processPayment(int orderId, const char *customerName, float amount);
void displayPayments();

void showAboutInfo();

void runBurpit();
void adminMenuManager();
void adminOrderManager();
void routeMenu();

void trimTrailingWhitespace(char* str);
void strToLower(char* str);

// [ User Authentication: Register / Login ]-----------------------------------------------------------------------------------

typedef struct {
    char username[MAX_NAME];
    char password[MAX_PASS];
    int isAdmin;
} User;

void securePasswordInput(char *password) {
    char ch;
    int i = 0;
    while (1) {
        ch = getch();
        if (ch == 13) {
            password[i] = '\0';
            break;
        } else if (ch == 8 && i > 0) {
            i--;
            printf("\b \b");
        } else if (i < MAX_PASS - 1 && ch >= 32 && ch <= 126) {
            password[i++] = ch;
            printf("*");
        }
    }
}

void registerUser() {
    User newUser;
    FILE *file;
    int exists = 0;
    br(5); printCentered("Enter username: ", 0);
    scanf("%s", newUser.username);
    file = fopen(USERS_FILE, "r");
    if (file) {
        User temp;
        while (fscanf(file, "%s %s %d", temp.username, temp.password, &temp.isAdmin) != EOF) {
            if (strcmp(temp.username, newUser.username) == 0) {
                exists = 1;
                break;
            }
        }
        fclose(file);
    }
    if (exists) {
        cls(); middle1();
        printCentered("Username already exists! Try again.",1);
        Sleep(1500);
        return;
    }
    br(1); printCentered("Enter password: ",0);
    securePasswordInput(newUser.password);
    int isAdminRequest;
    br(2); printCentered("Are you an Admin? (1 = Yes, 0 = No): ",0);
    scanf("%d", &isAdminRequest);
    if (isAdminRequest == 1) {
        char adminKey[50];
        br(1); printCentered("Enter Admin Registration Key: ",0);
        scanf("%s", adminKey);
        if (strcmp(adminKey, ADMIN_REG_KEY) != 0) {
            cls(); middle1();
            printCentered("Invalid Admin Key! Registration as admin denied.",1);
            Sleep(1500);
        return;
        } else {
            newUser.isAdmin = 1;
        }
    } else {
        newUser.isAdmin = 0;
    }
    file = fopen(USERS_FILE, "a");
    if (!file) {
        printf("Error opening user file for writing.\n");
        return;
    }
    fprintf(file, "%s %s %d\n", newUser.username, newUser.password, newUser.isAdmin);
    fclose(file);
    char msg[150];
    snprintf(msg, sizeof(msg), "User '%s' registered successfully! Login Now!", newUser.username);
    cls(); middle1();
    printCentered(msg, 1);
    Sleep(1500);
}

int loginUser(int *isAdmin) {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) {
        printf("User database not found. Please register first.\n");
        return 0;
    }
    User tempUser, inputUser;
    br(5); printCentered("Enter username: ",0);
    scanf("%s", inputUser.username);
    br(1); printCentered("Enter password: ",0);
    securePasswordInput(inputUser.password);
    while (fscanf(file, "%s %s %d", tempUser.username, tempUser.password, &tempUser.isAdmin) != EOF) {
        if (strcmp(tempUser.username, inputUser.username) == 0 &&
            strcmp(tempUser.password, inputUser.password) == 0) {
            *isAdmin = tempUser.isAdmin;
            fclose(file);
            char msg[150];
            snprintf(msg, sizeof(msg), "Login successful! Welcome, %s.", inputUser.username);
            cls(); middle1();
            printCentered(msg, 1);
            Sleep(1500);
            return 1;
        }
    }
    fclose(file);
    cls(); middle1();
    printCentered("Invalid username or password!", 1);
    Sleep(1500);
    return 0;
}

// [ Menu Management - Linked List ]-------------------------------------------------------------------------------------------

typedef struct MenuItem {
    int id;
    char name[MAX_NAME];
    float price;
    int stock;
    struct MenuItem *next;
} MenuItem;

MenuItem *menuHead = NULL;

MenuItem* findMenuItemById(int id) {
    MenuItem *current = menuHead;
    while (current) {
        if (current->id == id) return current;
        current = current->next;
    }
    return NULL;
}

void addMenuItem() {
    MenuItem *newItem = malloc(sizeof(MenuItem));
    if (!newItem) {
        printf("Memory allocation failed!\n");
        return;
    }
    br(10); printCentered("Enter Item ID: ", 0);
    scanf("%d", &newItem->id);
    br(1); printCentered("Enter Item Name: ",0);
    scanf("%s", newItem->name);
    br(1); printCentered("Enter Price: ",0);
    scanf("%f", &newItem->price);
    br(1); printCentered("Enter Stock Quantity: ",0);
    scanf("%d", &newItem->stock);
    newItem->next = menuHead;
    menuHead = newItem;
    saveMenu();
    cls(); middle1();
    printCentered("Item added successfully!",1);
    Sleep(1500);
}

void removeMenuItem() {
    displayMenu();
    int id;
    br(1); printCentered("Enter Item ID to remove: ",0);
    scanf("%d", &id);
    MenuItem *temp = menuHead, *prev = NULL;
    while (temp && temp->id != id) {
        prev = temp;
        temp = temp->next;
    }
    if (!temp) {
        cls(); middle1();
        printCentered("Item not found!", 1);
        Sleep(1500);
        return;
    }
    if (!prev)
        menuHead = temp->next;
    else
        prev->next = temp->next;
    free(temp);
    saveMenu();
    cls(); middle1();
    printCentered("Item removed successfully!", 1);
    Sleep(1500);
}

void updateMenuItem() {
    displayMenu();
    int id;
    br(1); printCentered("Enter Item ID to update: ",0);
    scanf("%d", &id);
    MenuItem *item = findMenuItemById(id);
    if (!item) {
        cls(); middle1();
        printCentered("Item not found!", 1);
        Sleep(1500);
        return;
    }
    br(1); printCentered("Enter new price: ",0);
    scanf("%f", &item->price);
    br(1); printCentered("Enter new stock: ",0);
    scanf("%d", &item->stock);
    saveMenu();
    cls(); middle1();
    printCentered("Item updated successfully!", 1);
    Sleep(1500);
}

float getPriceFromMenu(int foodId) {
    MenuItem *item = findMenuItemById(foodId);
    return item ? item->price : -1;
}

const char* getFoodItemNameFromMenu(int foodId) {
    MenuItem *item = findMenuItemById(foodId);
    return item ? item->name : NULL;
}

void reduceStock(char *itemName) {
    MenuItem *current = menuHead;
    while (current) {
        if (strcmp(current->name, itemName) == 0) {
            if (current->stock > 0) {
                current->stock--;
                saveMenu();
            } else {
                char msg[150];
                snprintf(msg, sizeof(msg), "Warning: %s is out of stock!", current->name);
                cls(); middle1();
                printCentered(msg, 1);
                Sleep(1500);
            }
            return;
        }
        current = current->next;
    }
}

void displayMenu() {
    MenuItem *temp = menuHead;
    if (!temp) {
        middle1();
        printCentered("No items in menu.", 1);
        Sleep(1500);
        return;
    }
    br(1);
    printCentered("-------------------------------------------------------------------", 1);
    printCentered("|  ID  |             Name             |  Price  |     In Stock    |", 1);
    printCentered("-------------------------------------------------------------------", 1);
    while (temp) {
        char line[120];
        snprintf(line, sizeof(line), "| %4d | %-28s | %7.2f | %15d |",
                 temp->id, temp->name, temp->price, temp->stock);
        printCentered(line, 1);
        printCentered("-------------------------------------------------------------------", 1);
        temp = temp->next;
        Sleep(150);
    }
    br(1);
    printCentered("Press any key to continue...", 1);
    getch();
}

// [ Order Management - Priority Queue ]---------------------------------------------------------------------------------------

typedef struct Order {
    int orderId;
    char customerName[MAX_NAME];
    char foodItem[MAX_NAME];
    float price;
    int priority;
    char city[MAX_NAME];
    struct Order *next;
} Order;

Order *front = NULL, *rear = NULL;

void enqueueOrder(Order *newOrder) {
    newOrder->next = NULL;
    if (!front) {
        front = rear = newOrder;
    } else if (newOrder->priority) {
        newOrder->next = front;
        front = newOrder;
    } else {
        rear->next = newOrder;
        rear = newOrder;
    }
}

Order* createOrderFromFile(int id, const char *cust, const char *food, float price, int priority) {
    Order *order = (Order *)malloc(sizeof(Order));
    if (!order) return NULL;
    order->orderId = id;
    strncpy(order->customerName, cust, MAX_NAME);
    strncpy(order->foodItem, food, MAX_NAME);
    order->price = price;
    order->priority = priority;
    order->next = NULL;
    return order;
}

void placeOrder() {
    displayMenu();
    Order *newOrder = (Order *)malloc(sizeof(Order));
    if (!newOrder) {
        printf("Memory allocation failed!\n");
        return;
    }
    br(1); printCentered("Enter Customer ID: ", 0);
    scanf("%d", &newOrder->orderId);
    br(1); printCentered("Enter Customer Name: ", 0);
    scanf("%s", newOrder->customerName);
    br(1); printCentered("Enter Delivery City: ", 0);
    scanf("%s", newOrder->city);
    br(1); printCentered("Enter Food Item ID: ", 0);
    int foodId;
    if (scanf("%d", &foodId) != 1) {
        cls(); middle1();
        printCentered("Invalid input!", 1);
        Sleep(1500);
        while (getchar() != '\n');
        free(newOrder);
        return;
    }
    float price = getPriceFromMenu(foodId);
    if (price == -1) {
        cls(); middle1();
        printCentered("Item ID not found in menu.", 1);
        Sleep(1500);
        free(newOrder);
        return;
    }
    const char *name = getFoodItemNameFromMenu(foodId);
    if (!name) {
        cls(); middle1();
        printCentered("Error retrieving food item name.", 1);
        Sleep(1500);
        free(newOrder);
        return;
    }
    newOrder->price = price;
    strncpy(newOrder->foodItem, name, MAX_NAME);
    br(1); printCentered("Is this an urgent order? (1 = Yes, 0 = No): ", 0);
    if (scanf("%d", &newOrder->priority) != 1 || (newOrder->priority != 0 && newOrder->priority != 1)) {
        cls(); middle1();
        printCentered("Invalid input for priority.", 1);
        Sleep(1500);
        while (getchar() != '\n');
        free(newOrder);
        return;
    }
    int success = processPayment(newOrder->orderId, newOrder->customerName, newOrder->price);
    if (success == 0) {
        cls(); middle1();
        printCentered("Order failed", 1);
        Sleep(1500);
        free(newOrder);
    } else {
        enqueueOrder(newOrder);
        saveOrders();
        char msg[150];
        snprintf(msg, sizeof(msg), "Order placed successfully for %s (%.2f).", newOrder->foodItem, newOrder->price);
        cls(); middle1();
        printCentered(msg, 1);
        Sleep(1500);
    }
}

void processOrder() {
    if (!front) {
        cls(); middle1();
        printCentered("No orders to process.\n", 1);
        Sleep(1500);
        return;
    }
    Order *order = front;
    char msg[256];
    sprintf(msg, "Processing Order #%d: %s from %s ordered %s %.2f)",
            order->orderId, order->customerName, order->city, order->foodItem, order->price);
    middle1();
    printCentered(msg, 1);
    Sleep(1500);
    char from[MAX_NAME_LEN], to[MAX_NAME_LEN];
    strcpy(from, "burpit");
    strncpy(to, order->city, MAX_NAME_LEN - 1);
    to[MAX_NAME_LEN - 1] = '\0';
    trimTrailingWhitespace(to);
    strToLower(to);
    int s = findLocationIndexByName(from);
    int d = findLocationIndexByName(to);
    if (s == -1 || d == -1) {
        cls(); middle1();
        printCentered("Invalid source or destination location.", 1);
        getch();
    } else {
        cls();
        findShortestRoute(s, d);
    }
    addOrderToHistory(order->orderId, order->customerName, order->foodItem, order->price, order->city);
    reduceStock(order->foodItem);
    front = front->next;
    if (!front) rear = NULL;
    free(order);
    saveOrders();
    saveMenu();
    cls(); middle1();
    printCentered("Order processed successfully!", 1);
    Sleep(1500);
}

void displayOrders() {
    if (!front) {
        middle1();
        printCentered("No pending orders.", 1);
        Sleep(1500);
        return;
    }
    br(1);
    printCentered("---------------------------------------------------------------------------------------", 1);
    printCentered("|  ID  |     Customer    |      Food Item     |  Price  |  Priority  |     City       |", 1);
    printCentered("---------------------------------------------------------------------------------------", 1);
    for (Order *temp = front; temp; temp = temp->next)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer),
                 "| %4d | %-15s | %-18s | %7.2f | %-10s | %-14s |",
                 temp->orderId,
                 temp->customerName,
                 temp->foodItem,
                 temp->price,
                 temp->priority ? "Urgent" : "Normal",
                 temp->city);
        printCentered(buffer, 1);
        printCentered("---------------------------------------------------------------------------------------", 1);
        Sleep(150);
    }
    br(1);
    printCentered("Press any key to continue...", 1);
    getch();
}

// [ Cancel & Undo System - Stack ]--------------------------------------------------------------------------------------------

typedef struct CancelledOrder {
    int orderId;
    char customerName[MAX_NAME];
    char foodItem[MAX_NAME];
    float price;
    int priority;
    char city[MAX_NAME];
    struct CancelledOrder *next;
} CancelledOrder;

CancelledOrder *cancelledOrdersTop = NULL;

void cancelOrder(int orderId) {
    Order *curr = front, *prev = NULL;
    while (curr && curr->orderId != orderId) {
        prev = curr;
        curr = curr->next;
    }
    if (!curr) {
        cls();
        char msg[50];
        snprintf(msg, sizeof(msg), "Order #%d not found!", orderId);
        middle1();
        printCentered(msg, 1);
        Sleep(1500);
        return;
    }
    CancelledOrder *canceled = (CancelledOrder *)malloc(sizeof(CancelledOrder));
    if (!canceled) {
        printf("Memory allocation failed!\n");
        return;
    }
    *canceled = (CancelledOrder){
        .orderId = curr->orderId,
        .price = curr->price,
        .priority = curr->priority,
        .next = cancelledOrdersTop
    };
    strncpy(canceled->customerName, curr->customerName, MAX_NAME);
    strncpy(canceled->foodItem, curr->foodItem, MAX_NAME);
    strncpy(canceled->city, curr->city, MAX_NAME);
    cancelledOrdersTop = canceled;
    if (prev) prev->next = curr->next;
    else front = curr->next;
    if (!front) rear = NULL;
    free(curr);
    saveOrders();
    cls();
    char msg[50];
    snprintf(msg, sizeof(msg), "Order #%d cancelled.", orderId);
    middle1();
    printCentered(msg, 1);
    Sleep(1500);
}

void undoLastCancelledOrder() {
    if (!cancelledOrdersTop) {
        cls(); middle1();
        printCentered("No cancelled orders to undo.\n",1);
        Sleep(1500);
        return;
    }
    CancelledOrder *cancelled = cancelledOrdersTop;
    Order *restored = (Order *)malloc(sizeof(Order));
    if (!restored) {
        printf("Memory allocation failed.\n");
        return;
    }
    *restored = (Order){
        .orderId = cancelled->orderId,
        .price = cancelled->price,
        .priority = cancelled->priority,
        .next = NULL
    };
    strncpy(restored->customerName, cancelled->customerName, MAX_NAME);
    strncpy(restored->foodItem, cancelled->foodItem, MAX_NAME);
    strncpy(restored->city, cancelled->city, MAX_NAME);
    enqueueOrder(restored);
    cancelledOrdersTop = cancelled->next;
    free(cancelled);
    saveOrders();
    char msg[50];
    snprintf(msg, sizeof(msg), "Order #%d restored successfully.", restored->orderId);
    middle1();
    printCentered(msg, 1);
    Sleep(1500);
}

// [ Delivery (Graph - Dijkstra) ]---------------------------------------------------------------------------------------------

int graph[MAX_LOCATIONS][MAX_LOCATIONS];
char locations[MAX_LOCATIONS][MAX_NAME_LEN];
int locationCount = 0;

void readString(char *buffer, int size) {
    if (fgets(buffer, size, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
    }
}

int stricmp(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return (unsigned char)tolower(*a) - (unsigned char)tolower(*b);
        a++; b++;
    }
    return (unsigned char)tolower(*a) - (unsigned char)tolower(*b);
}

int findLocationIndexByName(const char *name) {
    for (int i = 0; i < locationCount; i++) {
        if (stricmp(locations[i], name) == 0)
            return i;
    }
    return -1;
}

void addRoute() {
    cls();
    if (locationCount >= MAX_LOCATIONS) {
        middle1();
        printCentered("Max locations reached!\n",1);
        Sleep(1500);
        return;
    }
    char newLocation[MAX_NAME_LEN];
    middle1();
    printCentered("Enter new location name: ",0);
    scanf("%s", newLocation);
    if (findLocationIndexByName(newLocation) != -1) {
        middle1();
        printCentered("Location already exists!",1);
        Sleep(1500);
        return;
    }
    strcpy(locations[locationCount], newLocation);
    for (int i = 0; i <= locationCount; i++) {
        int dist;
        if (i == locationCount) {
            graph[locationCount][i] = 0;
            graph[i][locationCount] = 0;
            continue;
        }
        char prompt[150];
        snprintf(prompt, sizeof(prompt), "Distance from %s to %s: ", locations[locationCount], locations[i]);
        br(1); printCentered(prompt, 0);
        if (scanf("%d", &dist) != 1 || dist < 0) {
            cls(); middle1();
            printCentered("Invalid distance input!",0);
            Sleep(1500);
            while (getchar() != '\n');
            return;
        }
        graph[locationCount][i] = dist;
        graph[i][locationCount] = dist;
    }
    while (getchar() != '\n');
    locationCount++;
    saveRoutes();
    cls(); middle1();
    printCentered("Route added successfully!\n",1);
    Sleep(1500);
}

void findShortestRoute(int start, int destination) {
    if (start < 0 || destination < 0 || start >= locationCount || destination >= locationCount) {
        cls(); middle1();
        printCentered("Invalid location selected. Please try again.", 1);
        getch();
        return;
    }
    int dist[MAX_LOCATIONS], visited[MAX_LOCATIONS], prev[MAX_LOCATIONS];
    for (int i = 0; i < locationCount; i++) {
        dist[i] = INF;
        visited[i] = 0;
        prev[i] = -1;
    }
    dist[start] = 0;
    for (int i = 0; i < locationCount - 1; i++) {
        int minDist = INF, minIndex = -1;
        for (int j = 0; j < locationCount; j++) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                minIndex = j;
            }
        }
        if (minIndex == -1) break;
        visited[minIndex] = 1;
        for (int j = 0; j < locationCount; j++) {
            if (!visited[j] && graph[minIndex][j] > 0 &&
                dist[minIndex] + graph[minIndex][j] < dist[j]) {
                dist[j] = dist[minIndex] + graph[minIndex][j];
                prev[j] = minIndex;
            }
        }
    }
    cls(); br(10);
    if (dist[destination] == INF) {
        char msg[150];
        sprintf(msg, "No route exists between %s and %s.", locations[start], locations[destination]);
        printCentered(msg, 1);
        Sleep(1500);
        return;
    }
    char buffer[256];
    sprintf(buffer, "Shortest route from %s to %s: %d units", locations[start], locations[destination], dist[destination]);
    printCentered(buffer, 1);
    br(1);
    sprintf(buffer, "Estimated time: %d minutes", dist[destination]);
    printCentered(buffer, 1);
    br(1);
    sprintf(buffer, "Estimated delivery cost: %d", dist[destination] * 10);
    printCentered(buffer, 1);
    br(1);
    printCentered("Path:", 1);
    br(1);
    int path[MAX_LOCATIONS], count = 0, temp = destination;
    while (temp != -1) {
        path[count++] = temp;
        temp = prev[temp];
    }
    char pathStr[512] = "";
    for (int i = count - 1; i >= 0; i--) {
        strcat(pathStr, locations[path[i]]);
        if (i != 0)
            strcat(pathStr, " -> ");
    }
    printCentered(pathStr, 1);
    br(2);
    printCentered("Press any key to continue...", 1);
    getch();
}

void printWithOffset(const char* str, int consoleWidth) {
    int len = (int)strlen(str);
    int offset = (consoleWidth - len) / 2;
    if (offset < 0) offset = 0;
    printf("%*s%s\n", offset, "", str);
}

void displayRoutes() {
    if (locationCount == 0) {
        middle1();
        printCentered("No routes available.", 1);
        Sleep(1500);
        return;
    }
    int width, height;
    getConsoleSize(&width, &height);
    br(2);
    printCentered("--------------------------------[ Delivery Routes Matrix ]--------------------------------", 1);
    br(2);
    int totalCols = locationCount + 1;
    int totalWidth = totalCols * 12;
    char headerLine[1024] = {0};
    int pos = 0;
    pos += snprintf(headerLine + pos, sizeof(headerLine) - pos, "%-12s", "");
    for (int i = 0; i < locationCount; i++) {
        pos += snprintf(headerLine + pos, sizeof(headerLine) - pos, "%-12s", locations[i]);
    }
    printWithOffset(headerLine, width);
    for (int i = 0; i < locationCount; i++) {
        char rowLine[1024] = {0};
        pos = 0;
        pos += snprintf(rowLine + pos, sizeof(rowLine) - pos, "%-12s", locations[i]);
        for (int j = 0; j < locationCount; j++) {
            pos += snprintf(rowLine + pos, sizeof(rowLine) - pos, "%-12d", graph[i][j]);
        }
        printWithOffset(rowLine, width);
        Sleep(150);
    }
    br(2);
    printCentered("Press any key to continue...", 1);
    getch();
}

// [ History (Stack) ]---------------------------------------------------------------------------------------------------------

typedef struct OrderHistory {
    int orderId;
    char customerName[MAX_NAME];
    char foodItem[MAX_NAME];
    char city[MAX_NAME];
    float price;
    struct OrderHistory *next;
} OrderHistory;

OrderHistory *historyTop = NULL;

void loadOrderHistory() {
    FILE *file = fopen(HISTORY_FILE, "r");
    if (!file) {
        perror("Failed to open history file");
        return;
    }
    int orderId;
    float price;
    char customerName[MAX_NAME], foodItem[MAX_NAME];
    char city[MAX_NAME];
    while (fscanf(file, "%d %s %s %f %s", &orderId, customerName, foodItem, &price, &city) == 5) {
        OrderHistory *newHistory = (OrderHistory *)malloc(sizeof(OrderHistory));
        if (!newHistory) {
            printf("Memory allocation failed while loading history.\n");
            break;
        }
        newHistory->orderId = orderId;
        strcpy(newHistory->customerName, customerName);
        strcpy(newHistory->foodItem, foodItem);
        newHistory->price = price;
        strcpy(newHistory->city, city);
        newHistory->next = historyTop;
        historyTop = newHistory;
    }
    fclose(file);
}

void saveOrderHistory() {
    FILE *file = fopen(HISTORY_FILE, "w");
    if (!file) {
        perror("Error saving order history");
        return;
    }
    OrderHistory *temp = historyTop;
    while (temp) {
        fprintf(file, "%d %s %s %.2f %s\n", temp->orderId, temp->customerName, temp->foodItem, temp->price, temp->city);
        temp = temp->next;
    }
    fclose(file);
}

void addOrderToHistory(int orderId, char *customer, char *food, int price, char *city) {
    OrderHistory *newHistory = (OrderHistory *)malloc(sizeof(OrderHistory));
    if (!newHistory) {
        printf("Memory allocation failed while adding to history.\n");
        return;
    }
    newHistory->orderId = orderId;
    strncpy(newHistory->customerName, customer, MAX_NAME - 1);
    newHistory->customerName[MAX_NAME - 1] = '\0';
    strncpy(newHistory->foodItem, food, MAX_NAME - 1);
    newHistory->foodItem[MAX_NAME - 1] = '\0';
    strncpy(newHistory->city, city, MAX_NAME - 1);
    newHistory->city[MAX_NAME - 1] = '\0';
    newHistory->price = price;
    newHistory->next = historyTop;
    historyTop = newHistory;
    saveOrderHistory();
}

void displayOrderHistory() {
    if (!historyTop) {
        cls(); middle1();
        printCentered("No order history available!",1);
        Sleep(1500);
        return;
    }
    int width, height;
    getConsoleSize(&width, &height);
    const int tableWidth = 80;
    char buffer[256];
    br(1);
    snprintf(buffer, sizeof(buffer), "-------------------------------------------------------------------------------");
    printCentered(buffer, 1);
    snprintf(buffer, sizeof(buffer), "| %-5s | %-15s | %-20s | %-15s | %-8s |", "ID", "Customer", "Food Item", "City", "Price");
    printCentered(buffer, 1);
    snprintf(buffer, sizeof(buffer), "-------------------------------------------------------------------------------");
    printCentered(buffer, 1);
    OrderHistory *temp = historyTop;
    while (temp) {
        snprintf(buffer, sizeof(buffer), "| %-5d | %-15s | %-20s | %-15s | %-8.2f |",
                 temp->orderId, temp->customerName, temp->foodItem, temp->city, (float)temp->price);
        printCentered(buffer, 1);
        temp = temp->next;
        Sleep(150);
    }
    snprintf(buffer, sizeof(buffer), "-------------------------------------------------------------------------------");
    printCentered(buffer, 1);
    br(1); printCentered("Press any key to continue...", 1);
    getch();
}

// [ File Handling ]-----------------------------------------------------------------------------------------------------------

void loadMenu() {
    FILE *file = fopen(MENU_FILE, "r");
    if (!file) {
        printf("Menu file not found. Starting with an empty menu.\n");
        return;
    }
    int id, stock;
    float price;
    char name[MAX_NAME];
    while (fscanf(file, "%d %s %f %d", &id, name, &price, &stock) == 4) {
        MenuItem *newItem = malloc(sizeof(MenuItem));
        if (!newItem) {
            printf("Memory allocation failed!\n");
            fclose(file);
            return;
        }
        newItem->id = id;
        strcpy(newItem->name, name);
        newItem->price = price;
        newItem->stock = stock;
        newItem->next = menuHead;
        menuHead = newItem;
    }
    fclose(file);
}

void saveMenu() {
    FILE *file = fopen(MENU_FILE, "w");
    if (!file) {
        printf("Error saving menu!\n");
        return;
    }
    MenuItem *temp = menuHead;
    while (temp) {
        fprintf(file, "%d %s %.2f %d\n", temp->id, temp->name, temp->price, temp->stock);
        temp = temp->next;
    }
    fclose(file);
}

void trimTrailingWhitespace(char* str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\n'))
        str[--len] = '\0';
}

void strToLower(char* str) {
    for (int i = 0; str[i]; i++)
        str[i] = tolower((unsigned char)str[i]);
}

void loadOrders() {
    FILE *file = fopen(ORDERS_FILE, "r");
    if (!file) {
        perror("Orders file not found");
        return;
    }
    int id, priority;
    float price;
    char name[MAX_NAME], food[MAX_NAME], city[MAX_NAME];
    while (fscanf(file, "%d %s %s %f %d %s", &id, name, food, &price, &priority, city) == 6) {
        trimTrailingWhitespace(city);
        strToLower(city);
        Order *order = createOrderFromFile(id, name, food, price, priority);
        if (order) {
            strncpy(order->city, city, MAX_NAME - 1);
            order->city[MAX_NAME - 1] = '\0';
            enqueueOrder(order);
        }
    }
    fclose(file);
}

void saveOrders() {
    FILE *file = fopen(ORDERS_FILE, "w");
    if (!file) {
        perror("Error saving orders");
        return;
    }
    for (Order *temp = front; temp; temp = temp->next) {
        fprintf(file, "%d %s %s %.2f %d %s\n", temp->orderId, temp->customerName, temp->foodItem, temp->price, temp->priority, temp->city);
    }
    fclose(file);
}

void loadRoutes() {
    FILE *file = fopen(ROUTE_FILE, "r");
    if (!file) {
        printf("Route file not found. Starting fresh.\n");
        locationCount = 0;
        return;
    }
    if (fscanf(file, "%d\n", &locationCount) != 1 || locationCount < 0 || locationCount > MAX_LOCATIONS) {
        printf("Route file corrupted or invalid. Starting fresh.\n");
        locationCount = 0;
        fclose(file);
        return;
    }
    for (int i = 0; i < locationCount; i++) {
        if (fgets(locations[i], MAX_NAME_LEN, file) == NULL) {
            printf("Error reading location name from file.\n");
            locationCount = 0;
            fclose(file);
            return;
        }
        size_t len = strlen(locations[i]);
        if (len > 0 && locations[i][len - 1] == '\n')
            locations[i][len - 1] = '\0';
    }
    for (int i = 0; i < locationCount; i++)
        for (int j = 0; j < locationCount; j++)
            if (fscanf(file, "%d", &graph[i][j]) != 1)
                graph[i][j] = 0;

    fclose(file);
}

void saveRoutes() {
    FILE *file = fopen(ROUTE_FILE, "w");
    if (!file) {
        perror("Error saving routes");
        return;
    }
    fprintf(file, "%d\n", locationCount);
    for (int i = 0; i < locationCount; i++)
        fprintf(file, "%s\n", locations[i]);
    for (int i = 0; i < locationCount; i++) {
        for (int j = 0; j < locationCount; j++)
            fprintf(file, "%d ", graph[i][j]);
        fprintf(file, "\n");
    }
    fclose(file);
}

// [ Payment ]-----------------------------------------------------------------------------------------------------------------

typedef struct Payment {
    int orderId;
    char customerName[MAX_NAME];
    float amount;
    char method[10];
} Payment;

int processPayment(int orderId, const char *customerName, float amount) {
    FILE *file = fopen(PAYMENT_FILE, "a");
    if (!file) {
        perror("Error opening payment file");
        return 0;
    }
    int choice;
    char method[10], cardNumber[MAX_CARD + 1];
    br(1); printCentered("Choose Payment Method:", 1);
    br(1); printCentered("1. Cash", 1);
    br(1); printCentered("2. Card", 1);
    br(1); printCentered("Enter choice: ", 0);
    if (scanf("%d", &choice) != 1) {
        cls(); middle1();
        printCentered("Invalid input! Payment cancelled.", 1);
        Sleep(1500);
        while (getchar() != '\n');
        fclose(file);
        return 0;
    }
    if (choice == 1) {
        strcpy(method, "Cash");
    } else if (choice == 2) {
        strcpy(method, "Card");
        br(1); printCentered("Enter 16-digit Card Number: ", 0);
        scanf("%16s", cardNumber);
        while (getchar() != '\n');
        if (strlen(cardNumber) != 16) {
            cls(); middle1();
            printCentered("Invalid card number! Payment failed.", 1);
            Sleep(1500);
            fclose(file);
            return 0;
        }
    } else {
        cls(); middle1();
        printCentered("Invalid choice! Payment cancelled.", 1);
        Sleep(1500);
        fclose(file);
        return 0;
    }
    fprintf(file, "%d %s %.2f %s\n", orderId, customerName, amount, method);
    fclose(file);
    char msg[150];
    snprintf(msg, sizeof(msg), "Payment Successful! Order #%d paid via %s.", orderId, method);
    cls(); middle1();
    printCentered(msg, 1);
    Sleep(1500);
    return 1;
}

void displayPayments() {
    FILE *file = fopen(PAYMENT_FILE, "r");
    if (!file) {
        cls(); middle1();
        printCentered("No payment records found!\n", 1);
        return;
    }
    int width, height;
    getConsoleSize(&width, &height);
    float totalSales = 0;
    Payment temp;
    br(1);
    printCentered(" --------------------------------------------------------------", 1);
    printCentered(" | Order ID  |     Customer Name     |   Amount   |   Method  |", 1);
    printCentered(" --------------------------------------------------------------", 1);
    while (fscanf(file, "%d %s %f %s", &temp.orderId, temp.customerName, &temp.amount, temp.method) == 4) {
        char row[200];
        snprintf(row, sizeof(row), "| %-9d | %-21s | %-9.2f | %-10s |", 
                 temp.orderId, temp.customerName, temp.amount, temp.method);
        printCentered(row, 1);
        totalSales += temp.amount;
        Sleep(150);
    }
    fclose(file);
    printCentered("--------------------------------------------------------------", 1);
    char totalRow[150];
    snprintf(totalRow, sizeof(totalRow), "Total Sales: %.2f", totalSales);
    br(1);
    printCentered(totalRow, 1);
    br(1);
    printCentered("Press any key to continue...", 1);
    getch();
}

// [ About Our Team ]----------------------------------------------------------------------------------------------------------

void showAboutInfo() {
    br(2); printCentered("--------------------------------[ ABOUT BURPIT ]--------------------------------", 1);
    br(1); printCentered("Version: 1.0.0", 1);
    br(1); printCentered("Developed under guidance of Ms. Shobha Aswal", 1);
    br(2); printCentered("Team Members:", 1);
    br(1);
    printCentered("Sagar Negi    - 24711207 - sagarnegi13.0@gmail.com     ", 1);
    printCentered("Mohammad Kaif - 24711363 - kaiiff2802@gmail.com        ", 1);
    printCentered("Ritik Uniyal  - 24711185 - ritikuniyal9999@gmail.com   ", 1);
    printCentered("Rita Rathore  - 24712183 - Ritarathore97772@gmail.com  ", 1);
    br(2); printCentered("Press any key to return...", 1);
    getch();
    cls();
}

// [ Burpit ]------------------------------------------------------------------------------------------------------------------

void printWelcomeBanner() {
    printCentered("=========================================================================================", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=            [ WELCOME TO BURPIT - FOOD DELIVERY & ORDER MANAGEMENT SYSTEM ]            =", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=========================================================================================", 1);
}
void printCustomerBanner() {
    printCentered("=========================================================================================", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=                          [ WELCOME TO BURPIT - CUSTOMER PANEL ]                       =", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=========================================================================================", 1);
}
void printAdminBanner() {
    printCentered("=========================================================================================", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=                            [ WELCOME TO BURPIT - ADMIN PANEL ]                        =", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=========================================================================================", 1);
}
void menuBanner() {
    printCentered("=========================================================================================", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=                 [ WELCOME TO BURPIT - ADMIN MENU MANAGEMENT ]                         =", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=========================================================================================", 1);
}

void orderBanner() {
    printCentered("=========================================================================================", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=                 [ WELCOME TO BURPIT - ADMIN ORDER MANAGEMENT ]                        =", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=========================================================================================", 1);
}

void routeBanner() {
    printCentered("=========================================================================================", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=                 [ WELCOME TO BURPIT - ADMIN ROUTE MANAGEMENT ]                        =", 1);
    printCentered("=                                                                                       =", 1);
    printCentered("=========================================================================================", 1);
}

void runBurpit() {
    int choice, isAdmin, loggedIn = 0;
    loadingbar();
    cls();
    pwelcome();
    Sleep(1500);
    cls();
    while (1) {
        main_menu:
        cls();
        printWelcomeBanner();
        br(2); printCentered("Please select an option to continue",1); Sleep(300);
        br(1); printCentered("1. Register as a New User",1); Sleep(150);
        br(1); printCentered("2. Login to Your Account ",1); Sleep(150);
        br(1); printCentered("3. Exit BURPIT           ",1); Sleep(150);
        br(2); printCentered("Enter your choice: ",0);
        scanf("%d", &choice);
        cls();
        switch (choice) {
            case 1:
                cls();
                printWelcomeBanner();
                registerUser();
                break;
            case 2:
                cls();
                printWelcomeBanner();
                loggedIn = loginUser(&isAdmin);
                if (loggedIn) {
                    cls();
                    if (!isAdmin) {
                        int customerChoice;
                        while (1) {
                            cls();
                            br(1); printCustomerBanner();
                            br(2); printCentered("1. View Our Delicious Menu    ", 1); Sleep(150);
                            br(1); printCentered("2. Place a New Order          ", 1); Sleep(150);
                            br(1); printCentered("3. Cancel an Existing Order   ", 1); Sleep(150);
                            br(1); printCentered("4. Undo Your Last Cancellation", 1); Sleep(150);
                            br(1); printCentered("5. Learn More About BURPIT    ", 1); Sleep(150);
                            br(1); printCentered("6. Logout from Your Account   ", 1); Sleep(150);
                            br(2); printCentered("Please enter your choice: ", 0);
                            scanf("%d", &customerChoice);
                            cls();
                            switch (customerChoice)
                            {
                            case 1:
                                br(1); printCustomerBanner();
                                displayMenu();
                                cls();
                                break;
                            case 2:
                                br(1); printCustomerBanner();
                                placeOrder();
                                break;
                            case 3:
                            {
                                br(1); printCustomerBanner();
                                int customerId;
                                middle1();
                                printCentered("Enter your Customer ID to cancel an order: ", 0);
                                scanf("%d", &customerId);
                                cancelOrder(customerId);
                                break;
                            }
                            case 4:
                                br(1); printCustomerBanner();
                                undoLastCancelledOrder();
                                break;
                            case 5:
                                br(1); printCustomerBanner();
                                showAboutInfo();
                                break;
                            case 6:
                                cls(); middle1();
                                printCentered("Logging out of your account...", 1);
                                Sleep(1500);
                                goto main_menu;
                            default:
                                middle1();
                                printCentered("Invalid choice! Please select a valid option from the menu.", 1);
                                Sleep(1500);
                                break;
                            }
                        }
                    } else {
                        int adminChoice;
                        while (1) {
                            cls();
                            br(1); printAdminBanner(); 
                            br(2); printCentered("1. Manage Menu Items        ", 1); Sleep(150);
                            br(1); printCentered("2. Handle Orders            ", 1); Sleep(150);
                            br(1); printCentered("3. Configure Delivery Routes", 1); Sleep(150);
                            br(1); printCentered("4. View Sales Reports       ", 1); Sleep(150);
                            br(1); printCentered("5. Logout from Admin Panel  ", 1); Sleep(150);
                            br(2); printCentered("Please enter your choice: ", 0);
                            scanf("%d", &adminChoice);
                            cls();
                            switch (adminChoice)
                            {
                            case 1:
                                adminMenuManager();
                                break;
                            case 2:
                                adminOrderManager();
                                break;
                            case 3:
                                routeMenu();
                                break;
                            case 4:
                                br(1); printAdminBanner();
                                displayPayments();
                                break;
                            case 5:
                                cls(); middle1();
                                printCentered("Logging out of Admin Panel...", 1);
                                Sleep(1500);
                                goto main_menu;
                            default:
                                printCentered("Invalid selection! Please choose a valid option.", 1);
                                Sleep(1500);
                                break;
                            }
                        }
                    }
                }
                break;
            case 3:
                cls(); middle1();
                printCentered("Thank you for using BURPIT!",1);
                Sleep(1500);
                exit(0);
            default:
                printCentered("Invalid choice! Try again.",1);
                Sleep(1500);
                break;
        }
    }
}

void adminMenuManager() {
    int choice;
    do {
        cls();
        br(1); menuBanner();
        br(2); printCentered("1. View menu items        ", 1); Sleep(150);
        br(1); printCentered("2. Add a new item         ", 1); Sleep(150);
        br(1); printCentered("3. Remove an item         ", 1); Sleep(150);
        br(1); printCentered("4. Update an item         ", 1); Sleep(150);
        br(1); printCentered("5. Exit to Admin Dashboard", 1); Sleep(150);
        br(2); printCentered("Enter your choice: ", 0);
        scanf("%d", &choice);
        cls();
        switch (choice) {
            case 1:
                br(1); menuBanner();
                displayMenu();
                cls();
                break;
            case 2:
                br(1); menuBanner();
                addMenuItem();
                break;
            case 3:
                br(1); menuBanner();
                removeMenuItem();
                break;
            case 4:
                br(1); menuBanner();
                updateMenuItem();
                break;
            case 5:
                cls(); middle1();
                printCentered("Returning to Admin Dashboard...", 1);
                Sleep(1500);
                break;
            default:
                middle1();
                printCentered("Invalid choice! Please try again.", 1);
                Sleep(1500);
        }
    } while (choice != 5);
}

void adminOrderManager() {
    int choice, orderId;
    do {
        cls();
        br(1); orderBanner();
        br(2); printCentered("1. Display Pending Orders   ", 1); Sleep(150);
        br(1); printCentered("2. Process Next Order       ", 1); Sleep(150);
        br(1); printCentered("3. Cancel an Order          ", 1); Sleep(150);
        br(1); printCentered("4. Undo Last Cancelled Order", 1); Sleep(150);
        br(1); printCentered("5. Display Orders History   ", 1); Sleep(150);
        br(1); printCentered("6. Exit to Admin Dashboard  ", 1); Sleep(150);
        br(2); printCentered("Enter your choice: ", 0);
        scanf("%d", &choice);
        cls();
        switch (choice) {
            case 1:
                br(1); orderBanner();
                displayOrders();
                break;
            case 2:
                br(1); orderBanner();
                processOrder();
                break;
            case 3:
            {
                br(1); orderBanner();
                int customerId;
                middle1();
                printCentered("Enter your Customer ID to cancel an order: ", 0);
                scanf("%d", &customerId);
                cancelOrder(customerId);
                break;
            }
            case 4:
                br(1); orderBanner();
                undoLastCancelledOrder();
                break;
            case 5:
                br(1); orderBanner();
                displayOrderHistory();
                break;
            case 6:
                cls(); middle1();
                printCentered("Returning to Admin Dashboard...", 1);
                Sleep(1500);
                break;
            default:
                middle1();
                printCentered("Invalid choice! Please try again.", 1);
                Sleep(150);
                break;
        }
    } while (choice != 6);
}

void routeMenu() {
    int choice;
    do {
        cls();
        br(1); routeBanner();
        br(2); printCentered("1. Add a new route         ", 1); Sleep(150);
        br(1); printCentered("2. Show all routes         ", 1); Sleep(150);
        br(1); printCentered("3. Find the shortest route ", 1); Sleep(150);
        br(1); printCentered("4. Exit to Admin Dashboard ", 1); Sleep(150);
        br(2); printCentered("Enter your choice: ", 0);
        scanf("%d", &choice);
        cls();
        switch (choice) {
            case 1:
                br(1); routeBanner();
                addRoute();
                break;
            case 2:
                br(1); routeBanner();
                displayRoutes();
                break;
            case 3:
            {   
                br(1); routeBanner();
                char from[MAX_NAME_LEN], to[MAX_NAME_LEN];
                middle1();
                printCentered("Enter source location: ",0);
                scanf("%s", from);
                br(1);
                printCentered("Enter destination location: ",0);
                scanf("%s", to);
                int s = findLocationIndexByName(from);
                int d = findLocationIndexByName(to);
                if (s == -1 || d == -1)
                {
                    cls();
                    middle1();
                    printCentered("Invalid location name(s)!", 1);
                    Sleep(1500);
                }
                else
                {   
                    cls();
                    findShortestRoute(s, d);
                }
                break;
            }
            case 4:
                cls(); middle1();
                printCentered("Returning to Admin Dashboard...", 1);
                Sleep(1500);
                break;
            default:
                middle1();
                printCentered("Invalid choice! Please try again.", 1);
                Sleep(1500);
        }

    } while (choice != 4);
}

// [ Application Main ]--------------------------------------------------------------------------------------------------------

int main(){
    loadMenu();
    loadOrders();
    loadOrderHistory();
    loadRoutes();
    runBurpit();
    return 0;
}

// [ User Interface Components ]-----------------------------------------------------------------------------------------------

void cls() {
    system("cls");
}

void br(int line) {
    for (int i = 0; i < line; i++) printf("\n");
}

void middle1() {
    int lines = 12;
    for (int i = 0; i < lines; i++) printf("\n");
}

void ccolor(int index) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int fgColors[] = {12};
    int bgColors[] = {0};
    int numColors = sizeof(fgColors) / sizeof(fgColors[0]);
    int fg = fgColors[index % numColors];
    int bg = bgColors[(index / numColors) % (sizeof(bgColors) / sizeof(bgColors[0]))];
    SetConsoleTextAttribute(hConsole, fg | bg);
}

void getConsoleSize(int* width, int* height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        *width = 80;
        *height = 25;
    }
}

void setCursorPosition(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, pos);
}

void loadingbar() {
    int width, height;
    getConsoleSize(&width, &height);
    const int barLength = 50;
    cls();
    ccolor(26);
    int centerX = width / 2;
    int centerY = height / 2;
    int loadingTextLen = strlen("Loading: ");
    setCursorPosition(centerX - loadingTextLen / 2, centerY - 1);
    printf("Loading: ");
    for (int i = 10; i <= 100; i += 10) {
        setCursorPosition(centerX + loadingTextLen / 2, centerY - 1);
        printf("%3d%%", i);
        setCursorPosition(centerX - barLength / 2, centerY);
        for (int j = 0; j < i * barLength / 150; j++) {
            ccolor(120 + (j % 15));
            printf(" ");
        }
        ccolor(26);
        for (int j = i * barLength / 150; j < barLength; j++) {
            printf(" ");
        }
        Sleep(150);
    }
    ccolor(15);
    printf("\n\n");
}

void pwelcome() {
    int width, height;
    getConsoleSize(&width, &height);
    char welcome1[] = "WELCOME TO BURPIT";
    char welcome2[] = "FOOD DELIVERY AND ORDER MANAGEMENT SYSTEM";
    int centerX = width / 2;
    int centerY = height / 2;
    cls();
    int y1 = centerY - 2;
    int padding1 = centerX - (int)strlen(welcome1) / 2;
    setCursorPosition(padding1, y1);
    for (int i = 0; i < (int)strlen(welcome1); i++) {
        ccolor(120 + (i % 15));
        printf("%c", welcome1[i]);
        Sleep(80);
    }
    int y2 = y1 + 2;
    int padding2 = centerX - (int)strlen(welcome2) / 2;
    setCursorPosition(padding2, y2);
    for (int i = 0; i < (int)strlen(welcome2); i++) {
        ccolor(120 + (i % 15));
        printf("%c", welcome2[i]);
        Sleep(60);
    }
    ccolor(15);
    printf("\n\n");
    Sleep(1500);
}

void printCentered(const char* text, int newline) {
    int width, height;
    getConsoleSize(&width, &height);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int y = csbi.dwCursorPosition.Y;
    int x = (width - (int)strlen(text)) / 2;
    setCursorPosition(x, y);
    printf("%s", text);
    if (newline) printf("\n");
}