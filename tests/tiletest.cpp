#include "tctestpp.h"
#include "tile.h"
#include "tilefactory.h"
#include "entity.h"

struct TestObjs {
  Tile *floor;
  Tile *wall;
  Tile *goal;
  Entity *hero;
};

TestObjs *setup() {
  TestObjs *objs = new TestObjs;
  objs->floor = TileFactory::getInstance()->createFromChar('.');
  objs->wall = TileFactory::getInstance()->createFromChar('#');
  objs->goal = TileFactory::getInstance()->createFromChar('<');
  objs->hero = new Entity();
  objs->hero->setGlyph("@");
  objs->hero->setProperties("h");
  objs->hero->setPosition(Position(5, 5));
  return objs;
}

void cleanup(TestObjs *objs) {
  delete objs->floor;
  delete objs->wall;
  delete objs->goal;
  delete objs->hero;
  delete objs;
}

void testWallBlockMove(TestObjs *objs);
void testFloorAllowMove(TestObjs *objs);
void testGoalAllowMove(TestObjs *objs);
void testIsGoal(TestObjs *objs);

int main(int argc, char *argv[]) {
  TEST_INIT();

  // allow test name to be specified on the command line
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST(testWallBlockMove);
  TEST(testFloorAllowMove);
  TEST(testGoalAllowMove);
  TEST(testIsGoal);

  TEST_FINI();
}

void testWallBlockMove(TestObjs *objs) {
  Position heroPos = objs->hero->getPosition();
  ASSERT(objs->wall->checkMoveOnto(objs->hero, heroPos, heroPos.displace(Direction::RIGHT)) == MoveResult::BLOCK);
}

void testFloorAllowMove(TestObjs *objs) {
  Position heroPos = objs->hero->getPosition();
  ASSERT(objs->floor->checkMoveOnto(objs->hero, heroPos, heroPos.displace(Direction::RIGHT)) == MoveResult::ALLOW);
}

void testGoalAllowMove(TestObjs *objs) {
  Position heroPos = objs->hero->getPosition();
  ASSERT(objs->goal->checkMoveOnto(objs->hero, heroPos, heroPos.displace(Direction::RIGHT)) == MoveResult::ALLOW);
}

void testIsGoal(TestObjs *objs) {
  ASSERT(!objs->wall->isGoal());
  ASSERT(!objs->floor->isGoal());
  ASSERT(objs->goal->isGoal());
}
