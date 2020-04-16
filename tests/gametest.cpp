#include <sstream>
#include "tctestpp.h"
#include "position.h"
#include "tile.h"
#include "maze.h"
#include "entity.h"
#include "scriptedcontrol.h"
#include "basicgamerules.h"
#include "game.h"

Maze *readFromString(const std::string &s) {
  std::stringstream ss(s);
  return Maze::read(ss);
}

const char *m1 =
  "10 6\n"
  "##########\n"
  "#........#\n"
  "#.###....#\n"
  "#.#......#\n"
  "#.....<..#\n"
  "##########\n";

struct TestObjs {
  Game *game1;
};

TestObjs *setup() {
  TestObjs *objs = new TestObjs;
  objs->game1 = new Game();
  Maze *maze = readFromString(m1);
  objs->game1->setMaze(maze);

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

  objs->game1->setGameRules(new BasicGameRules());

  return objs;
}

void cleanup(TestObjs *objs) {
  delete objs->game1;
  delete objs;
}

void testGetEntitiesWithProperty(TestObjs *objs);
void testTakeTurn(TestObjs *objs);

int main(int argc, char *argv[]) {
  TEST_INIT();

  // allow test name to be specified on the command line
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST(testGetEntitiesWithProperty);
  TEST(testTakeTurn);

  TEST_FINI();
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
