/**
 * @author Flavio Oderzo, Islem Kerbadou, Felix Fromentin, Hugo Gougeon
 *
 **/

#define FPS_LIMIT 60

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <fstream>

#include <yaml-cpp/yaml.h>

#include "MinGL2_IUT_AIX/include/mingl/mingl.h"
#include "MinGL2_IUT_AIX/include/mingl/gui/text.h"
#include "MinGL2_IUT_AIX/include/mingl/shape/line.h"
#include "MinGL2_IUT_AIX/include/mingl/shape/triangle.h"
#include "MinGL2_IUT_AIX/include/mingl/shape/rectangle.h"
#include "MinGL2_IUT_AIX/include/mingl/shape/circle.h"

using namespace std;

// Initialisation des variables utilisées
typedef unsigned short contenueDUneCase;
typedef vector <contenueDUneCase> CVLigne; // un type représentant une ligne de la grille
typedef vector <CVLigne> CMatrice; // un type représentant la grille


unsigned tailleNvEditeur (0);
unsigned editeurCellules (0);

unsigned boardSize;
unsigned squareType (1);
unsigned cellSize (50);
unsigned gapSize (5);
unsigned totalTailleCell (cellSize + gapSize);
unsigned boardTopLeftX;
unsigned boardTopLeftY;

unsigned level (0);
unsigned clique (0);
unsigned premierClicCol (0);
unsigned premierClicLigne (0);
unsigned clicCol (0);
unsigned clicLigne (0);
unsigned essai (10);
unsigned inEditeur (1);

float animationProgres (0);
float explosionTime (0);

bool mouse_clicked = false;
bool initMats = false;
bool fullscreen = false;
bool inAnimation = false;
bool inExplosion = false;
bool isSwap = false;
bool filled = true;
bool swapAllowed = true;

unsigned score (0);
unsigned scoreMinimum (1);

CMatrice mat;

const unsigned KReset   (0);
const unsigned KNoir    (30);
const unsigned KRouge   (31);
const unsigned KVert    (32);
const unsigned KJaune   (33);
const unsigned KBleu    (34);
const unsigned KMagenta (35);
const unsigned KCyan    (36);
const unsigned KBlanc   (37);

// initialisation de la configuration du fichier yaml
YAML::Node params = YAML::LoadFile("config.yaml");

const contenueDUneCase KAIgnorer = params["KAIgnorer"].as<unsigned>();
const contenueDUneCase KPlusGrandNombreDansLaMatrice = params["KPlusGrandNombre"].as<unsigned>();
const unsigned nvAleatoireTaille = params["TailleNiveauAleatoire"].as<unsigned>();

nsGraphics::Vec2D triPos;
nsGraphics::RGBAcolor triColor = nsGraphics::KWhite;

void clearScreen () {
    cout << "\033[H\033[2J";
}

void couleur (const unsigned & coul) {
    cout << "\033[" << coul << "m";
}

void fond (const unsigned & coul) {
    cout << "\033[" << coul + 10 << "m";
}


/*
 * @brief Obsolète, affiche dans la console la matrice du jeu.
 */
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

/*
 * @brief Obsolète, affiche dans la console la matrice du jeu avec un fond de couleur sur les cases vides.
 */
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

/*
 * @brief Obsolète, affiche dans la console la matrice du jeu avec les numéros de lignes et colonnes, et avec un fond de couleur sur les cases vides.
 */
void afficheMatriceV2 (const CMatrice & Mat, const unsigned & score = 0,
                      const unsigned & rowSelect = 99, const unsigned & colSelect = 99) {
    cout << "   | ";
    for (size_t x = 0; x <= Mat.size(); ++x){
        if (x == 0) continue;
        cout << x << " | ";
    }
    if (score > 0) cout << string(10, ' ') << "Score: " << score;
    cout << endl;
    for (size_t i = 0; i < Mat.size(); ++i){
        cout << setw(2) << i + 1 << " | ";
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
            cout << Mat[i][j];
            couleur(KReset);
            cout << " | ";
        }
        cout << endl;
    }
}


/*
 * @brief Retire des carrés alignés à la verticale et fait descendre ceux d'au dessus.
 */
void explosionUneBombeHorizontale (CMatrice & mat, const size_t & numLigne,
                                  const size_t & numColonne, const size_t & combien){

    for(unsigned k = 0; k < combien; ++k){
        mat[numLigne][numColonne+k] = KAIgnorer;
        for(unsigned j = numLigne; j > 0; --j){
            swap(mat[j][numColonne+k], mat[j-1][numColonne+k]);

        }
    }

}

/*
 * @brief Détecte les carrés alignés à l'horizontale.
 */
bool detectionExplosionUneBombeHorizontale (CMatrice & mat, unsigned & score){
    bool auMoinsUneExplosion (false);

    //on parcours la matrice case / case
    // si on tombe sur la valeur KAIgnorer, on passe a la case suivante
    // sinon on compte combien de fois on a la même valeur

    if (explosionTime < 50) { // Temps avant l'explosion des carrés
        if (isSwap) {
            glutSetCursor(GLUT_CURSOR_WAIT);
            swapAllowed = false;
        }
        explosionTime += 0.5;
    }
    else {
        swapAllowed = true;
        explosionTime = 0;
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
                    explosionUneBombeHorizontale (mat, i, j, combienALaSuite);
                } else if (isSwap){
                    --essai;
                    isSwap = false;
                }
            }
        }
    }
    return auMoinsUneExplosion;
}


/*
 * @brief Retire des carrés alignés à la verticale et fait descendre ceux d'au dessus.
 */
void explosionUneBombeVerticale (CMatrice & mat, const size_t & numLigne,
                                const size_t & numColonne, const size_t & combien){
    for(unsigned k = 0; k < combien; ++k){
        mat[numLigne+k][numColonne] = KAIgnorer;
        for(unsigned j = numLigne; j > 0; --j){
            swap(mat[j+k][numColonne], mat[j-1+k][numColonne]);

        }
    }

}

/*
 * @brief Détecte les explosions à la verticale.
 */
bool detectionExplosionUneBombeVerticale (CMatrice & mat, unsigned & score){
    bool auMoinsUneExplosion (false);

    //on parcours la matrice case / case
    // si on tombe sur la valeur KAIgnorer, on passe a la case suivante
    // sinon on compte combien de fois on a la même valeur

    if (explosionTime < 50) { // Temps avant l'explosion des carrés
        if (isSwap) {
            glutSetCursor(GLUT_CURSOR_WAIT);
            swapAllowed = false;
        }
        explosionTime += 0.5;
    }
    else {
        explosionTime = 0;
        size_t combienALaSuite (1);
        //si on a aun moins 3 chiffres identiques a la suite
        for(size_t i = 0; i < mat.size(); ++i){
            for(size_t j = 0; j < mat[i].size(); ++j){
                if (mat[i][j] == KAIgnorer) continue;
                combienALaSuite = 1;
                for (size_t k (i+1); k < mat[i].size () && mat[i][j] == mat[k][j]; ++k) ++combienALaSuite;
                if (combienALaSuite >= 3){
                    auMoinsUneExplosion = true;
                    score += combienALaSuite;
                    explosionUneBombeVerticale (mat, i, j, combienALaSuite);
                } else if (isSwap){
                    --essai;
                    isSwap = false;
                }
            }
        }
    }
    return auMoinsUneExplosion;
}

/*
 * @brief Obsolète, importe un niveau par le terminal à partir d'une entrée du nom d'un fichier texte.
 */
void importNv(CMatrice & mat, const string & nomNv){
    ifstream fichNv;
    fichNv.open(nomNv);

    unsigned nb, haut, larg,
        comptHaut=0, comptLarg=0;
    fichNv >> haut;
    fichNv >> larg;

    mat.resize(haut);
    for(unsigned i = 0; i < haut; ++i){
        mat[i].resize(larg);
    }

    while(fichNv >> nb){
        if(comptLarg >= larg){
            comptLarg = 0;
            ++comptHaut;
        }

        mat[comptHaut][comptLarg] = nb;

        ++comptLarg;
    }

    fichNv.close();
}

/*
 * @brief Sauvegarde l'état actuel de la matrice dans un fichier texte nommé.
 */
void saveNv (CMatrice & mat, const string & nomNv){
    ofstream fichNv;
    unsigned haut, larg;
    fichNv.open(nomNv + ".txt");

    haut = mat.size();
    larg = mat[0].size();

    fichNv << haut << " " << larg << endl;

    for(size_t i = 0; i < mat.size(); ++i){
        for(size_t j = 0; j < mat[i].size(); ++j){
            fichNv << mat[i][j] << " ";
        }
        fichNv << endl;
    }

    fichNv.close();
}

/*
 * @brief Obsolète, editeur de niveau via le terminal.
 */
void editNv (CMatrice & mat){
    string nomNv;

    unsigned haut, larg, nb;

    cout << "Bienvenue dans l'éditeur de niveaux" << endl
         << "Nommez votre magnifique niveau :" << endl;
    cin >> nomNv;
    cout << "Nombre de colonnes ?" << endl;
    cin >> larg;
    cout << "Nombre de lignes ?" << endl;
    cin >> haut;

    mat.resize(larg);
    for (size_t i = 0; i < larg; ++i){
        mat[i].resize(haut);
    }

    for (size_t i = 0; i < larg; ++i){
        for(size_t j = 0; j < haut; ++j){
            cout << endl;
            cout << "Quel nombre placer dans la case surlignée ?" << endl;
            cin >> nb;
            mat[i][j] = nb;
        }
    }
    char choix;
    cout << endl << "Voici votre magnifique niveau, voulez-vous le sauvegarder? (y/n)" << endl;
    cin >> choix;
    if (choix == 'y') saveNv(mat, nomNv);
};



void initMat(CMatrice &mat, const unsigned & niveau, const unsigned &nbMax = KPlusGrandNombreDansLaMatrice,
             const unsigned & tailleNiveauPerso = 5) {
    string lvlName;
    size_t randomNumber;
    clique = 0;
    switch (niveau)
    {
    case 1:
        importNv(mat, "1.txt");
        break;
    case 2:
        importNv(mat, "2.txt");
        break;
    case 3:
        importNv(mat, "3.txt");
        break;
    case 4:
        importNv(mat, "4.txt");
        break;
    case 5:
        importNv(mat, "5.txt");
        break;
    case 6: // niveau aléatoire
    {
        mat.resize(nvAleatoireTaille);
        for (unsigned i = 0; i < nvAleatoireTaille; ++i) {
            mat[i].resize(nvAleatoireTaille);
            for (unsigned j = 0; j < nvAleatoireTaille; ++j) {
                while (true) {
                    randomNumber = rand() % nbMax + 1;
                    if ((j >= 2 && randomNumber == mat[i][j - 1] && randomNumber == mat[i][j - 2]) ||
                        (i >= 2 && randomNumber == mat[i - 1][j] && randomNumber == mat[i - 2][j])) continue;
                    break;
                } mat[i][j] = randomNumber;
            }
        }
        saveNv(mat, "7");
        break;
    }
    case 7: // dans le cas où on est dans l'éditeur de niveaux
    {
        mat.resize(tailleNiveauPerso);
        for (unsigned i = 0; i < tailleNiveauPerso; ++i) {
            mat[i].resize(tailleNiveauPerso);
            for (unsigned j = 0; j < tailleNiveauPerso; ++j) {
                mat[i][j] = KAIgnorer;
            }
        }
        saveNv(mat, "7");
        break;
    }
    default:
        cout << "Mauvais choix" << endl;
        break;
    }
}

void editeurNiveau(MinGL &window, const unsigned & x = 0, const unsigned & y = 0, const bool & isClick = false){
    // On récupère la taille de la fenêtre
    nsGraphics::Vec2D windowSize;
    windowSize = window.getWindowSize();
    unsigned wx = (windowSize.getX() - 640)/2;
    unsigned wy = (windowSize.getY() - 640)/4;

    if (inEditeur == 2){
        // On affiche les choix disponibles
        window << nsGui::Text(nsGraphics::Vec2D(330+wx, 230+wy), "Nombre de ligne(s) et colonne(s) : " + to_string(tailleNvEditeur), nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 260+wy), "5   6   7   8   9   10", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 330+wy), "Nombre de cellule(s) differente(s) : " + to_string(editeurCellules), nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 360+wy), "2   3   4   5   6", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 430+wy), "Nombre de score(s) necessaire(s) : " + to_string(scoreMinimum), nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 460+wy), "+1    +2    +5   +10", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 530+wy), "Nombre d'essai(s) : " + to_string(essai), nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 560+wy), "+1    +2    +5", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

        if (y >= 250+wy && y <= 260+wy){
            if (x >= 222+wx and x <= 230+wx){
                if (isClick) tailleNvEditeur = 5;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 259+wx && x <= 267+wx){
                if (isClick) tailleNvEditeur = 6;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 296+wx && x <= 303+wx){
                if (isClick) tailleNvEditeur = 7;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 331+wx && x <= 339+wx){
                if (isClick) tailleNvEditeur = 8;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 367+wx && x <= 376+wx){
                if (isClick) tailleNvEditeur = 9;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 403+wx && x <= 420+wx){
                if (isClick) tailleNvEditeur = 10;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
        }
        if (y >= 352+wy && y <= 362+wy){
            if (x >= 246+wx and x <= 253+wx){
                if (isClick) editeurCellules = 2;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 282+wx && x <= 290+wx){
                if (isClick) editeurCellules = 3;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 317+wx && x <= 327+wx){
                if (isClick) editeurCellules = 4;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 353+wx && x <= 363+wx){
                if (isClick) editeurCellules = 5;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 390+wx && x <= 398+wx){
                if (isClick) editeurCellules = 6;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
        }
        if (y >= 450+wy && y <= 462+wy){
            if (x >= 232+wx and x <= 249+wx){
                if (isClick) ++scoreMinimum;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 286+wx && x <= 304+wx){
                if (isClick) scoreMinimum += 2;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 341+wx && x <= 357+wx){
                if (isClick) scoreMinimum += 5;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 385+wx && x <= 412+wx){
                if (isClick) scoreMinimum += 10;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
        }
        if (y >= 550+wy && y <= 562+wy){
            if (x >= 259+wx and x <= 276+wx){
                if (isClick) ++essai;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 314+wx && x <= 330+wx){
                if (isClick) essai += 2;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
            if (x >= 367+wx && x <= 384+wx){
                if (isClick) essai += 5;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
        }
    }
}


void faitUnMouvement (CMatrice & mat) {
    swap(mat[premierClicCol][premierClicLigne], mat[clicCol][clicLigne]);
    isSwap = true;
    clique = 0;
}

void menu(MinGL &window, const int & wx, const int & wy){
    window << nsGui::Text(nsGraphics::Vec2D(20, 20), "Fait par : KERBADOU Islem, ODERZO Flavio", nsGraphics::KWhite);
    window << nsGui::Text(nsGraphics::Vec2D(20, 40), "FROMENTIN Felix, GOUGEON Hugo", nsGraphics::KWhite);
    window << nsGui::Text(nsGraphics::Vec2D(320+wx, 160+wy), "Cube Crusher", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                          nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

    window << nsGui::Text(nsGraphics::Vec2D(330+wx, 200+wy), "Choix du niveau : ", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                          nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

    window << nsGui::Text(nsGraphics::Vec2D(320+wx, 230+wy), "Niveau 1", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                          nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

    window << nsGui::Text(nsGraphics::Vec2D(320+wx, 260+wy), "Niveau 2", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                          nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

    window << nsGui::Text(nsGraphics::Vec2D(320+wx, 290+wy), "Niveau 3", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                          nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

    window << nsGui::Text(nsGraphics::Vec2D(320+wx, 320+wy), "Niveau 4", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                          nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

    window << nsGui::Text(nsGraphics::Vec2D(320+wx, 350+wy), "Niveau 5", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                          nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

    window << nsGui::Text(nsGraphics::Vec2D(320+wx, 380+wy), "Niveau aleatoire", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                          nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

    window << nsGui::Text(nsGraphics::Vec2D(320+wx, 410+wy), "Editeur de niveau", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                          nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

}

void dessineBoard(MinGL &window, const unsigned & board = 5, const unsigned & wx = 640, const unsigned & wy = 640){
    // On initialise les variables néscessaire pour faire le tableau
    boardSize = board;
    boardTopLeftX = 320 + wx - (board * totalTailleCell) / 2;
    boardTopLeftY = 200 + wy;
    bool isHorizontalSwap = false;
    bool isVerticalSwap = false;
    bool isDiagonalSwap = false;

    // On detecte et explose, on verifie egalement que l'on soit pas dans l'editeur de niveau
    if (inEditeur != 3) detectionExplosionUneBombeVerticale(mat, score);
    if (inEditeur != 3) detectionExplosionUneBombeHorizontale(mat, score);


    // On dessigne les lignes
    for (unsigned i = 0; i <= boardSize; ++i) {
        unsigned lineCoord = boardTopLeftX + i * totalTailleCell;

        // On dessine les lignes vertival
        window << nsShape::Line(nsGraphics::Vec2D(boardTopLeftX, boardTopLeftY + i * totalTailleCell),
                                nsGraphics::Vec2D(boardTopLeftX + boardSize * totalTailleCell, boardTopLeftY + i * totalTailleCell),
                                nsGraphics::KWhite);

        // On dessine les lignes horizontal
        window << nsShape::Line(nsGraphics::Vec2D(lineCoord, boardTopLeftY),
                                nsGraphics::Vec2D(lineCoord, boardTopLeftY + boardSize * totalTailleCell),
                                nsGraphics::KWhite);


        // On regarde quel type d'animation faire
        if (clique == 2){
            if ((premierClicLigne == clicLigne - 1 || premierClicLigne == clicLigne + 1) && premierClicCol == clicCol) isHorizontalSwap = true;

            else if ((premierClicCol == clicCol - 1 || premierClicCol == clicCol + 1) && premierClicLigne == clicLigne) isVerticalSwap = true;

            else if ((premierClicLigne == clicLigne - 1 || premierClicLigne == clicLigne + 1) && !(premierClicCol == clicCol)) isDiagonalSwap = true;
        }



        // On dessine les cellules
        for (unsigned row = 0; row < boardSize; ++row) {
            for (unsigned col = 0; col < boardSize; ++col) {
                unsigned lineX = boardTopLeftX + col * totalTailleCell + gapSize;
                unsigned lineY = boardTopLeftY + row * totalTailleCell + gapSize;
                // Ajout d'ajustements en fonction du type de swap (horizontal, vertical et diagonales)

                if (swapAllowed && isHorizontalSwap && ((row == premierClicCol && col == premierClicLigne) || (row == clicCol && col == clicLigne))) {
                    animationProgres += 0.2;
                    inAnimation = true;
                    if (animationProgres < 100.05) {
                        if (row == premierClicCol && col == premierClicLigne) lineX += totalTailleCell * (clicLigne - premierClicLigne) * animationProgres / 100;
                        if (row == clicCol && col == clicLigne) lineX -= totalTailleCell * (clicLigne - premierClicLigne) * animationProgres / 100;
                    } else {
                        if (clique == 2) faitUnMouvement(mat);
                        animationProgres = 0;
                        clicLigne = 0;
                        premierClicLigne = 5;
                        inAnimation = false;
                        isHorizontalSwap = false;
                    }
                }

                if (swapAllowed && isVerticalSwap && ((row == premierClicCol && col == premierClicLigne) || (row == clicCol && col == clicLigne))) {
                    animationProgres += 0.2;
                    inAnimation = true;
                    if (animationProgres < 100.05) {
                        if (row == premierClicCol && col == premierClicLigne) lineY += totalTailleCell * (clicCol - premierClicCol) * animationProgres / 100;
                        if (row == clicCol && col == clicLigne) lineY -= totalTailleCell * (clicCol - premierClicCol) * animationProgres / 100;
                    } else {
                        if (clique == 2) faitUnMouvement(mat);
                        explosionTime = 0.5;
                        animationProgres = 0;
                        clicLigne = 0;
                        premierClicLigne = 5;
                        inAnimation = false;
                        isVerticalSwap = false;
                    }
                }

                if (swapAllowed && isDiagonalSwap && ((row == premierClicCol && col == premierClicLigne) || (row == clicCol && col == clicLigne))) {
                    animationProgres += 0.2;
                    inAnimation = true;
                    if (animationProgres < 100.05) {
                        if (row == premierClicCol && col == premierClicLigne){
                            lineX += totalTailleCell * (clicLigne - premierClicLigne) * animationProgres / 100;
                            lineY += totalTailleCell * (clicCol - premierClicCol) * animationProgres / 100;
                        }
                        if (row == clicCol && col == clicLigne){
                            lineX -= totalTailleCell * (clicLigne - premierClicLigne) * animationProgres / 100;
                            lineY -= totalTailleCell * (clicCol - premierClicCol) * animationProgres / 100;
                        }
                    } else {
                        if (clique == 2) faitUnMouvement(mat);
                        explosionTime = 0.5;
                        animationProgres = 0;
                        clicLigne = 0;
                        premierClicLigne = 5;
                        inAnimation = false;
                        isDiagonalSwap = false;
                    }
                }

                switch (mat[row][col]) {
                case 0:
                    // Aucun chiffre, je laisse la cellule vide
                    break;
                case 1:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(lineX, lineY), nsGraphics::Vec2D(lineX + cellSize - 5, lineY + cellSize - 5), nsGraphics::KBlue);
                    break;
                case 2:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(lineX, lineY), nsGraphics::Vec2D(lineX + cellSize - 5, lineY + cellSize - 5), nsGraphics::KRed);
                    break;
                case 3:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(lineX, lineY), nsGraphics::Vec2D(lineX + cellSize - 5, lineY + cellSize - 5), nsGraphics::KBlack);
                    break;
                case 4:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(lineX, lineY), nsGraphics::Vec2D(lineX + cellSize - 5, lineY + cellSize - 5), nsGraphics::KYellow);
                    break;
                case 5:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(lineX, lineY), nsGraphics::Vec2D(lineX + cellSize - 5, lineY + cellSize - 5), nsGraphics::KGreen);
                    break;
                case 6:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(lineX, lineY), nsGraphics::Vec2D(lineX + cellSize - 5, lineY + cellSize - 5), nsGraphics::KWhite);
                    break;
                }
            }
        }
    }
}

void initLevel(MinGL &window, const unsigned & level, const int & wx, const int & wy){
    if (level == 1) {
        if (initMats == false){
            score = 0;
            essai = 5;
            scoreMinimum = 15;
            initMat(mat, level);
            initMats = true;
        } if (essai != 0 && score < scoreMinimum) dessineBoard(window, 5,  wx, wy);
    } else if (level == 2){
        if (initMats == false){
            score = 0;
            essai = 7;
            scoreMinimum = 18;
            initMat(mat, level);
            initMats = true;
        } if (essai != 0 && score < scoreMinimum) dessineBoard(window, 6, wx, wy);
    } else if (level == 3){
        if (initMats == false){
            score = 0;
            essai = 8;
            scoreMinimum = 27;
            initMat(mat, level);
            initMats = true;
        } if (essai != 0 && score < scoreMinimum) dessineBoard(window, 7, wx, wy);
    } else if (level == 4){
        if (initMats == false){
            score = 0;
            essai = 10;
            scoreMinimum = 30;
            initMat(mat, level);
            initMats = true;
            if (fullscreen == false) glutFullScreen();
            fullscreen = true;
        } if (essai != 0 && score < scoreMinimum) dessineBoard(window, 8, wx, wy);
    } else if (level == 5){
        if (initMats == false){
            score = 0;
            essai = 13;
            scoreMinimum = 44;
            initMat(mat, level);
            initMats = true;
            if (fullscreen == false) glutFullScreen();
            fullscreen = true;
        } if (essai != 0 && score < scoreMinimum) dessineBoard(window, 10, wx, wy);
    } else if (level == 6) {
        if (initMats == false){
            initMat(mat, level, 4);
            score = 0;
            essai = nvAleatoireTaille;
            scoreMinimum = 3 * nvAleatoireTaille;
            initMats = true;
            if (fullscreen == false) glutFullScreen();
            fullscreen = true;
        }
        if (essai != 0 && score < scoreMinimum) dessineBoard(window, nvAleatoireTaille, wx, wy);
    } else if (level == 7) {
        if (inEditeur == 1){
            tailleNvEditeur = 5;
            editeurCellules = 2;
            score = 0;
            essai = 1;
            scoreMinimum = 1;
            inEditeur = 2;
            if (fullscreen == false) glutFullScreen();
            fullscreen = true;
        }
        if (inEditeur == 2) editeurNiveau(window);
        if (inEditeur == 3) dessineBoard(window, tailleNvEditeur, wx, wy);
        if (inEditeur == 4 && (essai != 0 && score < scoreMinimum)) dessineBoard(window, tailleNvEditeur, wx, wy);
    }
}

void position(MinGL &window, const unsigned & x, const unsigned & y,
              const unsigned & wx, const unsigned & wy, const bool &isClick){
    bool arrowCursor = true;

    // On cherche la position x et y du x afin de oui ou non exécuter un évènement
    if (y >= 11+wy/10 && y <= 30+wy/10){
        if (x >= 610+wx*2 && x <= 632+wx*2){
            arrowCursor = false;
            if (isClick) glutDestroyWindow(1);
            else glutSetCursor(GLUT_CURSOR_INFO);
        } else if (x >= 585+wx*2 && x <= 605+wx*2) {
            arrowCursor = false;
            if (fullscreen){
                if (isClick){
                    window.setWindowSize(nsGraphics::Vec2D(640, 640));
                    fullscreen = false;
                }
                else glutSetCursor(GLUT_CURSOR_INFO);
            } else {
                if (isClick){
                    fullscreen = true;
                    glutFullScreen();
                }
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
        } else if (x >= 560+wx*2 && x <= 580+wx*2 && level != 0){
            arrowCursor = false;
            if (isClick){
                level = 0;
                initMats = false;
                if (inEditeur > 1) inEditeur = 1;
            }
            else glutSetCursor(GLUT_CURSOR_INFO);
        }
    }

    // Level == 0 correspond au menu, on cherche la position de x et y afin de savoir quel niveau choisir
    if (level == 0){
        if (x >= 285+wx && x <= 360+wx){
            if (y >= 220+wy && y <= 230+wy){
                arrowCursor = false;
                if (isClick) ++level;
                else glutSetCursor(GLUT_CURSOR_INFO);
            } else if (y >= 250+wy && y <= 265+wy){
                arrowCursor = false;
                if (isClick) level = 2;
                else glutSetCursor(GLUT_CURSOR_INFO);
            } else if (y >= 280+wy && y <= 295+wy){
                arrowCursor = false;
                if (isClick) level = 3;
                else glutSetCursor(GLUT_CURSOR_INFO);
            } else if (y >= 310+wy && y <= 325+wy){
                arrowCursor = false;
                if (isClick) level = 4;
                else glutSetCursor(GLUT_CURSOR_INFO);
            } else if (y >= 340+wy && y <= 355+wy){
                arrowCursor = false;
                if (isClick) level = 5;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
        }
        if (x >= 240+wx && x <= 393+wx){
            if (y >= 370+wy && y <= 382+wy){
                arrowCursor = false;
                if (isClick) level = 6;
                else glutSetCursor(GLUT_CURSOR_INFO);
            } else if (y >= 402+wy && y <= 412+wy){
                arrowCursor = false;
                if (isClick) level = 7;
                else glutSetCursor(GLUT_CURSOR_INFO);
            }
        }
    }

    // Si le joueur souhaite recommencer le niveau, on recommence une nouvelle matrice
    if (((level > 0 && level < 7) || inEditeur == 4)  && x >= 263+wx &&  x <= 380+wx && y >= 118+wy && y <= 133+wy){
        arrowCursor = false;
        if (isClick && (level > 0 && level < 7)) {
            initMats = false;
        }
        else if (isClick && inEditeur == 4) inEditeur = 2;
        else glutSetCursor(GLUT_CURSOR_INFO);
    }

    // Si le joueur clique sur l'une des cases de la cellule
    if (level != 0 && x >= boardTopLeftX && x <= boardTopLeftX + boardSize * totalTailleCell &&
        y >= boardTopLeftY && y <= boardTopLeftY + boardSize * totalTailleCell && !(inAnimation) && !(inExplosion)) {
        arrowCursor = false;

        if (isClick) {

            // On calcule l'indice de la cellule
            clicCol = (y - boardTopLeftY) / totalTailleCell;
            clicLigne = (x - boardTopLeftX) / totalTailleCell;

            // Si on est dans l'editeur de niveau et que l'on souhaite placer un cube
            if (inEditeur == 3) mat[clicCol][clicLigne] = squareType;

            // On verifie que les 2 cellules cliquées sont compatibles
            if (clique == 1 && (abs((int)premierClicCol - (int)clicCol) <= 1 && abs((int)premierClicLigne - (int)clicLigne) <= 1
                                && mat[clicCol][clicLigne] != mat[premierClicCol][premierClicLigne])
                && mat[clicCol][clicLigne] != KAIgnorer && mat[premierClicCol][premierClicLigne] != KAIgnorer){
                if (swapAllowed && ((level > 0 && level < 7) || inEditeur == 4)) ++clique;

            }

            if (clique < 1 && swapAllowed && mat[clicCol][clicLigne] != KAIgnorer) ++clique;

            if (clique == 1){
                premierClicCol = clicCol;
                premierClicLigne = clicLigne;
            }
        } else if (!isClick) glutSetCursor(GLUT_CURSOR_INFO);
    }

    if (inEditeur > 1 && x >= 299+wx && x <= 345+wx && y >= 117+wy && y <= 130+wy) {
        arrowCursor = false;
        if (isClick && inEditeur == 2) {
            tailleNvEditeur = 5;
            editeurCellules = 2;
            scoreMinimum = 1;
            essai = 1;
        } else if (isClick && inEditeur ==3) initMat(mat, level, editeurCellules, tailleNvEditeur);
        else glutSetCursor(GLUT_CURSOR_INFO);
    }
    if (inEditeur > 1 && x >= 273+wx && x <= 371+wx && y >= 78+wy && y <= 91+wy){
        arrowCursor = false;
        if (isClick){
            if (inEditeur == 2){
                initMat(mat, level, editeurCellules, tailleNvEditeur);
                inEditeur = 3;
            }
            for(size_t i = 0; i < mat.size(); ++i){
                for(size_t j = 0; j < mat[i].size(); ++j){
                    if (mat[i][j] == KAIgnorer) filled = false;
                }
            }
            if (inEditeur == 3 && filled){
                clique = 0;
                inEditeur = 4;
            }
            else filled = true;
        }
        else if (!isClick) glutSetCursor(GLUT_CURSOR_INFO);
    }
    if (inEditeur == 3 && x >= -84+wx && x <= -23+wx && y >= 257+wy && y <=270+wy){
        arrowCursor = false;
        if (isClick && squareType < editeurCellules){
            ++squareType;
        } else if (isClick) squareType = 1;
        if (!isClick) glutSetCursor(GLUT_CURSOR_INFO);
    }

    if (arrowCursor) glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

// On calcul les différents events (cliques de la souris, swap, ...)
void events(MinGL &window, int wx, int wy) {
    int clickedX, clickedY, realTimeX, realTimeY;
    bool isClick;

    // On vérifie chaque évènement de la queue d'évènements
    while (window.getEventManager().hasEvent())
    {
        const nsEvent::Event_t actualEvent = window.getEventManager().pullEvent();

        // On regarde le type d'évènement
        switch (actualEvent.eventType)
        {
        case nsEvent::EventType_t::MouseMove:
            // Il s'agit d'un mouvement de souris
            triPos.setX(actualEvent.eventData.moveData.x);
            triPos.setY(actualEvent.eventData.moveData.y);
            realTimeX = triPos.getX();
            realTimeY = triPos.getY();
            isClick = false;
            position(window, realTimeX, realTimeY, wx, wy, isClick);
            if (inEditeur == 2) editeurNiveau(window , realTimeX, realTimeY, isClick);
            break;

        case nsEvent::EventType_t::MouseClick:

            // Il s'agit d'un click de souris
            triColor = (actualEvent.eventData.clickData.state ? nsGraphics::KWhite : nsGraphics::KBlack);

            // On récupère la position de la souris en ignorant en trop le deuxième clique
            if (mouse_clicked == false){
                clickedX = triPos.getX();
                clickedY = triPos.getY();
                mouse_clicked = true;
                isClick = true;
                position(window, clickedX, clickedY, wx, wy, isClick);
                if (inEditeur == 2) editeurNiveau(window , clickedX, clickedY, isClick);
            } else mouse_clicked = false;

            break;

        default:
            // L'evenement ne nous intéresse pas
            break;
        }
    }
}


// Fonction utilisant MinGL pour dessiner
void dessiner(MinGL &window, const unsigned &wx, const unsigned &wy) {

    // On dessine le bouton pour fermer le jeu
    window << nsShape::Circle(nsGraphics::Vec2D(620+wx*2, 20+wy/10), 10, nsGraphics::KRed);
    window << nsShape::Line(nsGraphics::Vec2D(613+wx*2, 27+wy/10), nsGraphics::Vec2D(627+wx*2, 13+wy/10), nsGraphics::KWhite, 3.f);
    window << nsShape::Line(nsGraphics::Vec2D(613+wx*2, 13+wy/10), nsGraphics::Vec2D(627+wx*2, 27+wy/10), nsGraphics::KWhite, 3.f);

    // On dessine le bouton pour mettre en plein écran
    window << nsShape::Line(nsGraphics::Vec2D(588+wx*2, 12+wy/10), nsGraphics::Vec2D(604+wx*2, 12+wy/10), nsGraphics::KWhite, 3.f);
    window << nsShape::Line(nsGraphics::Vec2D(588+wx*2, 12+wy/10), nsGraphics::Vec2D(588+wx*2, 28+wy/10), nsGraphics::KWhite, 3.f);
    window << nsShape::Line(nsGraphics::Vec2D(587+wx*2, 28+wy/10), nsGraphics::Vec2D(604+wx*2, 28+wy/10), nsGraphics::KWhite, 3.f);
    window << nsShape::Line(nsGraphics::Vec2D(604+wx*2, 12+wy/10), nsGraphics::Vec2D(604+wx*2, 28+wy/10), nsGraphics::KWhite, 3.f);

    // On dessine le menu
    if (level == 0) menu(window, wx, wy);

    else {
        // On dessine le score
        window << nsGui::Text(nsGraphics::Vec2D(20, 20), "Point(s) : " + to_string(score), nsGraphics::KWhite);

        // On dessine le nombre d'essai
        window << nsGui::Text(nsGraphics::Vec2D(20, 40), "Essai(s) : " + to_string(essai), nsGraphics::KWhite);

        if (clique == 1 && ((level > 0 && level < 7) || inEditeur == 4) && (!(score >= scoreMinimum) || (essai != 0))) window << nsGui::Text(nsGraphics::Vec2D(320+wx, 170+wy), "Veuillez cliquer sur un autre cube", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);


        // On dessine le nom du nouveau (ex: niveau 1)
        if (level > 0 && level < 7){
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 55+wy), "Niveau " + to_string(level), nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        } else if (inEditeur == 4){
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 55+wy), "Niveau personnalise", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        }

        if ((level > 0 && level < 7) || inEditeur == 4){
            // On dessine le nombre de score néscessaire afin de gagner
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 90+wy), "Vous avez besoin de " + to_string(scoreMinimum) + " points !", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

            // On dessine le le texte qui permet au joueur de relancer le niveau
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 130+wy), "Recommencer", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

        } else if (inEditeur > 1 && inEditeur < 4) {
            // On dessine le nombre de score néscessaire afin de gagner
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 90+wy), "Suivant", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

            // On dessine le texte qui permet de reset
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 130+wy), "Reset", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

            if (inEditeur == 3) {
                window << nsGui::Text(nsGraphics::Vec2D(320+wx, 165+wy), "Placez vos carres", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                      nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
                window << nsGui::Text(nsGraphics::Vec2D(-55 + wx, 270+wy), "Suivant", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                      nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
                window << nsGui::Text(nsGraphics::Vec2D(-50 + wx, 300+wy), "Carre actuel : ", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                      nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
                switch(squareType){
                case 1:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(-85+wx, 320+wy), nsGraphics::Vec2D(-25+wx, 380 + wy), nsGraphics::KBlue);
                    break;
                case 2:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(-85+wx, 320+wy), nsGraphics::Vec2D(-25+wx, 380 + wy), nsGraphics::KRed);
                    break;
                case 3:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(-85+wx, 320+wy), nsGraphics::Vec2D(-25+wx, 380 + wy), nsGraphics::KBlack);
                    break;
                case 4:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(-85+wx, 320+wy), nsGraphics::Vec2D(-25+wx, 380 + wy), nsGraphics::KYellow);
                    break;
                case 5:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(-85+wx, 320+wy), nsGraphics::Vec2D(-25+wx, 380 + wy), nsGraphics::KGreen);
                    break;
                case 6:
                    window << nsShape::Rectangle(nsGraphics::Vec2D(-85+wx, 320+wy), nsGraphics::Vec2D(-25+wx, 380 + wy), nsGraphics::KWhite);
                    break;
                }
            }
        }

        // On dessine la flèche pour retourner au menu
        window << nsShape::Triangle(nsGraphics::Vec2D(570+wx*2, 30+wy/10), nsGraphics::Vec2D(570+wx*2, 10+wy/10), nsGraphics::Vec2D(560+wx*2, 20+wy/10), nsGraphics::KWhite);
        window << nsShape::Line(nsGraphics::Vec2D(570+wx*2, 20+wy/10), nsGraphics::Vec2D(580+wx*2, 20+wy/10), nsGraphics::KWhite, 3.f);

        // Si le joueur a atteint le score demandé, alors il a gagné le niveau ou s'il n'a plus d'essai, il a perdu
        if ((score >= scoreMinimum || (essai == 0 && score < scoreMinimum)) && (inEditeur == 4)){
            // On affiche le message pour que le joueur puisse recommencer une partie
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 280+wy), "VOUS POUVEZ RECOMMENCER !", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
            if (score >= scoreMinimum){
                // Si il a gagné, on affiche un messgae de victoire
                window << nsGui::Text(nsGraphics::Vec2D(320+wx, 250+wy), "VOUS AVEZ GAGNE !", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                      nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
            }   // Sinon, si il a perdu, on affiche un message de defaite
            else if (essai <= 0 && score < scoreMinimum){
                window << nsGui::Text(nsGraphics::Vec2D(320+wx, 250+wy), "VOUS AVEZ PERDU !", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                      nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
            }
        }

        // On dessine le niveau choisi
        if (level > 0) initLevel(window, level, wx, wy);
    }
}


// Fonction principale du programme
int main() {
    // Initialise le système
    MinGL window("Cube Crusher", nsGraphics::Vec2D(640, 640), nsGraphics::Vec2D(128, 128), nsGraphics::KPurple);
    window.initGlut();
    window.initGraphic();

    // Variable qui tient le temps de frame
    chrono::microseconds frameTime = chrono::microseconds::zero();

    // On fait tourner la boucle tant que la fenêtre est ouverte
    while (window.isOpen())
    {
        // On récupère la taille de la fenêtre
        nsGraphics::Vec2D windowSize;
        windowSize = window.getWindowSize();
        unsigned wx = (windowSize.getX() - 640)/2;
        unsigned wy = (windowSize.getY() - 640)/4;


        // Récupère l'heure actuelle
        chrono::time_point<chrono::steady_clock> start = chrono::steady_clock::now();

        // On efface la fenêtre
        window.clearScreen();

        // On dessine le texte
        events(window, wx, wy);
        dessiner(window, wx, wy);

        // On finit la frame en cours
        window.finishFrame();

        // On attend un peu pour limiter le framerate et soulager le CPU
        this_thread::sleep_for(chrono::milliseconds(1000 / FPS_LIMIT) - chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start));

        // On récupère le temps de frame
        frameTime = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start);
    }

    return 0;
}
