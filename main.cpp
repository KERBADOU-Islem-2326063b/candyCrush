#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
using namespace std;

//***********************************************************************************/
//***********************    R1.01 – Prog#10 Exercice 1   ***************************/
//***********************************************************************************/


void clearScreen () {
    cout << "\033[H\033[2J";
}

const unsigned KReset   (0);
const unsigned KNoir    (30);
const unsigned KRouge   (31);
const unsigned KVert    (32);
const unsigned KJaune   (33);
const unsigned KBleu    (34);
const unsigned KMagenta (35);
const unsigned KCyan    (36);
const unsigned KBGNoir    (40);
const unsigned KBGRouge   (41);
const unsigned KGBBleu    (44);
const unsigned KBGCyan    (46);

void couleur (const unsigned & coul) {
    cout << "\033[" << coul << "m";
}

void fond (const unsigned & coul) {
    cout << "\033[" << coul + 10 << "m";
}


//***********************************************************************************/
//***********************    R1.01 – Prog#10 Exercice 2   ***************************/
//***********************************************************************************/
/*
 *
*/

typedef ushort contenueDUneCase;
typedef vector <contenueDUneCase> CVLigne; // un type représentant une ligne de la grille
typedef vector <CVLigne> CMatrice; // un type représentant la grille

const contenueDUneCase KAIgnorer = 0;
const contenueDUneCase KPlusGrandNombreDansLaMatrice = 4;

//initialisation de la grille de jeu
void initMat (CMatrice & mat, const size_t & nbLignes = 10,
             const size_t & nbColonnes = 10,
              const unsigned & nbMax= KPlusGrandNombreDansLaMatrice){

    mat.resize(nbLignes);
    for (size_t i = 0; i < nbLignes; ++i){
        mat[i].resize(nbColonnes);
        for(size_t j = 0; j < nbColonnes; ++j){
            mat[i][j] = rand() % nbMax + 1;
        }

    }
}

// affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
void  afficheMatriceV0 (const CMatrice & Mat) {
    for (size_t i = 0; i < Mat.size(); ++i){
        for(size_t j = 0; j < Mat[i].size(); ++j){
            switch(Mat[i][j]){
                case 1: couleur(KReset); break;
                case 2: couleur(KNoir); break;
                case 3: couleur(KRouge); break;
                case 4: couleur(KJaune); break;
                case 5: couleur(KBleu); break;
                case 6: couleur(KMagenta); break;
                case 7: couleur(KVert); break;
                default: couleur(KCyan); break;
            }
            cout << Mat[i][j] << " ";
            couleur(KReset);
        }
        cout << endl;
    }
}
// affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche, mais avec un fond de couleur
//pour signifier que la case est a KAIgnorer
void  afficheMatriceV1 (const CMatrice & Mat) {
    for (size_t i = 0; i < Mat.size(); ++i){
        for(size_t j = 0; j < Mat[i].size(); ++j){
            switch(Mat[i][j]){
                case 1: couleur(KReset); break;
                case 2: couleur(KNoir); break;
                case 3: couleur(KRouge); break;
                case 4: couleur(KJaune); break;
                case 5: couleur(KBleu); break;
                case 6: couleur(KMagenta); break;
                case 7: couleur(KVert); break;
                default: couleur(KCyan); break;
            }
            if (Mat[i][j] == KAIgnorer) fond(KRouge);
            cout << Mat[i][j] << " ";
            couleur(KReset);
        }
        cout << endl;
    }
}


// affichage de la matrice avec les numéros de lignes / colonnes en haut / à gauche et avec un fond de couleur
//pour signifier que la case est a KAIgnorer
void  afficheMatriceV2 (const CMatrice & Mat) {
    for (size_t x = 0; x < Mat.size(); ++x){
        cout << x << " ";
    }
    cout << endl;
    for (size_t i = 0; i < Mat.size(); ++i){
        cout << i + 1 << " ";
        for(size_t j = 0; j < Mat[i].size(); ++j){
            switch(Mat[i][j]){
                case 1: couleur(KReset); break;
                case 2: couleur(KNoir); break;
                case 3: couleur(KRouge); break;
                case 4: couleur(KJaune); break;
                case 5: couleur(KBleu); break;
                case 6: couleur(KMagenta); break;
                case 7: couleur(KVert); break;
                default: couleur(KCyan); break;
            }
            if (Mat[i][j] == KAIgnorer) fond(KRouge);
            cout << Mat[i][j] << " ";
            couleur(KReset);
        }
        cout << endl;
    }
}



//***********************************************************************************/
//***********************    R1.01 – Prog#10 Exercice 2   ***************************/
//***********************************************************************************/


// on remplira cela plus tard, juste la définition de la fonction
void explosionUneBombeHorizontale (CMatrice & mat, const size_t & numLigne,
                                    const size_t & numColonne, const size_t & combien);

//
bool detectionExplositionUneBombeHorizontale (CMatrice & mat){
    bool auMoinsUneExplosion (false);

    //on parcours la matrice case / case
    // si on tombe sur la valeur KAIgnorer, on passe a la case suivante
    // sinon on compte combien de fois on a la même valeur
    size_t combienALaSuite (1);
    //si on a aun moins 3 chiffres identiques a la suite

    for(size_t i = 0; i < mat.size(); ++i){
        for(size_t j = 0; j < mat[i].size(); ++j){
            if (mat[i][j] == KAIgnorer) continue;
            combienALaSuite = 1;

            for (size_t k (j+1); k < mat[i].size () && mat[i][j] == mat[i][k]; ++k) ++combienALaSuite;
            if (combienALaSuite >= 3){
                auMoinsUneExplosion = true;
                cout << "on a une suite en position ligne = " << i+1
                     << "; colonne = " << j+1
                     << "; sur  " << combienALaSuite << " cases" << endl;
                cout << string (20, '-') << endl << "matrice avant suppression" << endl;
                afficheMatriceV2(mat);
                explosionUneBombeHorizontale (mat, i, j, combienALaSuite);
                cout << string (20, '-') << endl << "matrice après suppresion" << endl;
                afficheMatriceV2(mat);

            }
        }
    }

    return auMoinsUneExplosion;
}

//***********************************************************************************/
//***********************    R1.01 – Prog#10 Exercice 3   ***************************/
//***********************************************************************************/


//fait descendre toutes les cases d'une unité suite à une explosition
void explosionUneBombeHorizontale (CMatrice & mat, const size_t & numLigne,
                                    const size_t & numColonne, const size_t & combien){
    for(unsigned j = 0; j < combien; ++j){
        mat[numLigne][j] = KAIgnorer;
    }


}

void faitUnMouvement (CMatrice & mat, const char & deplacement, const size_t & numLigne,
                     const size_t & numCol) {

    size_t nvPosLigne (numLigne), nvPosCol (numCol);
    switch (tolower(deplacement)) {
    case 'a':
        --nvPosLigne;
        --nvPosCol;
        break;
    case 'z':
        --nvPosLigne;
        break;
    case 'e':
        --nvPosLigne;
        ++nvPosCol;
        break;
    case 'q':
        --nvPosCol;
        break;
    case 'd':
        ++nvPosCol;
        break;
    case 'w':
        ++nvPosLigne;
        --nvPosCol;
        break;
    case 'x':
        ++nvPosLigne;
        break;
    case 'c':
        ++nvPosLigne;
        ++nvPosCol;
        break;
    default:
        cout<<"A Z E Q D W X C ?"<<endl;
        break;
    }
    swap(mat[numLigne][numCol], mat[nvPosLigne][nvPosCol]);
}


int ppalExo01 (){
    CMatrice mat;
    initMat(mat);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    afficheMatriceV0 (mat);
    //detectionExplositionUneBombeHorizontale (mat);
    return 0;
}

int ppalExo02 (){
    CMatrice mat;
    initMat(mat);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    afficheMatriceV1 (mat);
    detectionExplositionUneBombeHorizontale (mat);
    afficheMatriceV1 (mat);
    return 0;
}

int ppalExo03 (){
    CMatrice mat;
    initMat(mat);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    afficheMatriceV1 (mat);
    while (detectionExplositionUneBombeHorizontale (mat)) {
        mat[5][8] = 4;
        afficheMatriceV1 (mat);
    }
    return 0;
}

int ppalExo04 (){
    CMatrice mat;
    initMat(mat);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    detectionExplositionUneBombeHorizontale (mat);
    afficheMatriceV2 (mat);
    //condition de victoire a trouver
    while (true) {
        cout << "Fait un mouvement ";
        cout << "numero de ligne : ";
        size_t numLigne;
        cout << numLigne;
        cout << "numero de colonne : ";
        size_t numCol;
        cout << numCol;
        cout << "Sens du deplacement : (A|Z|E|Q|D|W|X|C) : " << endl;
        char deplacement;
        cin >> deplacement;
        faitUnMouvement (mat, deplacement, numLigne, numCol);
        detectionExplositionUneBombeHorizontale (mat);
        afficheMatriceV2 (mat);
    }
    return 0;
}


int main() {

    // ---------Exercice 2 -----------------//
    //    clearScreen();

    //    CMatrix mat (10, CVLine (10, kEmpty));
    //    mat [0][mat.size()-1] = kTokenPlayer1;
    //    mat [mat.size()-1][0] = kTokenPlayer2;
    //    showMatrix(mat);
    //-------------------------------------//
    //return 0;

    // ---------Exercice 2 -----------------//
    //return ppalExo02();
    //-------------------------------------//

    // ---------Exercice 3 -----------------//
    //return ppalExo03();
    //-------------------------------------//

    // ---------Exercice 3 -----------------//
    return ppalExo04();
    //-------------------------------------//
}
