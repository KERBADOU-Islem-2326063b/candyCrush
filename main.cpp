#define FPS_LIMIT 60

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <thread>

#include "../MinGL2_IUT_AIX/include/mingl/mingl.h"

#include "../MinGL2_IUT_AIX/include/mingl/gui/text.h"
#include "../MinGL2_IUT_AIX/include/mingl/shape/line.h"
#include "../MinGL2_IUT_AIX/include/mingl/shape/triangle.h"
#include "../MinGL2_IUT_AIX/include/mingl/shape/circle.h"

using namespace std;


nsGraphics::Vec2D triPos;
nsGraphics::RGBAcolor triColor = nsGraphics::KWhite;

void events(MinGL &window, int& i)
{
    // On récupère la taille de la fenêtre
    nsGraphics::Vec2D windowSize;
    windowSize = window.getWindowSize();
    int wx = (windowSize.getX() - 640)/2;
    int wy = (windowSize.getY() - 640)/4;
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

            break;

        case nsEvent::EventType_t::MouseClick:
            // Il s'agit d'un click de souris
            triColor = (actualEvent.eventData.clickData.state ? nsGraphics::KWhite : nsGraphics::KBlack);

            // On récupère la position de la souris
            int x, y;
            x = triPos.getX();
            y = triPos.getY();
            cout << "Position x : " << x << " Position y : " << y << endl;
            if (x >= 610+wx*2 && x <= 632+wx*2 && y >=10+wy/10 && y <= 30+wy/10){
                window.stopGraphic();
            }
            if (i == 0){
                if (x > 285+wx && x < 360+wx){
                    if (y >= 220+wy && y <= 230+wy){
                        cout << "Vous avez choisi le niveau 1 !" << endl;
                        ++i;
                    } else if (y >= 250+wy && y <= 265+wy){
                        cout << "Vous avez choisi le niveau 2 !" << endl;
                        i = 2;
                    } else if (y >= 280+wy && y <= 295+wy){
                        cout << "Vous avez choisi le niveau 3 !" << endl;
                        i = 3;
                    } else if (y >= 310+wy && y <= 325+wy){
                        cout << "Vous avez choisi le niveau 4 !" << endl;
                        i = 4;
                    } else if (y >= 340+wy && y <= 355+wy){
                        cout << "Vous avez choisi le niveau 5 !" << endl;
                        i = 5;
                    }
                }
            }
            break;

        default:
            // L'évènement ne nous intéresse pas
            break;
        }
    }
}

void dessiner(MinGL &window, int& i)
{
    // On récupère la taille de la fenêtre
    int score = 0;
    nsGraphics::Vec2D windowSize;
    windowSize = window.getWindowSize();
    int wx = (windowSize.getX() - 640)/2;
    int wy = (windowSize.getY() - 640)/4;

    // MinGL 2 supporte l'affichage de texte sur la fenêtre assez simplement.
    window << nsShape::Circle(nsGraphics::Vec2D(620+wx*2, 20+wy/10), 10, nsGraphics::KRed);
    window << nsShape::Line(nsGraphics::Vec2D(613+wx*2, 27+wy/10), nsGraphics::Vec2D(627+wx*2, 13+wy/10), nsGraphics::KWhite, 3.f);
    window << nsShape::Line(nsGraphics::Vec2D(613+wx*2, 13+wy/10), nsGraphics::Vec2D(627+wx*2, 27+wy/10), nsGraphics::KWhite, 3.f);

    if (i == 0){
        window << nsGui::Text(nsGraphics::Vec2D(20, 20), "Fait par : KERBADOU Islem, ODERZO Flavio", nsGraphics::KWhite);
        window << nsGui::Text(nsGraphics::Vec2D(20, 40), "FREMENTIN Felix, BLABLA Hugo", nsGraphics::KWhite);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 160+wy), "Candy Crush", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15,
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
    } if (i == 1) {
        window << nsGui::Text(nsGraphics::Vec2D(20, 20), "Score : " + to_string(score), nsGraphics::KWhite);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 120+wy), "Niveau 1", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
        // Draw the grid lines
        const int boardSize = 10;
        const int cellSize = 30; // Adjusted cell size
        const int boardTopLeftX = 320 + wx - (boardSize * cellSize) / 2; // Centered below "Niveau 1"
        const int boardTopLeftY = 200 + wy;

        for (int row = 0; row <= boardSize; ++row)
        {
            int lineY = boardTopLeftY + row * cellSize;
            window << nsShape::Line(nsGraphics::Vec2D(boardTopLeftX, lineY), nsGraphics::Vec2D(boardTopLeftX + boardSize * cellSize, lineY), nsGraphics::KWhite);
        }

        for (int col = 0; col <= boardSize; ++col)
        {
            int lineX = boardTopLeftX + col * cellSize;
            window << nsShape::Line(nsGraphics::Vec2D(lineX, boardTopLeftY), nsGraphics::Vec2D(lineX, boardTopLeftY + boardSize * cellSize), nsGraphics::KWhite);
        }
    } else if (i == 2){
        window << nsGui::Text(nsGraphics::Vec2D(20, 20), "Score : " + to_string(score), nsGraphics::KWhite);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 120+wy), "Niveau 2", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
    } else if (i == 3){
        window << nsGui::Text(nsGraphics::Vec2D(20, 20), "Score : " + to_string(score), nsGraphics::KWhite);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 120+wy), "Niveau 3", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
    } else if (i == 4){
        window << nsGui::Text(nsGraphics::Vec2D(20, 20), "Score : " + to_string(score), nsGraphics::KWhite);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 120+wy), "Niveau 4", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
    } else if (i == 5){
        window << nsGui::Text(nsGraphics::Vec2D(20, 20), "Score : " + to_string(score), nsGraphics::KWhite);
        window << nsGui::Text(nsGraphics::Vec2D(320+wx, 120+wy), "Niveau 5", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_HELVETICA_18,
                              nsGui::Text::HorizontalAlignment::ALIGNH_CENTER);
    }
}

void souris(MinGL &window){
    // On dessine la souris et l'anime en direct
    window << nsShape::Triangle(triPos, triPos + nsGraphics::Vec2D(10, 10), nsGraphics::Vec2D(triPos.getX(), triPos.getY()+16), triColor);
    window << nsShape::Line(nsGraphics::Vec2D(triPos.getX()+3, triPos.getY()+10), nsGraphics::Vec2D(triPos.getX()+8, triPos.getY()+18), triColor, 3.f);
}

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
    cout << "   | ";
    for (size_t x = 0; x <= Mat.size(); ++x){
        if (x == 0) continue;
        cout << x << " | ";
    }
    cout << string(10, ' ') << "Score: " << score;
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
                //cout << string (20, '-') << endl << "matrice après suppression" << endl;
                //afficheMatriceV2(mat);

            }
        }
    }

    return auMoinsUneExplosion;
}

int game (){
    CMatrice mat;
    unsigned score (0);
    initMat(mat);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    detectionExplosionUneBombeHorizontale (mat, score);
    detectionExplosionUneBombeVerticale(mat, score);
    //condition de victoire a trouver
    while (true) {
        afficheMatriceV2 (mat, score);
        cout << "fais un mouvement, ";
        cout << "numero de ligne : ";
        size_t numLigne;
        cin >> numLigne;
        if (numLigne == 'f') continue;
        cout << "numero de colonne : ";
        size_t numCol;
        cin >> numCol;
        if (numCol == 'f') continue;
        afficheMatriceV2(mat, score, numLigne-1, numCol-1);
        cout << "Sens du deplacement : (A|Z|E|Q|D|W|X|C) : " << endl;
        char deplacement;
        cin >> deplacement;
        if (deplacement == 'f') continue;
        faitUnMouvement (mat, deplacement, numLigne-1, numCol-1);
        detectionExplosionUneBombeHorizontale (mat, score);
        detectionExplosionUneBombeVerticale(mat, score);
        afficheMatriceV2 (mat, score);
    }
    return 0;
}

int initMinGL(){
    // Initialise le système
    int i = 0;
    MinGL window("Candy Crush", nsGraphics::Vec2D(640, 640), nsGraphics::Vec2D(128, 128), nsGraphics::KPurple);
    window.initGlut();
    window.initGraphic();

    // Variable qui tient le temps de frame
    chrono::microseconds frameTime = chrono::microseconds::zero();

    // On fait tourner la boucle tant que la fenêtre est ouverte
    while (window.isOpen())
    {
        // Récupère l'heure actuelle
        chrono::time_point<chrono::steady_clock> start = chrono::steady_clock::now();

        // On efface la fenêtre
        window.clearScreen();

        // On dessine le texte
        events(window, i);
        dessiner(window, i);
        souris(window);

        // On finit la frame en cours
        window.finishFrame();

        // On vide la queue d'évènements
        // window.getEventManager().clearEvents();

        // On attend un peu pour limiter le framerate et soulager le CPU
        this_thread::sleep_for(chrono::milliseconds(1000 / FPS_LIMIT) - chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start));

        // On récupère le temps de frame
        frameTime = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start);
    }

    return 0;
}


int main() {
    initMinGL();
    // return game();
}
