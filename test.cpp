#include <UnitTest++/UnitTest++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

#include "AuthBase.h"
#include "Calculator.h"
#include "CryptoAuth.h"
#include "UserInterface.h"
#include "errors.h"

SUITE(UserInterfaceTests) {

TEST(Test_1_1) {
    UserInterface ui;
    const char* argv[] = {"server", "-h", nullptr};
    CHECK(!ui.parse(2, argv));
}

TEST(Test_1_2) {
    UserInterface ui;
    const char* argv[] = {"server", "--run", nullptr};
    CHECK(ui.parse(2, argv));
}

TEST(Test_1_3) {
    UserInterface ui;
    const char* argv[] = {"server", "--run", "-p", "40000", nullptr};
    CHECK(ui.parse(4, argv));
}

TEST(Test_1_4) {
    UserInterface ui;
    const char* argv[] = {"server", "-p", "40000", nullptr};
    CHECK(!ui.parse(3, argv));
}

TEST(Test_1_5) {
    UserInterface ui;
    const char* argv[] = {"server", "--run", "-x", nullptr};
    CHECK_THROW(ui.parse(3, argv), ServerError);
}

TEST(Test_1_6) {
    UserInterface ui;
    const char* argv[] = {"server", "--run", "-p", "99999", nullptr};
    CHECK_THROW(ui.parse(4, argv), ServerError);
}

}


SUITE(AuthBaseTests) {

struct AuthFixture {
    AuthFixture() {
        std::ofstream f("test_base.txt");
        f << "user:P@ssW0rd\n";
        f << "admin:12345\n";
    }
    ~AuthFixture() {
        std::remove("test_base.txt");
    }
};

struct EmptyFixture {
    EmptyFixture() {
        std::ofstream f("empty_base.txt");
    }
    ~EmptyFixture() {
        std::remove("empty_base.txt");
    }
};

struct CommentsFixture {
    CommentsFixture() {
        std::ofstream f("comments_base.txt");
        f << "# это комментарий\n";
        f << "\n";
        f << "user:pass1\n";
        f << "   \n";
        f << "admin:pass2\n";
        f << "# ещё комментарий\n";
    }
    ~CommentsFixture() {
        std::remove("comments_base.txt");
    }
};

struct SpecialFixture {
    SpecialFixture() {
        std::ofstream f("special_base.txt");
        f << "user:P@$$W0rd!#%\n";
        f << "admin:admin\n";
    }
    ~SpecialFixture() {
        std::remove("special_base.txt");
    }
};

TEST_FIXTURE(AuthFixture, Test_2_1) {
    AuthBase base;
    base.load("test_base.txt");
    CHECK_EQUAL(2u, base.size());
}

TEST_FIXTURE(EmptyFixture, Test_2_2) {
    AuthBase base;
    base.load("empty_base.txt");
    CHECK_EQUAL(0u, base.size());
}

TEST_FIXTURE(CommentsFixture, Test_2_3) {
    AuthBase base;
    base.load("comments_base.txt");
    CHECK_EQUAL(2u, base.size());
}

TEST_FIXTURE(SpecialFixture, Test_2_4) {
    AuthBase base;
    base.load("special_base.txt");
    std::string pass;
    CHECK(base.getPassword("user", pass));
    CHECK_EQUAL("P@$$W0rd!#%", pass);
}

TEST_FIXTURE(AuthFixture, Test_2_5) {
    AuthBase base;
    base.load("test_base.txt");
    std::string pass;
    CHECK(base.getPassword("user", pass));
    CHECK_EQUAL("P@ssW0rd", pass);
}

TEST_FIXTURE(AuthFixture, Test_2_6) {
    AuthBase base;
    base.load("test_base.txt");
    std::string pass;
    CHECK(!base.getPassword("nobody", pass));
}

}

SUITE(CryptoAuthTests) {

TEST(Test_3_1) {
    CHECK(CryptoAuth::isHexUpper("ABCDEF0123456789"));
    CHECK(CryptoAuth::isHexUpper("4F9C429F5C6884DB"));
    CHECK(CryptoAuth::isHexUpper("0000000000000000"));
    CHECK(CryptoAuth::isHexUpper("FFFFFFFFFFFFFFFF"));
}

TEST(Test_3_2) {
    CHECK(!CryptoAuth::isHexUpper("abcdef"));
    CHECK(!CryptoAuth::isHexUpper("4f9c429f5c6884db"));
    CHECK(!CryptoAuth::isHexUpper("a"));
}

TEST(Test_3_3) {
    CHECK(!CryptoAuth::isHexUpper(""));
}

TEST(Test_3_4) {
    std::string salt = "4F9C429F5C6884DB";
    std::string password = "P@ssW0rd";
    std::string wrongHash = "0000000000000000000000000000000000000000";
    CHECK(!CryptoAuth::verifyHashSha1(wrongHash, salt, password));
}

TEST(Test_3_5) {
    std::string salt = "4F9C429F5C6884DB";
    std::string password = "P@ssW0rd";
    std::string wrongPassword = "wrongpassword";
    std::string data = salt + wrongPassword;
    std::string binaryHash;

    CryptoPP::SHA1 sha1;
    CryptoPP::StringSource(
        data, true,
        new CryptoPP::HashFilter(
            sha1,
            new CryptoPP::StringSink(binaryHash)
        )
    );

    std::string hashHex;
    CryptoPP::StringSource(
        binaryHash, true,
        new CryptoPP::HexEncoder(
            new CryptoPP::StringSink(hashHex)
        )
    );

    CHECK(!CryptoAuth::verifyHashSha1(hashHex, salt, password));
}

}

SUITE(CalculatorTests) {

TEST(Test_4_1) {
    Calculator calc;
    Vector v = {10, 20, 30, 40};
    CHECK_EQUAL(25, calc.average(v));
}

TEST(Test_4_2) {
    Calculator calc;
    Vector v = {-10, -20, -30};
    CHECK_EQUAL(-20, calc.average(v));
}

TEST(Test_4_3) {
    Calculator calc;
    Vector v = {42};
    CHECK_EQUAL(42, calc.average(v));
}

TEST(Test_4_4) {
    Calculator calc;
    Vector v = {2000000000, 2000000000, 2000000000, 2000000000};
    CHECK_EQUAL(2147483647, calc.average(v));
}

TEST(Test_4_5) {
    Calculator calc;
    Vector v = {-2000000000, -2000000000, -2000000000, -2000000000};
    CHECK_EQUAL(-2147483648, calc.average(v));
}

TEST(Test_4_6) {
    Calculator calc;
    Vector v;
    CHECK_THROW(calc.average(v), ServerError);
}

}

int main() {
    return UnitTest::RunAllTests();
}
