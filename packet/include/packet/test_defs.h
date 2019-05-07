#ifndef PACKET_TEST_DEFS_H_
#define PACKET_TEST_DEFS_H_

#include <iostream>
#include <vector>

#include <packet/debug_helper.h>


struct TestCase;
static std::vector<TestCase*> ALL_TEST_CASES;

struct TestCase {
    TestCase(const char* name) : name_(name), failed_(false) {
      ALL_TEST_CASES.push_back(this);
    }
    virtual ~TestCase() {
      if (failed_) {
        fail();
      } else {
        success();
      }
    }

    void success(void)
    {
      std::cout << "Test case " << name_ << " SUCCESS\n";
    }

    void fail(void)
    {
      std::cout << "Test case " << name_ << " FAILED\n";
    }

    virtual void run(void) = 0;

    const char* name_;
    bool failed_;
};


#define RUN_ALL_TESTS \
  for (TestCase* tc : ALL_TEST_CASES) tc->run();


#define DEF_TEST(test_name, impl_body) \
  struct _Test_##test_name : TestCase {\
  _Test_##test_name() : TestCase(#test_name) {}\
  virtual void run(void) override impl_body\
};\
  static _Test_##test_name _instance_##test_name;


#define _EVAL_COND(cond) if(!(cond)) { failed_ = true; LOG_ERROR(#cond); }


#define EXPECT_EQ(a, b) _EVAL_COND(a == b)
#define EXPECT_GT(a, b) _EVAL_COND(a > b)
#define EXPECT_GE(a, b) _EVAL_COND(a >= b)


#endif // PACKET_TEST_DEFS_H_
