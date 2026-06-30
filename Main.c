#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* ── Couleurs Médicales (Vert et Blanc) ── */
#define VERT   "\033[32m"
#define BLANC  "\033[37m"
#define GRAS   "\033[1m"
#define ROUGE  "\033[31m"
#define RESET  "\033[0m"

#define MAX_PATIENTS 100

/* ── Structures de données ── */
typedef struct {
    int id_patient;          /* Automatique (Chiffres uniquement) */
    char nom[50];            /* Alphabet Français, Anglais, Arabe */
    char prenom[50];
    int age;                 /* Chiffres uniquement */
    char sexe[10];           /* Masculin / Féminin uniquement */
    char service[25];        /* Urgence, Médecine, Chirurgie, Maternité, Pédiatrie, Ophtalmologie, Cardiologie */
    int num_salle;
    int num_lit;
    char status[10];         /* Admis / Sortie */
    char diagnostique[200];    /* Uniquement alphabet français */
    int id_medecin_assigne;  /* Automatique */
} Patient;

typedef struct {
    int id_medecin;
    char nom[50];
    char specialite[50];
} Medecin;

/* ── Base de données fictive en mémoire ── */
Patient listePatients[MAX_PATIENTS];
int nbPatients = 0;
int prochainIdPatient = 1001;
int prochainIdConsultation = 7001;

Medecin medecins[] = {
    {10, "Dr. Mahamat", "Urgentiste"},
    {20, "Dr. Fatime", "Pediatre"},
    {30, "Dr. ISSA", "Chirurgien"},
    {40, "Dr. MAHAMAT NOUR", "Ophtalmologue"},
    {50, "Dr. AHMAT", "Cardiologue"},
    {60, "Dr. Youssouf", "Generaliste"}
};
int nbMedecins = 6;

/* ── Fonctions d'outils et de nettoyage ── */
void effacer() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void banniere() {
    printf(VERT GRAS);
    printf("==================================================\n");
    printf("         HOPITAL PROVINCIAL D'ABECHE\n");
    printf("==================================================\n" RESET);
}

void vider_tampon() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Vérification stricte de l'alphabet français pour l'historique */
int est_alphabet_francais(const char *str) {
    if (strlen(str) == 0) return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha((unsigned char)str[i]) && str[i] != ' ' && str[i] != '\'' && str[i] != '-') {
            return 0;
        }
    }
    return 1;
}

/* ── Fonctions Principales du Programme ── */

// 1. ENREGISTRER UN NOUVEAU PATIENT (Passage obligé par les Urgences)
void enregistrer_patient() {
    if (nbPatients >= MAX_PATIENTS) {
        printf(ROUGE "Erreur d'enregistrement : Hôpital complet.\n" RESET);
        return;
    }

    Patient p;
    p.id_patient = prochainIdPatient++;
    int valide = 0;

    printf(VERT "\n--- ENREGISTREMENT ADMISSION ---\n" RESET);
    printf(BLANC "Le patient passe d'abord par les Urgences.\n" RESET);

    // Saisie du Nom
    do {
        printf(BLANC "Nom du Patient : " RESET);
        fgets(p.nom, sizeof(p.nom), stdin);
        p.nom[strcspn(p.nom, "\n")] = 0;
        if (strlen(p.nom) > 0) valide = 1;
        else printf(ROUGE "Erreur d'enregistrement : Le nom ne peut pas être vide. Modifiez votre erreur.\n" RESET);
    } while (!valide);

    // Saisie du Prénom
    valide = 0;
    do {
        printf(BLANC "Prenom du Patient : " RESET);
        fgets(p.prenom, sizeof(p.prenom), stdin);
        p.prenom[strcspn(p.prenom, "\n")] = 0;
        if (strlen(p.prenom) > 0) valide = 1;
        else printf(ROUGE "Erreur d'enregistrement : Le prenom ne peut pas être vide. Modifiez votre erreur.\n" RESET);
    } while (!valide);

    // Saisie de l'Âge
    valide = 0;
    do {
        printf(BLANC "Âge du Patient : " RESET);
        if (scanf("%d", &p.age) == 1 && p.age >= 0) {
            valide = 1;
        } else {
            printf(ROUGE "Erreur d'enregistrement : Âge invalide. Entrez des chiffres. Modifiez votre erreur.\n" RESET);
            vider_tampon();
        }
    } while (!valide);
    vider_tampon();

    // Saisie du Sexe
    int choix_sexe;
    valide = 0;
    do {
        printf(BLANC "Sexe du Patient :\n" RESET);
        printf("1. Masculin\n2. Feminin\nVotre choix : ");
        if (scanf("%d", &choix_sexe) == 1 && (choix_sexe == 1 || choix_sexe == 2)) {
            if (choix_sexe == 1) strcpy(p.sexe, "Masculin");
            else strcpy(p.sexe, "Feminin");
            valide = 1;
        } else {
            printf(ROUGE "Erreur d'enregistrement : Option incorrecte (1 ou 2 uniquement). Modifiez votre erreur.\n" RESET);
            vider_tampon();
        }
    } while (!valide);
    vider_tampon();

    // Historique Médical (Alphabet Français uniquement)
    valide = 0;
    do {
        printf(BLANC "diagnostique  : " RESET);
        fgets(p.diagnostique, sizeof(p.diagnostique), stdin);
        p.diagnostique[strcspn(p.diagnostique, "\n")] = 0;
        if (est_alphabet_francais(p.diagnostique)) {
            valide = 1;
        } else {
            printf(ROUGE "Erreur d'enregistrement : Caractères non autorisés. Utilisez uniquement l'alphabet français. Modifiez votre erreur.\n" RESET);
        }
    } while (!valide);

    /* --- RÈGLE STRICTE : Tout patient commence aux Urgences --- */
    strcpy(p.service, "Urgence");
    p.num_salle = 1;               // Urgence possède une seule salle
    p.num_lit = (rand() % 20) + 1; // Cette salle a plusieurs lits
    strcpy(p.status, "Admis");

    // Choix du médecin (non automatique)
    printf(VERT "\nListe des médecins disponibles :\n" RESET);
    for (int i = 0; i < nbMedecins; i++) {
        printf("ID: %d | %s - Specialiter: %s\n", medecins[i].id_medecin, medecins[i].nom, medecins[i].specialite);
    }
    int id_medecin_choisi;
    int medecin_trouve = 0;
    do {
        printf(BLANC "Entrez l'ID du medecin qui prendra en charge le patient : " RESET);
        if (scanf("%d", &id_medecin_choisi) == 1) {
            for (int i = 0; i < nbMedecins; i++) {
                if (medecins[i].id_medecin == id_medecin_choisi) {
                    p.id_medecin_assigne = id_medecin_choisi;
                    medecin_trouve = 1;
                    break;
                }
            }
            if (!medecin_trouve) {
                printf(ROUGE "ID medecin invalide. Veuillez reessayer.\n" RESET);
            }
        } else {
            printf(ROUGE "Saisie invalide.\n" RESET);
            vider_tampon();
        }
    } while (!medecin_trouve);
    vider_tampon();

    listePatients[nbPatients] = p;
    nbPatients++;

    // Génération automatique des données de la consultation
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf(VERT "\n==================================================\n");
    printf("   PATIENT ENREGISTRE AUX URGENCES AVEC SUCCES !\n");
    printf("==================================================\n" RESET);
    printf("ID Patient               : %d\n", p.id_patient);
    printf("Service d'admission      : %s (Salle %d, Lit %d)\n", p.service, p.num_salle, p.num_lit);
    printf("ID Consultation          : %d\n", prochainIdConsultation++);
    // Récupération du nom du médecin choisi
    char nom_medecin[50];
    for (int i = 0; i < nbMedecins; i++) {
        if (medecins[i].id_medecin == p.id_medecin_assigne) {
            strcpy(nom_medecin, medecins[i].nom);
            break;
        }
    }
    printf("Médecin d'Urgence consulant : %s\n", nom_medecin);
    printf("Date de l'admission       : %02d/%02d/%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    printf("Heure de l'admission      : %02d:%02d\n", tm.tm_hour, tm.tm_min);
    printf("==================================================\n");
}

// 2. AFFICHER LA LISTE DES PATIENTS HOSPITALISÉS
void afficher_hospitalises() {
    int compteur = 0;
    printf(VERT "\n--- LISTE DES PATIENTS HOSPITALISER HPA ---\n\n" RESET);

    for (int i = 0; i < nbPatients; i++) {
        if (strcmp(listePatients[i].status, "Admis") == 0) {
            printf("ID : %d | %s %s | %d ans | Sexe : %s\n",
                   listePatients[i].id_patient, listePatients[i].nom, listePatients[i].prenom, listePatients[i].age, listePatients[i].sexe);
            printf("   -> Service : %s | Emplacement : Salle N° %d, Lit N° %d\n",
                   listePatients[i].service, listePatients[i].num_salle, listePatients[i].num_lit);
            printf("   -> Diagnostique : %s\n", listePatients[i].diagnostique);
            printf("------------------------------------------------------\n");
            compteur++;
        }
    }
    if (compteur == 0) printf("Aucun patient n'est actuellement hospitaliser.\n");
}

// 3. RECHERCHER UN PATIENT PAR SON NOM OU SON IDENTIFIANT
void rechercher_patient() {
    char recherche[50];
    int trouve = 0;

    printf("\nEntrez le nom ou l'identifiant automatique du patient : ");
    fgets(recherche, sizeof(recherche), stdin);
    recherche[strcspn(recherche, "\n")] = 0;

    printf(VERT "\n--- RESULTAT DE LA RECHERCHE ---\n\n" RESET);
    for (int i = 0; i < nbPatients; i++) {
        char id_str[20];
        sprintf(id_str, "%d", listePatients[i].id_patient);

        if (strcmp(listePatients[i].nom, recherche) == 0 || strcmp(id_str, recherche) == 0) {
            printf("Patient Trouver !\n");
            printf("ID Unique : %d | Statut : %s\n", listePatients[i].id_patient, listePatients[i].status);
            printf("Nom complet : %s %s | Âge : %d ans | Sexe : %s\n", listePatients[i].nom, listePatients[i].prenom, listePatients[i].age, listePatients[i].sexe);
            printf("Service actuel : %s\n", listePatients[i].service);
            printf("Position : Salle %d, Lit %d\n", listePatients[i].num_salle, listePatients[i].num_lit);
            printf("diagnostique : %s\n", listePatients[i].diagnostique);
            printf("-----------------------------------\n");
            trouve = 1;
        }
    }
    if (!trouve) printf(ROUGE "Aucun patient ne correspond à votre recherche.\n" RESET);
}

// 4. LIBÉRER (SUPPRIMER) UN PATIENT APRÈS SA SORTIE
void liberer_patient() {
    int id_recherche, trouve = -1;

    printf("\nEntrez l'identifiant du patient à liberer : ");
    if (scanf("%d", &id_recherche) != 1) {
        printf(ROUGE "Saisie incorrecte.\n" RESET);
        vider_tampon();
        return;
    }
    vider_tampon();

    for (int i = 0; i < nbPatients; i++) {
        if (listePatients[i].id_patient == id_recherche) {
            trouve = i;
            break;
        }
    }

    if (trouve != -1) {
        // Changement de statut d'abord
        strcpy(listePatients[trouve].status, "Sortie");

        // Suppression effective du tableau (décalage de mémoire)
        for (int i = trouve; i < nbPatients - 1; i++) {
            listePatients[i] = listePatients[i + 1];
        }
        nbPatients--;
        printf(VERT "Le patient a ete declarer 'Sortie', liberer de son lit et supprimer du systeme HPA.\n" RESET);
    } else {
        printf(ROUGE "Erreur : Identifiant introuvable.\n" RESET);
    }
}

// 5. MODIFIER PATIENT (Changer Maladie, Médecin ou ÉVACUER depuis les Urgences)
void modifier_patient() {
    int id_recherche, trouve = -1;

    printf("\nEntrez l'identifiant du patient a mettre a jour \n: ");
    if (scanf("%d", &id_recherche) != 1) {
        printf(ROUGE "Identifiant invalide.\n" RESET);
        vider_tampon();
        return;
    }
    vider_tampon();

    for (int i = 0; i < nbPatients; i++) {
        if (listePatients[i].id_patient == id_recherche) {
            trouve = i;
            break;
        }
    }

    if (trouve != -1) {
        int choix;
        printf(VERT "\n--- OPTIONS DE MODIFICATION (PATIENT ID: %d) ---\n" RESET, id_recherche);
        printf("1. Modifier la maladie \n");
        printf("2. Changer de medecin referent\n");
        printf("3. Evacuer le patient des Urgences vers un service specialiser\n");
        printf("Votre choix : ");
        scanf("%d", &choix);
        vider_tampon();

        if (choix == 1) {
            int valide = 0;
            do {
                printf("Nouvel consultation : ");
                fgets(listePatients[trouve].diagnostique, sizeof(listePatients[trouve].diagnostique), stdin);
                listePatients[trouve].diagnostique[strcspn(listePatients[trouve].diagnostique, "\n")] = 0;
                if (est_alphabet_francais(listePatients[trouve].diagnostique)) valide = 1;
                else printf(ROUGE "Erreur : Langue non respectee. Modifiez votre erreur.\n" RESET);
            } while (!valide);
            printf(VERT "Mise à jour de la maladie effectuee.\n" RESET);
        }
        else if (choix == 2) {
            printf("\nListe des medecins de l'HPA :\n");
            for (int j = 0; j < nbMedecins; j++) {
                printf("ID: %d | %s (Specialiter : %s)\n", medecins[j].id_medecin, medecins[j].nom, medecins[j].specialite);
            }
            int nouvel_id, medecin_valide = 0;
            printf("Entrez l'ID du medecin à assigner : ");
            scanf("%d", &nouvel_id);
            vider_tampon();

            for (int j = 0; j < nbMedecins; j++) {
                if (medecins[j].id_medecin == nouvel_id) {
                    listePatients[trouve].id_medecin_assigne = nouvel_id;
                    medecin_valide = 1;
                    break;
                }
            }
            if (medecin_valide) printf(VERT "Medecin assigner au patient avec succes.\n" RESET);
            else printf(ROUGE "Erreur : Medecin introuvable.\n" RESET);
        }
        else if (choix == 3) {
            // Logique complète d'Évacuation conditionnelle
            int choix_serv;
            int transfert_ok = 0;

            printf(VERT "\n--- SELECTION DU SERVICE D'EVACUATION ---\n" RESET);
            printf("1. Medecine \n");
            printf("2. Pediatrie \n");
            printf("3. Chirurgie \n");
            printf("4. Maternite \n");
            printf("5. Ophtalmologie \n");
            printf("6. Cardiologie \n");
            printf("Votre choix : ");
            scanf("%d", &choix_serv);
            vider_tampon();

            switch(choix_serv) {
                case 1: // Médecine
                    if (listePatients[trouve].age >= 18) {
                        strcpy(listePatients[trouve].service, "Medecine");
                        listePatients[trouve].num_salle = (rand() % 10) + 1;
                        listePatients[trouve].num_lit = (rand() % 6) + 1;
                        transfert_ok = 1;
                    } else printf(ROUGE "Transfert refuser : Le service Medecine exige un âge >= 18 ans.\n" RESET);
                    break;
                case 2: // Pédiatrie
                    if (listePatients[trouve].age < 18) {
                        strcpy(listePatients[trouve].service, "Pediatrie");
                        listePatients[trouve].num_salle = (rand() % 5) + 1;
                        listePatients[trouve].num_lit = (rand() % 8) + 1;
                        transfert_ok = 1;
                    } else printf(ROUGE "Transfert refuser : La pediatrie est reservee aux patients < 18 ans.\n" RESET);
                    break;
                case 3: // Chirurgie
                    strcpy(listePatients[trouve].service, "Chirurgie");
                    listePatients[trouve].num_salle = (rand() % 12) + 1;
                    listePatients[trouve].num_lit = (rand() % 4) + 1;
                    transfert_ok = 1;
                    break;
                case 4: // Maternité
                    if (strcmp(listePatients[trouve].sexe, "Feminin") == 0 && listePatients[trouve].age >= 18) {
                        strcpy(listePatients[trouve].service, "Maternite");
                        listePatients[trouve].num_salle = (rand() % 15) + 1;
                        listePatients[trouve].num_lit = (rand() % 5) + 1;
                        transfert_ok = 1;
                    } else printf(ROUGE "Transfert refusé : La Maternite exige un patient Feminin et un âge >= 18 ans.\n" RESET);
                    break;
                case 5: // Ophtalmologie
                    strcpy(listePatients[trouve].service, "Ophtalmologie");
                    listePatients[trouve].num_salle = (rand() % 6) + 1;   // Salle avec lit
                    listePatients[trouve].num_lit = (rand() % 4) + 1;
                    transfert_ok = 1;
                    break;
                case 6: // Cardiologie
                    strcpy(listePatients[trouve].service, "Cardiologie");
                    listePatients[trouve].num_salle = (rand() % 8) + 1;   // Salle avec lit
                    listePatients[trouve].num_lit = (rand() % 5) + 1;
                    transfert_ok = 1;
                    break;
                default:
                    printf(ROUGE "Option de service inconnue.\n" RESET);
            }

            if (transfert_ok) {
                printf(VERT "Patient evacuer avec succes vers le service %s.\n" RESET, listePatients[trouve].service);
            }
        }
    } else {
        printf(ROUGE "Patient introuvable.\n" RESET);
    }
}

/* ── Fonction Principale du Système HPA ── */
int main() {
    srand(time(NULL));
    int choix = 0;

    do {
        effacer();
        banniere();
        printf(BLANC "1. Enregistrer un nouveau Patient\n" RESET);
        printf(BLANC "2. Afficher la liste des Patients hospitaliser\n" RESET);
        printf(BLANC "3. Rechercher un Patient\n" RESET);
        printf(BLANC "4. Liberer un Patient apres sa sortie\n" RESET);
        printf(BLANC "5. changer les donner d'un patient\n" RESET);
        printf(BLANC "6. Quitter l'application HPA\n" RESET);
        printf(VERT "\nVotre choix : " RESET);

        if (scanf("%d", &choix) != 1) {
            choix = 0;
        }
        vider_tampon();

        switch (choix) {
            case 1: effacer(); banniere(); enregistrer_patient(); break;
            case 2: effacer(); banniere(); afficher_hospitalises(); break;
            case 3: effacer(); banniere(); rechercher_patient(); break;
            case 4: effacer(); banniere(); liberer_patient(); break;
            case 5: effacer(); banniere(); modifier_patient(); break;
            case 6: printf(VERT "\n BONNE GUERISON !\n" RESET); break;
            default: printf(ROUGE "Choix invalide. Veuillez sélectionner une option de la liste.\n" RESET); break;
        }

        if (choix != 6) {
            printf("\nAppuyez sur Entrée pour revenir au menu principal...");
            getchar();
        }
    } while (choix != 6);

    return 0;
}
