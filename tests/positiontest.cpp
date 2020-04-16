#include "tctestpp.h"
#include "position.h"

// Test fixture object data type.
struct TestObjs {
  Position p1, p2, p3, p4, p5, p6, p7, p8;

  TestObjs()
    : p1(16, 4)
    , p2(17, 5) // greater than p1 (y greater))
    , p3(15, 5) // greater than p1 (y greater)
    , p4(17, 4) // greater than p1 (y same, x greater)
    , p5(17, 3) // less than p1 (y less)
    , p6(15, 3) // less than p1 (y less)
    , p7(15, 4) // less than p1 (y same, x less)
    , p8(16, 4) // equal to p1
  {
  }
};

// Create a dynamic test fixture object.
TestObjs *setup() {
  return new TestObjs;
}

// Destroy a dynamic test fixture object.
void cleanup(TestObjs *objs) {
  delete objs;
}

// Declarations of test functions.
void testGetX(TestObjs *objs);
void testGetY(TestObjs *objs);
void testDistanceFrom(TestObjs *objs);
void testEquality(TestObjs *objs);
void testInequality(TestObjs *objs);
void testDisplace(TestObjs *objs);
void testAssign(TestObjs *objs);
void testInBounds(TestObjs *objs);
void testLessThan(TestObjs *objs);

// main function
int main(int argc, char *argv[]) {
  TEST_INIT();

  // allow test name to be specified on the command line
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST(testGetX);
  TEST(testGetY);
  TEST(testDistanceFrom);
  TEST(testEquality);
  TEST(testInequality);
  TEST(testDisplace);
  TEST(testAssign);
  TEST(testInBounds);
  TEST(testLessThan);

  TEST_FINI();
}

// The rest of the program is definitions of test functions

void testGetX(TestObjs *objs) {
  ASSERT(16 == objs->p1.getX());
  ASSERT(17 == objs->p2.getX());
  ASSERT(15 == objs->p3.getX());
  ASSERT(17 == objs->p4.getX());
  ASSERT(17 == objs->p5.getX());
  ASSERT(15 == objs->p6.getX());
  ASSERT(15 == objs->p7.getX());
  ASSERT(16 == objs->p8.getX());
}

void testGetY(TestObjs *objs) {
  ASSERT(4 == objs->p1.getY());
  ASSERT(5 == objs->p2.getY());
  ASSERT(5 == objs->p3.getY());
  ASSERT(4 == objs->p4.getY());
  ASSERT(3 == objs->p5.getY());
  ASSERT(3 == objs->p6.getY());
  ASSERT(4 == objs->p7.getY());
  ASSERT(4 == objs->p8.getY());
}

void testDistanceFrom(TestObjs *objs) {
  ASSERT(objs->p1.distanceFrom(objs->p1) == 0);
  ASSERT(objs->p1.distanceFrom(objs->p2) == 2);
  ASSERT(objs->p1.distanceFrom(objs->p3) == 2);
  ASSERT(objs->p1.distanceFrom(objs->p4) == 1);
  ASSERT(objs->p1.distanceFrom(objs->p5) == 2);
  ASSERT(objs->p1.distanceFrom(objs->p6) == 2);
  ASSERT(objs->p1.distanceFrom(objs->p7) == 1);
  ASSERT(objs->p1.distanceFrom(objs->p8) == 0);
}

void testEquality(TestObjs *objs) {
  ASSERT(!(objs->p1 == objs->p2));
  ASSERT(!(objs->p1 == objs->p3));
  ASSERT(!(objs->p1 == objs->p4));
  ASSERT(!(objs->p1 == objs->p5));
  ASSERT(!(objs->p1 == objs->p6));
  ASSERT(!(objs->p1 == objs->p7));
  ASSERT(objs->p1 == objs->p8);
}

void testInequality(TestObjs *objs) {
  ASSERT(objs->p1 != objs->p2);
  ASSERT(objs->p1 != objs->p3);
  ASSERT(objs->p1 != objs->p4);
  ASSERT(objs->p1 != objs->p5);
  ASSERT(objs->p1 != objs->p6);
  ASSERT(objs->p1 != objs->p7);
  ASSERT(!(objs->p1 != objs->p8));
}

void testDisplace(TestObjs *objs) {
  ASSERT(objs->p1.displace(Direction::UP) == Position(16, 3));
  ASSERT(objs->p1.displace(Direction::DOWN) == Position(16, 5));
  ASSERT(objs->p1.displace(Direction::LEFT) == Position(15, 4));
  ASSERT(objs->p1.displace(Direction::RIGHT) == Position(17, 4));
  ASSERT(objs->p1.displace(Direction::NONE) == objs->p1);
}

void testAssign(TestObjs *objs) {
  Position p(99, 99);

  p = objs->p1;
  ASSERT(p == objs->p1);
  p = objs->p2;
  ASSERT(p == objs->p2);
}

void testInBounds(TestObjs *objs) {
  const int w = 20, h = 10;
  ASSERT(objs->p1.inBounds(w, h));
  ASSERT(objs->p2.inBounds(w, h));
  ASSERT(objs->p3.inBounds(w, h));
  ASSERT(objs->p4.inBounds(w, h));
  ASSERT(objs->p5.inBounds(w, h));
  ASSERT(objs->p6.inBounds(w, h));
  ASSERT(objs->p7.inBounds(w, h));
  ASSERT(objs->p8.inBounds(w, h));

  Position out1(-1, 5);
  Position out2(20, 5);
  Position out3(10, -1);
  Position out4(10, 10);

  ASSERT(!out1.inBounds(w, h));
  ASSERT(!out2.inBounds(w, h));
  ASSERT(!out3.inBounds(w, h));
  ASSERT(!out4.inBounds(w, h));
}

void testLessThan(TestObjs *objs) {
  ASSERT(objs->p1 < objs->p2);
  ASSERT(objs->p1 < objs->p3);
  ASSERT(objs->p1 < objs->p4);
  ASSERT(objs->p5 < objs->p1);
  ASSERT(objs->p6 < objs->p1);
  ASSERT(objs->p7 < objs->p1);
}
