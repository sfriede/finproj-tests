#include <sstream>
#include "tctestpp.h"
#include "position.h"
#include "tile.h"
#include "maze.h"
#include "entity.h"
#include "scriptedcontrol.h"
#include "chasehero.h"
#include "textui.h"
#include "basicgamerules.h"
#include "game.h"
#include "ecfactory.h"
#include <iterator>
#include <vector>


#include <iostream>
using std::cout;
using std::endl;
using std::iterator;
typedef std::vector<Entity *> EntityVec;

Maze *readFromString(const std::string &s) {
  std::stringstream ss(s);
  return Maze::read(ss);
}

Game *readGameData(const std::string &s){
  std::stringstream ss(s);
  return Game::loadGame(ss);
}
//good maze data
const char *m1 =
  "10 6\n"
  "##########\n"
  "#........#\n"
  "#.###....#\n"
  "#.#......#\n"
  "#.....<..#\n"
  "##########\n";
//good game data
const char *g3_data =
  "10 6\n"
  "##########\n"
  "#........#\n"
  "#.###....#\n"
  "#.#......#\n"
  "#.....<..#\n"
  "##########\n"
  "@uh 3 4 Mam 2 4";


// A fourth scenario - Maddie
const char *g4_data =
  "15 10\n"
  "###############\n"
  "#.............#\n"
  "#............<#\n"
  "#.............#\n"
  "#.............#\n"
  "#..###........#\n"
  "#....#........#\n"
  "#..###........#\n"
  "#.............#\n"
  "###############\n";


struct TestObjs {
  Game *game1;
  Game *game2;
  Game *game3;
  Game *game4;
};

TestObjs *setup() {
  
  TestObjs *objs = new TestObjs;
  objs->game1 = new Game();
  Maze *maze = readFromString(m1);
  objs->game1->setMaze(maze);
  objs->game3 = readGameData(g3_data);
  objs->game3->setUI(nullptr);
  objs->game3->setGameRules(new BasicGameRules());
  

  // Create an Entity with a ScriptedControl as its controller,
  // so we can simulate a series of moves.
  Entity *hero = new Entity();
  hero->setPosition(Position(1, 3));
  hero->setProperties("h");
  hero->setGlyph("@");
  ScriptedControl *controller = new ScriptedControl();
  std::vector<Direction> moves = {
    Direction::DOWN,
    Direction::RIGHT,
    Direction::RIGHT,
    Direction::RIGHT,
    Direction::RIGHT,
    Direction::RIGHT,
  };
  controller->setInput(moves);
  hero->setController(controller);
  objs->game1->addEntity(hero);
  
  Entity *new_ent = new Entity();
  new_ent->setGlyph("*");
  new_ent->setProperties("m");
  new_ent->setPosition(Position(2, 6));
  EntityControllerFactory *ecfactory = EntityControllerFactory::getInstance();
  EntityController *ctrl = ecfactory->createFromChar('a');
  new_ent->setController(ctrl);
  objs->game1->addEntity(new_ent);
  
  objs->game1->setGameRules(new BasicGameRules());
  objs->game1->setUI(nullptr);

  // game2 is like game1, but no entities are added.
  // The individual test functions that use it will add entities
  // (so that various AI control scenarios can be tested.)
  objs->game2 = new Game();
  objs->game2->setMaze(readFromString(m1));
  objs->game2->setGameRules(new BasicGameRules());
  objs->game2->setUI(nullptr);

  // game 4 - larger maze with different wall placement
  // for further testing of chaseHero - Maddie 
  objs->game4 = new Game();
  objs->game4->setMaze(readFromString(g4_data));
  objs->game4->setGameRules(new BasicGameRules());
  objs->game4->setUI(nullptr);

  return objs;
}

void cleanup(TestObjs *objs) {
  delete objs->game1;
  delete objs->game2;
  delete objs->game3;
  delete objs->game4; //Maddie 
  delete objs;
}

void testGetEntitiesWithProperty(TestObjs *objs);
void testTakeTurn(TestObjs *objs);
void testGetEntityAt(TestObjs *objs);
void testLoadGame(TestObjs *objs);
void testChaseHero1(TestObjs *objs);
void testChaseHero2(TestObjs *objs); //Maddie 


int main(int argc, char *argv[]) {
  TEST_INIT();

  // allow test name to be specified on the command line
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST(testGetEntitiesWithProperty);
  TEST(testTakeTurn);
  TEST(testGetEntityAt);
  TEST(testLoadGame);
  TEST(testChaseHero1);
  TEST(testChaseHero2);


  TEST_FINI();
}

void testLoadGame(TestObjs *objs){
  
  const EntityVec entities = objs->game3->getEntities();
  EntityVec::const_iterator it = entities.cbegin();
  ASSERT((*it)->getGlyph() == "@");
  ASSERT((*it)->getProperties() == "h");
  Position p1(3, 4);
  ASSERT((*it)->getPosition()== p1);

  it = entities.cend();
  it--;
  ASSERT((*it)->getGlyph() == "M");
  ASSERT((*it)->getProperties() == "m");
  Position p2(2, 4);
  ASSERT((*it)->getPosition() == p2);
 
}
void testGetEntityAt(TestObjs *objs){ 
  Position p1(1, 3);
  Entity *ent  = objs->game1->getEntityAt(p1);
  ASSERT(ent->getGlyph() == "@");
  Position p3(0,0);
  Entity *ent1 = objs->game1->getEntityAt(p3);
  ASSERT(ent1 == nullptr);
  Position p2(2, 6);
  Entity *ent2 = objs->game1->getEntityAt(p2);
  ASSERT(ent2->getGlyph() == "*");

}
void testGetEntitiesWithProperty(TestObjs *objs) {
  std::vector<Entity *> heroes = objs->game1->getEntitiesWithProperty('h');
  ASSERT(1 == heroes.size());
  ASSERT(heroes[0]->getGlyph() == "@");
}

void testTakeTurn(TestObjs *objs) {
 
  Game *game1 = objs->game1;
  GameRules *gameRules1 = objs->game1->getGameRules();
  std::vector<Entity *> heroes = game1->getEntitiesWithProperty('h');
  Entity *hero = heroes.front();
  ASSERT(Position(1, 3) == hero->getPosition());
  ASSERT(gameRules1->checkGameResult(game1) == GameResult::UNKNOWN);

  // play the sequence of scripted moves
  game1->takeTurn(hero);
  ASSERT(Position(1, 4) == hero->getPosition());
  ASSERT(gameRules1->checkGameResult(game1) == GameResult::UNKNOWN);
  game1->takeTurn(hero);
  ASSERT(Position(2, 4) == hero->getPosition());
  ASSERT(gameRules1->checkGameResult(game1) == GameResult::UNKNOWN);
  game1->takeTurn(hero);
  ASSERT(Position(3, 4) == hero->getPosition());
  ASSERT(gameRules1->checkGameResult(game1) == GameResult::UNKNOWN);
  game1->takeTurn(hero);
  ASSERT(Position(4, 4) == hero->getPosition());
  ASSERT(gameRules1->checkGameResult(game1) == GameResult::UNKNOWN);
  game1->takeTurn(hero);
  ASSERT(Position(5, 4) == hero->getPosition());
  ASSERT(gameRules1->checkGameResult(game1) == GameResult::UNKNOWN);
  game1->takeTurn(hero);
  ASSERT(Position(6, 4) == hero->getPosition());
  // hero should be at the goal now
  ASSERT(gameRules1->checkGameResult(game1) == GameResult::HERO_WINS);
 
}

void testChaseHero1(TestObjs *objs) {
  Game *game2 = objs->game2;

  // add hero with scripted sequence of moves
  ScriptedControl *heroController = new ScriptedControl();
  std::vector<Direction> heroMoves = {
    Direction::NONE, Direction::DOWN, Direction::UP, Direction::UP
  };
  heroController->setInput(heroMoves);
  Entity *hero = new Entity();
  hero->setGlyph("@");
  hero->setProperties("h");
  hero->setController(heroController);
  hero->setPosition(Position(1, 3));
  game2->addEntity(hero);

  // add minotaur controlled by ChaseHero
  Entity *minotaur = new Entity();
  minotaur->setGlyph("M");
  minotaur->setProperties("m");
  minotaur->setController(new ChaseHero());
  minotaur->setPosition(Position(3, 3));
  game2->addEntity(minotaur);

  // hero's first turn does nothing
  game2->takeTurn(hero);
  ASSERT(Position(1, 3) == hero->getPosition());

  // minotaur should also not move because there is neither a vertical
  // nor horizontal move that gets closer to the hero
  game2->takeTurn(minotaur);
  ASSERT(Position(3, 3) == minotaur->getPosition());

  // hero's second turn should move down
  game2->takeTurn(hero);
  ASSERT(Position(1, 4) == hero->getPosition());

  // minotaur would prefer to move horizontally (left), but can't, so will move
  // vertically (down)
  game2->takeTurn(minotaur);
  ASSERT(Position(3, 4) == minotaur->getPosition());

  // hero's third turn should move up
  game2->takeTurn(hero);
  ASSERT(Position(1, 3) == hero->getPosition());

  // minotaur should now move horizontally (left) because it is no longer
  // blocked in that direction
  game2->takeTurn(minotaur);
  ASSERT(Position(2, 4) == minotaur->getPosition());

  // hero moves up again
  game2->takeTurn(hero);
  ASSERT(Position(1, 2) == hero->getPosition());

  // minotaur would prefer to move vertically, but can't, so moves horizontally
  game2->takeTurn(minotaur);
  ASSERT(Position(1, 4) == minotaur->getPosition());
}

void testChaseHero2(TestObjs *objs) { //Maddie
  Game *game4 = objs->game4;

  // add hero with scripted sequence of moves
  ScriptedControl *heroController = new ScriptedControl();
  std::vector<Direction> heroMoves = {Direction::DOWN, Direction::RIGHT, Direction::RIGHT, Direction::UP};
  heroController->setInput(heroMoves);

  Entity *hero = new Entity();
  hero->setGlyph("@");
  hero->setProperties("h");
  hero->setController(heroController);
  hero->setPosition(Position(6, 7));
  game4->addEntity(hero);

  Entity *minotaur = new Entity();
  minotaur->setGlyph("M");
  minotaur->setProperties("m");
  minotaur->setController(new ChaseHero());
  minotaur->setPosition(Position(2, 5));
  game4->addEntity(minotaur);

  // hero first moves down
  game4->takeTurn(hero);
  ASSERT(Position(6, 8) == hero->getPosition());

  // minotaur prefers to move right but is blocked, so moves down
  game4->takeTurn(minotaur);
  ASSERT(Position(2, 6) == minotaur->getPosition());

  // hero moves right
  game4->takeTurn(hero);
  ASSERT(Position(7, 8) == hero->getPosition());

  // farther horizontally and unblocked, minotaur moves right
  game4->takeTurn(minotaur);
  ASSERT(Position(3, 6) == minotaur->getPosition());

  // hero moves right again
  game4->takeTurn(hero);
  ASSERT(Position(8, 8) == hero->getPosition());

  // still farther horizontally, minotaur moves right again
  game4->takeTurn(minotaur);
  ASSERT(Position(4, 6) == minotaur->getPosition());

  // hero moves up
  game4->takeTurn(hero);
  ASSERT(Position(8, 7) == hero->getPosition());

  // minotaur prefers to move right or down, but both moves
  // are blocked. No move by minotaur
  game4->takeTurn(minotaur);
  ASSERT(Position(4, 6) == minotaur->getPosition());

}

