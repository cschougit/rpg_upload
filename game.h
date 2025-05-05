#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>
#include <QMap>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QList>
#include <QKeyEvent>
#include <QColor>

class Player;
class Pokemon;

class Game : public QGraphicsView {
    Q_OBJECT
public:
    Game(QWidget* parent = nullptr);
    ~Game();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event);
private:
    QMap<QString, QGraphicsScene*> scenes;
    QString currentSceneName;
    QGraphicsScene* currentScene;
    Player* player;
    QGraphicsPixmapItem* playerItem;
    QList<QGraphicsRectItem*> barriers;
    QList<QGraphicsPixmapItem*> yellowBoxes;
    QList<QGraphicsPixmapItem*> Pokeball_Mons;
    QList<QGraphicsRectItem*> Lbarriers;
    QList<QGraphicsPixmapItem*> WildPokemons;
    QList<Pokemon*> wildpokemonList;
    QList<QGraphicsRectItem*> TallGrass1;
    QList<QGraphicsRectItem*> TallGrass2;
    QList<QGraphicsRectItem*> TallGrass3;
    QList<QGraphicsRectItem*> TallGrass4;
    QList<QGraphicsRectItem*> TallGrass5;
    QList<QGraphicsRectItem*> TallGrass6;
    QMap<QString, int> inventory;
    QList<Pokemon*> pokemonList;
    bool bagMenuVisible;
    bool dialogVisible;
    bool pokemonMenuVisible;
    bool skillMenuVisible;
    QGraphicsPixmapItem* dialogBox;
    QGraphicsTextItem* dialogText;
  //QGraphicsRectItem* menuBox;
    QGraphicsPixmapItem* menuBox;
    QGraphicsPixmapItem* itemBox;
    QGraphicsRectItem* bagmenuBox;
    QGraphicsTextItem* menuText;
    QGraphicsPixmapItem* pokeBallItem; // New: Poke Ball image
    QGraphicsPixmapItem* potionItem;    // New: Potion image
    QGraphicsPixmapItem* etherItem;     // New: Ether image
    QGraphicsTextItem* pokeBallQuantity; // New: Poke Ball quantity text
    QGraphicsTextItem* potionQuantity;   // New: Potion quantity text
    QGraphicsTextItem* etherQuantity;    // New: Ether quantity text
    QGraphicsPixmapItem* pokemonItem; // New: Pokemon image
    QGraphicsPixmapItem* pokemonItem0; // New: Pokemon image
    QGraphicsPixmapItem* pokemonItem1; // New: Pokemon image
    QGraphicsPixmapItem* pokemonItem2; // New: Pokemon image
    QGraphicsPixmapItem* pokemonItem3; // New: Pokemon image
    QGraphicsTextItem* menuText0;
    QGraphicsTextItem* menuText1;
    QGraphicsTextItem* menuText2;
    QGraphicsTextItem* menuText3;
    QGraphicsTextItem* cursor;
    QGraphicsTextItem* battleText;
    QGraphicsPixmapItem* wildp;
    QGraphicsTextItem* wildp_Text;
    QGraphicsTextItem* wildp_Text2;
    QGraphicsRectItem* wildp_HP;
    Pokemon* wildPokemon;
    Pokemon* caughtPokemon;
    QGraphicsPixmapItem* pokemonItemActive;
    QGraphicsTextItem* pokemonItemActive_Text;
    QGraphicsTextItem* pokemonItemActive_Text2;
    QGraphicsTextItem* pokemonItemActive_Text3;
    QGraphicsTextItem* pokemonItemActive_Text4;
    QGraphicsRectItem* pokemonItemActive_HP;
    QGraphicsRectItem* pokemonItemActive_EXP;
    QGraphicsPixmapItem* skillmenuBox;
    QGraphicsTextItem* skillmenuText0;
    QGraphicsTextItem* skillmenuText1;
    QGraphicsTextItem* skillmenuText2;
    QGraphicsTextItem* skillmenuText3;
    QGraphicsTextItem* skillmenuText4;
    QGraphicsTextItem* skillmenuText5;

    QGraphicsTextItem* YesText;
    QRectF*            YesRect;

    bool pokeBall_Mon_ready;
    bool yellowBox_ready;
    int Encounter_Pokemon;
    bool Go_Battle;
    int cursorIndex;
    int activePokemon;
    int activePokemon_new;
    int selectSkill;
    int playerdirection;
    int selectItem;
    int winbattle;
    int catchpokemon;
    int gameover;
    bool firstSelect;

    QRectF fightRect;
    QRectF bagRect;
    QRectF pokemonRect;
    QRectF runRect;

    QRectF* pokemonRect_0;
    QRectF* pokemonRect_1;
    QRectF* pokemonRect_2;
    QRectF* pokemonRect_3;

    QRectF bagRect_0;
    QRectF bagRect_1;
    QRectF bagRect_2;

    QRectF skillRect_0;
    QRectF skillRect_1;
    QRectF skillRect_2;
    QRectF skillRect_3;

    QString skillname[3][4] = { {"Tackle" ,"Tail Whip","Protect",   "Wave Crash"},
                                {"Scratch","Growl",    "Scary Face","Flare Blitz"},
                                {"Tackle" ,"Growl",    "Growth",    "Razor Leaf"}};
//};


    QGraphicsPixmapItem* createPixmapItem(const QString& fileName, qreal x, qreal y, qreal w, qreal h, const QColor& fallbackColor);
    void switchScene(const QString& sceneName);
    void setupTitleScene();
    void setupTownScene();
    void placeYellowBoxes(QGraphicsScene* scene);
    void setupLaboratoryScene();
    void setupGrasslandScene();
    void setupBattleScene();
    void handleTownKeyPress(QKeyEvent* event);
    void handleLabKeyPress(QKeyEvent* event);
    void handleGrassLandKeyPress(QKeyEvent* event);
    void handleBattleKeyPress(QKeyEvent* event);
    void updatePlayerPosition();
    void updatePlayerPosition_lab();
    void updatePlayerPosition_land();
    void updatePlayerPosition_battle();
    void checkSceneTransitions();
    void checkSceneTransitions_lab();
    void checkSceneTransitions_land();
    void checkWildPokemon_land();
    void setPlayerbacktotown_lab();
    void setPlayerbacktotown_land();
    void setPlayerbacktograssland();
    void checkInteractions();
    void checkInteractions_lab();
    void checkInteractions_land();
    void checkInteractions_battle();
    void openYellowBox(QGraphicsPixmapItem* box);
    void showDialog(const QString& message);
    void hideDialog();
    void showBagMenu();
    void hideBagMenu();
    void showBagMenu_nopokemon();
    void hideBagMenu_nopokemon();
    void showPokemonMenu();
    void hidePokemonMenu();
    void showSkillMenu();
    void hideSkillMenu();
    void clearSkillRect();
    void updateCursorPosition();
    void selectOption();
    void removeWildPokemons();
    void showactivePokemon(int );
    void showactivePokemon_HP(int );
    void showYesButton();
    void hideYesButton();
    void wildpokemon_attack();
    void msleep(float );
    void showWildPokemon();
    void hideWildPokemon();
    void showWildPokemon_HP();
    void updateplayerdirection();
    void player_attack();
    void player_useitem();
    void copywildpokemon();
    void showSkillpp(int );
    int  checkgameover();
    void showgameover();
    void updatelvlup();
    QString getPokemonName(Pokemon*);
    int  pokemonid(QString );

};

#endif // GAME_H