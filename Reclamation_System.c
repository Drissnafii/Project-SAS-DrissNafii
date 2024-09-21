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
    char motif[250];
    char description[250];
    char categorie[50];
    int statut; // En attente / En cours / Resolu
    char clientUsername[50];    
    int priority; // basse / moyenne / haute priority
} Reclamation;

// Function prototypes

void inscrire();       //
int seConnecter();    // >> User Management Functions
void manageUsers();  //

void entrerReclamation(int userId);                  //
void voirReclamations(int userId);                  //
void editReclamation(int userId);                  // >> Reclamations Management Functions
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

        // >> Enter Reclamations 
void entrerReclamation(int userId) {
    if (nbrReclamations >= MAX_RECLAMATIONS) {
        printf("Nombre maximum de reclamations atteint.\n");
        return;
    }

    Reclamation newReclamation;
    newReclamation.id = nbrReclamations + 1;
    printf("Entrez la description de votre reclamation : ");
    fgets(newReclamation.description, sizeof(newReclamation.description), stdin);
    newReclamation.description[strcspn(newReclamation.description, "\n")] = '\0';
    
    newReclamation.statut = 0;  
    strcpy(newReclamation.clientUsername, Users[userId].username);

    reclamations[nbrReclamations++] = newReclamation;
    printf("Reclamation soumise avec succes. ID de la reclamation : %d\n", newReclamation.id);
}

        // >> See Reclamations
void voirReclamations(int userId) {
    printf("Affichage des reclamations pour l'utilisateur %d\n", userId);
    // Implement the logic to display reclamations here
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
    printf(" [4] Afficher les Reclamations (New !)\n");
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
        choice = atoi(Choice01);

        switch(choice) {
            case 1:
                entrerReclamation(userId);
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
        printf("[1]. Afficher toutes les reclamations\n");
        printf("[2]. Modifier ou supprimer une reclamation\n");
        printf("[3]. Rechercher une reclamation\n");
        printf("[4]. Afficher les reclamations par priorite\n");
        printf("[5]. Gerer les Users\n");
        printf("[6]. Deconnexion\n");
        printf("\t[x] >>> ");

        char Choice01[10];
        fgets(Choice01, sizeof(Choice01), stdin);
        choice = atoi(Choice01);

        switch(choice) {
            case 1:
                voirReclamations(userId);
                break;
            case 2:
                editReclamation(userId);
                break;
            case 3:
                searchReclamation();
                break;
            case 4:
                afficherRecByPriority();
                break;
            case 5:
                manageUsers();
                break;
            case 6:
                printf("Deconnexion...\n");
                break;
            default:
                printf("choice invalide.\n");
        }
    } while (choice != 6);
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

                    printf("Fermeture du programme. Au revoir !\n");
                    exit(0);
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
