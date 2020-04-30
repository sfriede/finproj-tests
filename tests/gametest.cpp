// Note: uncomment the following #define if you would like to use
// the test(s) for the TextUI class
//#define USE_TEXTUI_TESTS

// If you do define USE_TEXTUI_TESTS, then you will need to make the
// following (minor) changes to your project:
//
// (1) Delete (or comment out) the line reading
//       assert(s_instance == nullptr);
//     in the constructor for the UI class (in ui.cpp).
//
// (2) Add build/textui.o to the Makefile target for gametest

#include <sstream>
#include "tctestpp.h"
#include "position.h"
#include "tile.h"
#include "maze.h"
#ifdef USE_TEXTUI_TESTS
#  include "textui.h"
#endif // USE_TEXTUI_TESTS
#include "entity.h"
#include "scriptedcontrol.h"
#include "chasehero.h"
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
  Game *game2;
};

TestObjs *setup() {
  TestObjs *objs = new TestObjs;
  objs->game1 = new Game();
  Maze *maze = readFromString(m1);
  objs->game1->setMaze(maze);
#ifdef USE_TEXTUI_TESTS
  TextUI *t_ui = new TextUI();
  objs->game1->setUI(t_ui);
#endif // USE_TEXTUI_TESTS

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

  // game2 is like game1, but no entities are added.
  // The individual test functions that use it will add entities
  // (so that various AI control scenarios can be tested.)
  objs->game2 = new Game();
  objs->game2->setMaze(readFromString(m1));
  objs->game2->setGameRules(new BasicGameRules());

  return objs;
}

void cleanup(TestObjs *objs) {
  delete objs->game1;
  delete objs->game2;
  delete objs;
}

void testGetEntitiesWithProperty(TestObjs *objs);
void testTakeTurn(TestObjs *objs);
void testChaseHero1(TestObjs *objs);
#ifdef USE_TEXTUI_TESTS
void testTextUIRender(TestObjs *objs);
#endif // USE_TEXTUI_TESTS

int main(int argc, char *argv[]) {
  TEST_INIT();

  // allow test name to be specified on the command line
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST(testGetEntitiesWithProperty);
  TEST(testTakeTurn);
  TEST(testChaseHero1);
#ifdef USE_TEXTUI_TESTS
  TEST(testTextUIRender);
#endif // USE_TEXTUI_TESTS

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

#ifdef USE_TEXTUI_TESTS
void testTextUIRender(TestObjs *objs) {
  //redirect cout to a string stream
  std::stringstream renderOutput;
  std::streambuf *coutbuf = std::cout.rdbuf();
  std::cout.rdbuf(renderOutput.rdbuf());
  //call render
  objs->game1->getUI()->render(objs->game1);
  //direct cout back to cout
  std::cout.rdbuf(coutbuf);

  //Sanity Checks
  //std::cout << renderOutput.str() << std::endl;
  //std::cout << objs->OGMaze << std::endl;

  std::string expected_maze = 
  "##########\n"
  "#........#\n"
  "#.###....#\n"
  "#@#......#\n"
  "#.....<..#\n"
  "##########\n";

  std::string rendered = renderOutput.str();
  //ASSERT(renderOutput.str() == expected_maze);
  ASSERT(rendered == expected_maze);

}
#endif // USE_TEXTUI_TESTS
