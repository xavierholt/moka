#ifndef MOKA_TEST_H
#define MOKA_TEST_H

#include "must.h"

#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace Moka
{
  class Base {
  public:
    virtual void test(int level) = 0;
    virtual int  report(int level) = 0;
    virtual int  run() {
      this->test(0);
      std::cout << "\n\n";
      return this->report(0);
    }

    void indent(int level) {
      while(level --> 0) std::cout << "  ";
    }
  };

  class Test: public Base {
  protected:
    std::string           mName;
    std::function<void()> mFunction;
    Failure*              mError;
  public:
    Test(std::string name, std::function<void()> fn): mName(name), mFunction(fn) {
      mError = nullptr;
    }

    int report(int) {
      if(mError) {
        std::cout << "in " << mName << "\n";
        if(mError->file()) std::cout << "  in " << cli::bold(mError->file()) << ':' << mError->line() << "\n";
        std::cout << "    " << mError->what() << "\n\n";
        return 1;
      }
      else {
        return 0;
      }
    }

    void test(int level) {
      indent(level);

      try {
        mFunction();
        std::cout << cli::g("✔ ", true) << mName << "\n";
      }
      catch(Failure* error) {
        std::cout << cli::r("✘ ", true) << mName << "\n";
        mError = error;
      }
      catch(std::exception& error) {
        std::cout << cli::y("▲ ", true) << mName << "\n";
        mError = new Failure(std::string("Unexpected exception: ") + cli::y(error.what()));
      }
    }
  };

  class Context: public Base {
  protected:
    std::string           mName;
    std::vector<Base*>    mMembers;
    std::function<void()> mSetup;
    std::function<void()> mTeardown;
    bool                  mHasSetup;
    bool                  mHasTeardown;
  public:
    Context(std::string name): mName(name), mMembers() {
      mHasTeardown = false;
      mHasSetup = false;
    }

    Context(std::string name, std::function<void(Context&)> fn): mName(name), mMembers() {
      mHasTeardown = false;
      mHasSetup = false;
      fn(*this);
    }

    void has(std::string name, std::function<void(Context&)> fn) {
      Context* child = new Context(name, fn);
      mMembers.push_back(child);
    }

    int report(int level = 0) {
      int failures = 0;
      for(auto m: mMembers) failures += m->report(level + 1);
      return failures;
    }

    void setup(std::function<void()> fn) {
      mHasSetup = true;
      mSetup = fn;
    }

    void should(std::string name, std::function<void()> fn) {
      Test* test = new Test("should " + name, fn);
      mMembers.push_back(test);
    }

    void teardown(std::function<void()> fn) {
      mHasTeardown = true;
      mTeardown = fn;
    }

    void test(int level = 0) {
      indent(level);
      std::cout << cli::bold(mName) << "\n";
      if(mHasSetup) mSetup();
      for(auto m: mMembers) m->test(level + 1);
      if(mHasTeardown) mTeardown();
    }
  };
}

#endif
