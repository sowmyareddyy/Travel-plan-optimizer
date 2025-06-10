#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#define MAX_NAME_LENGTH 50
#define MAX_DESTINATIONS 10

// Structure to represent a tree node
struct TreeNode {
    char name[MAX_NAME_LENGTH];
    int days;
    int cost;
    struct TreeNode* activities;
    struct TreeNode* next;
};

// Structure to represent a destination and its distance
struct Destination {
    char name[MAX_NAME_LENGTH];
    int distance;
};

// Function to add a node to the tree and update remaining days and cost
void addNode(struct TreeNode* parent, const char* name, int days, int cost, int* remainingDays, int* remainingCost) {
    struct TreeNode* newNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    strncpy(newNode->name, name, sizeof(newNode->name) - 1);
    newNode->name[sizeof(newNode->name) - 1] = '\0';
    newNode->days = days;
    newNode->cost = cost;
    newNode->activities = NULL;
    newNode->next = NULL;

    if (parent->activities == NULL) {
        parent->activities = newNode;
    } else {
        struct TreeNode* current = parent->activities;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }

    // Update remaining days and cost
    *remainingDays -= days;
    *remainingCost -= cost;
}

// Function to display only activities in the tree structure
void displayActivities(struct TreeNode* root) {
    if (root == NULL) {
        return;
    }

    // Recursively display sub-activities
    struct TreeNode* current = root->activities;
    while (current != NULL) {
        printf("Activity: %s (Days: %d, Cost: $%d)\n", current->name, current->days, current->cost);
        displayActivities(current);
        current = current->next;
    }
}

// Dijkstra's algorithm to find the shortest path and distance
void dijkstra(struct Destination destinations[MAX_DESTINATIONS], int numDestinations, char* source, char* destination) {
    int dist[MAX_DESTINATIONS];
    int visited[MAX_DESTINATIONS];
    int prev[MAX_DESTINATIONS];

    // Initialize distances and visited flags
    for (int i = 0; i < numDestinations; i++) {
        dist[i] = INT_MAX;
        visited[i] = 0;
        prev[i] = -1;
    }

    // Find the source and destination indices
    int sourceIndex = -1;
    int destIndex = -1;
    for (int i = 0; i < numDestinations; i++) {
        if (strcmp(destinations[i].name, source) == 0) {
            sourceIndex = i;
        }
        if (strcmp(destinations[i].name, destination) == 0) {
            destIndex = i;
        }
    }

    if (sourceIndex == -1 || destIndex == -1) {
        printf("Source or destination not found in the list of destinations.\n");
        return;
    }

    dist[sourceIndex] = 0;

    for (int count = 0; count < numDestinations - 1; count++) {
        int u = -1;
        int minDist = INT_MAX;

        for (int v = 0; v < numDestinations; v++) {
            if (!visited[v] && dist[v] < minDist) {
                u = v;
                minDist = dist[v];
            }
        }

        visited[u] = 1;

        for (int v = 0; v < numDestinations; v++) {
            if (!visited[v] && destinations[u].distance + dist[u] < dist[v]) {
                dist[v] = destinations[u].distance + dist[u];
                prev[v] = u;
            }
        }
    }

    // Print the optimized distance
    printf("Optimized Distance for the user's route from %s to %s: %d\n", source, destination, dist[destIndex]);
}

// Function to calculate total cost of activities
int calculateTotalCost(struct TreeNode* node) {
    if (node == NULL) {
        return 0;
    }

    int totalCost = node->cost;

    struct TreeNode* current = node->activities;
    while (current != NULL) {
        totalCost += calculateTotalCost(current);
        current = current->next;
    }

    return totalCost;
}

// Function to calculate total days of activities
int calculateTotalDays(struct TreeNode* node) {
    if (node == NULL) {
        return 0;
    }

    int totalDays = node->days;

    struct TreeNode* current = node->activities;
    while (current != NULL) {
        totalDays += calculateTotalDays(current);
        current = current->next;
    }

    return totalDays;
}

int main() {
    printf("---------------------TRAVEL PLAN OPTIMIZER------------------------\n");

    struct TreeNode root;
    strncpy(root.name, "Trip", sizeof(root.name) - 1);
    root.name[sizeof(root.name) - 1] = '\0'; // Ensure null-termination
    root.days = 0;
    root.cost = 0;
    root.activities = NULL;
    root.next = NULL;
    int numDestinations = 0;
    char addAnotherDestination;

    do {
        printf("\n--- Enter Destination #%d ---\n", numDestinations + 1);
        char name[MAX_NAME_LENGTH];
        int days, cost;

        printf("Enter destination name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0; // Remove the trailing newline character

        do {
            printf("Enter number of days: ");
            scanf("%d", &days);
            if (days <= 0) {
                printf("Error: Number of days must be greater than zero.\n");
            }
        } while (days <= 0);

        do {
            printf("Enter cost: $");
            scanf("%d", &cost);
            if (cost <= 0) {
                printf("Error: Cost must be greater than zero.\n");
            }
        } while (cost <= 0);

        // Consume the newline character left in the input buffer
        getchar();

        struct TreeNode* destination = &root;
        int remainingDays = days;
        int remainingCost = cost;

        int numActivities = 0;
        char addAnotherActivity;

        do {
            char activityName[MAX_NAME_LENGTH];
            int activityDay, activityCost;

            printf("\n--- Enter Activity for %s ---\n", name);

            printf("Enter activity name: ");
            fgets(activityName, sizeof(activityName), stdin);
            activityName[strcspn(activityName, "\n")] = 0;

            do {
                printf("Enter day allocated (1-%d): ", remainingDays);
                scanf("%d", &activityDay);
                if (activityDay < 1 || activityDay > remainingDays) {
                    printf("Error: Invalid day. Please choose between 1 and %d.\n", remainingDays);
                }
            } while (activityDay < 1 || activityDay > remainingDays);

            do {
                printf("Enter cost: $");
                scanf("%d", &activityCost);
                if (activityCost <= 0 || activityCost > remainingCost) {
                    printf("Error: Cost must be greater than zero and not exceed the remaining budget of $%d.\n", remainingCost);
                }
            } while (activityCost <= 0 || activityCost > remainingCost);

            // Consume the newline character left in the input buffer
            getchar();

            addNode(destination, activityName, activityDay, activityCost, &remainingDays, &remainingCost);

            numActivities++;

            if (remainingDays == 0) {
                printf("No more days left for this destination.\n");
                break;
            } else if (remainingCost == 0) {
                printf("No more budget left for this destination.\n");
                break;
            } else if (remainingDays < 0 || remainingCost < 0) {
                printf("Error: Days or budget exceeded for this destination.\n");
                break;
            }

            printf("Remaining days: %d, Remaining cost: $%d\n", remainingDays, remainingCost);

            printf("Add another activity for %s? (y/n): ", name);
            scanf(" %c", &addAnotherActivity);
            getchar(); // Consume the newline character

        } while ((addAnotherActivity == 'y' || addAnotherActivity == 'Y') && remainingDays > 0 && remainingCost > 0);

        numDestinations++;

        printf("Add another destination? (y/n): ");
        scanf(" %c", &addAnotherDestination);
        getchar(); // Consume the newline character

    } while (addAnotherDestination == 'y' || addAnotherDestination == 'Y');

    printf("\n--- Your Travel Itinerary ---\n");
    displayActivities(&root); // Display only activities

    if (numDestinations == 0) {
        printf("No destinations added.\n");
    }

    struct Destination destinations[MAX_DESTINATIONS];
    int numDestinationsInput = 0;

    printf("\n--- Input Destination Information ---\n");

    do {
        printf("\n--- Enter Destination #%d ---\n", numDestinationsInput + 1);

        printf("Enter destination name: ");
        fgets(destinations[numDestinationsInput].name, sizeof(destinations[numDestinationsInput].name), stdin);
        destinations[numDestinationsInput].name[strcspn(destinations[numDestinationsInput].name, "\n")] = 0; // Remove the trailing newline character

        do {
            printf("Enter distance to the next destination: ");
            scanf("%d", &destinations[numDestinationsInput].distance);
            if (destinations[numDestinationsInput].distance < 0) {
                printf("Error: Distance must be non-negative.\n");
            }
        } while (destinations[numDestinationsInput].distance < 0);

        // Consume the newline character left in the input buffer
        getchar();

        numDestinationsInput++;

        printf("Add another destination? (y/n): ");
        scanf(" %c", &addAnotherDestination);
        getchar(); // Consume the newline character

    } while (addAnotherDestination == 'y' || addAnotherDestination == 'Y');

    char preferredSource[MAX_NAME_LENGTH];
    char preferredDestination[MAX_NAME_LENGTH];

    // Input user's preferred source and destination
    printf("\n--- User's Preferred Source and Destination ---\n");

    printf("Enter preferred source: ");
    fgets(preferredSource, sizeof(preferredSource), stdin);
    preferredSource[strcspn(preferredSource, "\n")] = 0;

    printf("Enter preferred destination: ");
    fgets(preferredDestination, sizeof(preferredDestination), stdin);
    preferredDestination[strcspn(preferredDestination, "\n")] = 0;

    // Add the optimized distance calculation here
    dijkstra(destinations, numDestinationsInput, preferredSource, preferredDestination);

    // Calculate and display total cost and total days
    int totalCost = calculateTotalCost(&root);
    int totalDays = calculateTotalDays(&root);

    printf("\nTotal Cost of Activities: $%d\n", totalCost);
    printf("Total Days of Activities: %d\n", totalDays);

    // Deallocate memory
    struct TreeNode* current = root.activities;
    while (current != NULL) {
        struct TreeNode* next = current->next;
        free(current);
        current = next;
    }

    return 0;
}