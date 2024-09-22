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
    int statut; // 0: en attente, 1: en cours, 2: resolu, 3: rejete
    char clientUsername[50];
    time_t dateSubmission; // Add Time >>> dada of Reclamation
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
void editReclamation(int userId) {};               // >> Reclamations Management Functions
void searchReclamation();                         //
void afficherRecByPriority();                    //

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

    printf("Pour Que nous acceptions votre Reclamation, \nvous devez suivre les etapes suivants pour avoir\nune vue d'ensemble de votre Reclamation.\n");
    printf("\t[x] Le motif (de 3 a 8 mots) >> ");
    fgets(newReclamation.motif, sizeof(newReclamation.motif), stdin);
    newReclamation.motif[strcspn(newReclamation.motif, "\n")] = 0;

         // >> count the number of words in the motif 
         int wordCount = countWords(newReclamation.motif);

         if (wordCount >= 3 && wordCount <= 8)
         {
           break;
         } else if (wordCount < 3)
         {
            printf("Le motif est trop court. Il doit contenir au moins 3 mots. \n\tVeuillez reessayer...\n");
         } else {
            printf("Le motif est trop long. Il doit contenir un maximum de 8 mots. \n\tVeuillez reessayer...\n");
         }
         
    }

        // Description of reclamation
    printf(">> Entrez la description detaillee du probleme : ");
    fgets(newReclamation.description, sizeof(newReclamation.description), stdin);
    newReclamation.description[strcspn(newReclamation.description, "\n")] = 0;

        // Categoriy of reclamation (As a choice)
    printf(">> Entrez la categorie de la reclamation : \n");
    for (int i = 0; i < numCategories; i++)
    {
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
    newReclamation.statut = 0; // en attent
    strcpy(newReclamation.clientUsername, Users[userId].username);
    newReclamation.dateSubmission = time(NULL); 


    reclamations[nbrReclamations] = newReclamation; // Edit the Nombre of Reclamations After that
    nbrReclamations++;

    printf("| Reclamation soumise avec succes. \n| ID de la reclamation : %d\n", newReclamation.id);
}

        // >> See Reclamation 
void voirReclamations(int userId) {
    char dateStr[20];
    if (userId == -1) { //

        for (int i = 0; i < nbrReclamations; i++) {
            printf(" _______________________________________\n");
            printf("|\tTous Les Infos de votre Reclamation:\n");
            printf(" ->>\n");
            printf(" \t> Reclamation ID: %d\n\n", reclamations[i].id);
            printf(" \t> Motif: %s\n\n", reclamations[i].motif);
            printf(" \t> Description: %s\n", reclamations[i].description);
            printf(" \t> Categorie: %s\n", reclamations[i].categorie);
            printf(" \t> Statut: %d\n", reclamations[i].statut);

            printf(" \t> Client Username: %s\n", reclamations[i].clientUsername);

            strftime(dateStr, sizeof(dateStr), "%d/%m/%Y %H:%M:%S", localtime(&reclamations[i].dateSubmission));
            printf(" \t> Date Submission: %s\n", dateStr);
        }
    } else { // Display reclamations for the given user
        for (int i = 0; i < nbrReclamations; i++) {
            if (strcmp(reclamations[i].clientUsername, Users[userId].username) == 0) {
            printf("\t> Reclamation ID: %d\n", reclamations[i].id);
            printf("\t> Motif: %s\n", reclamations[i].motif);
            printf("\t> Description: %s\n", reclamations[i].description);
            printf("\t> Categorie: %s\n", reclamations[i].categorie);
            printf("\t> Statut: %d\n", reclamations[i].statut); 

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
void editReclamations() {
    int ReclamationID, newStatue;
    printf("Entrez l'ID de la reclamation a mettre a jour : ");
    scanf("%d", &ReclamationID);
    getchar();
    
    printf("Entrez le nouveau statut \n[0]: En attente \n[1]: En cours  \n[2]: Resolu \n");
    printf("\t[x] >>> ");
    scanf("%d", &newStatue);
    getchar();
    
    for (int i = 0; i < nbrReclamations; i++) {
        if (reclamations[i].id == ReclamationID) {
            reclamations[i].statut = newStatue;
            printf("Statut de la reclamation mis a jour avec succes.\n");
            return;
        }
    }
    printf("Reclamation non trouvee.\n");
}

        // >> Search for Reclamations
void searchReclamation() {}

        // >> See Reclamations by priority
void afficherRecByPriority() {}



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
        if (Users[userId].role == 1 || Users[userId].role == 2) {
            printf("[3]. Gerer les reclamations\n");
        }
        if (Users[userId].role == 2) {
            printf("[4]. Gerer les Users\n");
        }
        printf("[5]. Deconnexion\n");
        printf("\t[x] >>> ");
        
        char Choice01[10];
        fgets(Choice01, sizeof(Choice01), stdin);
        Choice01[strcspn(Choice01, "\n")] = '\0'; // Removing newLine
        choice = atoi(Choice01);

        switch(choice) {
            case 1:
                soumettreReclamation(userId);
                break;
            case 2:
                voirReclamations(userId);
                break;
            case 3:
                if (Users[userId].role == 1 || Users[userId].role == 2) {

                    editReclamation(userId);
                }
                break;
            case 4:
                if (Users[userId].role == 2) {

                    manageUsers();
                }
                break;
            case 5:
                printf("Deconnexion...\n");
                break;
            default:
                printf("choice non valide. Veuillez reessayer.\n");
        }
    } while (choice != 5);
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
        printf("[7]. Deconnexion\n");
        printf("\t[x] >>> ");

        char Choice01[10];
        fgets(Choice01, sizeof(Choice01), stdin);
        Choice01[strcspn(Choice01, "\n")] = '\0'; // Removing newLine
        choice = atoi(Choice01);

        switch(choice) {
            case 1:
                voirReclamations(-1);
                break;
            case 2:
                soumettreReclamation(userId);
                break;
            case 3:
                editReclamation(userId);
                break;
            case 4:
                searchReclamation();
                break;
            case 5:
                afficherRecByPriority();
                break;
            case 6:
                manageUsers();
                break;
            case 7:
                printf("Deconnexion...\n");
                break;
            default:
                printf("Choix invalide.\n");
        }
    } while (choice != 7);
}

        // >> Agent Menu
void agentMenu(int userId) {
    int choice;
    do {
        printf("\n|------- Menu Agent de Reclamation -------|\n");
        printf("[1]. Voir mes reclamations\n");
        printf("[2]. Rechercher une reclamation\n");
        printf("[3]. Modifier une reclamation\n");
        printf("[4]. Deconnexion\n");
        printf("\t[x] >>> ");

        char Choice01[10];
        fgets(Choice01, sizeof(Choice01), stdin);
        Choice01[strcspn(Choice01, "\n")] = '\0'; // Removing newLine
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
                printf("Deconnexion...\n");
                break;
                default:
                printf("choice invalide.\n");
        }
    } while (choice != 4);
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

                    if (Users[UserConnecte].role == 1 || Users[UserConnecte].role == 2) { 
                        editReclamations();
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
