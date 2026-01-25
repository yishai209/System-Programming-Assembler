#include "assembler.h"



/*no API for this part , it is the functions that add to the stcuct lists */
/*no API for this part , it is the functions that add to the stcuct lists */
/*no API for this part , it is the functions that add to the stcuct lists */



void add_label_list(LABEL_LIST **label_head, char *label_name, int IC, char type[]) {
    LABEL_LIST *tmp;
    LABEL_LIST *new_node = malloc(sizeof(LABEL_LIST));
    char clean_label[MAX_LABEL_SIZE];
    int i = 0, j = 0,k;

    if (new_node == NULL) {
        printf("Error in Malloc\n");
        exit(1);
    }

    /* Remove leading/trailing whitespace from label_name */
    while (label_name[i] && isspace((unsigned char)label_name[i])) i++;
    while (label_name[i] && j < MAX_LABEL_SIZE - 1 && !isspace((unsigned char)label_name[i])) {
        clean_label[j++] = label_name[i++];
    }
    clean_label[j] = '\0';

    for (k = 0; clean_label[k]; ++k) {
        if (clean_label[k] == '\n' || clean_label[k] == '\r') {
            clean_label[k] = '\0';
            break;
        }
    }

    strcpy(new_node->label, clean_label);
    new_node->addr = IC;
    strcpy(new_node->type, type);
    new_node->next = NULL;

    if (*label_head == NULL) {
        *label_head = new_node;
        return;
    }

    tmp = *label_head;
    while (tmp->next != NULL) {
        tmp = tmp->next;
    }
    tmp->next = new_node;
}


 void add_to_code_list(Code_list **head, int addr, unsigned short code_word, char *label_name) {
    Code_list *tmp;
    Code_list *new_node = malloc(sizeof(Code_list));
    char clean_label[MAX_LABEL_SIZE];
    int i = 0, j = 0, k;
    if (new_node == NULL) {
        printf("Error in Malloc\n");
        exit(1);
    }

 while (label_name[i] && isspace((unsigned char)label_name[i])) i++;
    while (label_name[i] && j < MAX_LABEL_SIZE - 1 && !isspace((unsigned char)label_name[i])) {
        clean_label[j++] = label_name[i++];
    }
    clean_label[j] = '\0';

    for (k = 0; clean_label[k]; ++k) {
        if (clean_label[k] == '\n' || clean_label[k] == '\r') {
            clean_label[k] = '\0';
            break;
        }
    }


    
    new_node->addr = addr;
    new_node->code_word = code_word;
    strcpy(new_node->label_name, clean_label);
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
        return;
    }

    tmp = *head;
    while (tmp->next != NULL) {
        tmp = tmp->next;
    }
    tmp->next = new_node;

 }


 void add_to_data_list(Data_list **head, int addr, unsigned short data_word) {
    Data_list *new_node = malloc(sizeof(Data_list));
    if (!new_node) { printf("Error in malloc\n"); exit(1); }
    new_node->addr = addr;
    new_node->data_word = data_word;
    new_node->next = NULL;

    if (*head == NULL) *head = new_node;
    else {
        Data_list *tmp = *head;
        while (tmp->next) tmp = tmp->next;
        tmp->next = new_node;
    }
}


void add_entry_to_list(Entry_list **head, char *label, int addr)
{
    Entry_list *new_node;
    Entry_list *temp;
    char clean_label[MAX_LABEL_SIZE];
    int i = 0, j = 0;

    /* Remove leading/trailing whitespace from label */
    while (label[i] && isspace((unsigned char)label[i])) i++;
    while (label[i] && j < MAX_LABEL_SIZE - 1 && !isspace((unsigned char)label[i])) {
        clean_label[j++] = label[i++];
    }
    clean_label[j] = '\0';

    new_node = (Entry_list *)malloc(sizeof(Entry_list));
    if (new_node == NULL)
    {
        printf(" Memory allocation failed for Entry_list node.\n");
        return;
    }

    strncpy(new_node->label, clean_label, MAX_LABEL_SIZE - 1);
    new_node->label[MAX_LABEL_SIZE - 1] = '\0'; /* Ensure null-termination */
    new_node->addr = addr;
    new_node->next = NULL;

    if (*head == NULL)
    {
        *head = new_node;
    }
    else
    {
        temp = *head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}



void add_extern_to_list(Extern_list **head,char *label, int addr)
{
    Extern_list *new_node;
    Extern_list *temp;
    char clean_label[MAX_LABEL_SIZE];
    int i = 0, j = 0;

    /* Remove leading/trailing whitespace from label */
    while (label[i] && isspace((unsigned char)label[i])) i++;
    while (label[i] && j < MAX_LABEL_SIZE - 1 && !isspace((unsigned char)label[i])) {
        clean_label[j++] = label[i++];
    }
    clean_label[j] = '\0';

    new_node = (Extern_list *)malloc(sizeof(Extern_list));
    if (new_node == NULL)
    {
        printf("Memory allocation failed for Extern_list node.\n");
        return;
    }

    strncpy(new_node->label, clean_label, MAX_LABEL_SIZE - 1);
    new_node->label[MAX_LABEL_SIZE - 1] = '\0';
    new_node->addr = addr;
    new_node->next = NULL;

    if (*head == NULL)
    {
        *head = new_node;
    }
    else
    {
        temp = *head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}


 
