#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #define OS_OPEN_COMMAND "start %s"
#elif __APPLE__
    #define OS_OPEN_COMMAND "open %s"
#else
    #define OS_OPEN_COMMAND "xdg-open %s"
#endif

#define ALPHABET_SIZE 26

struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    char* contact_number;
    char* email;
    char* image_address;  // New field for image address
};

struct TrieNode* createNode() {
    struct TrieNode* newNode = (struct TrieNode*)malloc(sizeof(struct TrieNode));
    if (newNode) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            newNode->children[i] = NULL;
        }
        newNode->contact_number = NULL;
        newNode->email = NULL;
        newNode->image_address = NULL;  // Initialize image address to NULL
    }
    return newNode;
}

int isValidMobileNumber(const char* number) {
    int len = strlen(number);
    if (len != 10) {
        return 0; // Not 10 digits
    }
    for (int i = 0; i < len; i++) {
        if (!isdigit(number[i])) {
            return 0; // Contains non-digit characters
        }
    }
    return 1; // Valid mobile number
}

void insertContactWithImage(struct TrieNode* root, const char* name, const char* contact_number, const char* email, const char* image_address) {
    if (!isValidMobileNumber(contact_number)) {
        printf("Invalid mobile number. Please enter a 10-digit number.\n");
        return;
    }

    struct TrieNode* current = root;
    int len = strlen(name);

    for (int i = 0; i < len; i++) {
        char c = tolower(name[i]);
        if (c == ' ') {
            continue;
        }
        int index = c - 'a';
        if (!current->children[index]) {
            current->children[index] = createNode();
        }
        current = current->children[index];
    }

    current->contact_number = strdup(contact_number);
    current->email = strdup(email);
    current->image_address = strdup(image_address);
}

void displayContactWithImage(struct TrieNode* node, const char* prefix) {
    if (node == NULL) {
        return;
    }

    if (node->contact_number != NULL) {
        printf("Name: %s, Mobile Number: %s, Email: %s, Image Address: %s\n", prefix, node->contact_number, node->email, node->image_address);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            char newPrefix[strlen(prefix) + 2];
            sprintf(newPrefix, "%s%c", prefix, 'a' + i);
            displayContactWithImage(node->children[i], newPrefix);
        }
    }
}

void searchContactsWithImage(struct TrieNode* root, const char* prefix) {
    struct TrieNode* current = root;
    int len = strlen(prefix);

    for (int i = 0; i < len; i++) {
        char c = tolower(prefix[i]);
        if (c == ' ') {
            continue;
        }
        int index = c - 'a';
        if (!current->children[index]) {
            printf("No contacts found with the given prefix.\n");
            return;
        }
        current = current->children[index];
    }

    displayContactWithImage(current, prefix);
}

void saveContactsDFS(struct TrieNode* node, const char* currentName, FILE* file);

void saveContactsToFile(struct TrieNode* root, const char* filename) {
    FILE* file = fopen(filename, "ab+");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    saveContactsDFS(root, "", file);

    fclose(file);
}

void saveContactsDFS(struct TrieNode* node, const char* currentName, FILE* file) {
    if (node == NULL) {
        return;
    }

    if (node->contact_number != NULL) {
        fprintf(file, "Name: %s, Mobile Number: %s, Email: %s, Image Address: %s\n", currentName, node->contact_number, node->email, node->image_address);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            char newPrefix[strlen(currentName) + 2];
            sprintf(newPrefix, "%s%c", currentName, 'a' + i);
            saveContactsDFS(node->children[i], newPrefix, file);
        }
    }
}

void freeTrie(struct TrieNode* node) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        freeTrie(node->children[i]);
    }
    free(node->contact_number);
    free(node->email);
    free(node->image_address);
    free(node);
}

void openImage(const char* image_address) {
    char command[256];
    sprintf(command, OS_OPEN_COMMAND, image_address);
    system(command);
}

struct TrieNode* searchContactNode(struct TrieNode* root, const char* name) {
    struct TrieNode* current = root;
    int len = strlen(name);

    for (int i = 0; i < len; i++) {
        char c = tolower(name[i]);
        if (c == ' ') {
            continue;
        }
        int index = c - 'a';
        if (!current->children[index]) {
            return NULL;  // Contact not found
        }
        current = current->children[index];
    }

    return current;
}


int main() {
    struct TrieNode* root = createNode();
    int choice;
    char input[100], contact_number[20], email[50], image_address[100];

    while (1) {
        printf("\nPhone Book Directory\n");
        printf("1. Add a contact\n");
        printf("2. Search for a contact by prefix\n");
        printf("3. Save contacts to a file\n");
        printf("4. Add a contact with image\n");
        printf("5. Open image from contact\n");
        printf("6. Quit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            // Consume the invalid input
            scanf("%*s");
            continue;
        }

        // Consume the newline character from the previous input
        getchar();

        switch (choice) {
            case 1:
                printf("Enter name: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';

                printf("Enter contact number: ");
                fgets(contact_number, sizeof(contact_number), stdin);
                contact_number[strcspn(contact_number, "\n")] = '\0';

                printf("Enter email: ");
                fgets(email, sizeof(email), stdin);
                email[strcspn(email, "\n")] = '\0';

                printf("Enter image address: ");
                fgets(image_address, sizeof(image_address), stdin);
                image_address[strcspn(image_address, "\n")] = '\0';

                insertContactWithImage(root, input, contact_number, email, image_address);
                break;

            case 2:
                printf("Enter prefix to search: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                searchContactsWithImage(root, input);
                break;

            case 3:
                printf("Enter file name to save contacts: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                saveContactsToFile(root, input);
                printf("Contacts saved to file successfully.\n");
                break;

            case 4:
                printf("Enter name: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';

                printf("Enter contact number: ");
                fgets(contact_number, sizeof(contact_number), stdin);
                contact_number[strcspn(contact_number, "\n")] = '\0';

                printf("Enter email: ");
                fgets(email, sizeof(email), stdin);
                email[strcspn(email, "\n")] = '\0';

                printf("Enter image address: ");
                fgets(image_address, sizeof(image_address), stdin);
                image_address[strcspn(image_address, "\n")] = '\0';

                insertContactWithImage(root, input, contact_number, email, image_address);
                break;

            case 5:
                printf("Enter name to open image: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';

                // Search for the contact
                struct TrieNode* contactNode = searchContactNode(root, input);

                if (contactNode != NULL && contactNode->image_address != NULL) {
                    openImage(contactNode->image_address);
                } else {
                    printf("Contact not found or no image associated.\n");
                }
                break;

            case 6:
                freeTrie(root);
                printf("Thank you!\n");
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
