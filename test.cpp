#include <UnitTest++/UnitTest++.h>
#include <UnitTest++/TestReporter.h>
#include <cstdio>
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

struct TestInfo {
    int number;
    const char* suite;
    const char* name;
    const char* description;
};

TestInfo testList[] = {
    {1, "CalculatorTests", "NormalAverage", "Среднее арифметическое обычного вектора {10, 20, 30, 40}"},
    {2, "CalculatorTests", "NegativeAverage", "Среднее арифметическое отрицательного вектора"},
    {3, "CalculatorTests", "SingleElement", "Вектор из одного элемента"},
    {4, "CalculatorTests", "OverflowUp", "Переполнение вверх (возврат 2147483647)"},
    {5, "CalculatorTests", "OverflowDown", "Переполнение вниз (возврат -2147483648)"},
    {6, "CalculatorTests", "EmptyVector", "Пустой вектор вызывает исключение"},
    {7, "AuthBaseTests", "LoadAndFind", "Загрузка базы и поиск существующего пользователя"},
    {8, "AuthBaseTests", "NotFound", "Поиск несуществующего пользователя"},
    {9, "AuthBaseTests", "EmptyFile", "Загрузка пустого файла базы"},
    {10, "AuthBaseTests", "FileWithComments", "Файл с комментариями и пустыми строками"},
    {11, "AuthBaseTests", "SpecialCharsInPassword", "Пароль со спецсимволами"},
    {12, "CryptoAuthTests", "HexValid", "Корректная hex-строка заглавными буквами"},
    {13, "CryptoAuthTests", "HexInvalidLower", "Некорректная hex-строка строчными буквами"},
    {14, "CryptoAuthTests", "HexInvalidEmpty", "Пустая строка"},
    {15, "CryptoAuthTests", "VerifyHashWrong", "Неверный хеш отвергается"},
    {16, "CryptoAuthTests", "VerifyHashWrongPassword", "Хеш от другого пароля отвергается"},
    {17, "UserInterfaceTests", "HelpShort", "Флаг -h выводит справку"},
    {18, "UserInterfaceTests", "RunFlag", "Флаг --run запускает сервер"},
    {19, "UserInterfaceTests", "NoRunFlag", "Без --run выводится справка"},
    {20, "UserInterfaceTests", "RunWithPort", "--run с указанием порта"},
    {21, "UserInterfaceTests", "BadPort", "Некорректный порт вызывает исключение"},
    {22, "UserInterfaceTests", "UnknownOption", "Неизвестный параметр вызывает исключение"},
};

int testCount = sizeof(testList) / sizeof(testList[0]);

int findTestNumber(const char* testName) {
    for (int i = 0; i < testCount; i++) {
        if (strcmp(testList[i].name, testName) == 0) {
            return testList[i].number;
        }
    }
    return -1;
}

class NumberedReporter : public UnitTest::TestReporter {
public:
    void ReportTestStart(UnitTest::TestDetails const& details) override {
        int num = findTestNumber(details.testName);
        if (num > 0) {
            printf("Тест #%d: %s\n", num, testList[num-1].description);
        }
    }

    void ReportFailure(UnitTest::TestDetails const& details, char const* failure) override {
        int num = findTestNumber(details.testName);
        printf("\n!!! ОШИБКА В ТЕСТЕ #%d !!!\n", num);
        printf("  %s\n", failure);
        printf("  Файл: %s, строка: %d\n", details.filename, details.lineNumber);
    }

    void ReportTestFinish(UnitTest::TestDetails const& /*details*/, float /*secondsElapsed*/) override {
    }

    void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float /*secondsElapsed*/) override {
        printf("\n========================================\n");
        printf("ИТОГО: %d тестов, %d упало, всего ошибок: %d\n", totalTestCount, failedTestCount, failureCount);
        printf("========================================\n");
    }
};

void printTestInfo() {
    printf("========================================\n");
    printf("СПРАВКА О МОДУЛЬНЫХ ТЕСТАХ\n");
    printf("========================================\n\n");
    printf("Всего тестов: %d\n\n", testCount);

    for (int i = 0; i < testCount; i++) {
        printf("Тест #%d\n", testList[i].number);
        printf("  Набор: %s\n", testList[i].suite);
        printf("  Имя: %s\n", testList[i].name);
        printf("  Описание: %s\n", testList[i].description);
        printf("\n");
    }

    printf("========================================\n");
    printf("ЗАПУСК ТЕСТОВ\n");
    printf("========================================\n\n");
}

SUITE(CalculatorTests) {

TEST(NormalAverage) {
    Calculator calc;
    Vector v = {10, 20, 30, 40};
    CHECK_EQUAL(25, calc.average(v));
}

TEST(NegativeAverage) {
    Calculator calc;
    Vector v = {-10, -20, -30};
    CHECK_EQUAL(-20, calc.average(v));
}

TEST(SingleElement) {
    Calculator calc;
    Vector v = {42};
    CHECK_EQUAL(42, calc.average(v));
}

TEST(OverflowUp) {
    Calculator calc;
    Vector v = {2000000000, 2000000000, 2000000000, 2000000000};
    CHECK_EQUAL(2147483647, calc.average(v));
}

TEST(OverflowDown) {
    Calculator calc;
    Vector v = {-2000000000, -2000000000, -2000000000, -2000000000};
    CHECK_EQUAL(-2147483648, calc.average(v));
}

TEST(EmptyVector) {
    Calculator calc;
    Vector v;
    CHECK_THROW(calc.average(v), ServerError);
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

TEST_FIXTURE(AuthFixture, LoadAndFind) {
    AuthBase base;
    base.load("test_base.txt");
    CHECK_EQUAL(2u, base.size());

    std::string pass;
    CHECK(base.getPassword("user", pass));
    CHECK_EQUAL("P@ssW0rd", pass);

    CHECK(base.getPassword("admin", pass));
    CHECK_EQUAL("12345", pass);
}

TEST_FIXTURE(AuthFixture, NotFound) {
    AuthBase base;
    base.load("test_base.txt");
    std::string pass;
    CHECK(!base.getPassword("nobody", pass));
}

TEST_FIXTURE(EmptyFixture, EmptyFile) {
    AuthBase base;
    base.load("empty_base.txt");
    CHECK_EQUAL(0u, base.size());
}

TEST_FIXTURE(CommentsFixture, FileWithComments) {
    AuthBase base;
    base.load("comments_base.txt");
    CHECK_EQUAL(2u, base.size());

    std::string pass;
    CHECK(base.getPassword("user", pass));
    CHECK_EQUAL("pass1", pass);
}

TEST_FIXTURE(SpecialFixture, SpecialCharsInPassword) {
    AuthBase base;
    base.load("special_base.txt");
    std::string pass;
    CHECK(base.getPassword("user", pass));
    CHECK_EQUAL("P@$$W0rd!#%", pass);
}

}

SUITE(CryptoAuthTests) {

TEST(HexValid) {
    CHECK(CryptoAuth::isHexUpper("ABCDEF0123456789"));
    CHECK(CryptoAuth::isHexUpper("4F9C429F5C6884DB"));
    CHECK(CryptoAuth::isHexUpper("0000000000000000"));
    CHECK(CryptoAuth::isHexUpper("FFFFFFFFFFFFFFFF"));
}

TEST(HexInvalidLower) {
    CHECK(!CryptoAuth::isHexUpper("abcdef"));
    CHECK(!CryptoAuth::isHexUpper("4f9c429f5c6884db"));
    CHECK(!CryptoAuth::isHexUpper("a"));
}

TEST(HexInvalidEmpty) {
    CHECK(!CryptoAuth::isHexUpper(""));
}

TEST(VerifyHashWrong) {
    std::string salt = "4F9C429F5C6884DB";
    std::string password = "P@ssW0rd";
    std::string wrongHash = "0000000000000000000000000000000000000000";
    CHECK(!CryptoAuth::verifyHashSha1(wrongHash, salt, password));
}

TEST(VerifyHashWrongPassword) {
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

SUITE(UserInterfaceTests) {

TEST(HelpShort) {
    UserInterface ui;
    const char* argv[] = {"server", "-h", nullptr};
    CHECK(!ui.parse(2, argv));
}

TEST(RunFlag) {
    UserInterface ui;
    const char* argv[] = {"server", "--run", nullptr};
    CHECK(ui.parse(2, argv));
}

TEST(NoRunFlag) {
    UserInterface ui;
    const char* argv[] = {"server", "-p", "40000", nullptr};
    CHECK(!ui.parse(3, argv));
}

TEST(RunWithPort) {
    UserInterface ui;
    const char* argv[] = {"server", "--run", "-p", "40000", nullptr};
    CHECK(ui.parse(4, argv));
}

TEST(BadPort) {
    UserInterface ui;
    const char* argv[] = {"server", "--run", "-p", "99999", nullptr};
    CHECK_THROW(ui.parse(4, argv), ServerError);
}

TEST(UnknownOption) {
    UserInterface ui;
    const char* argv[] = {"server", "--run", "-x", nullptr};
    CHECK_THROW(ui.parse(3, argv), ServerError);
}

}

int main() {
    printTestInfo();
    NumberedReporter reporter;
    UnitTest::TestRunner runner(reporter);
    return runner.RunTestsIf(UnitTest::Test::GetTestList(), nullptr, UnitTest::True(), 0);
}
