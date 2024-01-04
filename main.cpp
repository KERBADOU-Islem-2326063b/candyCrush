// TODO

// --PRIORITES--
// Animation
// Simplifier le code, faire plus de fonctions peut-être et plus de paramètres

// --SECONDAIRE--
// Editeur de niveau pleins de bugs (enleve 1 et 2 pour cellule, quand on a fini le niveau y a pas marqué t'as gagné
// et le bouton recommencer ne marche pas

// Editeur de niveau placer les cellules nous mêmes


#define FPS_LIMIT 60

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <fstream>

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


int editeurColRow = 0;
int editeurCellules = 0;

int randomSize;
int boardSize;
int cellSize;
int gapSize;
int totalCellSize;
int boardTopLeftX;
int boardTopLeftY;

int level = 0;
int clique = 0;
int FirstclickedCol = 0;
int FirstclickedRow = 0;
int clickedCol = 0;
int clickedRow = 0;
int essai = 10;
int inEditeur = 1;

float animationProgress = 0;
float explosionTime = 0;
float explosionTime2 = 0;
float cliqueTime = 0;
float click = false;

bool isClick;
bool mouse_clicked = false;
bool ms = false;
bool initMats = false;
bool fullscreen = false;
bool inAnimation = false;
bool inExplosion = false; // TODO
bool isSwap = false;
bool isHorizontalSwap = false;
bool isVerticalSwap = false;
bool isDiagonalSwapTL = false;
bool isDiagonalSwapTR = false;
bool isDiagonalSwapBL = false;
bool isDiagonalSwapBR = false;

unsigned score (0);
unsigned neededScore (1);

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

const contenueDUneCase KAIgnorer = 0;
const contenueDUneCase KPlusGrandNombreDansLaMatrice = 4;

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

//
bool detectionExplosionUneBombeHorizontale (CMatrice & mat, unsigned & score){
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
                explosionUneBombeHorizontale (mat, i, j, combienALaSuite);
            } else if (isSwap){
                --essai;
                isSwap = false;
            }
        }
    }
    return auMoinsUneExplosion;
}



void explosionUneBombeVerticale (CMatrice & mat, const size_t & numLigne,
                                const size_t & numColonne, const size_t & combien){
    for(unsigned k = 0; k < combien; ++k){
        mat[numLigne+k][numColonne] = KAIgnorer;
        for(unsigned j = numLigne; j > 0; --j){
            swap(mat[j+k][numColonne], mat[j-1+k][numColonne]);
        }
    }

}

bool detectionExplosionUneBombeVerticale (CMatrice & mat, unsigned & score){
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

    return auMoinsUneExplosion;
}

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

void editeurNiveau(MinGL &window){
    // On récupère la taille de la fenêtre
    nsGraphics::Vec2D windowSize;
    windowSize = window.getWindowSize();
    int wx = (windowSize.getX() - 640)/2;
    int wy = (windowSize.getY() - 640)/4;

    if (inEditeur == 2){
        // On affiche les choix disponibles
        window << nsGui::Text(nsGraphics::Vec2D(330+wx, 230+wy), "Nombre de ligne(s) et colonne(s) : " + to_string(editeurColRow), nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 260+wy), "5   6   7   8   9   10", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 330+wy), "Nombre de cellule(s) differente(s) : " + to_string(editeurCellules), nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 360+wy), "1   2   3   4   5", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 430+wy), "Nombre de score(s) necessaire(s) : " + to_string(neededScore), nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 460+wy), "+1    +2    +5   +10", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 530+wy), "Nombre d'essai(s) : " + to_string(essai), nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 560+wy), "+1    +2    +5", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
    }
}

void initMat(CMatrice &mat, int level, const unsigned &nbMax = KPlusGrandNombreDansLaMatrice, int editeur = 5) {
    string lvlName;
    size_t randomNumber;
    clique = 0;
    switch (level)
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
    case 6:
    {
        mat.resize(randomSize);
        for (int i = 0; i < randomSize; ++i) {
            mat[i].resize(randomSize);
            for (int j = 0; j < randomSize; ++j) {
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
    case 7:
    {
        mat.resize(editeur);
        for (int i = 0; i < editeur; ++i) {
            mat[i].resize(editeur);
            for (int j = 0; j < editeur; ++j) {
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
    default:
        cout << "Mauvais choix" << endl;
        break;
    }
}


void faitUnMouvement (CMatrice & mat) {
    swap(mat[FirstclickedCol][FirstclickedRow], mat[clickedCol][clickedRow]);
    isSwap = true;
    clique = 0;
}

void menu(MinGL &window, int wx, int wy){
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

void dessineBoard(MinGL &window, int board = 5, int cell = 50, int gap = 5, int wx = 640, int wy = 640){
    // On initialise les variables néscessaire pour faire le tableau
    boardSize = board;
    cellSize = cell;
    gapSize = gap;
    totalCellSize = cell + gap;
    boardTopLeftX = 320 + wx - (board * totalCellSize) / 2;
    boardTopLeftY = 200 + wy;

    // On detecte et explose
    if (explosionTime < 100) explosionTime += 1.1;
    else {
        detectionExplosionUneBombeHorizontale(mat, score);
        explosionTime = 0;
    }

    if (explosionTime2 < 100) explosionTime2 += 1.1;
    else {
        detectionExplosionUneBombeVerticale(mat, score);
        explosionTime2 = 0;
    }

    // On dessigne les lignes
    for (int i = 0; i <= boardSize; ++i) {
        int lineCoord = boardTopLeftX + i * totalCellSize;

        // On dessine les lignes vertival
        window << nsShape::Line(nsGraphics::Vec2D(boardTopLeftX, boardTopLeftY + i * totalCellSize),
                                nsGraphics::Vec2D(boardTopLeftX + boardSize * totalCellSize, boardTopLeftY + i * totalCellSize),
                                nsGraphics::KWhite);

        // On dessine les lignes horizontal
        window << nsShape::Line(nsGraphics::Vec2D(lineCoord, boardTopLeftY),
                                nsGraphics::Vec2D(lineCoord, boardTopLeftY + boardSize * totalCellSize),
                                nsGraphics::KWhite);


        // /!\ A CHANGER /!\ On regarde quel type d'animation faire
        if (clique == 2){

            if ((FirstclickedRow == clickedRow - 1 || FirstclickedRow == clickedRow + 1) && FirstclickedCol == clickedCol) isHorizontalSwap = true;
            if ((FirstclickedCol == clickedCol - 1 || FirstclickedCol == clickedCol + 1) && FirstclickedRow == clickedRow) isVerticalSwap = true;

            if ((FirstclickedRow == clickedRow - 1 || FirstclickedRow == clickedRow + 1) && FirstclickedCol == clickedCol) isDiagonalSwapTL = true; // TODO
            if ((FirstclickedRow == clickedRow - 1 || FirstclickedRow == clickedRow + 1) && FirstclickedCol == clickedCol) isDiagonalSwapTR = true; // TODO
            if ((FirstclickedRow == clickedRow - 1 || FirstclickedRow == clickedRow + 1) && FirstclickedCol == clickedCol) isDiagonalSwapBL = true; // TODO
            if ((FirstclickedRow == clickedRow - 1 || FirstclickedRow == clickedRow + 1) && FirstclickedCol == clickedCol) isDiagonalSwapBR = true; // TODO


        }



        // On dessine les cellules
        for (int row = 0; row < boardSize; ++row) {
            for (int col = 0; col < boardSize; ++col) {
                int lineX = boardTopLeftX + col * totalCellSize + gapSize;
                int lineY = boardTopLeftY + row * totalCellSize + gapSize;
                // Ajout d'ajustements en fonction du type de swap (horizontal ou vertical)
                // TODO

                if (isHorizontalSwap && ((row == FirstclickedCol && col == FirstclickedRow) || (row == clickedCol && col == clickedRow))) {
                    animationProgress += 0.2;
                    inAnimation = true;
                    if (animationProgress < 100.05) {
                        if (row == FirstclickedCol && col == FirstclickedRow) lineX += totalCellSize * (clickedRow - FirstclickedRow) * animationProgress / 100;
                        if (row == clickedCol && col == clickedRow) lineX -= totalCellSize * (clickedRow - FirstclickedRow) * animationProgress / 100;
                    } else {
                        if (clique == 2) faitUnMouvement(mat);
                        animationProgress = 0;
                        clickedRow = 0;
                        FirstclickedRow = 5;
                        inAnimation = false;
                        isHorizontalSwap = false;
                    }
                }

                if (isVerticalSwap && ((row == FirstclickedCol && col == FirstclickedRow) || (row == clickedCol && col == clickedRow))) {
                    animationProgress += 0.2;
                    inAnimation = true;
                    if (animationProgress < 100.05) {
                        if (row == FirstclickedCol && col == FirstclickedRow) lineY += totalCellSize * (clickedCol - FirstclickedCol) * animationProgress / 100;
                        if (row == clickedCol && col == clickedRow) lineY -= totalCellSize * (clickedCol - FirstclickedCol) * animationProgress / 100;
                    } else {
                        faitUnMouvement(mat);
                        explosionTime = 0.5;
                        animationProgress = 0;
                        clickedRow = 0;
                        FirstclickedRow = 5;
                        inAnimation = false;
                        isVerticalSwap = false;
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

void initLevel(MinGL &window, int level, int wx, int wy){
    if (level == 1) {
        if (initMats == false){
            score = 0;
            essai = 5;
            neededScore = 15;
            initMat(mat, level);
            initMats = true;
        } if (essai != 0 && score < neededScore) dessineBoard(window, 5, 50, 5, wx, wy);
    } else if (level == 2){
        if (initMats == false){
            score = 0;
            essai = 7;
            neededScore = 18;
            initMat(mat, level);
            initMats = true;
        } if (essai != 0 && score < neededScore) dessineBoard(window, 6, 50, 5, wx, wy);
    } else if (level == 3){
        if (initMats == false){
            score = 0;
            essai = 8;
            neededScore = 27;
            initMat(mat, level);
            initMats = true;
        } if (essai != 0 && score < neededScore) dessineBoard(window, 7, 50, 5, wx, wy);
    } else if (level == 4){
        if (initMats == false){
            score = 0;
            essai = 10;
            neededScore = 30;
            initMat(mat, level);
            initMats = true;
            if (fullscreen == false) glutFullScreen();
            fullscreen = true;
        } if (essai != 0 && score < neededScore) dessineBoard(window, 8, 50, 5, wx, wy);
    } else if (level == 5){
        if (initMats == false){
            score = 0;
            essai = 13;
            neededScore = 44;
            initMat(mat, level);
            initMats = true;
            if (fullscreen == false) glutFullScreen();
            fullscreen = true;
        } if (essai != 0 && score < neededScore) dessineBoard(window, 10, 50, 5, wx, wy);
    } else if (level == 6) {
        if (initMats == false){
            randomSize = rand() % 6 + 5;
            initMat(mat, level, 4);
            score = 0;
            essai = randomSize;
            neededScore = 3 * randomSize;
            initMats = true;
            if (fullscreen == false) glutFullScreen();
            fullscreen = true;
        }
        if (essai != 0 && score < neededScore) dessineBoard(window, randomSize, 50, 5, wx, wy);
    } else if (level == 7) {
        if (inEditeur == 1){
            editeurCellules = 0;
            editeurColRow = 0;
            score = 0;
            essai = 0;
            neededScore = 0;
            inEditeur = 2;
        }
        editeurNiveau(window);
        if (essai != 0 && score < neededScore && inEditeur == 3) dessineBoard(window, editeurColRow, 50, 5, wx, wy);
    }
}

void position(MinGL &window, int x, int y, int wx, int wy, bool &isClick){
    int cpt = 0;
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
    if (level != 0 && x >= 263+wx &&  x <= 380+wx && y >= 118+wy && y <= 133+wy){
        arrowCursor = false;
        if (isClick) initMats = false;
        else glutSetCursor(GLUT_CURSOR_INFO);
    }

    // Si le joueur clique sur l'une des cases de la cellule
    if (level != 0 && x >= boardTopLeftX && x <= boardTopLeftX + boardSize * totalCellSize &&
             y >= boardTopLeftY && y <= boardTopLeftY + boardSize * totalCellSize && !(inAnimation) && !(inExplosion)) {
        arrowCursor = false;

        if (isClick) {
            // On calcule l'indice de la cellule
            clickedCol = (y - boardTopLeftY) / totalCellSize;
            clickedRow = (x - boardTopLeftX) / totalCellSize;

            // On verifie que les 2 cellules cliquées sont compatibles
            if (clique == 1 && (abs(FirstclickedCol - clickedCol) <= 1 && abs(FirstclickedRow - clickedRow) <= 1
                                && mat[clickedCol][clickedRow] != mat[FirstclickedCol][FirstclickedRow])
                && mat[clickedCol][clickedRow] != KAIgnorer && mat[FirstclickedCol][FirstclickedRow] != KAIgnorer){
                ++clique;
            } else if (clique == 1) clique = -1;

            if (clique < 1) ++clique;
            cout << clique << endl;

            cout << "Vous avez cliqué sur la cellule ligne (col) " << clickedCol << ", colonne (row) " << clickedRow << endl;
            if (clique == 1){
                FirstclickedCol = clickedCol;
                FirstclickedRow = clickedRow;
            }
        } else if (!isClick) glutSetCursor(GLUT_CURSOR_INFO);
    }

    if (arrowCursor) {
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        cpt = 0;
    }
}

// On calcul les différents events (cliques de la souris, swap, ...)
void events(MinGL &window, int wx, int wy) {
    int clickedX, clickedY, realTimeX, realTimeY;

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
            break;

        case nsEvent::EventType_t::MouseClick:

            // Il s'agit d'un click de souris
            triColor = (actualEvent.eventData.clickData.state ? nsGraphics::KWhite : nsGraphics::KBlack);

            // On récupère la position de la souris en ignorant en trop le deuxième clique
            if (mouse_clicked == false){
                clickedX = triPos.getX();
                clickedY = triPos.getY();
                cout << "Position x : " << clickedX << " Position y : " << clickedY << endl;
                mouse_clicked = true;
            } else mouse_clicked = false;

            isClick = true;
            position(window, clickedX, clickedY, wx, wy, isClick);
            break;

        default:
            // L'evenement ne nous intéresse pas
            break;
        }
    }
}


// Fonction utilisant MinGL pour dessiner
void dessiner(MinGL &window, int wx, int wy) {

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

        if (clique == 1 && (!(score >= neededScore) || (essai != 0))) window << nsGui::Text(nsGraphics::Vec2D(320+wx, 170+wy), "Veuillez cliquer sur un autre cube", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);


        // On dessine le nom du nouveau (ex: niveau 1)
        if (level > 0 && level < 6){
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 55+wy), "Niveau " + to_string(level), nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        }

        if (inEditeur != 2){
            // On dessine le nombre de score néscessaire afin de gagner
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 90+wy), "Vous avez besoin de " + to_string(neededScore) + " points !", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

            // On dessine le le texte qui permet au joueur de relancer le niveau
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 130+wy), "Recommencer", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        } else {
            // On dessine le nombre de score néscessaire afin de gagner
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 110+wy), "Commencer", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);

            // On dessine le le texte qui permet au joueur de relancer le niveau
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 165+wy), "Reset", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        }

        // On dessine la flèche pour retourner au menu
        window << nsShape::Triangle(nsGraphics::Vec2D(570+wx*2, 30+wy/10), nsGraphics::Vec2D(570+wx*2, 10+wy/10), nsGraphics::Vec2D(560+wx*2, 20+wy/10), nsGraphics::KWhite);
        window << nsShape::Line(nsGraphics::Vec2D(570+wx*2, 20+wy/10), nsGraphics::Vec2D(580+wx*2, 20+wy/10), nsGraphics::KWhite, 3.f);

        // Si le joueur a atteint le score demandé, alors il a gagné le niveau ou s'il n'a plus d'essai, il a perdu
        if ((score >= neededScore || (essai == 0 && score < neededScore)) && inEditeur == 1){
            // On affiche le message pour que le joueur puisse recommencer une partie
            window << nsGui::Text(nsGraphics::Vec2D(320+wx, 280+wy), "VOUS POUVEZ RECOMMENCER !", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                  nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
            if (score >= neededScore){
                // Si il a gagné, on affiche un messgae de victoire
                window << nsGui::Text(nsGraphics::Vec2D(320+wx, 250+wy), "VOUS AVEZ GAGNE(E) !", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                                      nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
            }   // Sinon, si il a perdu, on affiche un message de defaite
            else if (essai <= 0 && score < neededScore){
                window << nsGui::Text(nsGraphics::Vec2D(320+wx, 250+wy), "VOUS AVEZ PERDU(E) !", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
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
        int wx = (windowSize.getX() - 640)/2;
        int wy = (windowSize.getY() - 640)/4;


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
