#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct MeasurementModuleID {
    char oznacenie;
    int cislovanie;
    char druh;
};

struct Position {
    float latitude;
    float longitude;
};

struct Record {
    struct MeasurementModuleID moduleID;
    struct Position position;
    char MeasurementType[3];
    float Value;
    long Time;
    long long Date;
    struct Record* next;
};


int z_function(struct Record **head, int count) { // delete the record
    char oznacenie;
    int cislovanie;
    char druh;

    scanf(" %c%d%c", &oznacenie, &cislovanie, &druh);

    struct Record *current = *head;
    struct Record *prev = NULL;
    int deleted = 0;

    while (current != NULL) {
        if (current->moduleID.oznacenie == oznacenie &&
            current->moduleID.cislovanie == cislovanie &&
            current->moduleID.druh == druh) {
            // check if the record to be deleted is at the beginning of the list
            if (prev == NULL) {
                *head = current->next; // update the head to skip the deleted record
                printf("Zaznam pre ID: %c%d%c bol vymazany.\n", oznacenie, cislovanie, druh);
                free(current); // free the memory of the deleted record
                deleted++;
                current = *head; // move to the next record
            } else {
                prev->next = current->next;  // update the previous node's next pointer to skip the deleted record
                printf("Zaznam pre ID: %c%d%c bol vymazany.\n", oznacenie, cislovanie, druh);
                free(current); // free the memory of the deleted record
                deleted++;
                current = prev->next;
            }
        } else {
            prev = current;
            current = current->next;
        }
    }

    count = count - deleted;
    return count;
}


int n_function(FILE **file, struct Record **head) { // reading data and adding it to linked list
    *file = fopen("dataloger_V2.txt", "r");
    if (*file == NULL) { //check if file was previously opened
        printf("Zaznamy neboli nacitane!\n");
        return -1;
    }

    while (*head != NULL) { // free memory and linked list if it was previously allocated
        struct Record* temp = *head;
        *head = (*head)->next;
        free(temp);
    }

    struct Record* current = NULL;
    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), *file)) { // storing data to linked list
        if (strncmp(line, "$$$", 3) == 0) {
            struct Record* newRecord = malloc(sizeof(struct Record)); // allocate memory foe a new record
            if (newRecord == NULL) {
                return -1;
            }

            struct MeasurementModuleID moduleID;
            char type[3];
            char latStr[10], lonStr[10];

            if (fscanf(*file, "%c%d%c %9s %9s %3s %f %ld %lld", &moduleID.oznacenie, &moduleID.cislovanie, &moduleID.druh,
                       latStr, lonStr, type, &newRecord->Value, &newRecord->Time, &newRecord->Date) != 9) {
                free(newRecord);
                return -1;
            }

            // convert latitude and longitude strings to floats and store them in the new record

            sscanf(latStr, "%f", &newRecord->position.latitude);
            sscanf(lonStr, "%f", &newRecord->position.longitude);

            newRecord->moduleID = moduleID;
            strcpy(newRecord->MeasurementType, type);
            newRecord->next = NULL;
            // add a new record
            if (*head == NULL) {
                *head = newRecord; // if ll is empty add it as ita head
            } else {
                current->next = newRecord;
            }

            current = newRecord;
            count++;
        }
    }

    fclose(*file);
    printf("Nacitalo sa %d zaznamov\n", count);
    return count;
}


void v_function(struct Record *head) { // printing out the linked list
    int count = 1;
    struct Record* current = head;

    while (current != NULL) {
        printf("%d:\n", count);
        printf("ID: %c%03d%c\t%s\t%.2f\n", current->moduleID.oznacenie, current->moduleID.cislovanie, current->moduleID.druh,
               current->MeasurementType, current->Value);
        printf("Poz: %+.4f\t%+.4f\n", current->position.latitude, current->position.longitude);
        printf("DaC: %04lld\t%04ld\n\n", current->Date, current->Time);

        current = current->next;
        count++;
    }
}


int r_function(struct Record **head, int counter) {
    int c1, c2;
    scanf("%d %d", &c1, &c2);

    if (c1 <= 0 || c2 <= 0 || c1 > counter || c2 > counter || c1 == c2) {
        return counter;
    }
    // initialize pointers for the previous and current nodes corresponding to the two indices
    struct Record *prev1 = NULL, *prev2 = NULL;
    struct Record *record1 = *head, *record2 = *head;
    // traverse the linked list to find the records at the specified indices
    for (int i = 1; i < c1; i++) {
        if (record1 == NULL) {
            return counter;
        }
        prev1 = record1;
        record1 = record1->next;
    }

    for (int i = 1; i < c2; i++) {
        if (record2 == NULL) {
            return counter;
        }
        prev2 = record2;
        record2 = record2->next;
    }

    // Swap the positions of the records in the linked list
    if (prev1 != NULL) {
        prev1->next = record2;
    } else {
        *head = record2;
    }

    if (prev2 != NULL) {
        prev2->next = record1;
    } else {
        *head = record1;
    }

    struct Record *temp = record1->next;
    record1->next = record2->next;
    record2->next = temp;

    return counter;
}


int p_function(struct Record **head, int counter) { // add new record to the linked list
    int position;
    scanf("%d", &position);
    // allocate memory for the new record
    struct Record* newRecord = malloc(sizeof(struct Record));
    if (newRecord == NULL) {
        printf("Memory allocation failed.\n");
        return -1;
    }

    char line[256];

    if (position <= 0 || position > counter + 1) { // check the seq num
        position = counter + 1;
    }

    fgets(line, sizeof(line), stdin);

    // Read the data for the new record
    if (fgets(line, sizeof(line), stdin) != NULL) {
        sscanf(line, "%c%d%c", &newRecord->moduleID.oznacenie, &newRecord->moduleID.cislovanie, &newRecord->moduleID.druh);
    }

    if (fgets(line, sizeof(line), stdin) != NULL) {
        sscanf(line, "%f %f", &newRecord->position.latitude, &newRecord->position.longitude);
    }

    if (fgets(line, sizeof(line), stdin) != NULL) {
        sscanf(line, "%3s", newRecord->MeasurementType);
    }

    if (fgets(line, sizeof(line), stdin) != NULL) {
        sscanf(line, "%f", &newRecord->Value);
    }

    if (fgets(line, sizeof(line), stdin) != NULL) {
        sscanf(line, "%ld", &newRecord->Time);
    }

    if (fgets(line, sizeof(line), stdin) != NULL) {
        sscanf(line, "%lld", &newRecord->Date);
    }

    newRecord->next = NULL; // initialize the next point to the new record

    // add the new record to the linked list at the specified position

    if (position == 1) {
        newRecord->next = *head;
        *head = newRecord;
    } else {
        struct Record* current = *head;
        struct Record* prev = NULL;
        int currentPos = 1;

        // traverse the list to find the position to insert the new record
        while (current != NULL && currentPos < position) {
            prev = current;
            current = current->next;
            currentPos++;
        }
        // insert new record
        if (prev != NULL) {
            prev->next = newRecord;
        } else {
            *head = newRecord;
        }

        newRecord->next = current;
    }

    return counter + 1;
}


void u_function(struct Record **head) { // sort the list
    if (*head == NULL || (*head)->next == NULL) {
        printf("Spajany zoznam bol usporiadany.\n");
        return;
    }
    // initialize variables for sorting
    struct Record *sorted = NULL;
    struct Record *current = *head;
    *head = NULL;

    while (current != NULL) {
        struct Record *next = current->next;
        struct Record *prev = NULL;
        struct Record *sort_prev = NULL;
        struct Record *sort_current = sorted;
        // traverse the sorted list to find the correct position for the current node
        while (sort_current != NULL && (current->Date > sort_current->Date || (current->Date == sort_current->Date && current->Time >= sort_current->Time)))
        {
            sort_prev = sort_current;
            sort_current = sort_current->next;
        }
        // insert the current node into the sorted list
        if (sort_prev != NULL) {
            sort_prev->next = current;
        } else {
            sorted = current; // if the current node becomes the new head of the sorted list
        }

        current->next = sort_current;
        // update the original list by removing the current node
        if (prev != NULL) {
            prev->next = next;
        } else {
            // if the current node was the head of the original list
            *head = next;
        }
        // move to the next node in the original list
        current = next;
    }
    // update the head of the original list to point to the sorted list
    *head = sorted;
    printf("Spajany zoznam bol usporiadany.\n");
}


void k_function(struct Record **head) { // free memory
    while (*head != NULL) {
        struct Record* temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}


int main() {
    FILE *file = NULL;
    struct Record *head = NULL;
    int count = 0;

    char command;

    while (1) {
        scanf(" %c", &command);

        switch (command) {
            case 'n':
                count = n_function(&file, &head);
                break;
            case 'u':
                u_function(&head);
                break;
            case 'r':
                r_function(&head, count);
                break;
            case 'v':
                v_function(head);
                break;
            case 'z':
                count = z_function(&head, count);
                break;
            case 'p':
                count = p_function(&head, count);
                break;
            case 'k':
                k_function(&head);
                return 0;
            default:
                break;
        }

    }

}
