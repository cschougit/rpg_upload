#include "game.h"
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QTimer>
#include <QList>
#include <QDebug>
#include <QFont>
#include <QBrush>
#include <QColor>
#include <QTime>
#include <QPixmap>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

class Item {
public:
    QString name;
    Item(const QString& n) : name(n) {}
    virtual QString description() const = 0;
    virtual ~Item() {}
};

class PokeBall : public Item {
public:
    PokeBall() : Item("Poke Ball") {}
    QString description() const override { return "Can be used to capture wild Pokemon (up to three)."; }
};

class Potion : public Item {
public:
    Potion() : Item("Potion") {}
    QString description() const override { return "Restores 10 HP."; }
};

class Ether : public Item {
public:
    Ether() : Item("Ether") {}
    QString description() const override { return "Fully restores."; }
};

class Pokemon {
public:
    QString name;
    int hp;
    int maxHp;
    int lvl;
    int attack;
    int defence;
    int exp = 0;
    int power[4] = {10,15,20,25};
    int pp[4]    = {20,20,15,5};
    int maxpp[4] = {20,20,15,5};
    int moves[6] = { 1,0,0,0,0,0 };
    Pokemon(const QString& n, int h, int mh, int atk, int def) : name(n), hp(h), maxHp(mh), lvl(1), attack(atk), defence(def) {}
};

class Player {
public:
    qreal x, y;
    qreal x_town, y_town;
    qreal x_land, y_land;
    int NPC_talk = 0;
    int step = 0;
    //pokemonList mypokemonlist;
    Player(qreal startX, qreal startY) : x(startX), y(startY), NPC_talk(0) {}
    void move(qreal dx, qreal dy, const QList<QGraphicsRectItem*>& barriers, const QList<QGraphicsRectItem*>& Lbarriers) {
        qreal newX = x + dx;
        qreal newY = y + dy;
        QRectF newRect(newX, newY, 35, 48);
        for (const auto* barrier : barriers) {
            if (newRect.intersects(barrier->rect())) {
                return;//if hit the barriers, then move
            }
        }
        for (const auto lbarrier : Lbarriers) {
            if (newRect.intersects(lbarrier->rect())) {
                if (dy< -0.5) return;//only allow dy>0
                if (dx > 0.5) return;
                if (dx < -0.5) return;
            }
        }

        x = newX;
        y = newY;
    }
};

Game::Game(QWidget* parent) : QGraphicsView(parent), currentScene(nullptr), player(new Player(500, 500)),
playerItem(nullptr), bagMenuVisible(false), dialogVisible(false),
dialogBox(nullptr), dialogText(nullptr), menuBox(nullptr), menuText(nullptr),
pokeBallItem(nullptr), potionItem(nullptr), etherItem(nullptr),
pokeBallQuantity(nullptr), potionQuantity(nullptr), etherQuantity(nullptr),  
pokemonItem(nullptr), pokemonItem0(nullptr) , pokemonItem1(nullptr) , pokemonItem2(nullptr) , pokemonItem3(nullptr) ,
menuText0(nullptr), menuText1(nullptr), menuText2(nullptr), menuText3(nullptr), 
cursor(nullptr),
battleText(nullptr),
wildp(nullptr),
wildp_Text(nullptr),
wildp_HP(nullptr),
wildPokemon(nullptr),
pokemonItemActive(nullptr),
pokemonItemActive_Text(nullptr),
pokemonItemActive_Text2(nullptr),
pokemonItemActive_Text3(nullptr),
pokemonItemActive_Text4(nullptr),
pokemonItemActive_HP(nullptr),
pokemonItemActive_EXP(nullptr),
skillmenuBox(nullptr),
skillmenuText0(nullptr),
skillmenuText1(nullptr),
skillmenuText2(nullptr),
skillmenuText3(nullptr),
skillmenuText4(nullptr),
skillmenuText5(nullptr),
YesText(nullptr),
YesRect(nullptr),
pokeBall_Mon_ready(false), yellowBox_ready(false), Encounter_Pokemon(0), Go_Battle(false), cursorIndex(0),
activePokemon(-1), activePokemon_new(-1),
selectSkill(0),
playerdirection(2),
selectItem(-1),
winbattle(0),
catchpokemon(0),
gameover(0),
firstSelect(true),

pokemonRect_0(nullptr), pokemonRect_1(nullptr), pokemonRect_2(nullptr), pokemonRect_3(nullptr)
{
    setFixedSize(525, 450);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scenes["title"] = new QGraphicsScene(this);
    scenes["town"] = new QGraphicsScene(this);
    scenes["laboratory"] = new QGraphicsScene(this);
    scenes["grassland"] = new QGraphicsScene(this);
    scenes["battle"] = new QGraphicsScene(this);
    qsrand(QTime::currentTime().msec()); // Seed random number generator
    setupTitleScene();
    switchScene("title");
    setFocus();
}

Game::~Game() {
    delete player;
    for (Pokemon* pokemon : pokemonList) {
        delete pokemon;
    }
    // Qt manages scene items, so no need to delete playerItem, menuBox, menuText, etc.
}

void Game::keyPressEvent(QKeyEvent* event) {
    if ((bagMenuVisible || dialogVisible)&& (currentSceneName == "town")) {
        if (event->key() == Qt::Key_A && dialogVisible) {
            hideDialog();
        }
        else if (event->key() == Qt::Key_B && bagMenuVisible) {
            hideBagMenu();
        }
        return;
    }

    if ((bagMenuVisible || dialogVisible) && (currentSceneName == "grassland") && (Go_Battle==false)) {
        if (event->key() == Qt::Key_A && dialogVisible) {
            hideDialog();
        }
        else if (event->key() == Qt::Key_B && bagMenuVisible) {
            hideBagMenu();
        }
        return;
    }

    if (currentSceneName == "title") {
        switchScene("town");
    }
    else if (currentSceneName == "town") {
        handleTownKeyPress(event);
    }
    else if (currentSceneName == "laboratory") {
        if (event->key() == Qt::Key_Escape  && (!bagMenuVisible) && (!dialogVisible)) {
            setPlayerbacktotown_lab();
          //  if (player->x_town < 715) { player->x = 700; player->y = 810; }     //force the player position after switch to town
          //  else if (player->x_town >= 750) { player->x = 770; player->y = 810; }//force the player position after switch to town
          //  else { player->x = 720; player->y = 830; }//force the player position after switch to town

          //  player->x = player->x_town;
          //  player->y = player->y_town;
            switchScene("town");
        }
        else if (bagMenuVisible) {
            if (event->key() == Qt::Key_B) {
                hideBagMenu();
            }
            return;
        }
        else
            handleLabKeyPress(event);
    }
    else if (currentSceneName == "grassland") {
        if (event->key() == Qt::Key_Escape && (!bagMenuVisible) && (!dialogVisible)) {
            setPlayerbacktotown_land();
            switchScene("town");
        }
        else if (bagMenuVisible) {
            if (event->key() == Qt::Key_B) {
                hideBagMenu();
            }
            return;
        }
        else 
            handleGrassLandKeyPress(event);
    }
    else if (currentSceneName == "battle") {
        if (event->key() == Qt::Key_Escape && (!bagMenuVisible) && (!dialogVisible)) {
            if (pokemonMenuVisible)  hidePokemonMenu();

            setPlayerbacktograssland();
            switchScene("grassland");
        }
        else if (bagMenuVisible) {
            if (event->key() == Qt::Key_B) {
                hideBagMenu();
            }
            return;
        }
        else if (dialogVisible) {
            hideDialog();
            if (event->key() == Qt::Key_A) {
                if (gameover) { exit(0); }
                else if (winbattle) {
                    winbattle = 0;
                    setPlayerbacktograssland();
                    switchScene("grassland");
                }
                else if (catchpokemon){
                    catchpokemon = 0;
                    setPlayerbacktograssland();
                    switchScene("grassland");
                }
            }
        }
        else
            handleBattleKeyPress(event);
    }

}

void Game::mousePressEvent(QMouseEvent* event) {
    //auto* scene = scenes["battle"];
    if (currentSceneName != "battle") return;

    QPointF scenePos = event->pos();
    printf("x=%0.f, y=%0.f\n", scenePos.x(), scenePos.y());
    if (fightRect.contains(scenePos)) {
        printf("%s(): Fight!\n", __func__);
        cursorIndex = 0;
        updateCursorPosition();
        selectOption();

        hideYesButton();
        showactivePokemon(activePokemon);
    }
    else if (bagRect.contains(scenePos)) {
        printf("%s(): Bag!\n", __func__);
        cursorIndex = 1;
        updateCursorPosition();
        selectOption();

        hideYesButton();
        showactivePokemon(activePokemon);
    }
    else if (pokemonRect.contains(scenePos)) {
        printf("%s(): Pokemon!\n", __func__);
        cursorIndex = 2;
        updateCursorPosition();
        selectOption();

        hideYesButton();
        showactivePokemon(activePokemon);
    }
    else if (runRect.contains(scenePos)) {
        printf("%s(): Run!\n", __func__);
        cursorIndex = 3;
        updateCursorPosition();
        selectOption();

        hideYesButton();
    }
    else  if (bagRect_0.contains(scenePos) && bagMenuVisible) {
        cursorIndex = 10;
        selectItem  = 0;
        //updateCursorPosition();
        //selectOption();
        printf("%s(): item 0 being selected!\n",__func__);
        showYesButton();
    }
    else if (bagRect_1.contains(scenePos) && bagMenuVisible) {
        cursorIndex = 11;
        selectItem  = 1;
        //updateCursorPosition();
        //selectOption();
        printf("%s(): item 1 being selected!\n",__func__);
        showYesButton();
    }
    else if (bagRect_2.contains(scenePos) && bagMenuVisible) {
        cursorIndex = 12;
        selectItem  = 2;
        //updateCursorPosition();
        //selectOption();
        printf("%s(): item 2 being selected!\n",__func__);
        showYesButton();
    }
    else if (skillRect_0.contains(scenePos) && skillMenuVisible && pokemonList[activePokemon]->moves[0]) {
        showSkillpp(0);
        if (pokemonList[activePokemon]->pp[0]) {
            cursorIndex = 40;
            selectSkill = 0;
            printf("%s(): skill %d being selected! selectSkill=%d\n", __func__, selectSkill, selectSkill);
            showYesButton();
        }
        else {
            printf("%s(): skill PP=0, cannot use this skill\n", __func__);
            return;
        }
    }
    else if (skillRect_1.contains(scenePos) && skillMenuVisible && pokemonList[activePokemon]->moves[1]) {
        showSkillpp(1);
        if (pokemonList[activePokemon]->pp[1]) {
            cursorIndex = 41;
            selectSkill = 1;
            printf("%s(): skill %d being selected! selectSkill=%d\n", __func__, selectSkill, selectSkill);
            showYesButton();
        }
        else {
            printf("%s(): skill PP=0, cannot use this skill\n", __func__); 
            return;
        }
    }
    else if (skillRect_2.contains(scenePos) && skillMenuVisible && pokemonList[activePokemon]->moves[2]) {
        showSkillpp(2);
        if (pokemonList[activePokemon]->pp[2]) {
            cursorIndex = 42;
            selectSkill = 2;
            printf("%s(): skill %d being selected! selectSkill=%d\n", __func__, selectSkill, selectSkill);
            showYesButton();
        }
        else {
            printf("%s(): skill PP=0, cannot use this skill\n", __func__);
            return;
        }
    }
    else if (skillRect_3.contains(scenePos) && skillMenuVisible && pokemonList[activePokemon]->moves[3]) {
        showSkillpp(3);
        if (pokemonList[activePokemon]->pp[3]) {
            cursorIndex = 43;
            selectSkill = 3;
            printf("%s(): skill %d being selected! selectSkill=%d\n", __func__, selectSkill, selectSkill);
            showYesButton();
        }
        else {
            printf("%s(): skill PP=0, cannot use this skill\n", __func__);
            return;
        }
    }

    if (YesRect) {
        //printf("%s(): YesRect \n", __func__);
        if (YesRect->contains(scenePos) && (skillMenuVisible || bagMenuVisible || pokemonMenuVisible))
        {
            hideYesButton();
            if (skillMenuVisible) {
                firstSelect = false;
                printf("%s(): use skill %d\n", __func__, selectSkill);
                hideSkillMenu();
                player_attack();
                showWildPokemon_HP();
                if (wildPokemon->hp == 0) {
                    if (pokemonList[activePokemon]->lvl < 6)
                    {
                        showDialog("You win the battle! Lv Up!"); winbattle = 1;
                        pokemonList[activePokemon]->lvl++;
                    }
                    else {
                        pokemonList[activePokemon]->lvl = 6;
                        showDialog("You win the battle!"); winbattle = 1;
                    }

                    updatelvlup();
                    showactivePokemon(activePokemon);
                    return;
                }
                wildpokemon_attack();
                
                checkgameover();
                showactivePokemon(activePokemon);
            }
            else if (bagMenuVisible) {
                firstSelect = false;
                printf("%s(): use item cursorIndex=%d\n", __func__, cursorIndex);
                hideBagMenu_nopokemon();
                player_useitem();
                if (wildPokemon) {
                    wildpokemon_attack(); 
                    
                    checkgameover();
                    showactivePokemon(activePokemon);
                    showWildPokemon_HP();
                }
                else {
                    showDialog("You catch a new pokemon!");
                    catchpokemon = 1;// cursorIndex = 3;//wildPokemon being caught! not more fight 
                }
                
            }
            else {
                activePokemon = activePokemon_new;
                printf("%s(): change Pokemon to activePokemon=%d\n", __func__, activePokemon);
                hidePokemonMenu();
                if (!firstSelect)  wildpokemon_attack();
                else              firstSelect = false;
                
                checkgameover();
                showactivePokemon(activePokemon);
                showWildPokemon_HP();
                
            }
            return;
        }
    }
 
    if (pokemonRect_0) {
       // printf("%s(): PokemonRect0 \n", __func__);
        if (pokemonRect_0->contains(scenePos) && pokemonMenuVisible) {
            cursorIndex = 20;
            activePokemon_new = 0;
            //updateCursorPosition();
            //selectOption();
            printf("%s(): Pokemon 0 being selected! activePokemon_new=%d\n", __func__, activePokemon_new);
            showactivePokemon(activePokemon_new);
            hideYesButton();
            if (activePokemon != activePokemon_new) showYesButton();
            return;
        }
    }

    if (pokemonRect_1) {
       // printf("%s(): PokemonRect1 \n", __func__);
        if (pokemonRect_1->contains(scenePos) && pokemonMenuVisible) {
            cursorIndex = 21;
            activePokemon_new = 1;
            //updateCursorPosition();
            //selectOption();
            printf("%s(): Pokemon 1 being selected! activePokemon_new=%d\n", __func__, activePokemon_new);
            showactivePokemon(activePokemon_new);
            hideYesButton();
            if (activePokemon != activePokemon_new) showYesButton();
            return;
        }
    }
    
    if (pokemonRect_2) {
      //  printf("%s(): PokemonRect2 \n", __func__);
        if (pokemonRect_2->contains(scenePos) && pokemonMenuVisible) {
            cursorIndex = 22;
            activePokemon_new = 2;
            //updateCursorPosition();
            //selectOption();
            printf("%s(): Pokemon 2 being selected! activePokemon_new=%d\n", __func__, activePokemon_new);
            showactivePokemon(activePokemon_new);
            hideYesButton();
            if (activePokemon != activePokemon_new) showYesButton();
            return;
        }
    }

    if (pokemonRect_3) {
       //     printf("%s(): PokemonRect3 \n", __func__);
            if (pokemonRect_3->contains(scenePos) && pokemonMenuVisible) {
                cursorIndex = 23;
                activePokemon_new = 3;
                //updateCursorPosition();
                //selectOption();
                printf("%s(): Pokemon 3 being selected! activePokemon_new=%d\n", __func__, activePokemon_new);
                showactivePokemon(activePokemon_new);
                hideYesButton();
                if (activePokemon != activePokemon_new) showYesButton();
                return;
            }
     }

    return;

//    updatePlayerPosition_battle();

    QWidget::mousePressEvent(event);
}
/////////////////////////////////////////////
void Game::updatelvlup() {
    if (activePokemon >= 0) {
        Pokemon* p = pokemonList[activePokemon];
        if (p->lvl < 6) {
            //p->lvl++;
            if (p->lvl > 1) p->moves[1] = 1;
            if (p->lvl > 2) p->moves[2] = 1;
            if (p->lvl > 3) p->moves[3] = 1;
            if (p->lvl > 2) { p->maxHp = 80; p->attack = 10; p->defence = 10; }
            if (p->lvl > 4) { p->maxHp = 100; p->attack = 15; p->defence = 15; }
            printf("%s(): player pokemon level-up to %d\n", __func__, p->lvl);
        }
    }
}
/////////////////////////////////////////////
void Game::showgameover() {
    auto* scene = scenes["battle"];
    scene->setSceneRect(0, 0, 525, 450);
    auto* background = createPixmapItem("game_over.png", 0, 0, 525, 450, Qt::black);
    background->setZValue(100); // Ensure background is behind all other items
    scene->addItem(background);
}
////////////////////////////////////////////
int Game::checkgameover() {
    
    if (activePokemon >= 0) {
        if (pokemonList[activePokemon]->hp > 0) { return(0); }

        int pindex = 0;
        int over = 1;
        for (const auto* pokemon : pokemonList) {
            if (pokemon->hp > 0) { activePokemon = pindex; over = 0; showDialog("Player change to another Pokemon!"); return(0); }
            else pindex++;
        }
        if (over) { showDialog("Game over!"); showgameover(); gameover = 1; return(1); }
        else      return(0);
    }
    return(0);
}
/////////////////////////////////////////////
void Game::player_attack() {
    Pokemon* p = pokemonList[activePokemon];
    printf("%s(): player pokemon attack Start!\n", __func__);
    msleep(100);
    if (p->pp[selectSkill] > 0)
    {
        p->pp[selectSkill]--;
        int damage = (p->power[selectSkill] + p->attack - wildPokemon->defence)*p->lvl;
        if (damage > wildPokemon->hp) wildPokemon->hp = 0;
        else if(damage>0)             wildPokemon->hp -= damage;

        p->exp += 20;
    }
    else printf("%s(): Error: playerPokemon not enough PP!\n",__func__);
    printf("%s(): player pokemon attack End!\n", __func__);
    if ((p->exp >= 100+p->lvl*20) /*&& (p->lvl<6) */ ) { //level-up wildPokemon
        p->exp -= (100 + p->lvl * 20);
        p->lvl++;
        if (p->lvl > 1) p->moves[1] = 1;
        if (p->lvl > 2) p->moves[2] = 1;
        if (p->lvl > 3) p->moves[3] = 1;
        if (p->lvl > 2) { p->maxHp = 80; p->attack = 10; p->defence = 10; }
        if (p->lvl > 4) { p->maxHp = 100; p->attack = 15; p->defence = 15; }
        printf("%s(): player pokemon level-up to %d\n", __func__, p->lvl);
    }
}
////////////////////////////////////////////////////////
void Game::copywildpokemon() {
    caughtPokemon = new Pokemon(wildPokemon->name, 30, 30, 5, 5);
    printf("%s(): name=%s, lvl=%d, hp=%d, exp=%d\n", __func__, wildPokemon->name.toStdString().c_str(), wildPokemon->lvl, wildPokemon->hp, wildPokemon->exp);
    caughtPokemon->lvl  = wildPokemon->lvl;
    caughtPokemon->hp   = wildPokemon->hp;
    caughtPokemon->pp[0]= wildPokemon->pp[0];
    caughtPokemon->exp  = wildPokemon->exp;
}
//////////////////////////////////////////////////
void Game::player_useitem() {
    Pokemon* p = pokemonList[activePokemon];
    printf("%s(%d): wild Pokemon: %s\n", __func__, __LINE__, wildPokemon->name.toStdString().c_str());

    if (selectItem == 0) {
        printf("%s(): player use PokeBall!\n",__func__);
        
        if (wildPokemon->hp < 20 || ((qrand() % 3) == 0)) {
            if (pokemonList.count() >= 4) { printf("%s(): cannot get the Wild Pokemon, max Pokemon=4\n", __func__); return; }
            else {
                if (inventory.value("Poke Ball", 0) > 0) {
                    inventory["Poke Ball"]--;
                    printf("%s(): You use Pokeball!\n", __func__);
                }
                else {
                    printf("%s(): Error! Poke Ball number = 0, cannot use Pokeball\n", __func__); return;
                };

                printf("%s():You Caught wild Pokemon: %s\n", __func__, wildPokemon->name.toStdString().c_str());
                copywildpokemon();
                pokemonList.append(caughtPokemon);
                delete wildPokemon;
                wildPokemon = nullptr;
            }
        }
        else if (inventory.value("Poke Ball") > 0) inventory["Poke Ball"]--;
    }
    else if (selectItem == 1) {
        if (inventory.value("Potion", 0) > 0) { 
            inventory["Potion"]--; 
            printf("%s(): player use Potion!\n", __func__); 
        }
        else {
            printf("%s(): Error! Potion number=0, cannot use Potion\n", __func__); 
            return;
        }

        if ((p->hp + 10) <= p->maxHp) p->hp += 10;
        else                          p->hp = p->maxHp;
    }
    else if (selectItem == 2) {
        if (inventory.value("Ether", 0) > 0) {
            inventory["Ether"]--;
            printf("%s(): player use Ether!\n", __func__);
            p->pp[selectSkill] = p->maxpp[selectSkill];
        }
        else {
            printf("%s(): Error! Ether number=0, cannot use Ether\n", __func__);
            return;
        }
    }
    return;
}
/////////////////////////////////////////////////////
void Game::wildpokemon_attack() {
    Pokemon* p  = pokemonList[activePokemon];
    printf("%s(): wild pokemon attack Start!\n", __func__);
    msleep(100);
    if (wildPokemon->pp[0] > 0) {
        wildPokemon->pp[0]--;
        int damage = (wildPokemon->power[0] + wildPokemon->attack - p->defence) * wildPokemon->lvl;
        if (damage > p->hp) p->hp = 0;
        else if(damage>0)   p->hp -= damage;

        //wildPokemon->exp += 20; //don't accumulate wildPokemon exp
    }
    else printf("%s(): Error: wildPokemon not enough PP!\n",__func__);
    printf("%s(): wild pokemon attack End!\n", __func__);

    if ((wildPokemon->exp >= 100+wildPokemon->lvl*20) && (wildPokemon->lvl<6)) { //level-up wildPokemon
            wildPokemon->exp -= 100 + wildPokemon->lvl * 20;
            wildPokemon->lvl++;
        if (wildPokemon->lvl > 1) wildPokemon->moves[1] = 1;
        if (wildPokemon->lvl > 2) wildPokemon->moves[2] = 1;
        if (wildPokemon->lvl > 3) wildPokemon->moves[3] = 1;
        if (wildPokemon->lvl > 2) { wildPokemon->maxHp =  80; wildPokemon->attack = 10; wildPokemon->defence = 10;}
        if (wildPokemon->lvl > 4) { wildPokemon->maxHp = 100; wildPokemon->attack = 15; wildPokemon->defence = 15;}
        printf("%s(): wild pokemon level-up to %d\n", __func__, wildPokemon->lvl);
    }

    if (activePokemon >= 0) {
        Pokemon* p;
        p = pokemonList[activePokemon];
        p->exp += 20;
        if (p->exp >= (100 + p->lvl * 20)) {
            p->exp -= (100 + p->lvl * 20);
            if(p->lvl < 6)  p->lvl++;

            if (p->lvl >= 2) p->moves[1] = 1;
            if (p->lvl >= 3) p->moves[2] = 1;
            if (p->lvl >= 4) p->moves[3] = 1;
        }
    }

    return;
}
/////////////////////////////////////////////////////////
void Game::showWildPokemon() {
    hideWildPokemon();
    printf("%s(): show wildp, Encounter_Pokemon=%d\n", __func__,Encounter_Pokemon);
    if (Encounter_Pokemon == 1) {
        if(wildPokemon->lvl<=2)       wildp = createPixmapItem("squirtle.png", 330, 30, 150, 150, Qt::black);
        else if(wildPokemon->lvl<= 4) wildp = createPixmapItem("wartortle.png", 330, 30, 150, 150, Qt::black);
        else                          wildp = createPixmapItem("blastoise.png", 330, 30, 150, 150, Qt::black);
    }
    else if (Encounter_Pokemon == 2) {
        if(wildPokemon->lvl <= 2)        wildp = createPixmapItem("charmander.png", 330, 30, 150, 150, Qt::black);
        else if (wildPokemon->lvl <= 4)  wildp = createPixmapItem("charmeleon.png", 330, 30, 150, 150, Qt::black);
        else                             wildp = createPixmapItem("charizard.png", 330, 30, 150, 150, Qt::black);
        
    }
    else { 
             if(wildPokemon->lvl <= 2) wildp = createPixmapItem("bulbasaur.png", 330, 30, 150, 150, Qt::black); 
        else if (wildPokemon->lvl <= 4) wildp = createPixmapItem("ivysaur.png", 330, 30, 150, 150, Qt::black);
        else                            wildp = createPixmapItem("venusaur.png", 330, 30, 150, 150, Qt::black);
    }
    //wildp->setZValue(-100); // Ensure background is behind all other items
    currentScene->addItem(wildp);
}

void Game::hideWildPokemon() {
    if (wildp) { 
        printf("%s(): remove wildp\n", __func__);
        currentScene->removeItem(wildp); 
        delete wildp; 
        wildp = nullptr; 
    }
    return;
}

void Game::msleep(float msec)
{
    printf("%s() : sleep %.0f msec\n",__func__, msec);
    usleep(msec*1000);
    return;
    //QThread::msleep(msec);
}

void Game::updateCursorPosition() {
    // Update cursor position based on cursorIndex
    if (cursorIndex == 0) { // FIGHT
        cursor->setPos(340, 370);
    }
    else if (cursorIndex == 1) { // BAG
        cursor->setPos(420, 370);
    }
    else if (cursorIndex == 2) { // POKÉMON
        cursor->setPos(340, 400);
    }
    else if (cursorIndex == 3) { // RUN
        cursor->setPos(420, 400);
    }
}

void Game::selectOption() {
    // Update text box based on selected option
    switch (cursorIndex) {
    case 0: // FIGHT
        battleText->setPlainText("select a move!");
        if (pokemonMenuVisible)  hidePokemonMenu();
        if (bagMenuVisible)      hideBagMenu_nopokemon();

        if (skillMenuVisible)    hideSkillMenu();
        else                     showSkillMenu();

        break;
    case 1: // BAG
        battleText->setPlainText("select an item!");
        if (pokemonMenuVisible)  hidePokemonMenu();
        if (skillMenuVisible)  hideSkillMenu();

        if (bagMenuVisible)      hideBagMenu_nopokemon();
        else                     showBagMenu_nopokemon();
        break;
    case 2: // POKEMON
        battleText->setPlainText("select a POKEMON!");
        if (bagMenuVisible)      hideBagMenu_nopokemon();
        if (skillMenuVisible)    hideSkillMenu();

        if (pokemonMenuVisible)
            hidePokemonMenu();
        else
            showPokemonMenu();
        break;
    case 3: // RUN
        battleText->setPlainText("You ran away!");
        if (pokemonMenuVisible)  hidePokemonMenu();
        if (bagMenuVisible)      hideBagMenu_nopokemon();
        if (skillMenuVisible)    hideSkillMenu();

        setPlayerbacktograssland();
        switchScene("grassland");
        centerOn(player->x + 17, player->y + 24);
        updatePlayerPosition_land();
        break;
    }
}

QGraphicsPixmapItem* Game::createPixmapItem(const QString& fileName, qreal x, qreal y, qreal w, qreal h, const QColor& fallbackColor) {
    QPixmap pixmap(fileName);
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem();
    if (!pixmap.isNull()) {
        item->setPixmap(pixmap.scaled(w, h, Qt::IgnoreAspectRatio));
    }
    else {
        qDebug() << "Failed to load" << fileName << "- using fallback";
        QPixmap fallback(w, h);
        fallback.fill(fallbackColor);
        item->setPixmap(fallback);
    }
    item->setPos(x, y);
    return item;
}

void Game::switchScene(const QString& sceneName) {
    if (scenes.contains(sceneName)) {
        // Remove playerItem from current scene if it exists
        if (playerItem && currentScene) {
            currentScene->removeItem(playerItem);
            delete playerItem;
            playerItem = nullptr;
        }
        //if (wildp && currentScene) { currentScene->removeItem(wildp); delete wildp; wildp = nullptr; }
        //if (pokemonItemActive && currentScene) {
        //    currentScene->removeItem(pokemonItemActive);
        //    delete pokemonItemActive;
        //    pokemonItemActive = nullptr;
        //}

        currentSceneName = sceneName;
        currentScene = scenes[sceneName];
        setScene(currentScene);
        barriers.clear();
        Lbarriers.clear();
        //WildPokemons.clear(); printf("clear WildPokemons\n");
        TallGrass1.clear();
        TallGrass2.clear();
        TallGrass3.clear();
        TallGrass4.clear();
        TallGrass5.clear();
        TallGrass6.clear();
        //

        //yellowBoxes.clear();
        playerItem = nullptr; // Ensure playerItem is null for new scene
        if (sceneName == "town") {
            setupTownScene();
        }
        else if (sceneName == "laboratory") {
            setupLaboratoryScene();
        }
        else if (sceneName == "grassland") {
            setupGrasslandScene();
        }
        else if (sceneName == "battle") {
            setupBattleScene();
        }
        show();
    }
}

void Game::setupTitleScene() {
    auto* scene = scenes["title"];
    scene->setBackgroundBrush(QBrush(Qt::black));
    //auto* title = new QGraphicsTextItem("Pokemon");
    //title->setFont(QFont("Arial", 36));
    //title->setDefaultTextColor(Qt::yellow);
    //title->setPos(100, 100);
    //scene->addItem(title);
    //auto* version = new QGraphicsTextItem("FireRed Version");
    //version->setFont(QFont("Arial", 24));
    //version->setDefaultTextColor(Qt::red);
    //version->setPos(100, 150);
    //scene->addItem(version);
    // Placeholder for FireRed monster image
    //auto* monster = new QGraphicsRectItem(100, 200, 200, 200);
    //monster->setBrush(QBrush(Qt::red));
    //scene->addItem(monster);
    // title background
    auto* titlebackground = createPixmapItem("start_menu.png", 0, 0, 525, 450, Qt::green);
    titlebackground->setZValue(-100); // Ensure background is behind all other items
    scene->addItem(titlebackground);

    
}

void Game::setupTownScene() {
    auto* scene = scenes["town"];
    scene->setSceneRect(0, 0, 1000, 1000);
    // Set grass.jpg as background
    auto* background = createPixmapItem("Town.png", 0, 0, 1000, 1000, Qt::green);
    background->setZValue(-100); // Ensure background is behind all other items
    scene->addItem(background);
    // Trees around the town
    /*for (int x = 0; x < 1000; x += 50) {
        if (x == 500) continue; // Gap at top center
        auto* treeTop = createPixmapItem("tree.jpg", x, 0, 50, 50, Qt::darkGreen);
        scene->addItem(treeTop);
        QGraphicsRectItem* barrier = new QGraphicsRectItem(x, 0, 50, 50);
       // barrier->setVisible(false);
        scene->addItem(barrier);
        barriers.append(barrier);
    }*/
    QGraphicsRectItem* barrier = new QGraphicsRectItem(0, 0, 487, 92);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

     barrier = new QGraphicsRectItem(582, 0, 417, 92);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

     barrier = new QGraphicsRectItem(0, 0, 90, 999);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

     barrier = new QGraphicsRectItem(0, 961, 237, 38);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

     barrier = new QGraphicsRectItem(909, 0, 90, 999);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

     barrier = new QGraphicsRectItem(761, 961, 238, 38);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

    barrier = new QGraphicsRectItem(237, 998, 524, 1);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

    barrier = new QGraphicsRectItem(208, 173, 415-208, 397-173);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

    barrier = new QGraphicsRectItem(165, 347, 208-165, 397 - 347);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

    barrier = new QGraphicsRectItem(582, 173, 790 - 582, 397 - 173);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

    barrier = new QGraphicsRectItem(541, 347, 582-541, 397 - 347);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

    barrier = new QGraphicsRectItem(537, 478, 830-537, 696-478-20);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

    barrier = new QGraphicsRectItem(209, 558, 414-209, 16);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

   /* barrier = new QGraphicsRectItem(209, 705, 247 - 209, 742 - 705);
    // barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);*/

    barrier = new QGraphicsRectItem(540, 807, 787-540, 19);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

    barrier = new QGraphicsRectItem(293, 858, 453-293, 999-858);
     barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

















    /*for (int x = 0; x < 1000; x += 50) {
        auto* treeBottom = createPixmapItem("tree.jpg", x, 950, 50, 50, Qt::darkGreen);
        scene->addItem(treeBottom);
        QGraphicsRectItem* barrier = new QGraphicsRectItem(x, 950, 50, 50);
       // barrier->setVisible(false);
        scene->addItem(barrier);
        barriers.append(barrier);
    }
    for (int y = 0; y < 1000; y += 50) {
        auto* treeLeft = createPixmapItem("tree.jpg", 0, y, 50, 50, Qt::darkGreen);
        scene->addItem(treeLeft);
        QGraphicsRectItem* barrier = new QGraphicsRectItem(0, y, 50, 50);
        //barrier->setVisible(false);
        scene->addItem(barrier);
        barriers.append(barrier);
        auto* treeRight = createPixmapItem("tree.jpg", 950, y, 50, 50, Qt::darkGreen);
        scene->addItem(treeRight);
        barrier = new QGraphicsRectItem(950, y, 50, 50);
       // barrier->setVisible(false);
        scene->addItem(barrier);
        barriers.append(barrier);
    }*/
    // Scene transition to Grassland
    auto* transition2 = createPixmapItem("transition_point.jpg", 534, 10, 30, 30, Qt::blue);
    scene->addItem(transition2);
    auto* num2 = new QGraphicsTextItem("2");
    num2->setFont(QFont("Arial", 12));
    num2->setDefaultTextColor(Qt::white);
    num2->setPos(540, 10);
    scene->addItem(num2);
    // Houses
   ///* auto* house1 = createPixmapItem("house.jpg", 100, 50, 200, 200, QColor(139, 69, 19));
   // scene->addItem(house1);
   // auto* house1Barrier = new QGraphicsRectItem(100, 50, 200, 200);
   //// house1Barrier->setVisible(false);
   // scene->addItem(house1Barrier);
   // barriers.append(house1Barrier);*/
   ///*auto* house2 = createPixmapItem("house.jpg", 700, 50, 200, 200, QColor(139, 69, 19));
   // scene->addItem(house2);
   // auto* house2Barrier = new QGraphicsRectItem(700, 50, 200, 200);
   //// house2Barrier->setVisible(false);
   // scene->addItem(house2Barrier);
   // barriers.append(house2Barrier);*/
   // //pond
   // /*auto* pond = createPixmapItem("pond.jpg", 300, 850, 100, 100, QColor(139, 69, 19));
   // scene->addItem(pond);
   // auto* pondBarrier = new QGraphicsRectItem(300, 850, 100, 100);
   // //pondBarrier->setVisible(false);
   // scene->addItem(pondBarrier);
   // barriers.append(pondBarrier);*/

    // Bulletin Boards for Houses
    auto* board1 = createPixmapItem("bulletin_board.jpg", 374, 558, 40, 30, Qt::gray);
    board1->setVisible(false);
    scene->addItem(board1);
    auto* board1Barrier = new QGraphicsRectItem(374, 558, 40, 30);
    board1Barrier->setVisible(false);
    scene->addItem(board1Barrier);
    barriers.append(board1Barrier);
    auto* board2 = createPixmapItem("bulletin_board.jpg", 666, 807, 40, 30, Qt::gray);
    board2->setVisible(false);
    scene->addItem(board2);
    auto* board2Barrier = new QGraphicsRectItem(666, 807, 40, 30);
    board2Barrier->setVisible(false);
    scene->addItem(board2Barrier);
    barriers.append(board2Barrier);
    // Garden
   // auto* garden = createPixmapItem("garden.jpg", 100, 600, 200, 200, QColor(144, 238, 144));
    //scene->addItem(garden);
    //auto* gardenBarrier = new QGraphicsRectItem(100, 600, 200, 200);
    //gardenBarrier->setVisible(false);
    //scene->addItem(gardenBarrier);
    //barriers.append(gardenBarrier);
    auto* gardenBox = createPixmapItem("mailbox2.jpg", 209, 705, 40, 32, Qt::gray);
    gardenBox->setVisible(false);
    scene->addItem(gardenBox);
    auto* gardenBoxBarrier = new QGraphicsRectItem(209, 705, 40, 16);
    gardenBoxBarrier->setVisible(false);
    scene->addItem(gardenBoxBarrier);
    barriers.append(gardenBoxBarrier);

   // auto* fence = createPixmapItem("fence.jpg", 100, 550, 200, 50, Qt::darkGray);
  //  scene->addItem(fence);
  //  auto* fenceBarrier = new QGraphicsRectItem(100, 550, 200, 50);
   // fenceBarrier->setVisible(false);
   // scene->addItem(fenceBarrier);
   // barriers.append(fenceBarrier);
   // auto* gardenBoard = createPixmapItem("bulletin_board.jpg", 260, 560, 40, 30, Qt::gray);
   // scene->addItem(gardenBoard);
  //  auto* gardenBoardBarrier = new QGraphicsRectItem(260, 560, 40, 30);
   // gardenBoardBarrier->setVisible(false);
   // scene->addItem(gardenBoardBarrier);
   // barriers.append(gardenBoardBarrier);
    // Laboratory
    //auto* lab = createPixmapItem("laboratory.jpg", 600, 600, 300, 200, Qt::white);
    //scene->addItem(lab);
   // auto* labBarrier = new QGraphicsRectItem(600, 600, 300, 200);
   // labBarrier->setVisible(false);
   // scene->addItem(labBarrier);
  //  barriers.append(labBarrier);
    auto* transition1 = createPixmapItem("transition_point.jpg", 669, 696, 30, 30, Qt::blue);
    scene->addItem(transition1);
    auto* num1 = new QGraphicsTextItem("1");
    num1->setFont(QFont("Arial", 12));
    num1->setDefaultTextColor(Qt::white);
    num1->setPos(674, 711-10);
    scene->addItem(num1);
    // Yellow Boxes
    if(!yellowBox_ready) placeYellowBoxes(scene);
    // Player
    playerItem = createPixmapItem("player_B.png", player->x, player->y, 35, 48, Qt::blue);
    scene->addItem(playerItem);
    // Center view on player
    centerOn(player->x + 17, player->y + 24);
}

void Game::placeYellowBoxes(QGraphicsScene* scene) {

    for (QGraphicsPixmapItem* removebox : yellowBoxes) {

        scene->removeItem(removebox);
        yellowBoxes.removeOne(removebox);
        delete removebox;

    }

    for (int i = 0; i < 15; ++i) {
        bool valid = false;
        QGraphicsPixmapItem* box = nullptr;
        qreal bx, by;
        while (!valid) {
            bx = (qrand() % (950 - 50)) + 50;
            by = (qrand() % (950 - 50)) + 50;
            box = createPixmapItem("box.png", bx, by, 32, 32, Qt::yellow);
            QRectF boxRect(bx, by, 32, 32);
            valid = true;
            for (const auto* barrier : barriers) {
                if (boxRect.intersects(barrier->rect())) {
                    valid = false;
                    delete box;
                    break;
                }
            }
        }
        scene->addItem(box);
        yellowBoxes.append(box);
    }
    //yellowBox_ready = true;
}

void Game::setupLaboratoryScene() {
    auto* scene = scenes["laboratory"];
    scene->setSceneRect(0, 0, 438, 455);
    //scene->setSceneRect(0, 0, 525, 450);
    scene->setBackgroundBrush(QBrush(Qt::black));
    //auto* text = new QGraphicsTextItem("Laboratory - Press Escape to return");
    //text->setFont(QFont("Arial", 16));
    //text->setPos(50, 50);
    //scene->addItem(text);
    printf("%s(1)\n", __func__);
    // Set lab.jpg as background
    auto* background = createPixmapItem("Lab.png", 0, 0, 438, 455, Qt::black);
  //auto* background = createPixmapItem("lab_empty.jpg", 0, 0, 525, 450, Qt::black);
    background->setZValue(-100); // Ensure background is behind all other items
    scene->addItem(background);
    printf("%s(2)\n", __func__);

    // Set lab_top
   // auto* labTop = createPixmapItem("lab_top.jpg", 0, 0, 438, 70, Qt::black);
  //auto* labTop = createPixmapItem("lab_top.jpg", 0, 0, 525, 60, Qt::black);
  //  labTop->setVisible(false);
  //  scene->addItem(labTop);
    QGraphicsRectItem* barrierTop = new QGraphicsRectItem(0, 0, 438, 45);
  //QGraphicsRectItem* barrierTop = new QGraphicsRectItem(0, 0, 525, 60);
    barrierTop->setVisible(false);
    scene->addItem(barrierTop);
    barriers.append(barrierTop);
    printf("%s(3)\n", __func__);

    // Set lab_left block
   // auto* labBlock1 = createPixmapItem("lab_block1.jpg", 0, 260, 170, 70, Qt::black);
  //  scene->addItem(labBlock1);
    QGraphicsRectItem* barrierBlock1 = new QGraphicsRectItem(0, 260, 170, 70);
    barrierBlock1->setVisible(false);
    scene->addItem(barrierBlock1);
    barriers.append(barrierBlock1);
    printf("%s(3.1)\n", __func__);

    // Set lab_right block
   // auto* labBlock2 = createPixmapItem("lab_block2.jpg", 270, 260, 170, 70, Qt::black);
   // scene->addItem(labBlock2);
    QGraphicsRectItem* barrierBlock2 = new QGraphicsRectItem(270, 260, 170, 70);
    barrierBlock2->setVisible(false);
    scene->addItem(barrierBlock2);
    barriers.append(barrierBlock2);
    printf("%s(3.2)\n", __func__);

    // Set lab_table block
  //  auto* labTable = createPixmapItem("lab_table.jpg", 260, 120, 120, 40, Qt::black);
   // scene->addItem(labTable);
    QGraphicsRectItem* barrierTable = new QGraphicsRectItem(282, 132, 384-282, 149-132);
    barrierTable->setVisible(false);
    scene->addItem(barrierTable);
    barriers.append(barrierTable);
    printf("%s(3.3)\n", __func__);

    //Set pokeball
    printf("%s(3.5), pokeBall_Mon_ready=%d\n", __func__, pokeBall_Mon_ready);
   if (pokeBall_Mon_ready==false)
    {
       QGraphicsPixmapItem* pokeBall_Mon1 = nullptr;
       QGraphicsPixmapItem* pokeBall_Mon2 = nullptr;
       QGraphicsPixmapItem* pokeBall_Mon3 = nullptr;
        pokeBall_Mon1 = createPixmapItem("ball.png", 270, 140, 21, 30, Qt::black);
        pokeBall_Mon2 = createPixmapItem("ball.png", 310, 140, 21, 30, Qt::black);
        pokeBall_Mon3 = createPixmapItem("ball.png", 350, 140, 21, 30, Qt::black);
        scene->addItem(pokeBall_Mon1);
        scene->addItem(pokeBall_Mon2);
        scene->addItem(pokeBall_Mon3);
        Pokeball_Mons.append(pokeBall_Mon1);
        Pokeball_Mons.append(pokeBall_Mon2);
        Pokeball_Mons.append(pokeBall_Mon3);

        printf("%s(3.3)\n", __func__);
        pokeBall_Mon_ready = true;

        printf("%s(3.6), pokeBall_Mon_ready=%d\n", __func__, pokeBall_Mon_ready);

    }
   else {
       printf("%s(3.7), pokeBall_Mon_ready=%d\n", __func__, pokeBall_Mon_ready);
       for (QGraphicsPixmapItem* lab_ball : Pokeball_Mons)  lab_ball->setZValue(100);//scene->addItem(lab_ball);
       printf("%s(3.8), pokeBall_Mon_ready=%d\n", __func__, pokeBall_Mon_ready);
   }
    // Set reactor block
   // auto* reactor = createPixmapItem("reactor.jpg", 80, 120, 40, 50, Qt::black);
  //  scene->addItem(reactor);
    QGraphicsRectItem* barrierReactor = new QGraphicsRectItem(39, 114, 102-39, 170-114);
    barrierReactor->setVisible(false);
    scene->addItem(barrierReactor);
    barriers.append(barrierReactor);
    printf("%s(3.4)\n", __func__);

    // Set stuff block
    QGraphicsRectItem* barrierstuff = new QGraphicsRectItem(0, 71, 36, 161-71);
    barrierstuff->setVisible(false);
    scene->addItem(barrierstuff);
    barriers.append(barrierstuff);
    printf("%s(3.4)\n", __func__);








    // Set lab_right_tree
   // auto* tree_right = createPixmapItem("lab_tree_right.jpg", 400, 385, 40, 70, Qt::black);
   // scene->addItem(tree_right);
    QGraphicsRectItem* barrierTreeR = new QGraphicsRectItem(400, 400, 40, 70);
    barrierTreeR->setVisible(false);
    scene->addItem(barrierTreeR);
    barriers.append(barrierTreeR);
    printf("%s(3.5)\n", __func__);

    // Set lab_left_tree
   // auto* tree_left = createPixmapItem("lab_tree_left.jpg", 0, 385, 40, 70, Qt::black);
  //  scene->addItem(tree_left);
    QGraphicsRectItem* barrierTreeL = new QGraphicsRectItem(0, 400, 40, 70);
    barrierTreeL->setVisible(false);
    scene->addItem(barrierTreeL);
    barriers.append(barrierTreeL);
    printf("%s(3.6)\n", __func__);

    // Set lab_boundary
    QGraphicsRectItem* barrierLSide = new QGraphicsRectItem(0, 0, 2, 455);
    barrierLSide->setVisible(false);
    scene->addItem(barrierLSide);
    barriers.append(barrierLSide);
    printf("%s(3.7)\n", __func__);

    QGraphicsRectItem* barrierRSide = new QGraphicsRectItem(436, 0, 2, 455);
    barrierRSide->setVisible(false);
    scene->addItem(barrierRSide);
    barriers.append(barrierRSide);
    printf("%s(3.8)\n", __func__);

    QGraphicsRectItem* barrierBottomSide = new QGraphicsRectItem(0, 454, 438, 2);
    barrierBottomSide->setVisible(false);
    scene->addItem(barrierBottomSide);
    barriers.append(barrierBottomSide);
    printf("%s(3.9)\n", __func__);

    auto* NPC = createPixmapItem("NPC.png", 210, 70, 35, 47, Qt::gray);
    scene->addItem(NPC);
    auto* NPCBarrier = new QGraphicsRectItem(215, 70, 25, 38);
    NPCBarrier->setVisible(false);
    scene->addItem(NPCBarrier);
    barriers.append(NPCBarrier);


    // Scene transition to Town
    auto* transition1 = createPixmapItem("transition_point.jpg", 200, 425, 30, 30, Qt::blue);
    scene->addItem(transition1);
    auto* num1 = new QGraphicsTextItem("1");
    num1->setFont(QFont("Arial", 12));
    num1->setDefaultTextColor(Qt::white);
    num1->setPos(210, 430);
    scene->addItem(num1);
    printf("%s(4)\n", __func__);

    // Trees around the town

  // 
    // Player
    playerItem = createPixmapItem("player_B.png", player->x, player->y, 35, 48, Qt::blue);
    scene->addItem(playerItem);
    // Center view on player
    centerOn(player->x + 17, player->y + 24);
    printf("%s(5)\n", __func__);

  //adjust scenePosition according to player's position
    updatePlayerPosition_lab();
}

void Game::setupGrasslandScene() {
    auto* scene = scenes["grassland"];
    scene->setSceneRect(0, 0, 1000, 1667);
    scene->setBackgroundBrush(QBrush(Qt::darkGreen));
    auto* background = createPixmapItem("GrassLand.png", 0, 0, 1000, 1667, Qt::black);

    background->setZValue(-100); // Ensure background is behind all other items
    scene->addItem(background);

    removeWildPokemons();

    for (int x = 0; x < 1000; x += 50) {
       if (x == 500) continue; // Gap at bottom center
       if (x == 550) continue; // Gap at bottom center
      
        QGraphicsRectItem* barrier = new QGraphicsRectItem(x, 1509, 50, 161);
        barrier->setVisible(false);
        scene->addItem(barrier);
        barriers.append(barrier);
    }
    for (int x = 0; x < 1000; x += 50) {
      
        QGraphicsRectItem* barrier = new QGraphicsRectItem(x, 0, 50, 50);
        barrier->setVisible(false);
        scene->addItem(barrier);
        barriers.append(barrier);
    }
    for (int y = 0; y < 1617; y += 50) {
      
        QGraphicsRectItem* barrier = new QGraphicsRectItem(0, y, 90, 50);
        barrier->setVisible(false);
        scene->addItem(barrier);
        barriers.append(barrier);
     
        barrier = new QGraphicsRectItem(930, y, 50, 50);
        barrier->setVisible(false);
        scene->addItem(barrier);
        barriers.append(barrier);
    }


//tree barrier
    QGraphicsRectItem* barrier = new QGraphicsRectItem(0, 1013, 490, 72-13);
    barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

    barrier = new QGraphicsRectItem(0, 605, 150, 52-5);
    barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

    barrier = new QGraphicsRectItem(440, 605, 644-440, 52 - 5);
    barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

    barrier = new QGraphicsRectItem(346, 134, 404-346, 454-134);
    barrier->setVisible(false);
    scene->addItem(barrier);
    barriers.append(barrier);

//bulletin
    auto* gardenBox = createPixmapItem("mailbox2.jpg", 374, 1297, 40, 32, Qt::gray);
    gardenBox->setVisible(false);
    scene->addItem(gardenBox);
    auto* gardenBoxBarrier = new QGraphicsRectItem(374, 1297, 40, 16);
     gardenBoxBarrier->setVisible(false);
    scene->addItem(gardenBoxBarrier);
    barriers.append(gardenBoxBarrier);







    // Scene transition to Town
    auto* transition2 = createPixmapItem("transition_point.jpg", 534, 1637, 30, 30, Qt::blue);
    scene->addItem(transition2);
    auto* num2 = new QGraphicsTextItem("2");
    num2->setFont(QFont("Arial", 12));
    num2->setDefaultTextColor(Qt::white);
    num2->setPos(540, 1640);
    scene->addItem(num2);
    printf("%s(4)\n", __func__);


    //Ledge
    //QGraphicsPixmapItem* ledge = nullptr;
    QGraphicsRectItem* ledgebarrier = nullptr;
    

    
    ledgebarrier = new QGraphicsRectItem(0, 229, 668, 20);
    ledgebarrier->setVisible(false);
    //scene->addItem(ledge);
    Lbarriers.append(ledgebarrier);

    ledgebarrier = new QGraphicsRectItem(0, 438, 340, 20);
    ledgebarrier->setVisible(false);
    //scene->addItem(ledge);
    Lbarriers.append(ledgebarrier);

    ledgebarrier = new QGraphicsRectItem(0, 646, 457, 20);
    ledgebarrier->setVisible(false);
    //scene->addItem(ledge);
    Lbarriers.append(ledgebarrier);





    ledgebarrier = new QGraphicsRectItem(0, 855, 150, 20);
    ledgebarrier->setVisible(false);
    //scene->addItem(ledge);
    Lbarriers.append(ledgebarrier);

    ledgebarrier = new QGraphicsRectItem(222, 855, 372-222, 20);
    ledgebarrier->setVisible(false);
    //scene->addItem(ledge);
    Lbarriers.append(ledgebarrier);

    ledgebarrier = new QGraphicsRectItem(470, 855, 1152-470, 20);
    ledgebarrier->setVisible(false);
    //scene->addItem(ledge);
    Lbarriers.append(ledgebarrier);





    ledgebarrier = new QGraphicsRectItem(764, 1103, 1152 - 764, 20);
    ledgebarrier->setVisible(false);
    //scene->addItem(ledge);
    Lbarriers.append(ledgebarrier);

    ledgebarrier = new QGraphicsRectItem(0, 1297, 236, 20);
    ledgebarrier->setVisible(false);
    //scene->addItem(ledge);
    Lbarriers.append(ledgebarrier);

    ledgebarrier = new QGraphicsRectItem(412, 1297, 1152-412, 20);
    ledgebarrier->setVisible(false);
    //scene->addItem(ledge);
    Lbarriers.append(ledgebarrier);









    // Tallgrass
    QGraphicsRectItem* tallgrass = nullptr;

    //int x0 = 50; int y0 = 500;
    tallgrass = new QGraphicsRectItem( 460,     292,     400, 407-292);
    tallgrass->setVisible(false);
    scene->addItem(tallgrass);
    TallGrass1.append(tallgrass);
    

    //x0 = 700; y0 = 600;
    tallgrass = new QGraphicsRectItem(720, 592, 870-720, 711-592);
    tallgrass->setVisible(false);
    scene->addItem(tallgrass);
    TallGrass2.append(tallgrass);
    

    //x0 = 200; y0 = 1000;
    tallgrass = new QGraphicsRectItem(540, 1040, 710-540, 1170-1040);
    tallgrass->setVisible(false);
    scene->addItem(tallgrass);
    TallGrass3.append(tallgrass);
    


    //x0 = 100; y0 = 1300;
    tallgrass = new QGraphicsRectItem(505,       1470,      70, 110);
    tallgrass->setVisible(false);
    scene->addItem(tallgrass);
    TallGrass4.append(tallgrass);
    




    //x0 = 100; y0 = 1300;
    tallgrass = new QGraphicsRectItem( 178, 1344, 444-178, 1410-1344);
    tallgrass->setVisible(false);
    scene->addItem(tallgrass);
    TallGrass5.append(tallgrass);
    tallgrass = new QGraphicsRectItem( 120, 1410, 363-120, 1491-1410);
    tallgrass->setVisible(false);
    scene->addItem(tallgrass);
    TallGrass5.append(tallgrass);
    




    //x0 = 100; y0 = 1300;
    tallgrass = new QGraphicsRectItem( 730, 1340, 890-730, 100);
    tallgrass->setVisible(false);
    scene->addItem(tallgrass);
    TallGrass6.append(tallgrass);
    tallgrass = new QGraphicsRectItem(640, 1440, 820-640, 50);
    tallgrass->setVisible(false);
    scene->addItem(tallgrass);
    TallGrass6.append(tallgrass);
  
    //WildPokemons
    QGraphicsPixmapItem* wildpokemon = nullptr;
    int rand1 = qrand() % 350;
    int rand2 = qrand() % 100;
    if (qrand() % 5)
    {
        int rand3 = qrand() % 3;
        Pokemon* pokemontype;
        switch (rand3) {
        case 0: pokemontype = new Pokemon("Squirtle", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("squirtle.png", 460 + rand1, 292 + rand2, 40, 40, Qt::darkGreen);
            break;

        case 1: pokemontype = new Pokemon("Charmander", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("charmander.png", 460 + rand1, 292 + rand2, 40, 40, Qt::darkGreen);
            break;

        default: pokemontype = new Pokemon("Bulbasaur", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("bulbasaur.png", 460 + rand1, 292 + rand2, 40, 40, Qt::darkGreen);
            break;

        }

        wildpokemonList.append(pokemontype);
        scene->addItem(wildpokemon);
        WildPokemons.append(wildpokemon);
    }

    
    if (qrand() % 5)
    {
        rand1 = qrand() % 150;
        rand2 = qrand() % 150;
        int rand3 = qrand() % 3;
        Pokemon* pokemontype;
        switch (rand3) {
        case 0: pokemontype = new Pokemon("Squirtle", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("squirtle.png", 720 + rand1, 592 + rand2, 40, 40, Qt::darkGreen);
            break;

        case 1: pokemontype = new Pokemon("Charmander", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("charmander.png", 720 + rand1, 592 + rand2, 40, 40, Qt::darkGreen);
            break;

        default: pokemontype = new Pokemon("Bulbasaur", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("bulbasaur.png", 720 + rand1, 592 + rand2, 40, 40, Qt::darkGreen);
            break;

        }

        wildpokemonList.append(pokemontype);

        scene->addItem(wildpokemon);
        WildPokemons.append(wildpokemon);
    }

    if (qrand() % 5)
    {
        rand1 = qrand() % 150;
        rand2 = qrand() % 150;

        int rand3 = qrand() % 3;
        Pokemon* pokemontype;
        switch (rand3) {
        case 0: pokemontype = new Pokemon("Squirtle", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("squirtle.png", 540 + rand1, 1040 + rand2, 40, 40, Qt::darkGreen);
            break;

        case 1: pokemontype = new Pokemon("Charmander", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("charmander.png", 540 + rand1, 1040 + rand2, 40, 40, Qt::darkGreen);
            break;

        default: pokemontype = new Pokemon("Bulbasaur", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("bulbasaur.png", 540 + rand1, 1040 + rand2, 40, 40, Qt::darkGreen);
            break;
        }
    

         wildpokemonList.append(pokemontype);


        scene->addItem(wildpokemon);
        WildPokemons.append(wildpokemon);
    }

    if (qrand() % 5)
    {
        rand1 = qrand() % 50;
        rand2 = qrand() % 70;

        int rand3 = qrand() % 3;
        Pokemon* pokemontype;
        switch (rand3) {
        case 0: pokemontype = new Pokemon("Squirtle", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("squirtle.png", 510 + rand1, 1460 + rand2, 40, 40, Qt::darkGreen);
            break;

        case 1: pokemontype = new Pokemon("Charmander", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("charmander.png", 510 + rand1, 1460 + rand2, 40, 40, Qt::darkGreen);
            break;

        default: pokemontype = new Pokemon("Bulbasaur", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("bulbasaur.png", 510 + rand1, 1460 + rand2, 40, 40, Qt::darkGreen);
            break;

        }

            wildpokemonList.append(pokemontype);


        scene->addItem(wildpokemon);
        WildPokemons.append(wildpokemon);
    }

    if (qrand() % 5)
    {
        rand1 = qrand() % 100;
        rand2 = qrand() % 100;

        int rand3 = qrand() % 3;
        Pokemon* pokemontype;
        switch (rand3) {
        case 0: pokemontype = new Pokemon("Squirtle", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("squirtle.png", 177 + rand1, 1349 + rand2, 40, 40, Qt::darkGreen);
            break;

        case 1: pokemontype = new Pokemon("Charmander", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("charmander.png", 177 + rand1, 1349 + rand2, 40, 40, Qt::darkGreen);
            break;

        default: pokemontype = new Pokemon("Bulbasaur", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("bulbasaur.png", 177 + rand1, 1349 + rand2, 40, 40, Qt::darkGreen);
            break;

        }

            wildpokemonList.append(pokemontype);


        scene->addItem(wildpokemon);
        WildPokemons.append(wildpokemon);
    }

    if (qrand() % 5)
    {
        rand1 = qrand() % 70;
        rand2 = qrand() % 90;

        int rand3 = qrand() % 3;
        Pokemon* pokemontype;
        switch (rand3) {
        case 0: pokemontype = new Pokemon("Squirtle", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("squirtle.png", 718 + rand1, 1347 + rand2, 40, 40, Qt::darkGreen);
            break;

        case 1: pokemontype = new Pokemon("Charmander", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("charmander.png", 718 + rand1, 1347 + rand2, 40, 40, Qt::darkGreen);
            break;

        default: pokemontype = new Pokemon("Bulbasaur", 30, 30, 5, 5);
            wildpokemon = createPixmapItem("bulbasaur.png", 718 + rand1, 1347 + rand2, 40, 40, Qt::darkGreen);
            break;

        }

            wildpokemonList.append(pokemontype);

        scene->addItem(wildpokemon);
        WildPokemons.append(wildpokemon);
    }


    // Player
    playerItem = createPixmapItem("player_B.png", player->x, player->y, 35, 48, Qt::darkGreen);
    scene->addItem(playerItem);
    // Center view on player
    centerOn(player->x + 17, player->y + 24);
    updatePlayerPosition_land();

    //auto* text = new QGraphicsTextItem("Grassland - Press Escape to return");
    //text->setFont(QFont("Arial", 16));
    //text->setPos(50, 50);
    //scene->addItem(text);
}

void Game::setupBattleScene() {
    auto* scene = scenes["battle"];
    scene->setSceneRect(0, 0, 525, 450);
    winbattle = 0;
    catchpokemon = 0;
    firstSelect = true;
   
    
    auto* background = createPixmapItem("battle_scene.png", 0, 0, 525, 450, Qt::black);
    background->setZValue(-100); // Ensure background is behind all other items
    scene->addItem(background);

    if (wildp) { scene->removeItem(wildp);      delete wildp; wildp = nullptr; }
    if (wildp_Text) { scene->removeItem(wildp_Text); delete wildp_Text; wildp_Text = nullptr; }
    if (cursor) { scene->removeItem(cursor);     delete cursor; cursor = nullptr; }
    if (battleText) { scene->removeItem(battleText); delete battleText; battleText = nullptr; }
    if (pokemonItemActive) {
        scene->removeItem(pokemonItemActive);
        delete pokemonItemActive;
        pokemonItemActive = nullptr;
    }
    if (pokemonItemActive_Text) {
        scene->removeItem(pokemonItemActive_Text);
        delete pokemonItemActive_Text;
        pokemonItemActive_Text = nullptr;
    }
    if (pokemonItemActive_Text2) {
        scene->removeItem(pokemonItemActive_Text2);
        delete pokemonItemActive_Text2;
        pokemonItemActive_Text2 = nullptr;
    }
    if (pokemonItemActive_Text3) {
        scene->removeItem(pokemonItemActive_Text3);
        delete pokemonItemActive_Text3;
        pokemonItemActive_Text3 = nullptr;
    }
    if (pokemonItemActive_Text4) {
        scene->removeItem(pokemonItemActive_Text4);
        delete pokemonItemActive_Text4;
        pokemonItemActive_Text4 = nullptr;
    }
    if (pokemonItemActive_HP) {
        scene->removeItem(pokemonItemActive_HP);
        delete pokemonItemActive_HP;
        pokemonItemActive_HP = nullptr;
    }

    if (wildPokemon) {
        delete wildPokemon;
        wildPokemon = nullptr;
    }
    //QGraphicsPixmapItem* wildp=nullptr;
    printf("%s(%d): Encounter_Pokemon=%d\n", __func__, __LINE__, Encounter_Pokemon);
    if (Encounter_Pokemon == 1) { 
        wildPokemon = new Pokemon("Squirtle", 30, 30, 5, 5);

        wildp = createPixmapItem("squirtle.png", 330, 25, 150, 150, Qt::black); 
        QString wtext="";
        wtext += "Squirtle  LV " + QString::number(wildPokemon->lvl);
        wildp_Text = new QGraphicsTextItem(wtext);
        wildp_Text->setFont(QFont("Arial", 14));
        wildp_Text->setPos(45, 50);

    }
    else if (Encounter_Pokemon == 2) { 
        //wildPokemon = new Pokemon;
        wildPokemon = new Pokemon("Charmander", 30, 30, 5, 5);

        wildp = createPixmapItem("charmander.png", 330, 25, 150, 150, Qt::black);
        wildp_Text = new QGraphicsTextItem("Charmander  Lv " + QString::number(wildPokemon->lvl));
        wildp_Text->setFont(QFont("Arial", 14));
        wildp_Text->setPos(45, 50);
    }
    else if(Encounter_Pokemon==3) {
        wildPokemon = new Pokemon("Bulbasaur", 30, 30, 5, 5);

        wildp = createPixmapItem("bulbasaur.png", 330, 25, 150, 150, Qt::black);
        wildp_Text = new QGraphicsTextItem("Bulbasaur  Lv "+ QString::number(wildPokemon->lvl));
        wildp_Text->setFont(QFont("Arial", 14));
        wildp_Text->setPos(45, 50);
    }
    else {
        printf("%s(%d): Error! Encounter_Pokemon=%d\n",__func__,__LINE__, Encounter_Pokemon);
    }
    //wildp->setZValue(-100); // Ensure background is behind all other items
    scene->addItem(wildp);
    scene->addItem(wildp_Text);
    showWildPokemon_HP();

    printf("%s(%d): wild Pokemon: %s\n", __func__, __LINE__, wildPokemon->name.toStdString().c_str());
//auto* text = new QGraphicsTextItem("Laboratory - Press Escape to return");
//text->setFont(QFont("Arial", 16));
//text->setPos(50, 50);
//scene->addItem(text);

    battleText = new QGraphicsTextItem("What will Player do?");
    battleText->setFont(QFont("Arial", 16));
    //battleText->setDefaultTextColor(Qt::green);
    battleText->setPos(20, 370);
    scene->addItem(battleText);

    QGraphicsTextItem* fightText = new QGraphicsTextItem("FIGHT");
    fightText->setPos(350, 370);
    scene->addItem(fightText);
    fightRect = QRectF(350, 370, 60, 20);

    QGraphicsTextItem* bagText = new QGraphicsTextItem("BAG");
    bagText->setPos(430, 370);
    scene->addItem(bagText);
    bagRect = QRectF(430, 370, 60, 20);

    QGraphicsTextItem* pokemonText = new QGraphicsTextItem("POKEMON");
    pokemonText->setPos(350, 400);
    scene->addItem(pokemonText);
    pokemonRect = QRectF(350, 400, 60, 20);

    QGraphicsTextItem* runText = new QGraphicsTextItem("RUN");
    runText->setPos(430, 400);
    scene->addItem(runText);
    runRect = QRectF(430, 400, 60, 20);

    cursor = new QGraphicsTextItem(">");
    cursor->setPos(340, 370);
    scene->addItem(cursor);

    // Player
    //playerItem = createPixmapItem("player_B.png", player->x, player->y, 35, 48, Qt::darkGreen);
    //scene->addItem(playerItem);
    // Center view on player
    //centerOn(player->x + 17, player->y + 24);
    //updatePlayerPosition_land();
    int pindex = 0;
    for (const auto* pokemon : pokemonList) {
        QString pokemonText = "";
        if (activePokemon < 0)
        {
            pokemonText += pokemon->name + " (HP: " + QString::number(pokemon->hp) + "/" + QString::number(pokemon->maxHp) + ")";
            if (pindex == 0) {
                activePokemon = 0;

                showactivePokemon(activePokemon);
            }
        }
        else if (activePokemon == pindex) {
            pokemonText += pokemon->name + " (HP: " + QString::number(pokemon->hp) + "/" + QString::number(pokemon->maxHp) + ")";
            activePokemon = pindex;
            showactivePokemon(activePokemon);
        }
            pindex++;
    }
    // auto* text = new QGraphicsTextItem("Grassland - Press Escape to return");
    // text->setFont(QFont("Arial", 16));
    // text->setPos(50, 50);
    // scene->addItem(text);

     updatePlayerPosition_battle();
}


void Game::handleTownKeyPress(QKeyEvent* event) {
    qreal dx = 0, dy = 0;
    qreal speed = 10;
    if (event->key() == Qt::Key_Up) {
        playerdirection = 1;
        dy = -speed;
    }
    else if (event->key() == Qt::Key_Down) {
        playerdirection = 2;
        dy = speed;
    }
    else if (event->key() == Qt::Key_Left) {
        playerdirection = 3;
        dx = -speed;
    }
    else if (event->key() == Qt::Key_Right) {
        playerdirection = 4;
        dx = speed;
    }
    else if (event->key() == Qt::Key_A) {
        checkInteractions();
        return;
    }
    else if (event->key() == Qt::Key_B) {
        showBagMenu();
        return;
    }
    if (dx != 0 || dy != 0) {
        player->move(dx, dy, barriers, Lbarriers);
        updatePlayerPosition();
        checkSceneTransitions();
    }
}

void Game::handleLabKeyPress(QKeyEvent* event) {
    qreal dx = 0, dy = 0;
    qreal speed = 10;
    if (event->key() == Qt::Key_Up && !dialogVisible) {
        playerdirection = 1;
        dy = -speed;
    }
    else if (event->key() == Qt::Key_Down && !dialogVisible) {
        playerdirection = 2;
        dy = speed;
    }
    else if (event->key() == Qt::Key_Left && !dialogVisible) {
        playerdirection = 3;
        dx = -speed;
    }
    else if (event->key() == Qt::Key_Right && !dialogVisible) {
        playerdirection = 4;
        dx = speed;
    }
    else if (event->key() == Qt::Key_A) {
        checkInteractions_lab();
        return;
    }
    else if (event->key() == Qt::Key_B && !dialogVisible) {
        if (bagMenuVisible)  hideBagMenu();
        else                 showBagMenu();
        return;
    }
    if ((dx != 0 || dy != 0) && (!dialogVisible)){
        player->move(dx, dy, barriers, Lbarriers);
        updatePlayerPosition_lab();
        checkSceneTransitions_lab();
    }
}

void Game::handleGrassLandKeyPress(QKeyEvent* event) {
    qreal dx = 0, dy = 0;
    qreal speed = 10;
    if (event->key() == Qt::Key_Up && !dialogVisible) {
        playerdirection = 1;
        dy = -speed;
    }
    else if (event->key() == Qt::Key_Down && !dialogVisible) {
        playerdirection = 2;
        dy = speed;
    }
    else if (event->key() == Qt::Key_Left && !dialogVisible) {
        playerdirection = 3;
        dx = -speed;
    }
    else if (event->key() == Qt::Key_Right && !dialogVisible) {
        playerdirection = 4;
        dx = speed;
    }
    else if (event->key() == Qt::Key_A) {
        checkInteractions_land();
        return;
    }
    else if (event->key() == Qt::Key_B && !dialogVisible) {
        if (bagMenuVisible)  hideBagMenu();
        else                 showBagMenu();
        return;
    }
    if ((dx != 0 || dy != 0) && (!dialogVisible)) {
        player->move(dx, dy, barriers, Lbarriers);
        updatePlayerPosition_land();
        checkSceneTransitions_land();
        checkWildPokemon_land();
    }
}

void Game::handleBattleKeyPress(QKeyEvent* event) {
    //qreal dx;// = 0, 
    //qreal dy;// = 0;
    //qreal speed = 10;
    if (event->key() == Qt::Key_Up && !dialogVisible) {
        //dy = -speed;
    }
    else if (event->key() == Qt::Key_Down && !dialogVisible) {
        //dy = speed;
    }
    else if (event->key() == Qt::Key_Left && !dialogVisible) {
        //dx = -speed;
    }
    else if (event->key() == Qt::Key_Right && !dialogVisible) {
        //dx = speed;
    }
    else if (event->key() == Qt::Key_A) {
        checkInteractions_battle();
        return;
    }
    else if (event->key() == Qt::Key_B && !dialogVisible) {
        if (bagMenuVisible)  hideBagMenu();
        else                 showBagMenu();
        return;
    }
    //if ((dx != 0 || dy != 0) && (!dialogVisible)) {
    //    player->move(dx, dy, barriers, Lbarriers);
    //    updatePlayerPosition_land();
    //    checkSceneTransitions_land();
    //    checkWildPokemon_land();
    //}
}


void Game::updateplayerdirection() {

    if (playerItem) {
        currentScene->removeItem(playerItem);
        delete playerItem;
        playerItem = nullptr;
    }
    switch (playerdirection) {

    case 1: playerItem = createPixmapItem("player_B.png", player->x, player->y, 35, 48, Qt::blue);
        break;
    case 2: playerItem = createPixmapItem("player_F.png", player->x, player->y, 35, 48, Qt::blue);
        break;
    case 3: playerItem = createPixmapItem("player_L.png", player->x, player->y, 35, 48, Qt::blue);
        break;
    default: playerItem = createPixmapItem("player_R.png", player->x, player->y, 35, 48, Qt::blue);
        break;

    }
    currentScene->addItem(playerItem);

}

void Game::updatePlayerPosition() {
    if (!playerItem) {
        qDebug() << "Error: playerItem is null";
        return;
    }
    
    updateplayerdirection();

    playerItem->setPos(player->x, player->y);
    centerOn(player->x + 17, player->y + 24);
    printf("%s(): player_x=%.0f, player_y=%.0f\n", __func__, player->x, player->y);
    // Ensure view stays within map bounds
    QRectF viewRect = sceneRect();
    if (player->x < 262.5) viewRect.setLeft(0);
    if (player->x > 734.5) viewRect.setRight(1000);
    if (player->y < 225) viewRect.setTop(0);
    if (player->y > 759) viewRect.setBottom(1000);
    setSceneRect(viewRect);
}

void Game::updatePlayerPosition_lab() {
    if (!playerItem) {
        qDebug() << "Error: playerItem is null";
        return;
    }
    updateplayerdirection();
    playerItem->setPos(player->x, player->y);
    centerOn(player->x + 17, player->y + 24);
    printf("%s(): player_x=%.0f, player_y=%.0f\n", __func__, player->x, player->y);
    // Ensure view stays within map bounds
    QRectF viewRect = sceneRect();
    if (player->x < 219) viewRect.setLeft(player->x-229);
    if (player->x > 219) viewRect.setLeft(player->x-229);
    if (player->y < 228) viewRect.setTop(player->y - 228);
    if (player->y > 228) viewRect.setTop(player->y - 228);
    //if (player->x > 400.5) viewRect.setRight(438);
    //if (player->y < 100) viewRect.setTop(0);
    //if (player->y > 420) viewRect.setBottom(455);
    setSceneRect(viewRect);
}

void Game::updatePlayerPosition_land() {
    if (!playerItem) {
        qDebug() << "Error: playerItem is null";
        return;
    }
    updateplayerdirection();

    playerItem->setPos(player->x, player->y);
    centerOn(player->x + 17, player->y + 24);
    printf("%s(): player_x=%.0f, player_y=%.0f\n", __func__, player->x, player->y);
    // Ensure view stays within map bounds
    QRectF viewRect = sceneRect();
         if (player->x < 262.5) viewRect.setLeft(0);
         if (player->x > 734.5) viewRect.setRight(1000);
              if (player->y < 225.5) viewRect.setTop(0);
         else if (player->y > 1442.5)  viewRect.setBottom(1667);
         else                        viewRect.setBottom(player->y + 225.0);

    setSceneRect(viewRect);
}

void Game::updatePlayerPosition_battle() {
   // if (!playerItem) {
   //     qDebug() << "Error: playerItem is null";
   //     return;
   // }
   // playerItem->setPos(262, 225);
    centerOn(262 , 225 );
   // printf("%s(): player_x=%.0f, player_y=%.0f\n", __func__, player->x, player->y);
    // Ensure view stays within map bounds
    QRectF viewRect = sceneRect();
    viewRect.setLeft(0);
    viewRect.setTop(0);
    setSceneRect(viewRect);
}

void Game::checkSceneTransitions() {
    QRectF playerRect(player->x, player->y, 35, 48);
    for (const auto* item : currentScene->items()) {
        auto* pixmapItem = qgraphicsitem_cast<const QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->pixmap().size() == QSize(30, 30)) { // Transition points
            QRectF itemRect(pixmapItem->x(), pixmapItem->y(), 30, 30);
            if (playerRect.intersects(itemRect)) {
                QString number;
                for (const auto* textItem : currentScene->items()) {
                    auto* text = qgraphicsitem_cast<const QGraphicsTextItem*>(textItem);
                    if (text && text->pos().x() > pixmapItem->x() && text->pos().x() < pixmapItem->x() + 30) {
                        number = text->toPlainText();
                        break;
                    }
                }
                if (number == "1") {
                    printf("player_x=%.0f, player_y=%.0f\n", player->x, player->y);
                    player->x_town = player->x;
                    player->y_town = player->y;
                    player->x = 200;
                    player->y = 200;
                    switchScene("laboratory");
                }
                else if (number == "2") {
                    printf("player_x=%.0f, player_y=%.0f\n", player->x, player->y);
                    player->x_town = player->x;
                    player->y_town = player->y;
                    player->x = 500;
                    player->y = 1590;
                    switchScene("grassland");
                }
                break;
            }
        }
    }
}

void Game::checkSceneTransitions_lab() {
    QRectF playerRect(player->x, player->y, 35, 48);
    for (const auto* item : currentScene->items()) {
        auto* pixmapItem = qgraphicsitem_cast<const QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->pixmap().size() == QSize(30, 30)) { // Transition points
            QRectF itemRect(pixmapItem->x(), pixmapItem->y(), 30, 30);
            
            if (playerRect.intersects(itemRect)) {
            //    QString number;
            //    for (const auto* textItem : currentScene->items()) {
            //        auto* text = qgraphicsitem_cast<const QGraphicsTextItem*>(textItem);
            //        if (text && text->pos().x() > pixmapItem->x() && text->pos().x() < pixmapItem->x() + 30) {
            //            number = text->toPlainText();
            //            break;
            //        }
            //    }
            //    if (number == "1") {
                    printf("player_x=%.0f, player_y=%.0f\n", player->x, player->y);
                    setPlayerbacktotown_lab();
                  //  if (player->x_town < 715) { player->x = 700; player->y = 810; }     //force the player position after switch to town
                  //  else if (player->x_town >= 750) { player->x = 770; player->y = 810;}//force the player position after switch to town
                  //  else                            {  player->x = 720; player->y = 830; }//force the player position after switch to town
                    switchScene("town");
            //    }
               // else if (number == "2") {
               //     switchScene("grassland");
               // }
                break;
            }
        }
    }
}

void Game::checkSceneTransitions_land() {
    QRectF playerRect(player->x, player->y, 35, 48);
    for (const auto* item : currentScene->items()) {
        auto* pixmapItem = qgraphicsitem_cast<const QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->pixmap().size() == QSize(30, 30)) { // Transition points
            QRectF itemRect(pixmapItem->x(), pixmapItem->y(), 30, 30);

            if (playerRect.intersects(itemRect)) {
                printf("player_x=%.0f, player_y=%.0f\n", player->x, player->y);
                setPlayerbacktotown_land();
                switchScene("town");
                break;
            }
        }
    }
}

void Game::checkWildPokemon_land() {
    QRectF playerRect(player->x, player->y, 35, 48);
    bool   out_of_tallgrass = true;

    int entrycounter = 0;

    for (QGraphicsPixmapItem* wildpokemon : WildPokemons) {




        QRectF boxRect(wildpokemon->x(), wildpokemon->y(), 40, 40);
        QRectF adjacentRect(boxRect.x(), boxRect.y(), boxRect.width() + 2, boxRect.height() + 2);
        if (playerRect.intersects(adjacentRect)) {
            if (!dialogVisible && Encounter_Pokemon==0)
            {
                QString itemName;

                itemName = wildpokemonList[entrycounter]->name;





                //int rand = qrand() % 3;
                if (itemName == "Squirtle") {
                   // itemName = "Squirtle";
                    Encounter_Pokemon = 1;
                }
                else if (itemName == "Charmander") {
                  //  itemName = "Charmander";
                    Encounter_Pokemon = 2;
                }
                else {
                 //   itemName = "Bulbasaur";
                    Encounter_Pokemon = 3;
                }

                //              if (pokemonList.count() >= 4) {
                 //                 showDialog("Cannot get it!\n max. 4 Pokemon!");
                 //                 return;
                 //             }


                showDialog("Encounter " + itemName + "!");
                currentScene->removeItem(wildpokemon);
                WildPokemons.removeOne(wildpokemon);
                delete wildpokemon;
                dialogVisible = true;
                //Encounter_Pokemon = true;
                Go_Battle = true;
                printf("%s(%d): Encounter_Pokemon=%d\n", __func__, __LINE__, Encounter_Pokemon);
                return;
                //append pokemon into "pokemonList"

   //             Pokemon* pokemon1 = new Pokemon(itemName, 30, 30, 5, 5, 10, 30);
   //             pokemon1->moves[0] = 0;
   //             pokemon1->moves[1] = 0;
   //             pokemon1->moves[2] = 0;
   //             pokemon1->moves[3] = 0;
//
 //               pokemonList.append(pokemon1);

            }
            
        }

        entrycounter++;

    }

    ////Checking if the player out of Tall grass
    for (QGraphicsRectItem* tallgrass : TallGrass1) {
        
        QRectF boxRect = tallgrass->rect();
        QRectF adjacentRect(boxRect.x(), boxRect.y(), boxRect.width(), boxRect.height());
        if (playerRect.intersects(adjacentRect)) {
            printf("%s(1):is in tallgrass 1, Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);
            out_of_tallgrass = false;
        }
    }

    for (QGraphicsRectItem* tallgrass : TallGrass2) {
        QRectF boxRect = tallgrass->rect();
        QRectF adjacentRect(boxRect.x(), boxRect.y(), boxRect.width(), boxRect.height());
        if (playerRect.intersects(adjacentRect)) {
            printf("%s(2):is in tallgrass 2, Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);
            out_of_tallgrass = false;
        }
    }

    for (QGraphicsRectItem* tallgrass : TallGrass3) {
        QRectF boxRect = tallgrass->rect(); 
        QRectF adjacentRect(boxRect.x(), boxRect.y(), boxRect.width(), boxRect.height());
        if (playerRect.intersects(adjacentRect)) {
            printf("%s(3):is in tallgrass 3, Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);
            out_of_tallgrass = false;
        }
    }

    for (QGraphicsRectItem* tallgrass : TallGrass4) {
        QRectF boxRect = tallgrass->rect(); 
        QRectF adjacentRect(boxRect.x(), boxRect.y(), boxRect.width(), boxRect.height());
        if (playerRect.intersects(adjacentRect)) {
            printf("%s(4):is in tallgrass 4, Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);
            out_of_tallgrass = false;
        }
    }

    for (QGraphicsRectItem* tallgrass : TallGrass5) {
        QRectF boxRect = tallgrass->rect(); 
        QRectF adjacentRect(boxRect.x(), boxRect.y(), boxRect.width(), boxRect.height());
        if (playerRect.intersects(adjacentRect)) {
            printf("%s(5):is in tallgrass 5, Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);
            out_of_tallgrass = false;
        }
    }

    for (QGraphicsRectItem* tallgrass : TallGrass6) {
        QRectF boxRect = tallgrass->rect(); 
        QRectF adjacentRect(boxRect.x(), boxRect.y(), boxRect.width(), boxRect.height());
        if (playerRect.intersects(adjacentRect)) {
            printf("%s(6):is in tallgrass 6, Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);
            out_of_tallgrass = false;
        }
    }

    printf("%s(5):Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);

    if (out_of_tallgrass) { 
        printf("%s(6):Encounter_Pokemon=%d\n", __func__,Encounter_Pokemon);
        printf("%s(7):is out_of_tallgrass\n",__func__);
        Encounter_Pokemon = 0; 
        printf("%s(8):Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);
    }

//    for (const auto* item : currentScene->items()) {
//        auto* pixmapItem = qgraphicsitem_cast<const QGraphicsPixmapItem*>(item);
//        if (pixmapItem && pixmapItem->pixmap().size() == QSize(30, 30)) { // Transition points
//            QRectF itemRect(pixmapItem->x(), pixmapItem->y(), 30, 30);
//
//            if (playerRect.intersects(itemRect)) {
//                printf("player_x=%.0f, player_y=%.0f\n", player->x, player->y);
//                setPlayerbacktotown_land();
//                switchScene("town");
//                break;
//            }
//        }
//    }
}

void Game::setPlayerbacktotown_lab() {
             player->x = 600; player->y = 700;    //force the player position after switch to town
     //  else if (player->x_town >= 750) { player->x = 770; player->y = 810; }//force the player position after switch to town
      //  else                            { player->x = 720; player->y = 830; }//force the player position after switch to town
}

void Game::setPlayerbacktotown_land() {
    //if (player->x_town < 715) { player->x = 700; player->y = 810; }     //force the player position after switch to town
    //else if (player->x_town >= 750) { player->x = 770; player->y = 810; }//force the player position after switch to town
    //else { player->x = 720; player->y = 830; }//force the player position after switch to town
    player->x = 510;
    player->y = 40;
}

void Game::setPlayerbacktograssland() {
    //if (player->x_town < 715) { player->x = 700; player->y = 810; }     //force the player position after switch to town
    //else if (player->x_town >= 750) { player->x = 770; player->y = 810; }//force the player position after switch to town
    //else { player->x = 720; player->y = 830; }//force the player position after switch to town
    //player->x = 510;
    //player->y = 40;
    player->x = player->x_land;
    player->y = player->y_land;

}

void Game::checkInteractions() {
    QRectF playerRect(player->x, player->y, 35, 48);
    // Check bulletin boards
    for (const auto* item : currentScene->items()) {
        auto* pixmapItem = qgraphicsitem_cast<const QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->pixmap().size() == QSize(40, 30)) { // Bulletin boards
            QRectF boardRect(pixmapItem->x(), pixmapItem->y(), 40, 30);
            QRectF adjacentRect(boardRect.x() - 5, boardRect.y() - 5, boardRect.width() + 1, boardRect.height() + 10);
            if (playerRect.intersects(adjacentRect)) {
                showDialog("This is Pallet Town\nBegin your adventure");
                return;
            }
        }
        else if (pixmapItem && pixmapItem->pixmap().size() == QSize(40, 32)) { // Garden Box
            QRectF boardRect(pixmapItem->x(), pixmapItem->y(), 40, 32);
            QRectF adjacentRect(boardRect.x() - 2, boardRect.y() - 2, boardRect.width() + 1, boardRect.height() + 5);
            if (playerRect.intersects(adjacentRect)) {
                showDialog("Please don't cross the garden\n");
                return;
            }
        }

    }
    // Check yellow boxes
    for (QGraphicsPixmapItem* box : yellowBoxes) {
        QRectF boxRect(box->x(), box->y(), 32, 32);
        QRectF adjacentRect(boxRect.x() - 32, boxRect.y() - 32, boxRect.width() + 64, boxRect.height() + 64);
        if (playerRect.intersects(adjacentRect)) {
            openYellowBox(box);
            return;
        }
    }
}

void Game::checkInteractions_lab() {
    QRectF playerRect(player->x, player->y, 35, 48);
    // Check bulletin boards
    for (const auto* item : currentScene->items()) {
        auto* pixmapItem = qgraphicsitem_cast<const QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->pixmap().size() == QSize(35, 47)) { // NPC
            QRectF boardRect(pixmapItem->x(), pixmapItem->y(), 35, 47);
            QRectF adjacentRect(boardRect.x() - 3, boardRect.y() - 3, boardRect.width() + 3, boardRect.height() + 3);
            if (playerRect.intersects(adjacentRect)) {
                if (player->NPC_talk == 0) {
                    showDialog(" I am Professor Oak. Welcome to my laboratory!\n");
                    player->NPC_talk = 1;
                    printf("NPC_talk=%d\n", player->NPC_talk);
                    return;
                }
                else if (player->NPC_talk == 1) {
                    hideDialog();
                    showDialog("You can choose one from three Poke Balls \n as your initial Pokemon in Laboratory.\n");
                    player->NPC_talk = 2;
                    printf("NPC_talk=%d\n", player->NPC_talk);
                    return;
                }
                else if (player->NPC_talk == 2) {
                    hideDialog();
                    player->NPC_talk = 0;
                    printf("NPC_talk=%d\n", player->NPC_talk);
                    return;
                }

            }
        }
        //else if (pixmapItem && pixmapItem->pixmap().size() == QSize(21, 21)) { // Pokeball_Mon1
        //    QRectF boardRect(pixmapItem->x(), pixmapItem->y(), 40, 32);
        //    QRectF adjacentRect(boardRect.x() - 32, boardRect.y() - 32, boardRect.width() + 64, boardRect.height() + 64);
        //    if (playerRect.intersects(adjacentRect)) {
        //        if (!dialogVisible)
        //        {
        //            showDialog("Get PokeMon\n");
        //            return;
        //        }
        //        else {
        //           // QGraphicsPixmapItem box = *pixmapItem;
        //            hideDialog();
        //           // currentScene->removeItem(&box);
        //            return;
        //        }
        //    }
        //}

    }
    // Check yellow boxes
    if (dialogVisible) { hideDialog(); return; }
    
    for (QGraphicsPixmapItem* lab_ball : Pokeball_Mons) {
        QRectF boxRect(lab_ball->x(), lab_ball->y(), 21, 21);
        QRectF adjacentRect(boxRect.x() - 2, boxRect.y() - 2, boxRect.width() + 2, boxRect.height() + 2);
        if (playerRect.intersects(adjacentRect)) {
            if (!dialogVisible)
                       {
                           QString itemName;
                           //int rand = qrand() % 3;
                                if (lab_ball->x() <= 290) itemName = "Squirtle";
                           else if (lab_ball->x() >= 330 ) itemName = "Charmander";
                           else itemName = "Bulbasaur";

                           if (pokemonList.count() >= 1) {
                               showDialog("Cannot get it!\n max get 1 Pokemon!");
                               return;
                           }
                            showDialog("Get PokeMon " + itemName + "!");
                            currentScene->removeItem(lab_ball);
                            Pokeball_Mons.removeOne(lab_ball);
                            delete lab_ball;
                            //append pokemon into "pokemonList"

                           // class Pokemon {
                           // public:
                           //     QString name;
                           //     int hp;
                           //     int maxHp;
                           //     int lvl;
                           //     int attack;
                           //     int defence;
                           //     int power;
                           //     int pp;
                           //     int moves[4];
                           //    Pokemon(const QString& n, int h, int mh, int atk, int def, int p, int pp) : name(n), hp(h), maxHp(mh), lvl(1), attack(atk), defence(def), power(p), pp(pp) {}
                           // };

                            Pokemon *pokemon1 = new Pokemon(itemName, 30, 30, 5, 5);
                            pokemon1->moves[0] = 1;
                            pokemon1->moves[1] = 0;
                            pokemon1->moves[2] = 0;
                            pokemon1->moves[3] = 0;
                            pokemon1->moves[4] = 0;
                            pokemon1->moves[5] = 0;

                            pokemonList.append(pokemon1);

                       }
                        else {
                           // QGraphicsPixmapItem box = *pixmapItem;
                            hideDialog();
                            //currentScene->removeItem(lab_ball);
                            //Pokeball_Mons.removeOne(lab_ball);
                            //delete lab_ball;
                        }
            //openYellowBox(box);
            return;
        }
    }
}

void Game::checkInteractions_land() {
    QRectF playerRect(player->x, player->y, 35, 48);
    // Check bulletin boards
    for (const auto* item : currentScene->items()) {
        auto* pixmapItem = qgraphicsitem_cast<const QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->pixmap().size() == QSize(35, 47)) { // NPC
            QRectF boardRect(pixmapItem->x(), pixmapItem->y(), 35, 47);
            QRectF adjacentRect(boardRect.x() - 3, boardRect.y() - 3, boardRect.width() + 3, boardRect.height() + 3);
            if (playerRect.intersects(adjacentRect)) {
                if (player->NPC_talk == 0) {
                    showDialog(" I am Professor Oak. Welcome to my laboratory!\n");
                    player->NPC_talk = 1;
                    printf("NPC_talk=%d\n", player->NPC_talk);
                    return;
                }
                else if (player->NPC_talk == 1) {
                    hideDialog();
                    showDialog("You can choose one from three Poke Balls \n as your initial Pokemon in Laboratory.\n");
                    player->NPC_talk = 2;
                    printf("NPC_talk=%d\n", player->NPC_talk);
                    return;
                }
                else if (player->NPC_talk == 2) {
                    hideDialog();
                    player->NPC_talk = 0;
                    printf("NPC_talk=%d\n", player->NPC_talk);
                    return;
                }

            }
        }
    }

    for (const auto* item : currentScene->items()) {
        auto* pixmapItem = qgraphicsitem_cast<const QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->pixmap().size() == QSize(40, 30)) { // Bulletin boards
            QRectF boardRect(pixmapItem->x(), pixmapItem->y(), 40, 30);
            QRectF adjacentRect(boardRect.x() - 5, boardRect.y() - 5, boardRect.width() + 1, boardRect.height() + 10);
            if (playerRect.intersects(adjacentRect)) {
                showDialog("This is Pallet Town\nBegin your adventure");
                return;
            }
        }
        else if (pixmapItem && pixmapItem->pixmap().size() == QSize(40, 32)) { // Garden Box
            QRectF boardRect(pixmapItem->x(), pixmapItem->y(), 40, 32);
            QRectF adjacentRect(boardRect.x() - 2, boardRect.y() - 2, boardRect.width() + 1, boardRect.height() + 5);
            if (playerRect.intersects(adjacentRect)) {
                showDialog("Be aware of pokemon!\n");
                return;
            }
        }

    }







    // Check yellow boxes
    if (dialogVisible) { 
        hideDialog(); 
        printf("%s(1): hideDialog, Encounter_Pokemon=%d\n",__func__,Encounter_Pokemon);
      // if (Encounter_Pokemon) {
          //Encounter_Pokemon = false;
          printf("%s(2): switch to battle\n",__func__);
          player->x_land = player->x;
          player->y_land = player->y;
          Go_Battle = false;
          switchScene("battle");
          updatePlayerPosition_battle();
      //  }
    return; 
    }

    //int entrycounter = 0;
    //for (QGraphicsPixmapItem* wildpokemon : WildPokemons) {
    //    QRectF boxRect(wildpokemon->x(), wildpokemon->y(), 40, 40);
    //    QRectF adjacentRect(boxRect.x(), boxRect.y(), boxRect.width() + 2, boxRect.height() + 2);
    //           
    //    if (playerRect.intersects(adjacentRect)) {
    //        if (!dialogVisible && Encounter_Pokemon==0)
    //        {
    //              QString itemName;
    //             // int rand = qrand() % 3;
    //             // if (rand == 0) itemName = "Squirtle";
    //            //  else if (rand == 1) itemName = "Charmander";
    //            //  else itemName = "Bulbasaur";

    //              itemName = wildpokemonList[entrycounter]->name;

    //              printf("%s(4): Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);
    //              if (itemName == "Squirtle")   Encounter_Pokemon = 1;
    //              else if (itemName == "Charmander") Encounter_Pokemon = 2;
    //              else if (itemName == "Bulbasaur")  Encounter_Pokemon = 3;
    //              else                               Encounter_Pokemon = 4;
    //              printf("%s(5): Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);
    //              showDialog("Encounter " + itemName + "!");
    //              printf("%s(6): Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);
    //              currentScene->removeItem(wildpokemon);
    //              WildPokemons.removeOne(wildpokemon);
    //              delete wildpokemon;
    //              dialogVisible = true;
    //            
    //              printf("%s(7): Encounter_Pokemon=%d\n", __func__, Encounter_Pokemon);
    //              return;
    //        }
    //        else {
    //            // QGraphicsPixmapItem box = *pixmapItem;
    //            hideDialog(); return;
    //            //currentScene->removeItem(lab_ball);
    //            //Pokeball_Mons.removeOne(lab_ball);
    //            //delete lab_ball;
    //        }
    //        //openYellowBox(box);
    //        //return;
    //    }
    //    entrycounter++;
    //}
}

void Game::checkInteractions_battle() {
    QRectF playerRect(player->x, player->y, 35, 48);
    // Check bulletin boards
    for (const auto* item : currentScene->items()) {
        auto* pixmapItem = qgraphicsitem_cast<const QGraphicsPixmapItem*>(item);
        if (pixmapItem && pixmapItem->pixmap().size() == QSize(35, 47)) { // NPC
            QRectF boardRect(pixmapItem->x(), pixmapItem->y(), 35, 47);
            QRectF adjacentRect(boardRect.x() - 3, boardRect.y() - 3, boardRect.width() + 3, boardRect.height() + 3);
            if (playerRect.intersects(adjacentRect)) {
                if (player->NPC_talk == 0) {
                    showDialog(" I am Professor Oak. Welcome to my laboratory!\n");
                    player->NPC_talk = 1;
                    printf("NPC_talk=%d\n", player->NPC_talk);
                    return;
                }
                else if (player->NPC_talk == 1) {
                    hideDialog();
                    showDialog("You can choose one from three Poke Balls \n as your initial Pokemon in Laboratory.\n");
                    player->NPC_talk = 2;
                    printf("NPC_talk=%d\n", player->NPC_talk);
                    return;
                }
                else if (player->NPC_talk == 2) {
                    hideDialog();
                    player->NPC_talk = 0;
                    printf("NPC_talk=%d\n", player->NPC_talk);
                    return;
                }

            }
        }
    }
    // Check yellow boxes
    if (dialogVisible) { hideDialog(); return; }

    for (QGraphicsPixmapItem* wildpokemon : WildPokemons) {
        QRectF boxRect(wildpokemon->x(), wildpokemon->y(), 40, 40);
        QRectF adjacentRect(boxRect.x(), boxRect.y(), boxRect.width() + 2, boxRect.height() + 2);
        if (playerRect.intersects(adjacentRect)) {
            if (!dialogVisible)
            {
                QString itemName;
                int rand = qrand() % 3;
                if (rand == 0) itemName = "Squirtle";
                else if (rand == 1) itemName = "Charmander";
                else itemName = "Bulbasaur";

                //              if (pokemonList.count() >= 4) {
                 //                 showDialog("Cannot get it!\n max. 4 Pokemon!");
                 //                 return;
                 //             }
                showDialog("Encounter " + itemName + "!");
                currentScene->removeItem(wildpokemon);
                WildPokemons.removeOne(wildpokemon);
                delete wildpokemon;
                dialogVisible = true;

            }
            else {
                // QGraphicsPixmapItem box = *pixmapItem;
                hideDialog();
                //currentScene->removeItem(lab_ball);
                //Pokeball_Mons.removeOne(lab_ball);
                //delete lab_ball;
            }
            //openYellowBox(box);
            return;
        }
    }
}

void Game::openYellowBox(QGraphicsPixmapItem* box) {
    QString itemName;
    int rand = qrand() % 3;
    if (rand == 0) itemName = "Poke Ball";
    else if (rand == 1) itemName = "Potion";
    else itemName = "Ether";

    if (itemName == "Poke Ball" && inventory.value("Poke Ball", 0) >= 3) {
        showDialog("Cannot carry more Poke Balls!");
    }
    else {
        inventory[itemName]++;
        showDialog("Obtained " + itemName + "!");
        currentScene->removeItem(box);
        yellowBoxes.removeOne(box);
        delete box;
    }
}

void Game::removeWildPokemons() {
    for (QGraphicsPixmapItem* wildpokemon : WildPokemons) {
        currentScene->removeItem(wildpokemon);
        WildPokemons.removeOne(wildpokemon);
        delete wildpokemon;
    }
    for (Pokemon* wildpokemontype : wildpokemonList) {
        wildpokemonList.removeOne(wildpokemontype);
        delete wildpokemontype;
    }
}

void Game::showDialog(const QString& message) {
    dialogVisible = true;
    // Position dialog box in viewport coordinates (bottom of window)
    QPointF topLeft = mapToScene(0, 0); // Top-left of viewport in scene coordinates
    dialogBox = createPixmapItem("dialog.png", topLeft.x() + 50, topLeft.y() + 300, 425, 100, Qt::green);
   // dialogBox = new QGraphicsRectItem(topLeft.x() + 50, topLeft.y() + 300, 425, 100);
   // dialogBox->setBrush(QBrush(Qt::white));
    //dialogBox->setPen(QPen(Qt::black, 2));
    dialogBox->setZValue(100); // Ensure dialog is on top
    currentScene->addItem(dialogBox);
    dialogText = new QGraphicsTextItem(message);
    dialogText->setFont(QFont("Arial", 12));
    dialogText->setPos(topLeft.x() + 60, topLeft.y() + 310);
    dialogText->setZValue(100);
    currentScene->addItem(dialogText);
}

void Game::hideDialog() {
    if (dialogVisible) {
        currentScene->removeItem(dialogBox);
        currentScene->removeItem(dialogText);
        delete dialogBox;
        delete dialogText;
        dialogBox = nullptr;
        dialogText = nullptr;
        dialogVisible = false;
    }
}

void Game::clearSkillRect() {
    if (skillMenuVisible) {
        /*
        if (skillRect_0) {
            currentScene->removeItem(skillRect_0);
            delete skillRect_0;
            skillRect_0 = nullptr;
        }
        if (skillRect_1) {
            currentScene->removeItem(skillRect_1);
            delete skillRect_1;
            skillRect_1 = nullptr;
        }
        if (skillRect_2) {
            currentScene->removeItem(skillRect_2);
            delete skillRect_2;
            skillRect_2 = nullptr;
        }
        if (skillRect_3) {
            currentScene->removeItem(skillRect_3);
            delete skillRect_3;
            skillRect_3 = nullptr;
        }

        */
    }
}
int  Game::pokemonid(QString pokemon_name)
{
    if (pokemon_name == "Squirtle") return 0;
    if (pokemon_name == "Charmander") return 1;
    if (pokemon_name == "Bulbasaur")  return 2;
    
    printf("unknown Pokemon name: %s\n", pokemon_name.toStdString().c_str());
    return 0;
}

void Game::showSkillpp(int skill)
{
    Pokemon* pokemon;
    pokemon = pokemonList[activePokemon];

    if (pokemonItemActive_Text2) { 
        currentScene->removeItem(pokemonItemActive_Text2);
        delete pokemonItemActive_Text2;
        pokemonItemActive_Text2 = nullptr;
    }

    QString pText = QString::number(pokemon->pp[skill]) + " / " + QString::number(pokemon->maxpp[skill]);
    pokemonItemActive_Text2 = new QGraphicsTextItem(pText);
    pokemonItemActive_Text2->setFont(QFont("Arial", 13));
    pokemonItemActive_Text2->setPos(430, 275); // Adjusted to below item row
    pokemonItemActive_Text2->setZValue(100);
    currentScene->addItem(pokemonItemActive_Text2);
}

void Game::showSkillMenu() {
    skillMenuVisible = true;
    clearSkillRect();
    selectSkill = 0;
    // Position bag menu in viewport coordinates (top of window)
    QPointF topLeft = mapToScene(0, 0); // Top-left of viewport in scene coordinates
    skillmenuBox = createPixmapItem("fight_skill.png", topLeft.x() + 30, topLeft.y() + 130, 280, 60, Qt::white);
    //menuBox->setBrush(QBrush(Qt::white));
    //menuBox->setPen(QPen(Qt::black, 2));
    skillmenuBox->setZValue(100); // Ensure menu is on top
    currentScene->addItem(skillmenuBox);

//    QString name;
//    int hp;
//    int maxHp;
//    int lvl;
//    int attack;
//    int defence;
//    int power;
//    int pp;
//    int moves[6];

    Pokemon* pokemon;
    if (activePokemon >= 0) {
        pokemon = pokemonList[activePokemon];
        printf("[%d]%s: lvl=%d, hp=%d, maxHp=%d, PP=[%d %d %d %d], power=[%d %d %d %d], attack=%d, defence=%d, moves[0:5]=%d, %d, %d, %d, %d, %d\n",
            activePokemon,
            pokemon->name.toStdString().c_str(),
            pokemon->lvl, pokemon->hp, pokemon->maxHp, 
            pokemon->pp[0], pokemon->pp[1], pokemon->pp[2], pokemon->pp[3], 
            pokemon->power[0], pokemon->power[1], pokemon->power[2], pokemon->power[3],
            pokemon->attack, pokemon->defence,
            pokemon->moves[0], pokemon->moves[1], pokemon->moves[2], pokemon->moves[3], pokemon->moves[4], pokemon->moves[5]);

        if (pokemon->moves[0] == 1) {
            skillRect_0 = QRectF(40, 140, 40, 10);
            QString skill = skillname[pokemonid(pokemon->name)][0];
            skillmenuText0 = new QGraphicsTextItem(skill);
            skillmenuText0->setFont(QFont("Arial", 8));
            skillmenuText0->setPos(40, 140); // Adjusted to below item row
            skillmenuText0->setZValue(100);
            currentScene->addItem(skillmenuText0);
        }
        if (pokemon->moves[1] == 1) {
            skillRect_1 = QRectF(40, 160, 40, 10);
            QString skill = skillname[pokemonid(pokemon->name)][1];
            skillmenuText1 = new QGraphicsTextItem(skill);
            skillmenuText1->setFont(QFont("Arial", 8));
            skillmenuText1->setPos(40, 160); // Adjusted to below item row
            skillmenuText1->setZValue(100);
            currentScene->addItem(skillmenuText1);
        }
        if (pokemon->moves[2] == 1) {
            skillRect_2 = QRectF(90, 140, 40, 10);
            QString skill = skillname[pokemonid(pokemon->name)][2];
            skillmenuText2 = new QGraphicsTextItem(skill);
            skillmenuText2->setFont(QFont("Arial", 8));
            skillmenuText2->setPos(90, 140); // Adjusted to below item row
            skillmenuText2->setZValue(100);
            currentScene->addItem(skillmenuText2);
        }
        if (pokemon->moves[3] == 1) {
            skillRect_3 = QRectF(90, 160, 40, 10);
            QString skill = skillname[pokemonid(pokemon->name)][3];
            skillmenuText3 = new QGraphicsTextItem(skill);
            skillmenuText3->setFont(QFont("Arial", 8));
            skillmenuText3->setPos(90, 160); // Adjusted to below item row
            skillmenuText3->setZValue(100);
            currentScene->addItem(skillmenuText3);
        }
    }
    

  //// Item images (32x32) in top row: Poke Ball, Potion, Ether
  


}

void Game::hideSkillMenu() {
    if (skillMenuVisible) {
        printf("%s(): hideSkillMenu\n",__func__);
        if(skillmenuBox) {
            currentScene->removeItem(skillmenuBox);
            delete skillmenuBox;
            skillmenuBox = nullptr;
        }
        if (skillmenuText0) {
            currentScene->removeItem(skillmenuText0);
            delete skillmenuText0;
            skillmenuText0 = nullptr;
        }
        if (skillmenuText1) {
            currentScene->removeItem(skillmenuText1);
            delete skillmenuText1;
            skillmenuText1 = nullptr;
        }
        if (skillmenuText2) {
            currentScene->removeItem(skillmenuText2);
            delete skillmenuText2;
            skillmenuText2 = nullptr;
        }
        if (skillmenuText3) {
            currentScene->removeItem(skillmenuText3);
            delete skillmenuText3;
            skillmenuText3 = nullptr;
        }
        if (skillmenuText4) {
            currentScene->removeItem(skillmenuText4);
            delete skillmenuText4;
            skillmenuText4 = nullptr;
        }
        if (skillmenuText5) {
            currentScene->removeItem(skillmenuText5);
            delete skillmenuText5;
            skillmenuText5 = nullptr;
        }

    skillMenuVisible = false;
    }
    msleep(100.0);
    return;

}

void Game::showBagMenu_nopokemon() {
    bagMenuVisible = true;
    // Position bag menu in viewport coordinates (top of window)
    QPointF topLeft = mapToScene(0, 0); // Top-left of viewport in scene coordinates

    bagRect_0 = QRectF(60, 60, 0, 0);
    bagRect_1 = QRectF(120, 60,0, 0);
    bagRect_2 = QRectF(180, 60,0, 0);
    //if (bagRect_1) { delete bagRect_1; bagRect_1 = nullptr; }
    //if (bagRect_2) { delete bagRect_2; bagRect_2 = nullptr; }
    //menuBox = new QGraphicsRectItem(topLeft.x() + 50, topLeft.y() + 50, 300, 50);
  //menuBox = createPixmapItem("bag.png", topLeft.x() + 50, topLeft.y() + 50, 300, 50, Qt::white);
    menuBox = createPixmapItem("bag_nopokemon.png", topLeft.x() + 50, topLeft.y() + 50, 200, 50, Qt::white);
    //menuBox->setBrush(QBrush(Qt::white));
    //menuBox->setPen(QPen(Qt::black, 2));
    menuBox->setZValue(100); // Ensure menu is on top
    currentScene->addItem(menuBox);


    //// Item images (32x32) in top row: Poke Ball, Potion, Ether
    pokeBallItem = createPixmapItem("pokeball.jpg", topLeft.x() + 60, topLeft.y() + 60, 32, 32, Qt::red);
    pokeBallItem->setZValue(100);
    currentScene->addItem(pokeBallItem);
    potionItem = createPixmapItem("potion.jpg", topLeft.x() + 120, topLeft.y() + 60, 32, 32, Qt::blue);
    potionItem->setZValue(100);
    currentScene->addItem(potionItem);
    etherItem = createPixmapItem("ether.jpg", topLeft.x() + 180, topLeft.y() + 60, 32, 32, Qt::magenta);
    etherItem->setZValue(100);
    currentScene->addItem(etherItem);

    //// Quantity text (e.g., "x2") in black, right of each image
    pokeBallQuantity = new QGraphicsTextItem("x" + QString::number(inventory.value("Poke Ball", 0)));
    pokeBallQuantity->setFont(QFont("Arial", 12));
    pokeBallQuantity->setDefaultTextColor(Qt::black);
    pokeBallQuantity->setPos(topLeft.x() + 92, topLeft.y() + 60);
    pokeBallQuantity->setZValue(100);
    currentScene->addItem(pokeBallQuantity);
    if (inventory.value("Poke Ball", 0)>0)  bagRect_0 = QRectF(60, 60, 30, 30);

    potionQuantity = new QGraphicsTextItem("x" + QString::number(inventory.value("Potion", 0)));
    potionQuantity->setFont(QFont("Arial", 12));
    potionQuantity->setDefaultTextColor(Qt::black);
    potionQuantity->setPos(topLeft.x() + 152, topLeft.y() + 60);
    potionQuantity->setZValue(100);
    currentScene->addItem(potionQuantity);
    if (inventory.value("Potion", 0) > 0)  bagRect_1 = QRectF(120, 60, 30, 30);

    etherQuantity = new QGraphicsTextItem("x" + QString::number(inventory.value("Ether", 0)));
    etherQuantity->setFont(QFont("Arial", 12));
    etherQuantity->setDefaultTextColor(Qt::black);
    etherQuantity->setPos(topLeft.x() + 212, topLeft.y() + 60);
    etherQuantity->setZValue(100);
    currentScene->addItem(etherQuantity);
    if (inventory.value("Ether", 0) > 0)  bagRect_2 = QRectF(180, 60, 30, 30);
}

void Game::hideBagMenu_nopokemon() {
    if (bagMenuVisible) {
        if (menuBox) {
            currentScene->removeItem(menuBox);
            delete menuBox;
            menuBox = nullptr;
        }
        if (menuText) {
            currentScene->removeItem(menuText);
            delete menuText;
            menuText = nullptr;
        }
        if (menuText0) {
            currentScene->removeItem(menuText0);
            delete menuText0;
            menuText0 = nullptr;
        }
        if (menuText1) {
            currentScene->removeItem(menuText1);
            delete menuText1;
            menuText1 = nullptr;
        }
        if (menuText2) {
            currentScene->removeItem(menuText2);
            delete menuText2;
            menuText2 = nullptr;
        }
        if (menuText3) {
            currentScene->removeItem(menuText3);
            delete menuText3;
            menuText3 = nullptr;
        }

        if (pokeBallItem) {
            currentScene->removeItem(pokeBallItem);
            delete pokeBallItem;
            pokeBallItem = nullptr;
        }
        if (pokemonItem) {
            currentScene->removeItem(pokemonItem);
            delete pokemonItem;
            pokemonItem = nullptr;
        }
        if (pokemonItem0) {
            currentScene->removeItem(pokemonItem0);
            delete pokemonItem0;
            pokemonItem0 = nullptr;
        }
        if (pokemonItem1) {
            currentScene->removeItem(pokemonItem1);
            delete pokemonItem1;
            pokemonItem1 = nullptr;
        }
        if (pokemonItem2) {
            currentScene->removeItem(pokemonItem2);
            delete pokemonItem2;
            pokemonItem2 = nullptr;
        }
        if (pokemonItem3) {
            currentScene->removeItem(pokemonItem3);
            delete pokemonItem3;
            pokemonItem3 = nullptr;
        }
        if (potionItem) {
            currentScene->removeItem(potionItem);
            delete potionItem;
            potionItem = nullptr;
        }
        if (etherItem) {
            currentScene->removeItem(etherItem);
            delete etherItem;
            etherItem = nullptr;
        }
        if (pokeBallQuantity) {
            currentScene->removeItem(pokeBallQuantity);
            delete pokeBallQuantity;
            pokeBallQuantity = nullptr;
        }
        if (potionQuantity) {
            currentScene->removeItem(potionQuantity);
            delete potionQuantity;
            potionQuantity = nullptr;
        }
        if (etherQuantity) {
            currentScene->removeItem(etherQuantity);
            delete etherQuantity;
            etherQuantity = nullptr;
        }
        bagMenuVisible = false;
    }
}


void Game::showPokemonMenu() {
    pokemonMenuVisible = true;
    // Position bag menu in viewport coordinates (top of window)
    QPointF topLeft = mapToScene(0, 0); // Top-left of viewport in scene coordinates
    //menuBox = new QGraphicsRectItem(topLeft.x() + 50, topLeft.y() + 50, 300, 250);
    menuBox = createPixmapItem("bag.png", topLeft.x() + 50, topLeft.y() + 100, 265, 220, Qt::white);
    //menuBox->setBrush(QBrush(Qt::white));
    //menuBox->setPen(QPen(Qt::black, 2));
    menuBox->setZValue(100); // Ensure menu is on top
    currentScene->addItem(menuBox);

    //// Item images (32x32) in top row: Poke Ball, Potion, Ether
    //pokeBallItem = createPixmapItem("pokeball.jpg", topLeft.x() + 60, topLeft.y() + 60, 32, 32, Qt::red);
    //pokeBallItem->setZValue(100);
    //currentScene->addItem(pokeBallItem);
    //potionItem = createPixmapItem("potion.jpg", topLeft.x() + 120, topLeft.y() + 60, 32, 32, Qt::blue);
    //potionItem->setZValue(100);
    //currentScene->addItem(potionItem);
    //etherItem = createPixmapItem("ether.jpg", topLeft.x() + 180, topLeft.y() + 60, 32, 32, Qt::magenta);
    //etherItem->setZValue(100);
    //currentScene->addItem(etherItem);

    //// Quantity text (e.g., "x2") in black, right of each image
    //pokeBallQuantity = new QGraphicsTextItem("x" + QString::number(inventory.value("Poke Ball", 0)));
    //pokeBallQuantity->setFont(QFont("Arial", 12));
    //pokeBallQuantity->setDefaultTextColor(Qt::black);
    //pokeBallQuantity->setPos(topLeft.x() + 92, topLeft.y() + 60);
    //pokeBallQuantity->setZValue(100);
    //currentScene->addItem(pokeBallQuantity);

    //potionQuantity = new QGraphicsTextItem("x" + QString::number(inventory.value("Potion", 0)));
    //potionQuantity->setFont(QFont("Arial", 12));
    //potionQuantity->setDefaultTextColor(Qt::black);
    //potionQuantity->setPos(topLeft.x() + 152, topLeft.y() + 60);
    //potionQuantity->setZValue(100);
    //currentScene->addItem(potionQuantity);

    //etherQuantity = new QGraphicsTextItem("x" + QString::number(inventory.value("Ether", 0)));
    //etherQuantity->setFont(QFont("Arial", 12));
    //etherQuantity->setDefaultTextColor(Qt::black);
    //etherQuantity->setPos(topLeft.x() + 212, topLeft.y() + 60);
    //etherQuantity->setZValue(100);
    //currentScene->addItem(etherQuantity);

    // Pokémon list below items
    //QString pokemonText = "Pokemon:\n";
    //QString pokemonText = "";
    int pindex = 0;
    Pokemon* p;
    for (const auto* pokemon : pokemonList) {
        p = pokemonList[pindex];
        QString pokemonText = "";
        pokemonText += getPokemonName(p) + " (HP: " + QString::number(pokemon->hp) + "/" + QString::number(pokemon->maxHp) + ")";
        if (pindex == 0) {
            if (getPokemonName(p) == "Squirtle")   pokemonItem0 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmander") pokemonItem0 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Bulbasaur")  pokemonItem0 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Wartortle")  pokemonItem0 = createPixmapItem("wartortle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Blastoise")  pokemonItem0 = createPixmapItem("blastoise.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmeleon") pokemonItem0 = createPixmapItem("charmeleon.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charizard")  pokemonItem0 = createPixmapItem("charizard.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Ivysaur")    pokemonItem0 = createPixmapItem("ivysaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Venusaur")   pokemonItem0 = createPixmapItem("venusaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);

            //if (pokemon->name == "Squirtle") {
            //    pokemonItem0 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            //else if (pokemon->name == "Charmander") {
            //    pokemonItem0 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            //else if (pokemon->name == "Bulbasaur") {
            //    pokemonItem0 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            pokemonItem0->setZValue(100);
            currentScene->addItem(pokemonItem0);

            menuText0 = new QGraphicsTextItem(pokemonText);
            menuText0->setFont(QFont("Arial", 12));
            menuText0->setPos(topLeft.x() + 60, topLeft.y() + 120 + pindex * 50); // Adjusted to below item row
            menuText0->setZValue(100);
            currentScene->addItem(menuText0);
            pokemonRect_0 = new QRectF(66, 120, 280-66, 130-100);

        }

        if (pindex == 1) {
            if (getPokemonName(p) == "Squirtle")   pokemonItem1 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmander") pokemonItem1 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Bulbasaur")  pokemonItem1 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Wartortle")  pokemonItem1 = createPixmapItem("wartortle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Blastoise")  pokemonItem1 = createPixmapItem("blastoise.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmeleon") pokemonItem1 = createPixmapItem("charmeleon.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charizard")  pokemonItem1 = createPixmapItem("charizard.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Ivysaur")    pokemonItem1 = createPixmapItem("ivysaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Venusaur")   pokemonItem1 = createPixmapItem("venusaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);

            //if (pokemon->name == "Squirtle") {
            //    pokemonItem1 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            //else if (pokemon->name == "Charmander") {
            //    pokemonItem1 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            //else if (pokemon->name == "Bulbasaur") {
            //    pokemonItem1 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            pokemonItem1->setZValue(100);
            currentScene->addItem(pokemonItem1);

            menuText1 = new QGraphicsTextItem(pokemonText);
            menuText1->setFont(QFont("Arial", 12));
            menuText1->setPos(topLeft.x() + 60, topLeft.y() + 120 + pindex * 50); // Adjusted to below item row
            menuText1->setZValue(100);
            currentScene->addItem(menuText1);

            pokemonRect_1 = new QRectF(66, 50+120, 280 - 66, 180 - 150);
        }

        if (pindex == 2) {
            if (getPokemonName(p) == "Squirtle")   pokemonItem2 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmander") pokemonItem2 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Bulbasaur")  pokemonItem2 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Wartortle")  pokemonItem2 = createPixmapItem("wartortle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Blastoise")  pokemonItem2 = createPixmapItem("blastoise.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmeleon") pokemonItem2 = createPixmapItem("charmeleon.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charizard")  pokemonItem2 = createPixmapItem("charizard.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Ivysaur")    pokemonItem2 = createPixmapItem("ivysaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Venusaur")   pokemonItem2 = createPixmapItem("venusaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);

            //if (pokemon->name == "Squirtle") {
            //    pokemonItem2 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            //else if (pokemon->name == "Charmander") {
            //    pokemonItem2 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            //else if (pokemon->name == "Bulbasaur") {
            //    pokemonItem2 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            pokemonItem2->setZValue(100);
            currentScene->addItem(pokemonItem2);

            menuText2 = new QGraphicsTextItem(pokemonText);
            menuText2->setFont(QFont("Arial", 12));
            menuText2->setPos(topLeft.x() + 60, topLeft.y() + 120 + pindex * 50); // Adjusted to below item row
            menuText2->setZValue(100);
            currentScene->addItem(menuText2);
            pokemonRect_2 = new QRectF(66, 100+120, 280 - 66, 230 - 200);

        }

        if (pindex == 3) {
            if (getPokemonName(p) == "Squirtle")   pokemonItem3 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmander") pokemonItem3 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Bulbasaur")  pokemonItem3 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Wartortle")  pokemonItem3 = createPixmapItem("wartortle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Blastoise")  pokemonItem3 = createPixmapItem("blastoise.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmeleon") pokemonItem3 = createPixmapItem("charmeleon.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charizard")  pokemonItem3 = createPixmapItem("charizard.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Ivysaur")    pokemonItem3 = createPixmapItem("ivysaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Venusaur")   pokemonItem3 = createPixmapItem("venusaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);

            //if (pokemon->name == "Squirtle") {
            //    pokemonItem3 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            //else if (pokemon->name == "Charmander") {
            //    pokemonItem3 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            //else if (pokemon->name == "Bulbasaur") {
            //    pokemonItem3 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            //}
            pokemonItem3->setZValue(100);
            currentScene->addItem(pokemonItem3);

            menuText3 = new QGraphicsTextItem(pokemonText);
            menuText3->setFont(QFont("Arial", 12));
            menuText3->setPos(topLeft.x() + 60, topLeft.y() + 120 + pindex * 50); // Adjusted to below item row
            menuText3->setZValue(100);
            currentScene->addItem(menuText3);
            pokemonRect_3 = new QRectF(66, 150+120, 280 - 66, 280 - 250);

        }

        //currentScene->addItem(pokemonItem);

        //}
//        menuText = new QGraphicsTextItem(pokemonText);
//        menuText->setFont(QFont("Arial", 12));
//        menuText->setPos(topLeft.x() + 60, topLeft.y() + 100+ pindex * 50); // Adjusted to below item row
//        menuText->setZValue(100);
//        currentScene->addItem(menuText);
        pindex++;
    }
}

QString Game::getPokemonName(Pokemon* pokemon)
{
    QString name="";
    if (pokemon) {
        if (pokemon->lvl <= 2) {
            if (pokemon->name == "Charmander")      name = "Charmander";
            else if (pokemon->name == "Squirtle")   name = "Squirtle";
            else if (pokemon->name == "Bulbasaur")  name = "Bulbasaur";
        }
        else if (pokemon->lvl <= 4) {
            if (pokemon->name == "Charmander")      name = "Charmeleon";
            else if (pokemon->name == "Squirtle")   name = "Wartortle";
            else if (pokemon->name == "Bulbasaur")  name = "Ivysaur";
        }
        else {
            if (pokemon->name == "Charmander")      name = "Charizard";
            else if (pokemon->name == "Squirtle")   name = "Blastoise";
            else if (pokemon->name == "Bulbasaur")  name = "Venusaur";
        }
    }
    return(name);
}

void Game::showWildPokemon_HP()
{
    Pokemon* pokemon;
    if (wildPokemon) {
        QString pText = "";
        //remove current pokemon first
        if (wildp_HP) {
            currentScene->removeItem(wildp_HP);
            delete wildp_HP;
            wildp_HP = nullptr;
        }

        if (wildp_Text2) {
            currentScene->removeItem(wildp_Text2);
            delete wildp_Text2;
            wildp_Text2 = nullptr;
        }


        pokemon = wildPokemon;
        ///HP:Bar
        wildp_HP = new QGraphicsRectItem(118, 94, ((float)pokemon->hp / (float)pokemon->maxHp) * 100.0, 8);
        QBrush brush(Qt::red);
        wildp_HP->setBrush(brush);
        currentScene->addItem(wildp_HP);

        ///HP:Text
        pText = "" + QString::number(pokemon->hp) + "/" + QString::number(pokemon->maxHp);
        wildp_Text2 = new QGraphicsTextItem(pText);
        wildp_Text2->setFont(QFont("Arial", 12));
        wildp_Text2->setPos(36, 82); // Adjusted to below item row
        wildp_Text2->setZValue(100);
        currentScene->addItem(wildp_Text2);

        showWildPokemon();

    }
}


void Game::showactivePokemon_HP(int showPokemon)
{
    Pokemon* pokemon;
    if (showPokemon >= 0) {
        QString pText = "";
        //remove current pokemon first
        if (pokemonItemActive_HP) {
            currentScene->removeItem(pokemonItemActive_HP);
            delete pokemonItemActive_HP;
            pokemonItemActive_HP = nullptr;
        }

        if (pokemonItemActive_EXP) {
            currentScene->removeItem(pokemonItemActive_EXP);
            delete pokemonItemActive_EXP;
            pokemonItemActive_EXP = nullptr;
        }

        if (pokemonItemActive_Text3) {
            currentScene->removeItem(pokemonItemActive_Text3);
            delete pokemonItemActive_Text3;
            pokemonItemActive_Text3 = nullptr;
        }

        if (pokemonItemActive_Text4) {
            currentScene->removeItem(pokemonItemActive_Text4);
            delete pokemonItemActive_Text4;
            pokemonItemActive_Text4 = nullptr;
        }


        pokemon = pokemonList[showPokemon];
        ///HP:Bar
        pokemonItemActive_HP = new QGraphicsRectItem(384, 258, ((float)pokemon->hp/(float)pokemon->maxHp)*100.0, 8);
        QBrush brush(Qt::red);
        pokemonItemActive_HP->setBrush(brush);
        currentScene->addItem(pokemonItemActive_HP);

        ///EXP:Bar
        pokemonItemActive_EXP = new QGraphicsRectItem(350, 305, ((float)pokemon->exp / ((float)pokemon->lvl*20+100)) * 80.0, 8);
        QBrush brush2(Qt::blue);
        pokemonItemActive_EXP->setBrush(brush2);
        currentScene->addItem(pokemonItemActive_EXP);

        ///HP:Text
        pText = "HP:"+ QString::number(pokemon->hp) + " / " + QString::number(pokemon->maxHp);
        pokemonItemActive_Text3 = new QGraphicsTextItem(pText);
        pokemonItemActive_Text3->setFont(QFont("Arial", 13));
        pokemonItemActive_Text3->setPos(309, 275); // Adjusted to below item row
        pokemonItemActive_Text3->setZValue(100);
        currentScene->addItem(pokemonItemActive_Text3);

        ///EXP:Text
        pText = "EXP:" + QString::number(pokemon->exp);
        pokemonItemActive_Text4 = new QGraphicsTextItem(pText);
        pokemonItemActive_Text4->setFont(QFont("Arial", 10));
        pokemonItemActive_Text4->setPos(290, 295); // Adjusted to below item row
        pokemonItemActive_Text4->setZValue(100);
        currentScene->addItem(pokemonItemActive_Text4);

    }
}


void Game::showactivePokemon(int showPokemon)
{
    Pokemon* pokemon;
    if (showPokemon >= 0) {
        QString pText = "";
        //remove current pokemon first
        if (pokemonItemActive) {
            currentScene->removeItem(pokemonItemActive);
            delete pokemonItemActive;
            pokemonItemActive = nullptr;
        }
        if (pokemonItemActive_Text) {
            currentScene->removeItem(pokemonItemActive_Text);
            delete pokemonItemActive_Text;
            pokemonItemActive_Text = nullptr;
        }
        if (pokemonItemActive_Text2) {
            currentScene->removeItem(pokemonItemActive_Text2);
            delete pokemonItemActive_Text2;
            pokemonItemActive_Text2 = nullptr;
        }


        pokemon = pokemonList[showPokemon];
        printf("[%d]%s: lvl=%d, hp=%d, maxHp=%d, PP=[%d %d %d %d], power=[%d %d %d %d], attack=%d, defence=%d, moves[0:5]=%d, %d, %d, %d, %d, %d\n",
            activePokemon,
            pokemon->name.toStdString().c_str(),
            pokemon->lvl, pokemon->hp, pokemon->maxHp, 
            pokemon->pp[0], pokemon->pp[1], pokemon->pp[2], pokemon->pp[3], 
            pokemon->power[0], pokemon->power[1], pokemon->power[2], pokemon->power[3],
            pokemon->attack, pokemon->defence,
            pokemon->moves[0], pokemon->moves[1], pokemon->moves[2], pokemon->moves[3], pokemon->moves[4], pokemon->moves[5]);
        if (pokemon->lvl <= 2) {
                 if (pokemon->name == "Charmander") pokemonItemActive = createPixmapItem("charmander_back.png", 70, 180, 150, 150, Qt::white);
            else if (pokemon->name == "Squirtle")   pokemonItemActive = createPixmapItem("squirtle_back.png", 70, 180, 150, 150, Qt::white);
            else if (pokemon->name == "Bulbasaur")  pokemonItemActive = createPixmapItem("bulbasaur_back.png", 70, 180, 150, 150, Qt::white);
        }
        else if (pokemon->lvl <= 4) {
                 if (pokemon->name == "Charmander") pokemonItemActive = createPixmapItem("charmeleon_back.png", 70, 180, 150, 150, Qt::white);
            else if (pokemon->name == "Squirtle")   pokemonItemActive = createPixmapItem("wartortle_back.png", 70, 180, 150, 150, Qt::white);
            else if (pokemon->name == "Bulbasaur")  pokemonItemActive = createPixmapItem("ivysaur_back.png", 70, 180, 150, 150, Qt::white);
        }
        else {
                 if (pokemon->name == "Charmander") pokemonItemActive = createPixmapItem("charizard_back.png", 70, 180, 150, 150, Qt::white);
            else if (pokemon->name == "Squirtle")   pokemonItemActive = createPixmapItem("blastoise_back.png", 70, 180, 150, 150, Qt::white);
            else if (pokemon->name == "Bulbasaur")  pokemonItemActive = createPixmapItem("venusaur_back.png", 70, 180, 150, 150, Qt::white);
        }

        pokemonItemActive->setZValue(50);
        currentScene->addItem(pokemonItemActive);
                
        pText += getPokemonName(pokemon)+ "   Lv " + QString::number(pokemon->lvl);
        pokemonItemActive_Text = new QGraphicsTextItem(pText);
        pokemonItemActive_Text->setFont(QFont("Arial", 16));
        pokemonItemActive_Text->setPos(314, 220); // Adjusted to below item row
        pokemonItemActive_Text->setZValue(100);
        currentScene->addItem(pokemonItemActive_Text);

        pText  = QString::number(pokemon->pp[selectSkill]) + " / " + QString::number(pokemon->maxpp[selectSkill]);
        pokemonItemActive_Text2 = new QGraphicsTextItem(pText);
        pokemonItemActive_Text2->setFont(QFont("Arial", 13));
        pokemonItemActive_Text2->setPos(430, 275); // Adjusted to below item row
        pokemonItemActive_Text2->setZValue(100);
        currentScene->addItem(pokemonItemActive_Text2);

        showactivePokemon_HP(showPokemon);
        
    }
}

void Game::showYesButton() {
    QPointF topLeft = mapToScene(0, 0);
    hideYesButton();//remomve existed item first

    YesText = new QGraphicsTextItem("Yes");
    YesText->setFont(QFont("Arial", 16));
    YesText->setPos(topLeft.x() + 225, topLeft.y() + 385); // Adjusted to below item row
    YesText->setZValue(100);
    currentScene->addItem(YesText);
    YesRect = new QRectF(topLeft.x() + 225, topLeft.y() + 385, 30, 20);
}

void Game::hideYesButton() {
    if (YesText) {
        currentScene->removeItem(YesText);
        delete YesText;
        YesText = nullptr;
     }
    if (YesRect) {
        delete YesRect;
        YesRect = nullptr;
    }
}

void Game::hidePokemonMenu() {
    if (pokemonMenuVisible) {
        if (menuBox) {
            currentScene->removeItem(menuBox);
            delete menuBox;
            menuBox = nullptr;
        }
        if (menuText) {
            currentScene->removeItem(menuText);
            delete menuText;
            menuText = nullptr;
        }
        if (menuText0) {
            currentScene->removeItem(menuText0);
            delete menuText0;
            menuText0 = nullptr;
        }
        if (menuText1) {
            currentScene->removeItem(menuText1);
            delete menuText1;
            menuText1 = nullptr;
        }
        if (menuText2) {
            currentScene->removeItem(menuText2);
            delete menuText2;
            menuText2 = nullptr;
        }
        if (menuText3) {
            currentScene->removeItem(menuText3);
            delete menuText3;
            menuText3 = nullptr;
        }

        if (pokeBallItem) {
            currentScene->removeItem(pokeBallItem);
            delete pokeBallItem;
            pokeBallItem = nullptr;
        }
        if (pokemonItem) {
            currentScene->removeItem(pokemonItem);
            delete pokemonItem;
            pokemonItem = nullptr;
        }
        if (pokemonItem0) {
            currentScene->removeItem(pokemonItem0);
            delete pokemonItem0;
            pokemonItem0 = nullptr;
        }
        if (pokemonItem1) {
            currentScene->removeItem(pokemonItem1);
            delete pokemonItem1;
            pokemonItem1 = nullptr;
        }
        if (pokemonItem2) {
            currentScene->removeItem(pokemonItem2);
            delete pokemonItem2;
            pokemonItem2 = nullptr;
        }
        if (pokemonItem3) {
            currentScene->removeItem(pokemonItem3);
            delete pokemonItem3;
            pokemonItem3 = nullptr;
        }
        if (potionItem) {
            currentScene->removeItem(potionItem);
            delete potionItem;
            potionItem = nullptr;
        }
        if (etherItem) {
            currentScene->removeItem(etherItem);
            delete etherItem;
            etherItem = nullptr;
        }
        if (pokeBallQuantity) {
            currentScene->removeItem(pokeBallQuantity);
            delete pokeBallQuantity;
            pokeBallQuantity = nullptr;
        }
        if (potionQuantity) {
            currentScene->removeItem(potionQuantity);
            delete potionQuantity;
            potionQuantity = nullptr;
        }
        if (etherQuantity) {
            currentScene->removeItem(etherQuantity);
            delete etherQuantity;
            etherQuantity = nullptr;
        }
        pokemonMenuVisible = false;

    }
}

void Game::showBagMenu() {
    bagMenuVisible = true;
    if (pokemonMenuVisible) { hidePokemonMenu(); }
    // Position bag menu in viewport coordinates (top of window)
    QPointF topLeft = mapToScene(0, 0); // Top-left of viewport in scene coordinates
  //menuBox = new QGraphicsRectItem(topLeft.x() + 50, topLeft.y() + 50, 300, 250);
    if (itemBox) { currentScene->removeItem(itemBox); delete itemBox; itemBox = nullptr; }
    if (menuBox) { currentScene->removeItem(menuBox); delete menuBox; menuBox = nullptr; }
    itemBox = createPixmapItem("itembox_empty.png", topLeft.x() + 50, topLeft.y() + 50, 300, 50, Qt::white);
    menuBox = createPixmapItem("bag.png", topLeft.x() + 50, topLeft.y() + 100, 300, 220, Qt::white);
  //menuBox->setBrush(QBrush(Qt::white));
  //menuBox->setPen(QPen(Qt::black, 2));
    itemBox->setZValue(100); // Ensure menu is on top
    menuBox->setZValue(100); // Ensure menu is on top
    currentScene->addItem(itemBox);
    currentScene->addItem(menuBox);
    printf("%s(1):\n", __func__);
    // Item images (32x32) in top row: Poke Ball, Potion, Ether
    pokeBallItem = createPixmapItem("pokeball.jpg", topLeft.x() + 60, topLeft.y() + 60, 32, 32, Qt::red);
    pokeBallItem->setZValue(100);
    currentScene->addItem(pokeBallItem);
    potionItem = createPixmapItem("potion.jpg", topLeft.x() + 120, topLeft.y() + 60, 32, 32, Qt::blue);
    potionItem->setZValue(100);
    currentScene->addItem(potionItem);
    etherItem = createPixmapItem("ether.jpg", topLeft.x() + 180, topLeft.y() + 60, 32, 32, Qt::magenta);
    etherItem->setZValue(100);
    currentScene->addItem(etherItem);
    printf("%s(2):\n", __func__);

    // Quantity text (e.g., "x2") in black, right of each image
    pokeBallQuantity = new QGraphicsTextItem("x" + QString::number(inventory.value("Poke Ball", 0)));
    pokeBallQuantity->setFont(QFont("Arial", 12));
    pokeBallQuantity->setDefaultTextColor(Qt::black);
    pokeBallQuantity->setPos(topLeft.x() + 92, topLeft.y() + 60);
    pokeBallQuantity->setZValue(100);
    currentScene->addItem(pokeBallQuantity);
    printf("%s(3):\n", __func__);

    potionQuantity = new QGraphicsTextItem("x" + QString::number(inventory.value("Potion", 0)));
    potionQuantity->setFont(QFont("Arial", 12));
    potionQuantity->setDefaultTextColor(Qt::black);
    potionQuantity->setPos(topLeft.x() + 152, topLeft.y() + 60);
    potionQuantity->setZValue(100);
    currentScene->addItem(potionQuantity);
    printf("%s(4):\n", __func__);

    etherQuantity = new QGraphicsTextItem("x" + QString::number(inventory.value("Ether", 0)));
    etherQuantity->setFont(QFont("Arial", 12));
    etherQuantity->setDefaultTextColor(Qt::black);
    etherQuantity->setPos(topLeft.x() + 212, topLeft.y() + 60);
    etherQuantity->setZValue(100);
    currentScene->addItem(etherQuantity);
    printf("%s(5):\n", __func__);

    // Pokémon list below items
    //QString pokemonText = "Pokemon:\n";
    //QString pokemonText = "";
    int pindex = 0;
    Pokemon* p;
    for (const auto* pokemon : pokemonList) {
        p = pokemonList[pindex];
        QString pokemonText = "";
        pokemonText += getPokemonName(p) + " (HP: " + QString::number(pokemon->hp) + "/" + QString::number(pokemon->maxHp) + ")";
        printf("%s(6):\n", __func__);
        if (pindex == 0) {
            if(getPokemonName(p) =="Squirtle")    pokemonItem0 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if(getPokemonName(p) == "Charmander") pokemonItem0 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if(getPokemonName(p) == "Bulbasaur")  pokemonItem0 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white); 
            if(getPokemonName(p) == "Wartortle")  pokemonItem0 = createPixmapItem("wartortle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white); 
            if(getPokemonName(p) == "Blastoise")  pokemonItem0 = createPixmapItem("blastoise.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white); 
            if(getPokemonName(p) == "Charmeleon") pokemonItem0 = createPixmapItem("charmeleon.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if(getPokemonName(p) == "Charizard")  pokemonItem0 = createPixmapItem("charizard.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if(getPokemonName(p) == "Ivysaur")    pokemonItem0 = createPixmapItem("ivysaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white); 
            if(getPokemonName(p) == "Venusaur")   pokemonItem0 = createPixmapItem("venusaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            printf("%s(7):\n", __func__);

            /*if (pokemon->name == "Squirtle") {
                pokemonItem0 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }
            else if (pokemon->name == "Charmander") {
                pokemonItem0 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }
            else if (pokemon->name == "Bulbasaur") {
                pokemonItem0 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }*/
            pokemonItem0->setZValue(100);
            currentScene->addItem(pokemonItem0);

            menuText0 = new QGraphicsTextItem(pokemonText);
            menuText0->setFont(QFont("Arial", 12));
            menuText0->setPos(topLeft.x() + 60, topLeft.y() + 120 + pindex * 50); // Adjusted to below item row
            menuText0->setZValue(100);
            currentScene->addItem(menuText0);
            printf("%s(8):\n", __func__);
        }

        if (pindex == 1) {
            printf("%s(%d): name=%s\n", __func__, __LINE__, getPokemonName(p).toStdString().c_str());
            if (getPokemonName(p) == "Squirtle")   pokemonItem1 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmander") pokemonItem1 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Bulbasaur")  pokemonItem1 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Wartortle")  pokemonItem1 = createPixmapItem("wartortle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Blastoise")  pokemonItem1 = createPixmapItem("blastoise.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmeleon") pokemonItem1 = createPixmapItem("charmeleon.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charizard")  pokemonItem1 = createPixmapItem("charizard.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Ivysaur")    pokemonItem1 = createPixmapItem("ivysaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Venusaur")   pokemonItem1 = createPixmapItem("venusaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            printf("%s(9):\n", __func__);
           /* if (pokemon->name == "Squirtle") {
                pokemonItem1 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }
            else if (pokemon->name == "Charmander") {
                pokemonItem1 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }
            else if (pokemon->name == "Bulbasaur") {
                pokemonItem1 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }*/
            pokemonItem1->setZValue(100);
            printf("%s(%d):\n", __func__,__LINE__);
            currentScene->addItem(pokemonItem1);
            printf("%s(%d):\n", __func__, __LINE__);

            menuText1 = new QGraphicsTextItem(pokemonText);
            printf("%s(%d):\n", __func__, __LINE__);
            menuText1->setFont(QFont("Arial", 12));
            printf("%s(%d):\n", __func__, __LINE__);
            menuText1->setPos(topLeft.x() + 60, topLeft.y() + 120 + pindex * 50); // Adjusted to below item row
            printf("%s(%d):\n", __func__, __LINE__);
            menuText1->setZValue(100);
            printf("%s(%d):\n", __func__, __LINE__);
            currentScene->addItem(menuText1);
            printf("%s(10):\n", __func__);
        }

        if (pindex == 2) {
            if (getPokemonName(p) == "Squirtle")   pokemonItem2 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmander") pokemonItem2 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Bulbasaur")  pokemonItem2 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Wartortle")  pokemonItem2 = createPixmapItem("wartortle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Blastoise")  pokemonItem2 = createPixmapItem("blastoise.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmeleon") pokemonItem2 = createPixmapItem("charmeleon.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charizard")  pokemonItem2 = createPixmapItem("charizard.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Ivysaur")    pokemonItem2 = createPixmapItem("ivysaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Venusaur")   pokemonItem2 = createPixmapItem("venusaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            printf("%s(11):\n", __func__);


           /* if (pokemon->name == "Squirtle") {
                pokemonItem2 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }
            else if (pokemon->name == "Charmander") {
                pokemonItem2 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }
            else if (pokemon->name == "Bulbasaur") {
                pokemonItem2 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }*/
            pokemonItem2->setZValue(100);
            currentScene->addItem(pokemonItem2);

            menuText2 = new QGraphicsTextItem(pokemonText);
            menuText2->setFont(QFont("Arial", 12));
            menuText2->setPos(topLeft.x() + 60, topLeft.y() + 120 + pindex * 50); // Adjusted to below item row
            menuText2->setZValue(100);
            currentScene->addItem(menuText2);
            printf("%s(12):\n", __func__);

        }

        if (pindex == 3) {
            if (getPokemonName(p) == "Squirtle")   pokemonItem3 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmander") pokemonItem3 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Bulbasaur")  pokemonItem3 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Wartortle")  pokemonItem3 = createPixmapItem("wartortle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Blastoise")  pokemonItem3 = createPixmapItem("blastoise.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charmeleon") pokemonItem3 = createPixmapItem("charmeleon.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Charizard")  pokemonItem3 = createPixmapItem("charizard.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Ivysaur")    pokemonItem3 = createPixmapItem("ivysaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            if (getPokemonName(p) == "Venusaur")   pokemonItem3 = createPixmapItem("venusaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            printf("%s(13):\n", __func__);

           /* if (pokemon->name == "Squirtle") {
                pokemonItem3 = createPixmapItem("squirtle.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }
            else if (pokemon->name == "Charmander") {
                pokemonItem3 = createPixmapItem("charmander.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }
            else if (pokemon->name == "Bulbasaur") {
                pokemonItem3 = createPixmapItem("bulbasaur.png", topLeft.x() + 250, topLeft.y() + 120 + pindex * 50, 32, 32, Qt::white);
            }*/
            pokemonItem3->setZValue(100);
            currentScene->addItem(pokemonItem3);

            menuText3 = new QGraphicsTextItem(pokemonText);
            menuText3->setFont(QFont("Arial", 12));
            menuText3->setPos(topLeft.x() + 60, topLeft.y() + 120 + pindex * 50); // Adjusted to below item row
            menuText3->setZValue(100);
            currentScene->addItem(menuText3);
            printf("%s(14):\n", __func__);

        }

        //currentScene->addItem(pokemonItem);
        
        //}
//        menuText = new QGraphicsTextItem(pokemonText);
//        menuText->setFont(QFont("Arial", 12));
//        menuText->setPos(topLeft.x() + 60, topLeft.y() + 100+ pindex * 50); // Adjusted to below item row
//        menuText->setZValue(100);
//        currentScene->addItem(menuText);
        pindex++;
    }
}

void Game::hideBagMenu() {
    if (bagMenuVisible) {
        if (menuBox) {
            currentScene->removeItem(menuBox);
            delete menuBox;
            menuBox = nullptr;
        }
        if (itemBox) { currentScene->removeItem(itemBox); 
                       delete itemBox; 
                       itemBox = nullptr; 
        }
        if (menuText) {
            currentScene->removeItem(menuText);
            delete menuText;
            menuText = nullptr;
        }
        if (menuText0) {
            currentScene->removeItem(menuText0);
            delete menuText0;
            menuText0 = nullptr;
        }
        if (menuText1) {
            currentScene->removeItem(menuText1);
            delete menuText1;
            menuText1 = nullptr;
        }
        if (menuText2) {
            currentScene->removeItem(menuText2);
            delete menuText2;
            menuText2 = nullptr;
        }
        if (menuText3) {
            currentScene->removeItem(menuText3);
            delete menuText3;
            menuText3 = nullptr;
        }

        if (pokeBallItem) {
            currentScene->removeItem(pokeBallItem);
            delete pokeBallItem;
            pokeBallItem = nullptr;
        }
        if (pokemonItem) {
            currentScene->removeItem(pokemonItem);
            delete pokemonItem;
            pokemonItem = nullptr;
        }
        if (pokemonItem0) {
            currentScene->removeItem(pokemonItem0);
            delete pokemonItem0;
            pokemonItem0 = nullptr;
        }
        if (pokemonItem1) {
            currentScene->removeItem(pokemonItem1);
            delete pokemonItem1;
            pokemonItem1 = nullptr;
        }
        if (pokemonItem2) {
            currentScene->removeItem(pokemonItem2);
            delete pokemonItem2;
            pokemonItem2 = nullptr;
        }
        if (pokemonItem3) {
            currentScene->removeItem(pokemonItem3);
            delete pokemonItem3;
            pokemonItem3 = nullptr;
        }
        if (potionItem) {
            currentScene->removeItem(potionItem);
            delete potionItem;
            potionItem = nullptr;
        }
        if (etherItem) {
            currentScene->removeItem(etherItem);
            delete etherItem;
            etherItem = nullptr;
        }
        if (pokeBallQuantity) {
            currentScene->removeItem(pokeBallQuantity);
            delete pokeBallQuantity;
            pokeBallQuantity = nullptr;
        }
        if (potionQuantity) {
            currentScene->removeItem(potionQuantity);
            delete potionQuantity;
            potionQuantity = nullptr;
        }
        if (etherQuantity) {
            currentScene->removeItem(etherQuantity);
            delete etherQuantity;
            etherQuantity = nullptr;
        }
        bagMenuVisible = false;
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Game game;
    game.show();
    return app.exec();
}