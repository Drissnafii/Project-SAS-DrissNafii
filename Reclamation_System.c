#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>


#define MAX_USERS 100
#define MAX_LENGTH 50

    // La Structure Qui stock les info d'utilisateur

typedef struct {
    char username[50];
    char password[50];
    int role; // 0: Client, 1:Agent, 3:Admin
    int LoginAttempts;
    bool Locked;
    
} User;

User users[MAX_USERS];
int userCount = 0;

    // Password Validator

int  isValide(const char* nom, const char* password) {
    int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSpecial = 0;
    int length = strlen(password);

    // Password must contain at least 8 characters
    if (length < 8) {
        return 0;
    }
    
    // Must not contain the username
    if (strstr(password, nom) != NULL) {
        return 0;
    }

    // Check for required character types
    for (int i = 0; i < length; i++) {
        if (isupper(password[i])) {
            hasUpper = 1;
        }
        if (islower(password[i])) {
            hasLower = 1;
        }
        if (isdigit(password[i])) {
            hasDigit = 1;
        }
        if (strchr("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", password[i])) {
            hasSpecial = 1;
        }
    }

    // Return 1 if all conditions are true

    return hasUpper && hasLower && hasDigit && hasSpecial;
}

    // Sign Up 

void signUp() {
    if (userCount >= MAX_USERS) {
        printf("Nombre maximum d#utilisateurs atteints.\n");
        return;
    }

    User newUser;
    printf("Entrer Le Nom d#utilisateur: ");
    scanf("%s", newUser.username);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, newUser.username) == 0) {
            printf("Le nom d#utilisateur existe deja. Veuillez en choisir un autre.\n");
            return;
        }
    }


    char password[MAX_LENGTH]; 

    do {
        printf("Entrer Le mot de passe : ");
        scanf("%s", &password);
        
        
        if (isValide(password, newUser.username)) {
            printf("Password does not meet requirements. Try again.\n");
        }
    } while (isValide(password, newUser.username));

    strcpy(newUser.password, password);

    printf("Entrer Ton Role >>>>> (0: Client, 1: Agent, 2: Admin): ");
    scanf("%d", &newUser.role);

    newUser.LoginAttempts = 0;
    newUser.Locked = false;

    users[userCount++] = newUser;
    printf("User registered successfully.\n");
}


    // Sign In

int signIn() {

    char username[MAX_LENGTH], password[MAX_LENGTH];
    printf("Entrer Ton Nom : ");
    scanf("%s", username);
    printf("Entrer Le mot de passe : ");
    scanf("%s", password);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            if (users[i].Locked) {
                printf("Le compte est bloque. Veuillez contacter un administrateur.\n");
                return -1;
            }
            if (strcmp(users[i].password, password) == 0) {
                users[i].LoginAttempts = 0;
                printf("Connexion reussie. Bienvenue, %s!\n", username);
                return i;
            } else {
                users[i].LoginAttempts++;
                if (users[i].LoginAttempts >= 3) {
                    users[i].Locked = true;
                    printf("Trop de tentatives. Compte bloque !\n");
                } else {
                    printf("Mot de passe incorrect. Tentatives restantes : %d\n", 3 - users[i].LoginAttempts);
                }
                return -1;
            }
        }
    }
    printf("Utilisateur non trouve.\n");
    return -1;
}

    // Cette Fonction et pour Afficher le Menu 

void displayMainMenu() {
    printf("\n|___________\n");
    printf("\n| Main Menu:\n");
    printf(" [1] Sign Up\n");
    printf(" [2] Sign In\n");
    printf(" [3] Exit\n");
    printf("Entrer Ton Choix : ");
}

    // The Main Function

int main() {
    
    int choice, year;
    int loggedInUser = -1;

        // Password for the entire software!

    printf("\n\n\t ------------------------------------------------------------------------------------------\n");
    printf("\t| >> Welcome to the Customer Complaint Management System.                                        \n");
    printf("\t|    To check if you use the righ Version of our Programm\n\t Enter the year [xxxx]: ");
    scanf("%d", &year);
    
    if ( year == 2024 ) {
        do {
            displayMainMenu();

            int choice;
            scanf("%d", &choice);

            switch(choice) {
                case 1:
                    signUp();
                    break;
                case 2:
                    loggedInUser = signIn();
                    if (loggedInUser != -1) {
                        printf("Entree Comme %s Avec un role %d\n", 
                               users[loggedInUser].username, users[loggedInUser].role);
                    }
                    break;
                case 3:
                    printf("Quitter le programme. Au revoir !\n");
                    exit(0);
                default:
                    printf("Choix non valide. Veuillez reessayer.\n");
            }
        } while (1); // Changed to an infinite loop, you can exit with case 3
    } else {
        printf("\n\n\t     :/ Désolé, cette version du programme n'est pas compatible avec l'année saisie. \n\n\t     Veuillez essayer une autre version.\n\n");
    }
   
    return 0;
}
