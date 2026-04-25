#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//STRUCTURES DE DONNÉES

/* Structure pour un joueur */
typedef struct Joueur {
    int numero;             
    char nom[50];           
    int age;                
    int parties_gagnees;    
    int parties_perdues;    
    int victoires_consec;   
    int defaites_consec;    
    int score_cumul;        
    struct Joueur* suivant; 
} Joueur;

/* Structure pour une file */
typedef struct {
    Joueur* tete;   
    Joueur* queue;  
    int taille;     
} File;

/* Structure pour une partie */
typedef struct {
    int numero;             
    Joueur* joueur1;        
    Joueur* joueur2;        
    int score1, score2;     
    int strategie;          
} Partie;


//FONCTIONS POUR LES FILES

/* Initialiser une file */
void initFile(File* f) {
    f->tete = NULL;
    f->queue = NULL;
    f->taille = 0;
}

/* Vérifier si une file est vide */
int fileVide(File* f) {
    return f->tete == NULL;
}

/* Ajouter un joueur à la fin dune file */
void enfile(File* f, Joueur* j) {
    if (fileVide(f)) {
        f->tete = j;
        f->queue = j;
    } else {
        f->queue->suivant = j;
        f->queue = j;
    }
    j->suivant = NULL;
    f->taille++;
}

/* Retirer et retourner le premier joueur dune file */
Joueur* defile(File* f) {
    if (fileVide(f)) return NULL;
    
    Joueur* temp = f->tete;
    f->tete = f->tete->suivant;
    if (f->tete == NULL) f->queue = NULL;
    
    temp->suivant = NULL;
    f->taille--;
    return temp;
}

/* Voir le premier joueur*/
Joueur* teteFile(File* f) {
    return f->tete;
}


 //FONCTIONS POUR LES LISTES CHAINÉES (LG et LP)

/* Ajouter un joueur à la liste des perdants (LP) */
void ajouterLP(Joueur** liste, Joueur* j) {
    j->suivant = *liste;
    *liste = j;
}

/* Ajouter un joueur à la liste des gagnants (LG) trie par score */
void ajouterLGTrie(Joueur** liste, Joueur* j) {
    if (*liste == NULL || j->score_cumul > (*liste)->score_cumul) {
        j->suivant = *liste;
        *liste = j;
    } else {
        Joueur* courant = *liste;
        while (courant->suivant != NULL && 
               courant->suivant->score_cumul >= j->score_cumul) {
            courant = courant->suivant;
        }
        j->suivant = courant->suivant;
        courant->suivant = j;
    }
}


 //FONCTIONS DE CALCUL POUR LES STRATÉGIES

/* Calculer la somme des chiffres dun nombre */
int sommeChiffres(int n) {
    int somme = 0;
    n = abs(n);
    while (n > 0) {
        somme += n % 10;
        n /= 10;
    }
    return somme;
}

/* Verifier si un chiffre est présent dans un nombre */
int chiffreDansNombre(int chiffre, int nombre) {
    if (chiffre == 0 && nombre == 0) return 1;
    
    nombre = abs(nombre);
    while (nombre > 0) {
        if (nombre % 10 == chiffre) return 1;
        nombre /= 10;
    }
    return 0;
}

/* Calculer le PGCD de deux nombres */
int pgcd(int a, int b) {
    int temp;
    a = abs(a);
    b = abs(b);
    while (b != 0) {
        temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

/* Vérifier condition Partie I : somme chiffres multiple de 5 */
int conditionPartieI(int nombre) {
    return (sommeChiffres(nombre) % 5 == 0);
}

/* Vérifier condition Partie II : chiffre PGCD dans nombres */
int conditionPartieII(int n1, int n2) {
    int p = pgcd(n1, n2);
    
    // Extraire chaque chiffre du PGCD
    int temp = abs(p);
    if (temp == 0) return 0;
    
    while (temp > 0) {
        int chiffre = temp % 10;
        if (chiffreDansNombre(chiffre, n1) || 
            chiffreDansNombre(chiffre, n2)) {
            return 1;
        }
        temp /= 10;
    }
    return 0;
}


 //FONCTIONS DE GESTION DES JOUEURS

/* Créer un nouveau joueur */
Joueur* creerJoueur(int num, char* nom, int age) {
    Joueur* j = (Joueur*)malloc(sizeof(Joueur));
    j->numero = num;
    strcpy(j->nom, nom);
    j->age = age;
    j->parties_gagnees = 0;
    j->parties_perdues = 0;
    j->victoires_consec = 0;
    j->defaites_consec = 0;
    j->score_cumul = 0;
    j->suivant = NULL;
    return j;
}

/* Mettre à jour les stats d'un joueur après une partie */
void majStats(Joueur* j, int aGagne, int score) {
    if (aGagne) {
        j->parties_gagnees++;
        j->victoires_consec++;
        j->defaites_consec = 0;
        j->score_cumul += score;
    } else {
        j->parties_perdues++;
        j->defaites_consec++;
        j->victoires_consec = 0;
    }
}

/* Afficher les infos d'un joueur */
void afficherJoueur(Joueur* j) {
    if (j == NULL) return;
    printf("  %2d. %-15s (âge: %2d) - G:%2d P:%2d\n", 
           j->numero, j->nom, j->age, 
           j->parties_gagnees, j->parties_perdues);
}


 //FONCTIONS DE SÉLECTION DES JOUEURS
 
/* Sélectionner 2 joueurs selon les regles de priorité */
int selectionnerJoueurs(File* F, File* F1, File* F3, 
                         Joueur** j1, Joueur** j2) {
    if (F1->taille >= 2) {
        *j1 = defile(F1);
        *j2 = defile(F1);
        return 1;
    }
    else if (F1->taille == 1 && F->taille >= 1) {
        *j1 = defile(F1);
        *j2 = defile(F);
        return 1;
    }
    else if (F->taille >= 2) {
        *j1 = defile(F);
        *j2 = defile(F);
        return 1;
    }
    else if (F->taille == 1 && F3->taille >= 1) {
        *j1 = defile(F);
        *j2 = defile(F3);
        return 1;
    }
    else if (F3->taille >= 2) {
        *j1 = defile(F3);
        *j2 = defile(F3);
        return 1;
    }
    else {
        *j1 = *j2 = NULL;
        return 0;
    }
}


 //FONCTION POUR JOUER UNE PARTIE

/* Jouer une partie */
Partie* jouerPartie(Joueur* j1, Joueur* j2, int numPartie, int strategie) {
    Partie* p = (Partie*)malloc(sizeof(Partie));
    p->numero = numPartie;
    p->joueur1 = j1;
    p->joueur2 = j2;
    p->score1 = p->score2 = 0;
    p->strategie = strategie;
    
    int maxTours = (strategie == 1) ? 12 : 16;
    int tour = 0;
    
    printf("\n=== PARTIE %d ===\n", numPartie);
    printf("Joueurs: %s vs %s\n", j1->nom, j2->nom);
    printf("Strategie: %s\n", (strategie == 1) ? "Somme chiffres" : "PGCD");
    
    while (tour < maxTours && abs(p->score1 - p->score2) < 3) {
        tour++;
        
        // Tour du joueur 1
        if (strategie == 1) {
            int val = rand() % 1000000;
            if (conditionPartieI(val)) p->score1++;
        } else {
            int val1 = rand() % 1000;
            int val2 = rand() % 1000;
            if (conditionPartieII(val1, val2)) p->score1++;
        }
        
        // Tour du joueur 2
        if (strategie == 1) {
            int val = rand() % 1000000;
            if (conditionPartieI(val)) p->score2++;
        } else {
            int val1 = rand() % 1000;
            int val2 = rand() % 1000;
            if (conditionPartieII(val1, val2)) p->score2++;
        }
        
        printf(" Tour %2d: %s=%d  %s=%d\n", 
               tour, j1->nom, p->score1, j2->nom, p->score2);
    }
    
    return p;
}

 //FONCTIONS D'AFFICHAGE 

/* Afficher letat des files et listes */
void afficherEtat(File* F, File* F1, File* F3, Joueur* LG, Joueur* LP) {
    printf("\n--- ETAT ACTUEL ---\n");
    
    printf("File F (%d joueurs):\n", F->taille);
    Joueur* curr = F->tete;
    while (curr) { afficherJoueur(curr); curr = curr->suivant; }
    
    printf("File F1 (%d joueurs):\n", F1->taille);
    curr = F1->tete;
    while (curr) { afficherJoueur(curr); curr = curr->suivant; }
    
    printf("File F3 (%d joueurs):\n", F3->taille);
    curr = F3->tete;
    while (curr) { afficherJoueur(curr); curr = curr->suivant; }
    
    printf("Liste Gagnants LG:\n");
    curr = LG;
    while (curr) { afficherJoueur(curr); curr = curr->suivant; }
    
    printf("Liste Perdants LP:\n");
    curr = LP;
    while (curr) { afficherJoueur(curr); curr = curr->suivant; }
}

/* Afficher les 3 premiers gagnants */
void afficherTroisPremiers(Joueur* LG) {
    printf("\n=== LES 3 PREMIERS GAGNANTS ===\n");
    if (LG == NULL) {
        printf("Aucun gagnant\n");
        return;
    }
    
    Joueur* courant = LG;
    for (int i = 1; i <= 3 && courant != NULL; i++) {
        printf("%d. %s - Score cumule: %d (G:%d P:%d)\n", 
               i, courant->nom, courant->score_cumul,
               courant->parties_gagnees, courant->parties_perdues);
        courant = courant->suivant;
    }
}
    
 //FONCTIONS POUR LES STATISTIQUES

/* Compter les joueurs avec N victoires */
void joueursAvecVictoires(Joueur* liste, int n) {
    printf("Joueurs avec %d victoire(s): ", n);
    Joueur* curr = liste;
    int compte = 0;
    
    while (curr) {
        if (curr->parties_gagnees == n) {
            if (compte > 0) printf(", ");
            printf("%s", curr->nom);
            compte++;
        }
        curr = curr->suivant;
    }
    
    if (compte == 0) printf("Aucun");
    printf("\n");
}

/* Compter les joueurs avec N défaites */
void joueursAvecDefaites(Joueur* liste, int n) {
    printf("Joueurs avec %d defaite(s): ", n);
    Joueur* curr = liste;
    int compte = 0;
    
    while (curr) {
        if (curr->parties_perdues == n) {
            if (compte > 0) printf(", ");
            printf("%s", curr->nom);
            compte++;
        }
        curr = curr->suivant;
    }
    
    if (compte == 0) printf("Aucun");
    printf("\n");
}

 //FONCTION PRINCIPALE

int main() {
    // Initialisation des files et listes
    File F, F1, F3;
    initFile(&F);
    initFile(&F1);
    initFile(&F3);
    
    Joueur* LG = NULL; // Liste gagnants
    Joueur* LP = NULL; // Liste perdants
    
    // Création automatique de joueurs (pour tests)
    printf("================== DEBUT DU JEU ==================\n");
    printf("Initialisation des joueurs...\n");
    
    // Liste de noms pour création automatique
    char* noms[] = {"Ali", "mehfoudi", "Omar", "mohamed", "Karim", 
                   "moh", "Youssef", "wassim", "Hakim", "chabbi"};
    int nbJoueurs = 8;  // On crée 8 joueurs
    
    for (int i = 0; i < nbJoueurs; i++) {
        Joueur* j = creerJoueur(i+1, noms[i], 18 + rand() % 15);
        enfile(&F, j);
        printf("Joueur cree: %s (num:%d, age:%d)\n", j->nom, j->numero, j->age);
    }
    
    int partieCount = 0;
    int strategie = 1; // Commencer avec stratégie 1
    int maxPartiesStrat1 = 3 * nbJoueurs; // 3n parties
    
    // Boucle principale du jeu
    while ((!fileVide(&F) || !fileVide(&F1) || !fileVide(&F3))) {
        partieCount++;
        
        // Changement de stratégie après 3n parties
        if (strategie == 1 && partieCount > maxPartiesStrat1) {
            printf("\n>>> CHANGEMENT DE STRATEGIE (Partie II) <<<\n");
            printf(">>> Passage a la strategie PGCD <<<\n");
            strategie = 2;
        }
        
        // Sélection des joueurs
        Joueur* j1 = NULL;
        Joueur* j2 = NULL;
        
        if (!selectionnerJoueurs(&F, &F1, &F3, &j1, &j2)) {
            // Cas particulier : joueurs seuls restants
            if (!fileVide(&F1)) {
                Joueur* seul = defile(&F1);
                printf("\nJoueur seul dans F1: %s va dans LG\n", seul->nom);
                ajouterLGTrie(&LG, seul);
            } 
            else if (!fileVide(&F3)) {
                Joueur* seul = defile(&F3);
                printf("\nDernier joueur dans F3: %s va dans LP\n", seul->nom);
                ajouterLP(&LP, seul);
            }
            break;
        }
        
        // Jouer la partie
        Partie* partie = jouerPartie(j1, j2, partieCount, strategie);
        
        // Déterminer gagnant et perdant
        Joueur* gagnant = NULL;
        Joueur* perdant = NULL;
        int egalite = 0;
        
        if (partie->score1 > partie->score2) {
            gagnant = j1;
            perdant = j2;
        } 
        else if (partie->score2 > partie->score1) {
            gagnant = j2;
            perdant = j1;
        } 
        else {
            egalite = 1;
        }
        
        // Traiter le résultat
        if (egalite) {
            printf("Resultat: EGALITE %d-%d\n", partie->score1, partie->score2);
            // Remettre les deux en queue de F
            enfile(&F, j1);
            enfile(&F, j2);
        } 
        else {
            printf("Resultat: %s gagne %d-%d\n", 
                   gagnant->nom, 
                   (gagnant == j1) ? partie->score1 : partie->score2,
                   (perdant == j1) ? partie->score1 : partie->score2);
            
            // Mettre à jour les stats
            majStats(gagnant, 1, (gagnant == j1) ? partie->score1 : partie->score2);
            majStats(perdant, 0, (perdant == j1) ? partie->score1 : partie->score2);
            
            // Appliquer les règles selon la stratégie
            if (strategie == 1) {
                // Règles Partie I
                enfile(&F, perdant); // Perdant en queue de F
                
                // Vérifier conditions spéciales Partie I
                if (gagnant->victoires_consec >= 3) {
                    printf("-> %s a 3 victoires consecutives -> F1\n", gagnant->nom);
                    enfile(&F1, gagnant);
                }
                else if (gagnant->parties_gagnees >= 5) {
                    printf("-> %s a 5 victoires -> LG\n", gagnant->nom);
                    ajouterLGTrie(&LG, gagnant);
                }
                else {
                    enfile(&F, gagnant); // Gagnant rejoue
                }
                
                if (perdant->defaites_consec >= 3) {
                    printf("-> %s a 3 defaites consecutives -> F3\n", perdant->nom);
                    enfile(&F3, perdant);
                }
                else if (perdant->parties_perdues >= 5) {
                    printf("-> %s a 5 defaites -> LP\n", perdant->nom);
                    ajouterLP(&LP, perdant);
                }
            } 
            else {
                // Règles Partie II
                printf("-> %s va dans F1 (gagnant)\n", gagnant->nom);
                printf("-> %s va dans F3 (perdant)\n", perdant->nom);
                enfile(&F1, gagnant);
                enfile(&F3, perdant);
                
                // Conditions élimination Partie II
                if (gagnant->victoires_consec >= 2) {
                    printf("-> %s a 2 victoires consecutives -> LG\n", gagnant->nom);
                    ajouterLGTrie(&LG, gagnant);
                }
                if (perdant->parties_perdues >= 2) {
                    printf("-> %s a 2 defaites -> LP\n", perdant->nom);
                    ajouterLP(&LP, perdant);
                }
            }
        }
        
        // Afficher état actuel
        afficherEtat(&F, &F1, &F3, LG, LP);
        
        // Libérer la mémoire de la partie
        free(partie);
        
        // Pause pour lire les résultats
        printf("\n--- Appuyez sur Entree pour continuer ---");
        getchar();
        
        // Arrêt conditionnel (tous joueurs dans LG ou LP)
        if (F.taille == 0 && F1.taille == 0 && F3.taille == 0) {
            printf("\nToutes les files sont vides. Fin du jeu.\n");
            break;
        }
    }
    
    // Fin du jeu - affichage final
    printf("\n================== FIN DU JEU ==================\n");
    printf("Nombre total de parties jouees: %d\n", partieCount);
    
    // Afficher les 3 premiers gagnants
    afficherTroisPremiers(LG);
    
    // Afficher les statistiques demandées
    printf("\n=== STATISTIQUES STRATEGIE 1 ===\n");
    joueursAvecVictoires(LG, 1);
    joueursAvecVictoires(LG, 2);
    joueursAvecVictoires(LG, 3);
    
    joueursAvecDefaites(LP, 1);
    joueursAvecDefaites(LP, 2);
    joueursAvecDefaites(LP, 3);
    
    printf("\n=== STATISTIQUES STRATEGIE 2 ===\n");
    joueursAvecVictoires(LG, 1);
    joueursAvecVictoires(LG, 2);
    
    joueursAvecDefaites(LP, 1);
    joueursAvecDefaites(LP, 2);
    
    // Joueurs sans victoires
    printf("\n=== JOUEURS SANS VICTOIRE ===\n");
    Joueur* curr = LP;
    int aucun = 1;
    while (curr) {
        if (curr->parties_gagnees == 0) {
            printf("- %s\n", curr->nom);
            aucun = 0;
        }
        curr = curr->suivant;
    }
    if (aucun) printf("Aucun joueur sans victoire\n");
    
    return 0;
}