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
const unsigned KBlanc   (37);

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
void afficheMatriceV2 (const CMatrice & Mat, unsigned & score, unsigned rowSelect = 99, unsigned colSelect = 99) {
    cout << " ";
    for (size_t x = 0; x <= Mat.size(); ++x){
        cout << x << " ";
    }
    cout << string(10, ' ') << "Score: " << score;
    cout << endl;
    for (size_t i = 0; i < Mat.size(); ++i){
        cout << setw(2) << i + 1 << " ";
        for(size_t j = 0; j < Mat[i].size(); ++j){
            switch(Mat[i][j]){
                case 1: couleur(KReset); break;
                case 2: couleur(KVert); break;
                case 3: couleur(KRouge); break;
                case 4: couleur(KJaune); break;
                case 5: couleur(KBleu); break;
                case 6: couleur(KMagenta); break;
                case 7: couleur(KNoir); break;
                default: couleur(KCyan); break;
            }
            if (i == rowSelect && j == colSelect && colSelect != 99 && rowSelect != 99) fond(KBlanc);
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
bool detectionExplositionUneBombeHorizontale (CMatrice & mat, unsigned & score){
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

            for (size_t k (j+1); k < mat[i].size() && mat[i][j] == mat[i][k]; ++k) ++combienALaSuite;
            if (combienALaSuite >= 3){
                auMoinsUneExplosion = true;
                score += combienALaSuite;
                /*cout << "on a une suite en position ligne = " << i+1
                     << "; colonne = " << j+1
                     << "; sur  " << combienALaSuite << " cases" << endl;
                cout << string (20, '-') << endl << "matrice avant suppression" << endl;
                afficheMatriceV2(mat);*/
                explosionUneBombeHorizontale (mat, i, j, combienALaSuite);
                //cout << string (20, '-') << endl << "matrice après suppression" << endl;
                //afficheMatriceV2(mat);

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
    for(unsigned k = 0; k < combien; ++k){
        mat[numLigne][numColonne+k] = KAIgnorer;
        for(unsigned j = numLigne; j > 0; --j){
            swap(mat[j][numColonne+k], mat[j-1][numColonne+k]);
        }
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


bool detectionExplositionUneBombeVerticale (CMatrice & mat){
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

            for (size_t k (j+1); k < mat[i].size () && mat[i][j] == mat[k][j]; ++k) ++combienALaSuite;
            if (combienALaSuite >= 3){
                auMoinsUneExplosion = true;
                /*cout << "on a une suite en position ligne = " << i+1
                     << "; colonne = " << j+1
                     << "; sur  " << combienALaSuite << " cases" << endl;
                cout << string (20, '-') << endl << "matrice avant suppression" << endl;
                afficheMatriceV2(mat);*/
                //explosionUneBombeVerticale (mat, i, j, combienALaSuite);
                //cout << string (20, '-') << endl << "matrice après suppression" << endl;
                //afficheMatriceV2(mat);

            }
        }
    }

    return auMoinsUneExplosion;
}


int ppalExo04 (){
    CMatrice mat;
    unsigned score (0);
    initMat(mat);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    detectionExplositionUneBombeHorizontale (mat, score);
    afficheMatriceV2 (mat, score);
    //condition de victoire a trouver
    while (true) {
        cout << "Fait un mouvement ";
        cout << "numero de ligne : ";
        size_t numLigne;
        cin >> numLigne;
        cout << numLigne;
        cout << "numero de colonne : ";
        size_t numCol;
        cin >> numCol;
        cout << numCol;
        afficheMatriceV2(mat, score, numLigne-1, numCol-1);
        cout << "Sens du deplacement : (A|Z|E|Q|D|W|X|C) : " << endl;
        char deplacement;
        cin >> deplacement;
        faitUnMouvement (mat, deplacement, numLigne-1, numCol-1);
        detectionExplositionUneBombeHorizontale (mat, score);
        afficheMatriceV2 (mat, score);
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
