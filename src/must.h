#ifndef MOKA_MUST_H
#define MOKA_MUST_H

#include <sstream>
#include <string>

#define must_equal(a, b)     Moka::must::equal(__FILE__, __LINE__, a, b)
#define must_not_equal(a, b) Moka::must::not_equal(__FILE__, __LINE__, a, b)
#define must_throw(T, f)     Moka::must::throoow<T>(__FILE__, __LINE__, #T, f)
#define must_fail(m)         Moka::must::fail(__FILE__, __LINE__, m)

#define R(x) "\e[31m" << (x) << "\e[0m"
#define G(x) "\e[32m" << (x) << "\e[0m"
#define Y(x) "\e[33m" << (x) << "\e[0m"

namespace Moka
{
  // Representation helpers for types that print funny:
  template<class T> const T& rep(const T& t) {return t;}
  const char* rep(bool b) {return b ? "true" : "false";}
  const char* rep(const std::nullptr_t&) {return "nullptr";}

  class Failure: public std::exception {
    const char* mFile;
    const int   mLine;
    std::string mWhat;
  public:
    Failure(const std::string& m): mFile(nullptr), mLine(0), mWhat(m) {}
    Failure(const char* f, int l, const std::string& m): mFile(f), mLine(l), mWhat(m) {}
    ~Failure() throw() {}

    const char* file() const {
      return mFile;
    }

    int line() const {
      return mLine;
    }

    const char* what() const throw() {
      return mWhat.c_str();
    }
  };

  namespace must {
    template <class A, class B>
    void equal(const char* f, int l, const A& a, const B& b) {
      if(a != b) {
        std::stringstream message;
        message << "Expected " << G(rep(b)) << " but got " << R(rep(a));
        throw new Failure(f, l, message.str());
      }
    }

    void fail(const char* f, int l,  const char* m) {
      std::stringstream message;
      message << R(m);
      throw new Failure(f, l, message.str());
    }

    template <class A, class B>
    void not_equal(const char* f, int l, const A& a, const B& b) {
      if(a == b) {
        std::stringstream message;
        message << "Expected anything but " << R(rep(b)) << " but got " << R(rep(a));
        throw new Failure(f, l, message.str());
      }
    }

    template <class E>
    void throoow(const char* f, int l, const char* type, std::function<void()> fn) {
      try {
        fn();
      }
      catch(E& e) {
        // Good catch!
        return;
      }
      catch(std::exception& e) {
        std::stringstream message;
        message << "Expected to catch a " << G(type) << " but got " << R(e.what());
        throw new Failure(f, l, message.str());
      }

      std::stringstream message;
      message << "Expected to catch a " << G(type) << " but nothing was thrown!";
      throw new Failure(f, l, message.str());
    }
  }
}

#undef R
#undef G
#undef Y

#endif
