#include <stdlib.h>
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS


#define CM_SUCCESS              (-3)
#define GAUSS_SUCCESS           (-8)
#define CM_ERR_LIGNE_VIDE       (-9)
#define CM_ERR_NON_NUMERIQUE    (-5)
#define CN_ERR_LIGNE_VIDE       (-9)


char** ChargeNoms(FILE *fd, int *n, int *code);
void MessageErreur(char* erreur, int code);
double **CreationMatriceAugmentee(int n);
double *CreationVecteurResultats(int n);
int ChargeMatrice(FILE *fd, double **m, int n);
int Gauss(double **m, int n);
void Solution(double **m, double *sol, int n);


int main(void)
{
    char *nomFichier = "Donnees.dat",**noms;
    FILE * fd;
    int i, n, code;
    double **m, *sol;

    if ((fd = fopen(nomFichier, "r")) == NULL)
    {
        printf("Erreur a l'ouverture du fichier %s\n", nomFichier);
        //_getch();
        exit(0);
    }

    if ((noms = ChargeNoms(fd, &n, &code)) == NULL)
    {
        fclose(fd);
        MessageErreur("Erreur au chargement des noms", code);
    }

    m = CreationMatriceAugmentee(n);

    sol = CreationVecteurResultats(n);

    if ((code = ChargeMatrice(fd, m, n)) != CM_SUCCESS)
    {
        fclose(fd);
        MessageErreur("Erreur au chargement de la matrice", code);
    }

    fclose(fd);

    Gauss(m, n);

//    if ((code = Gauss(m, n)) != GAUSS_SUCCESS)
//        MessageErreur("Erreur lors de la resolution: ", code);
//
//    Solution(m, sol, n);
//
//    for (i = 0; i<n; ++i)
//        printf("%s = %lf\n", noms[i], sol[i]);
//
//    //_getch();

    return 0;
}



char** ChargeNoms(FILE *fd, int *n, int *code) {
    int finLigne = 0;
    int PremiereFois = 1;
    char currentChar = '0';
    char** tableau;
    int i;
    int j;
    int nbrChars = 0;
    *n = 0;

    fseek(fd, 0, SEEK_SET);

    while (finLigne != 1) {			//Cette boucle compte le nombre de noms et le met dans n
        currentChar = fgetc(fd);

        if (PremiereFois == 1) {	//S'exÈcute seulement ‡ la premiËre itÈration du while
            if (currentChar == '\n' && currentChar == EOF) {	//VÈrifie si le char est un retour de ligne ou la fin du fichier
                *code = CN_ERR_LIGNE_VIDE;
                return(NULL);
            }
            PremiereFois = 0;
        }

        if (currentChar == '\n') {
            finLigne = 1;
            *n = *n +1;
        }
        else if (currentChar == ' ') {
            *n = *n +1;
        }
    }
    tableau = malloc(sizeof(char *)*(*n));				//Alloue l'espace pour le nombre de noms dans le tableau

    fseek(fd, 0, SEEK_SET);

    for (i = 0; i < *n; ++i) {		//Cette boucle compte le nombre de charactËres dans le nom, alloue l'espace dans cet emplacement du tableau
        nbrChars = 0;				//et copie le nom dans le tableau, pour chaque nom
        currentChar = 0;
        while (currentChar != ' ' && currentChar != '\n') {		//Compte le nombre de charatËres dans le nom
            currentChar = fgetc(fd);
            if (currentChar != ' ' && currentChar != '\n') {
                nbrChars++;
            }
        }

        tableau[i] = (char*)malloc(nbrChars * sizeof(char));	//Alloue l'espace pour le nouveau nom
        fseek(fd, -(nbrChars + 1), SEEK_CUR);					//Recule au dÈbut de ce nom
        if (currentChar == '\n') {
            fseek(fd, -1, SEEK_CUR);
        }


        for (j = 0; j < nbrChars; ++j) {
            currentChar = fgetc(fd);						//Ajoute le nom dans le tableau, un charactËre ‡ la fois
            tableau[i][j] = currentChar;
        }
        fgetc(fd);												//La boucle for s'est arrÍtÈ avant l'espace, cette ligne avance aprËs l'espace
    }

    return tableau;
}

int ChargeMatrice(FILE *fd, double **m, int n) {
    int ligneVide = 1;
    char currentChar = '0';
    int nbrCharLigne1 = 0;


    fseek(fd, 0, SEEK_SET);
    while (currentChar != '\n'){
        currentChar = fgetc(fd);
        nbrCharLigne1++;
    }
    fseek(fd, nbrCharLigne1, SEEK_SET);



    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n+1; ++j) {
            if (fscanf(fd, "%lf", &m[i][j]) == 0) {	//lis les donnÈes et vÈrifie si elle sont numÈriques
                return CM_ERR_NON_NUMERIQUE;
            }
        }
    }


    for (int k = 0; k < n; k++) {
        for (int l = 0; l < n; ++l) {	//on ne met pas n+1, car on ne veut pas savoir si il ya a quelquechose dans le cÙtÈ augmentÈ de la matrice
            if (m[k][l] != 0) {
                ligneVide = 0;
            }
        }
        if (ligneVide == 1) {
            return CM_ERR_LIGNE_VIDE;
        }
        ligneVide = 1;
    }


    return CM_SUCCESS;
}
double** CreationMatriceAugmentee(int n){

    double** m;

    m = malloc(n * sizeof(double *));

    for (int i = 0; i < n; ++i) {
        m[i] = malloc( (n+1) * sizeof(double));
    }
    
    return m;
}

double* CreationVecteurResultats(int n){

    double* sol;

    sol = malloc(n * sizeof(double));

    return sol;

}

void MessageErreur(char* erreur, int code){
    printf("Erreur : %d", code);
}


/**
 * Substrats two line numbers by a coefficient.
 *
 * ================================
 * lineNumber - coef * LineNumber2
 * ================================
 *
 * @param lineNumber [0 to ...]
 * @param lineNumber2 [0 to ...]
 * @param coef
 * @param m (The array)
 * @param lineLength (n)
 */
void lineMinusLine(int lineNumber, int lineNumber2, double coef, double** m, int lineLength){

    for (int i = 0; i < lineLength; ++i) {
        m[lineNumber][i] -= coef * m[lineNumber2][i];
    }
}

int Gauss(double **m, int n){

    int cursorV, cursorH;
    double coef;

    cursorV = 0;
    cursorH = 0;
    coef = 0.0;

    for (int i = 1; i < n; ++i) {
        coef = m[cursorV + i][cursorH] / m[cursorV][cursorH];
        lineMinusLine(cursorV + i, cursorV, coef, m, n);
    }

    return 0;
}
