#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

// Structure definitions
typedef struct {
    char username[50];
    char password[50];
    int role; // 0: Client, 1: Agent, 2: Admin
    int LoginAttempts;
    bool Locked;
} User;

typedef struct {
    int id;
    char motif[100];
    char description[250];
    char categorie[50];
    int statut; 
    char clientUsername[50];
    time_t dateSubmission;
    int priority; // Add this line right here
} Reclamation;

// categories of Reclamations
const char categories[5][20] = {
    "Technique",
    "Client",
    "Facturation",
    "Produit",
    "Autre"
};
const int numCategories = 5;

// Function to count words
int countWords(char str[]) {
    int count = 0;
    int inWord = 0;
    int i = 0;

    while (str[i] != '\0') {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t') {
            inWord = 0; // We are not in a word
        } else {
            if (inWord == 0) {
                count++; // Found a new word
            }
            inWord = 1; // We are in a word
        }
        i++;
    }
    return count;
}

// Function prototypes

void inscrire();       //
int seConnecter();    // >> User Management Functions
void manageUsers();  //

void soumettreReclamation(int userId);               //
void voirReclamations(int userId);                  //
void editReclamation(int userId);                  // 
void supprimerReclamation(int userId);            // >> Reclamations Management Functions
void searchReclamation();                        //
void afficherRecByPriority();                   //
void genererRapportJournalier();               //

int isValidee(const char* nom, const char* password); // >> For password validation

void afficherMenuPrincipal();       //
void userMenu(int userId);         // >> Menu Management Functions
void adminMenu(int userId);       //
void agentMenu(int userId);      //

// Constants
#define MAX_USERS 100
#define MAX_LENGTH 50
#define MAX_RECLAMATIONS 1000
#define ROLE_CLIENT 0
#define ROLE_AGENT 1
#define ROLE_ADMIN 2

// Global variables
User Users[MAX_USERS];
int nbrUsers = 0;
Reclamation reclamations[MAX_RECLAMATIONS];
int nbrReclamations = 0;

    // in this update of The code I star coding in step by step to ensure better organization and logical flow in my program...

    // [1] Start with the user management functions :

        // >> Sign Up
void inscrire() {
    if (nbrUsers >= MAX_USERS) {
        printf("Nombre maximum d'Utilisateurs atteint.\n");
        return;
    }

    User newUser;
    printf("Entrez le nom d'Utilisateurs : ");
    fgets(newUser.username, sizeof(newUser.username), stdin);
    newUser.username[strcspn(newUser.username, "\n")] = '\0';

    for (int i = 0; i < nbrUsers; i++) {
        if (strcmp(Users[i].username, newUser.username) == 0) {
            printf("Le nom d'Utilisateurs existe deja. Veuillez en choisir un autre.\n");
            return;
        }
    }

    char password[MAX_LENGTH]; 
    int validationResult;

    do {
        printf("Entrez le mot de passe : ");
        fgets(password, sizeof(password), stdin);
        password[strcspn(password, "\n")] = '\0';

        validationResult = isValidee(newUser.username, password);
        switch (validationResult) {
            case 0:
                break; 
            case 1:
                printf("Le mot de passe doit contenir au moins 8 caracteres. \n\t>> Veuillez reessayer...\n");
                break;
            case 2:
                printf("Le mot de passe ne doit pas contenir le nom d'Utilisateur. \n\t>> Veuillez reessayer...\n");
                break;
            case 3:
                printf("Le mot de passe doit contenir au moins une lettre majuscule. \n\t>> Veuillez reessayer...\n");
                break;
            case 4:
                printf("Le mot de passe doit contenir au moins une lettre minuscule. \n\t>> Veuillez reessayer...\n");
                break;
            case 5:
                printf("Le mot de passe doit contenir au moins un chiffre. \n\t>> Veuillez reessayer...\n");
                break;
            case 6:
                printf("Le mot de passe doit contenir au moins un caractere special (!@#$%^&*). \n\t>> Veuillez reessayer...\n");
                break;
        }
    } while (validationResult != 0);

    strcpy(newUser.password, password);

    printf("Entrez votre role ");
    printf("\n[0]: Client \n[1]: Agent \n[2]: Admin \n");
    printf("\t[x] >>> ");
    char entrerRole[10];
    fgets(entrerRole, sizeof(entrerRole), stdin);
    newUser.role = atoi(entrerRole);

    newUser.LoginAttempts = 0;
    newUser.Locked = false;

    Users[nbrUsers++] = newUser;
    printf("L'utilisateur est enregistre avec succes.\n");
}

        // >> Sign In
int seConnecter() {
    char username[MAX_LENGTH], password[MAX_LENGTH];
    printf("Entrez votre nom : ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    
    printf("Entrez le mot de passe : ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    for (int i = 0; i < nbrUsers; i++) {
        if (strcmp(Users[i].username, username) == 0) {
            if (Users[i].Locked) {
                printf("Le compte est bloque. Veuillez contacter un administrateur.\n");
                return -1;
            }
            if (strcmp(Users[i].password, password) == 0) {
                Users[i].LoginAttempts = 0;
                printf("Connexion reussie. Bienvenue, %s!\n", username);
                return i;
            } else {
                Users[i].LoginAttempts++;
                if (Users[i].LoginAttempts >= 3) {
                    Users[i].Locked = true;
                    printf("Trop de tentatives. Compte bloque !\n");
                } else {
                    printf("Mot de passe incorrect. Tentatives restantes : %d\n", 3 - Users[i].LoginAttempts);
                }
                return -1;
            }
        }
    }
    printf("Utilisateur non trouve.\n");
    return -1;
}

        // >> Managing Users 
void manageUsers() {        // ! Just for admins 
    printf("Liste des utilisateurs :\n");
    for (int i = 0; i < nbrUsers; i++) {
        printf("Nom: %s \nrole: %d \nBloque: %s\n", 
               Users[i].username, Users[i].role, Users[i].Locked ? "Oui" : "Non");
    }
    
    char username[MAX_LENGTH];
    printf("Entrez le nom d'User a gerer : ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    
    for (int i = 0; i < nbrUsers; i++) {
        if (strcmp(Users[i].username, username) == 0) {
            printf("[1] Changer le role\n[2] Bloquer/Debloquer le compte\n");
            printf("\t[x] >>> ");
            int choice;
            scanf("%d", &choice);
            getchar();
            
            if (choice == 1) {
                printf("Entrez le nouveau role ");
                printf("\n[0]: Client \n[1]: Agent \n[2]: Admin \n");
                printf("\t[x] >>> ");
                scanf("%d", &Users[i].role);
                getchar();
            } else if (choice == 2) {
                Users[i].Locked = !Users[i].Locked;
                printf("Compte %s.\n", Users[i].Locked ? "bloque" : "debloque");
            }
            return;
        }
    }
    printf("User non trouve.\n");
}




    // [2] implement the claims management functions :

        // ID generator
int generateRandomId() {
    return rand() % 10000 + 1000;
}
        // >> Enter Reclamations 


void soumettreReclamation(int userId) {
    if (nbrReclamations >= MAX_RECLAMATIONS) {
        printf("Nombre maximum de reclamations atteint.\n");
        return;
    }

    Reclamation newReclamation;
    newReclamation.id = generateRandomId();

    while (1) {
        printf("Pour que nous acceptions votre Reclamation, \nvous devez suivre les etapes suivantes pour avoir\nune vue d'ensemble de votre Reclamation.\n");
        printf("\t[x] Le motif (de 3 a 8 mots) >> ");
        fgets(newReclamation.motif, sizeof(newReclamation.motif), stdin);
        newReclamation.motif[strcspn(newReclamation.motif, "\n")] = 0;

        // Count the number of words in the motif 
        int wordCount = countWords(newReclamation.motif);

        if (wordCount >= 3 && wordCount <= 8) {
            break;
        } else if (wordCount < 3) {
            printf("Le motif est trop court. Il doit contenir au moins 3 mots. \n\tVeuillez reessayer...\n");
        } else {
            printf("Le motif est trop long. Il doit contenir un maximum de 8 mots. \n\tVeuillez reessayer...\n");
        }
    }

    // Description of reclamation
    printf(">> Entrez la description detaillee du probleme : ");
    fgets(newReclamation.description, sizeof(newReclamation.description), stdin);
    newReclamation.description[strcspn(newReclamation.description, "\n")] = 0;

    // Category of reclamation (As a choice)
    printf(">> Entrez la categorie de la reclamation : \n");
    for (int i = 0; i < numCategories; i++) {
        printf("%d. %s\n", i + 1, categories[i]);
    }

    int categoryChoice;
    do {
        printf("Entrez le numero de la categorie : ");
        scanf("%d", &categoryChoice);
        getchar();

        if (categoryChoice < 1 || categoryChoice > numCategories) {
            printf("Choix invalide. Veuillez reessayer.\n");
        }
    } while (categoryChoice < 1 || categoryChoice > numCategories);

    strcpy(newReclamation.categorie, categories[categoryChoice - 1]);
    
    // Update Status and date of submission
    newReclamation.statut = 0; // en attente
    strcpy(newReclamation.clientUsername, Users[userId].username);
    newReclamation.dateSubmission = time(NULL); 

    // Get priority
    do {
        printf("Entrez la priorite de la reclamation (1: Basse, 2: Moyenne, 3: Haute): ");
        scanf("%d", &newReclamation.priority);
        getchar();

        if (newReclamation.priority < 1 || newReclamation.priority > 3) {
            printf("Priorite invalide. Veuillez entrer 1, 2 ou 3.\n");
        }
    } while (newReclamation.priority < 1 || newReclamation.priority > 3);

    
    reclamations[nbrReclamations] = newReclamation;
    nbrReclamations++;

    printf("| Reclamation soumise avec succes. \n| ID de la reclamation : %d\n", newReclamation.id);
}

        // >> See Reclamation 
void voirReclamations(int userId) {
    char dateStr[20];
    if (userId == -1) {

        for (int i = 0; i < nbrReclamations; i++) {
            printf(" _______________________________________\n");
            printf("|\tTous Les Infos de votre Reclamation:\n");
            printf(" ->>\n");
            printf(" \t> Reclamation ID: %d\n\n", reclamations[i].id);
            printf(" \t> Motif: %s\n\n", reclamations[i].motif);
            printf(" \t> Description: %s\n", reclamations[i].description);
            printf(" \t> Categorie: %s\n", reclamations[i].categorie);
            // printf(" \t> Statut: %d\n", reclamations[i].statut); removing this
            // Than add this : 
                switch (reclamations[i].statut)
                {

                case 0:
                    printf("\t> Statut: En Attente\n");
                    break;
                case 1:
                    printf("\t> Statut: En Cours\n");
                    break;
                case 2:
                    printf("\t> Statut: Resolu\n");
                    break;
                case 3:
                    printf("\t> Statut: Rejete\n");
                    break;
                default:
                    printf("\t> Statut: Inconnu\n");
                    break;
        }

            printf(" \t> Client Username: %s\n", reclamations[i].clientUsername);

            strftime(dateStr, sizeof(dateStr), "%d/%m/%Y %H:%M:%S", localtime(&reclamations[i].dateSubmission));
            printf(" \t> Date Submission: %s\n", dateStr);
        }
    } else { // Display reclamations for the given user
        for (int i = 0; i < nbrReclamations; i++) {
            if (strcmp(reclamations[i].clientUsername, Users[userId].username) == 0) {
            printf(" _______________________________________\n");
            printf("|\tTous Les Infos de votre Reclamation:\n");
            printf(" ->>\n");
            printf("\t> Reclamation ID: %d\n", reclamations[i].id);
            printf("\t> Motif: %s\n", reclamations[i].motif);
            printf("\t> Description: %s\n", reclamations[i].description);
            printf("\t> Categorie: %s\n", reclamations[i].categorie);
            // printf("\t> Statut: %d\n", reclamations[i].statut); removing this 

                switch (reclamations[i].statut)
                {

                case 0:
                    printf("\t> Statut: En Attente\n");
                    break;
                case 1:
                    printf("\t> Statut: En Cours\n");
                    break;
                case 2:
                    printf("\t> Statut: Resolu\n");
                    break;
                case 3:
                    printf("\t> Statut: Rejete\n");
                    break;
                default:
                    printf("\t> Statut: Inconnu\n");
                    break;
        }


            printf("\t> Client Username: %s\n", reclamations[i].clientUsername);

            strftime(dateStr, sizeof(dateStr), "%d/%m/%Y %H:%M:%S", localtime(&reclamations[i].dateSubmission));
            printf("\t> Date Submission: %s\n", dateStr);
            printf("------------------------------------------\n");
            }
        }
    }
}

        // >> Edit Reclamations
void editReclamation(int userId) {
    int reclamationId, choice, newStatut, categoryChoice;
    char newMotif[100], newDescription[250];

    printf("Entrez l'ID de la reclamation a modifier : ");
    scanf("%d", &reclamationId);
    getchar();

    // Find the reclamation
    int found = 0;
    for (int i = 0; i < nbrReclamations; i++) {
        if (reclamations[i].id == reclamationId) {
            found = 1;
            
            if (Users[userId].role == ROLE_CLIENT && 
                strcmp(reclamations[i].clientUsername, Users[userId].username) != 0) {
                printf("Vous n'etes pas autorise a modifier cette reclamation.\n");
                return; 
            }

            // Display current details
            printf(">> Details actuels de la reclamation : \n");
            printf(">> ID: %d\n", reclamations[i].id);
            printf(">> Motif: %s\n", reclamations[i].motif);
            printf(">> Description: %s\n", reclamations[i].description);
            printf(">> Categorie: %s\n", reclamations[i].categorie);
            printf(">> Statut: %d\n", reclamations[i].statut);
            printf(">> Priorite: %d\n", reclamations[i].priority); // Add this line to display current priority

            // Ask what the user wants to edit
            printf("=======================================\n");
            printf("  MODIFICATION MENU\n");
            printf("=======================================\n\n");

            printf("Que souhaitez-vous modifier ?\n");
            printf("[1] > Motif\n");
            printf("[2] > Description\n");
            printf("[3] > Categorie\n");
            printf("[4] > Statut (Agents & Admin only)\n");
            printf("[5] > Priorite\n");
            printf("[6] > Annuler\n");
            printf("\t[x] >> ");
            scanf("%d", &choice);
            getchar();

            switch (choice) {
                case 1:
                    printf("Entrez le nouveau motif : ");
                    fgets(newMotif, sizeof(newMotif), stdin);
                    newMotif[strcspn(newMotif, "\n")] = '\0';
                    strcpy(reclamations[i].motif, newMotif);
                    break;

                case 2:
                    printf("Entrez la nouvelle description : ");
                    fgets(newDescription, sizeof(newDescription), stdin);
                    newDescription[strcspn(newDescription, "\n")] = '\0';
                    strcpy(reclamations[i].description, newDescription);
                    break;

                case 3:
                    printf("Entrez la nouvelle categorie (choisir un nombre):\n");
                    for (int j = 0; j < numCategories; j++) {
                        printf("%d. %s\n", j + 1, categories[j]);
                    }
                    scanf("%d", &categoryChoice);
                    getchar();

                    if (categoryChoice >= 1 && categoryChoice <= numCategories) {
                        strcpy(reclamations[i].categorie, categories[categoryChoice - 1]);
                    } else {
                        printf("Choix de categorie invalide.\n");
                    }
                    break;

                case 4:
                    if (Users[userId].role == ROLE_AGENT || Users[userId].role == ROLE_ADMIN) {
                        printf("Entrez le nouveau statut :\n");
                        printf("[0]> En attente.\n");
                        printf("[1]> En cours.\n");
                        printf("[2]> Resolu\n");
                        printf("[3]> Rejete\n");
                        printf("\t[x] >> ");
                        scanf("%d", &newStatut);
                        getchar();

                        if (newStatut >= 0 && newStatut <= 3) {
                            reclamations[i].statut = newStatut;
                        } else {
                            printf("Statut invalide.\n");
                        }
                    } else {
                        printf("Vous n'etes pas autorise a modifier le statut de la reclamation.\n");
                    }
                    break;

                case 5: // Here's where you add the code for priority
                    printf("Entrez la nouvelle priorite (1: Basse, 2: Moyenne, 3: Haute): ");
                    scanf("%d", &reclamations[i].priority);
                    getchar();
                    break;

                case 6:
                    printf("Modification annulee.\n");
                    break;

                default:
                    printf("Choix invalide.\n");
            }

            if (choice >= 1 && choice <= 5) { // Adjust the condition to include case 5
                printf("Reclamation modifiee avec succes !\n");
            }
            return;
        }
    }

    if (!found) {
        printf("Aucune reclamation trouvee avec l'ID %d.\n", reclamationId);
    }
}

void supprimerReclamation(int userId) {
    int reclamationId;
    printf("Entrez l'ID de la reclamation a supprimer : ");
    scanf("%d", &reclamationId);
    getchar();

    for (int i = 0; i < nbrReclamations; i++) {
        if (reclamations[i].id == reclamationId) {
            // Check if the user hqve permition to do this action
            if (Users[userId].role == ROLE_CLIENT && 
                strcmp(reclamations[i].clientUsername, Users[userId].username) != 0) {
                printf("Vous n'etes pas autorise a supprimer cette reclamation.\n");
                return;
            }

            // Check the limit of time to do the qction (24 h)
            if (Users[userId].role == ROLE_CLIENT) {
                time_t now = time(NULL);
                if (difftime(now, reclamations[i].dateSubmission) > 24 * 60 * 60) {
                    printf("Le delai de 24 heures pour supprimer cette reclamation est depasse.\n");
                    return;
                }
            }

            // delete 
            for (int j = i; j < nbrReclamations - 1; j++) {
                reclamations[j] = reclamations[j + 1];
            }
            nbrReclamations--;

            printf("Reclamation supprimee avec succes.\n");
            return;
        }
    }

    printf("Reclamation introuvable.\n");
}

        // >> Search for Reclamations
void searchReclamation() {
    int choice;
    int reclamationId;
    char clientName[MAX_LENGTH];
    time_t startDate, endDate;
    struct tm startTm, endTm;
    char dateStr[20];

    printf("\n|------- Menu de Recherche de Reclamation -------|\n");
    printf("1. Rechercher par ID de reclamation\n");
    printf("2. Rechercher par nom de client\n");
    printf("3. Rechercher par date de soumission\n");
    printf("\t[x] >>> ");
    char Choice01[10];
    fgets(Choice01, sizeof(Choice01), stdin);
    Choice01[strcspn(Choice01, "\n")] = '\0';
    choice = atoi(Choice01);

    // Add search Options
    switch (choice)
    {
        case 1:
            // Search by Reclamation ID
            printf("Entrez l'ID de la reclamation : ");
            scanf("%d", &reclamationId);
            getchar();

            for (int i = 0; i < nbrReclamations; i++) {
                if (reclamations[i].id == reclamationId) {
                    printf(" _______________________________________\n");
                    printf("|\tTous Les Infos de votre Reclamation:\n");
                    printf(" ->>\n");
                    printf("\t> Reclamation ID: %d\n", reclamations[i].id);
                    printf("\t> Motif: %s\n", reclamations[i].motif);
                    printf("\t> Description: %s\n", reclamations[i].description);
                    printf("\t> Categorie: %s\n", reclamations[i].categorie);
                    switch (reclamations[i].statut) {
                        case 0:
                            printf("\t> Statut: En Attente\n");
                            break;
                        case 1:
                            printf("\t> Statut: En Cours\n");
                            break;
                        case 2:
                            printf("\t> Statut: Resolu\n");
                            break;
                        case 3:
                            printf("\t> Statut: Rejete\n");
                            break;
                        default:
                            printf("\t> Statut: Inconnu\n");
                            break;
                    }
                    printf("\t> Client Username: %s\n", reclamations[i].clientUsername);
                    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y %H:%M:%S", localtime(&reclamations[i].dateSubmission));
                    printf("\t> Date Submission: %s\n", dateStr);
                    return;
                }
            }
            printf("Aucune reclamation trouvee avec l'ID %d.\n", reclamationId);
            break;


        case 2:
            // Search by Client Name
            printf("Entrez le nom du client : ");
            fgets(clientName, sizeof(clientName), stdin);
            clientName[strcspn(clientName, "\n")] = '\0';

            for (int i = 0; i < nbrReclamations; i++) {
                if (strcmp(reclamations[i].clientUsername, clientName) == 0) {
                    printf(" _______________________________________\n");
                    printf("|\tTous Les Infos de votre Reclamation:\n");
                    printf(" ->>\n");
                    printf("\t> Reclamation ID: %d\n", reclamations[i].id);
                    printf("\t> Motif: %s\n", reclamations[i].motif);
                    printf("\t> Description: %s\n", reclamations[i].description);
                    printf("\t> Categorie: %s\n", reclamations[i].categorie);
                    switch (reclamations[i].statut) {
                        case 0:
                            printf("\t> Statut: En Attente\n");
                            break;
                        case 1:
                            printf("\t> Statut: En Cours\n");
                            break;
                        case 2:
                            printf("\t> Statut: Resolu\n");
                            break;
                        case 3:
                            printf("\t> Statut: Rejete\n");
                            break;
                        default:
                            printf("\t> Statut: Inconnu\n");
                            break;
                    }
                    printf("\t> Client Username: %s\n", reclamations[i].clientUsername);
                    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y %H:%M:%S", localtime(&reclamations[i].dateSubmission));
                    printf("\t> Date Submission: %s\n", dateStr);
                }
            }
            printf("Aucune reclamation trouvee pour le client %s.\n", clientName);
            break;
        case 3:
            // Search by Submission Date
            printf("Entrez la date de debut (JJ/MM/AAAA) : ");
            scanf("%d/%d/%d", &startTm.tm_mday, &startTm.tm_mon, &startTm.tm_year);
            startTm.tm_mon -= 1;
            startTm.tm_year -= 1900;
            startDate = mktime(&startTm);

            printf("Entrez la date de fin (JJ/MM/AAAA) : ");
            scanf("%d/%d/%d", &endTm.tm_mday, &endTm.tm_mon, &endTm.tm_year);
            endTm.tm_mon -= 1; // Adjust month (0-11)
            endTm.tm_year -= 1900; // Adjust year
            endDate = mktime(&endTm);

            for (int i = 0; i < nbrReclamations; i++) {
                if (reclamations[i].dateSubmission >= startDate && reclamations[i].dateSubmission <= endDate) {
                    printf(" _______________________________________\n");
                    printf("|\tTous Les Infos de votre Reclamation:\n");
                    printf(" ->>\n");
                    printf("\t> Reclamation ID: %d\n", reclamations[i].id);
                    printf("\t> Motif: %s\n", reclamations[i].motif);
                    printf("\t> Description: %s\n", reclamations[i].description);
                    printf("\t> Categorie: %s\n", reclamations[i].categorie);
                    switch (reclamations[i].statut) {
                        case 0:
                            printf("\t> Statut: En Attente\n");
                            break;
                        case 1:
                            printf("\t> Statut: En Cours\n");
                            break;
                        case 2:
                            printf("\t> Statut: Resolu\n");
                            break;
                        case 3:
                            printf("\t> Statut: Rejete\n");
                            break;
                        default:
                            printf("\t> Statut: Inconnu\n");
                            break;
                    }
                    printf("\t> Client Username: %s\n", reclamations[i].clientUsername);
                    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y %H:%M:%S", localtime(&reclamations[i].dateSubmission));
                    printf("\t> Date Submission: %s\n", dateStr);
                }
            }
            printf("Aucune reclamation trouvee dans la periode selectionnee.\n");
            break;
    
        default:
            printf("Choix invalide.\n");
    }
}

        // >> See Reclamations by priority
void afficherRecByPriority() {
    // Step 1: Create a temporary array of reclamations
    Reclamation sortedReclamations[MAX_RECLAMATIONS];
    
    // Step 2: Copy all reclamations to the temporary array
    for (int i = 0; i < nbrReclamations; i++) {
        sortedReclamations[i] = reclamations[i];
    }
    
    // Step 3: Sort the temporary array by priority (descending order)
    for (int i = 0; i < nbrReclamations - 1; i++) {
        for (int j = 0; j < nbrReclamations - i - 1; j++) {
            if (sortedReclamations[j].priority < sortedReclamations[j + 1].priority) {
                // Swap the elements
                Reclamation temp = sortedReclamations[j];
                sortedReclamations[j] = sortedReclamations[j + 1];
                sortedReclamations[j + 1] = temp;
            }
        }
    }
    
    // Step 4: Display the sorted reclamations
    printf("\nReclamations triees par priorite (de la plus haute a la plus basse):\n");
    for (int i = 0; i < nbrReclamations; i++) {
        printf("\n--- Reclamation %d ---\n", i + 1);
        printf("ID: %d\n", sortedReclamations[i].id);
        printf("Motif: %s\n", sortedReclamations[i].motif);
        printf("Priorite: %d\n", sortedReclamations[i].priority);
        printf("Statut: %d\n", sortedReclamations[i].statut);
        printf("Client: %s\n", sortedReclamations[i].clientUsername);
    }
}

        // >> Generate Daily Report
void genererRapportJournalier() {
    time_t now;
    struct tm local;
    char filename[50];
    FILE *file;
    int i;

    time(&now);
    local = *localtime(&now);

    sprintf(filename, "rapport_%04d%02d%02d.txt", local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);

    file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erreur lors de la creation du fichier de rapport.\n");
        return;
    }

    fprintf(file, "Rapport Journalier - %02d/%02d/%04d\n\n", local.tm_mday, local.tm_mon + 1, local.tm_year + 1900);
    fprintf(file, "Nouvelles Reclamations:\n");

    for (i = 0; i < nbrReclamations; i++) {
        if (reclamations[i].dateSubmission >= now - 86400) { // Last 24 hours
            fprintf(file, "ID: %d, Motif: %s, Categorie: %s\n", 
                    reclamations[i].id, reclamations[i].motif, reclamations[i].categorie);
        }
    }

    fprintf(file, "\nReclamations Resolues:\n");
    
    for (i = 0; i < nbrReclamations; i++) {
        if (reclamations[i].statut == 2 && reclamations[i].dateSubmission >= now - 86400) {
            fprintf(file, "ID: %d, Motif: %s, Categorie: %s\n", 
                    reclamations[i].id, reclamations[i].motif, reclamations[i].categorie);
        }
    }

    fclose(file);
    printf("Rapport genere avec succes: %s\n", filename);
}


    // [3] Validation of inputs :

        // >> Password Validator
int isValidee(const char* nom, const char* password) {
    int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSpecial = 0;
    int length = strlen(password);

    // Le mot de passe doit contenir au moins 8 caracteres
    if (length < 8) {
        return 1;
    }
    
    // Ne doit pas contenir le nom d'Utilisateur
    if (strstr(password, nom) != NULL) {
        return 2;
    }

    // tipe Char Verifications
    for (int i = 0; i < length; i++) {
        if (isupper(password[i])) {
            hasLower = 1;
        }
        if (islower(password[i])) {
            hasUpper = 1;
        }
        if (isdigit(password[i])) {
            hasDigit = 1;
        }
        if (strchr("!@#$%^&*!@#$^&*%_+=?/|~`:;""'<>{} ", password[i])) {
            hasSpecial = 1;
        }
    }

    // Possible errors...
    if (!hasLower) return 3;
    if (!hasUpper) return 4;
    if (!hasDigit) return 5;
    if (!hasSpecial) return 6; 

    return 0;
}



    // [4] Implement menu functions :

        // >> Displaying the Main Menu
void afficherMenuPrincipal() {
    printf("\n|___________\n");
    printf("\n| Menu Principal:\n");
    printf(" [1] S'inscrire\n");
    printf(" [2] Se connecter\n");
    printf(" [3] Quitter\n");
    printf("\t[x] >>> ");
}

        // >> User Menu
void userMenu(int userId) {
    int choice;
    do {
        printf("\n|------- Menu User -------|\n");
        printf("[1]. Soumettre une reclamation\n");
        printf("[2]. Voir mes reclamations\n");
        printf("[3]. Rechercher une reclamation\n");
        printf("[4]. Gerer les reclamations\n");
        printf("[5]. Supprimer une reclamation\n");
        printf("[6]. Deconnexion\n");
        printf("\t[x] >>> ");

        char Choice01[10];
        fgets(Choice01, sizeof(Choice01), stdin);
        Choice01[strcspn(Choice01, "\n")] = '\0';
        choice = atoi(Choice01);

        switch(choice) {
            case 1:
                soumettreReclamation(userId);
                break;
            case 2:
                voirReclamations(userId);
                break;
            case 3:
                searchReclamation();
                break;
            case 4:
                if (Users[userId].role == ROLE_AGENT || Users[userId].role == ROLE_ADMIN) {
                    editReclamation(userId);
                } else {
                    printf("Action reservee aux agents et administrateurs.\n");
                }
                break;
            case 5:
                supprimerReclamation(userId);
                break;
            case 6:
                printf("Deconnexion...\n");
                break;
            default:
                printf("Choix non valide. Veuillez reessayer.\n");
        }
    } while (choice != 6);
}
    
        // >> Administrator Menu
void adminMenu(int userId) {
    int choice;
    do {
        printf("\n|------- Menu Administrateur -------|\n");
        printf("[1]. Voir toutes les reclamations\n");
        printf("[2]. Ajouter une reclamation\n");
        printf("[3]. Modifier une reclamation\n");
        printf("[4]. Rechercher une reclamation\n");
        printf("[5]. Voir reclamations par priorite\n");
        printf("[6]. Gerer les utilisateurs\n");
        printf("[7]. Generer rapport journalier\n");
        printf("[8]. Supprimer une reclamation\n");
        printf("[9]. Deconnexion\n");
        printf("\t[x] >>> ");

        char Choice01[10];
        fgets(Choice01, sizeof(Choice01), stdin);
        Choice01[strcspn(Choice01, "\n")] = '\0';
        choice = atoi(Choice01);

        switch(choice) {
            // ... (previous cases remain the same)
            case 8:
                supprimerReclamation(userId); 
                break;
            case 9:
                printf("Deconnexion...\n");
                break;
            default:
                printf("Choix invalide.\n");
        }
    } while (choice != 9);
}

        // >> Agent Menu
void agentMenu(int userId) {
    int choice;
    do {
        printf("\n|------- Menu Agent de Reclamation -------|\n");
        printf("[1]. Voir mes reclamations\n");
        printf("[2]. Rechercher une reclamation\n");
        printf("[3]. Modifier une reclamation\n");
        printf("[4]. Supprimer une reclamation\n");
        printf("[5]. Deconnexion\n");
        printf("\t[x] >>> ");

        char Choice01[10];
        fgets(Choice01, sizeof(Choice01), stdin);
        Choice01[strcspn(Choice01, "\n")] = '\0';
        choice = atoi(Choice01);

        switch(choice) {
            case 1:
                voirReclamations(userId);
                break;
            case 2:
                searchReclamation();
                break;
            case 3:
                editReclamation(userId);
                break;
            case 4:
                supprimerReclamation(userId); 
                break;
            case 5:
                printf("Deconnexion...\n");
                break;
            default:
                printf("Choix invalide.\n");
        }
    } while (choice != 5);
}



    // The Main Function To Combinate All Those Function at the Tooooop.

int main() {
    srand(time(NULL));
    int choice, annee;
    int UserConnecte = -1;

        // Password for the entire software! 

    printf("\n\n\t ------------------------------------------------------------------------------------------\n");
    printf("\t| >> Bienvenue dans le Systeme de Gestion des Reclamations Clients.                              \n");
    printf("\t|    Pour verifier si vous utilisez la bonne version de notre programme\n\t Entrez l'annee [xxxx] : ");
    scanf("%d", &annee);
    getchar();
    
    if (annee == 2024) { // You see the password of Software is 2024, you can change it anytime you want!
        do {
            afficherMenuPrincipal();

            char Choice01[10];
            fgets(Choice01, sizeof(Choice01), stdin);
            choice = atoi(Choice01);

            switch(choice) {
                case 1:

                    inscrire();
                    break;
                case 2:

                    UserConnecte = seConnecter();

                    if (UserConnecte != -1) {
                        printf("Connecte en tant que %s avec le role %d\n", 
                               Users[UserConnecte].username, Users[UserConnecte].role);


                        switch (Users[UserConnecte].role) {
                            case ROLE_CLIENT:
                                userMenu(UserConnecte);
                                break;
                            case ROLE_AGENT:
                                agentMenu(UserConnecte);
                                break;
                            case ROLE_ADMIN:
                                adminMenu(UserConnecte);
                                break;
                        }
                    }
                    break;
                case 3:
                
                    if (Users[UserConnecte].role == ROLE_AGENT || Users[UserConnecte].role == ROLE_ADMIN) {
                        editReclamation(UserConnecte);
                    }
                    break;

                    
                case 4:
                    
                    printf("We are working on more options. Be patien...\n");
                    break;
                default:
                    printf("choice non valide. Veuillez reessayer.\n");
            }
        } while (1);
    } else {
        printf("\n\n\t     :/ Desole, cette version du programme n'est pas compatible avec l'annee saisie. \n\n\t     Veuillez essayer une autre version.\n\n");
    }
   
    return 0;
}