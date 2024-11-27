#include <gtest/gtest.h>
#include "../includes/includes.h"


using namespace testing;


TFactory factory = TFactory();
class BasicFunctionStr: public Test{};


TEST_F(BasicFunctionStr, TestPolynomialStr) {
    TFunctionPtr f1 = factory.CreateObject("polynomial", {-1});
    TFunctionPtr f2 = factory.CreateObject("polynomial", {0, 0, 0, 0, 0, 10.0});
    TFunctionPtr f3 = factory.CreateObject("polynomial", {1, 0, 3, 4});
    TFunctionPtr f4 = factory.CreateObject("polynomial", {-1, 2, -3, 4});
    TFunctionPtr f5 = factory.CreateObject("polynomial", {1, 0, 3, -4});
    TFunctionPtr f6 = factory.CreateObject("polynomial", {0, 1, 3, 1});

    EXPECT_EQ(f1->ToString(), "-1");
    EXPECT_EQ(f2->ToString(), "10x^5");
    EXPECT_EQ(f3->ToString(), "1 + 3x^2 + 4x^3");
    EXPECT_EQ(f4->ToString(), "-1 + 2x - 3x^2 + 4x^3");
    EXPECT_EQ(f5->ToString(), "1 + 3x^2 - 4x^3");
    EXPECT_EQ(f6->ToString(), "x + 3x^2 + x^3");
}

TEST_F(BasicFunctionStr, TestOtherStr) {
    TFunctionPtr f1 = factory.CreateObject("ident");
    TFunctionPtr f2 = factory.CreateObject("const", 10);
    TFunctionPtr f3 = factory.CreateObject("power", 2);
    TFunctionPtr f4 = factory.CreateObject("exp");

    EXPECT_EQ(f1->ToString(), "x");
    EXPECT_EQ(f2->ToString(), "10.000000");
    EXPECT_EQ(f3->ToString(), "x^2.000000");
    EXPECT_EQ(f4->ToString(), "e^x");
}


class BasicFunctionality: public Test{};


TEST_F(BasicFunctionality, Identity) {
    auto ident = factory.CreateObject("ident");
    EXPECT_DOUBLE_EQ((*ident)(256), 256);
    EXPECT_DOUBLE_EQ(ident->DerivativeValue(2), 1);
}

TEST_F(BasicFunctionality, Constant) {
    auto ident = factory.CreateObject("const", 10);
    EXPECT_DOUBLE_EQ((*ident)(2), 10);
    EXPECT_DOUBLE_EQ(ident->DerivativeValue(2), 0);
}

TEST_F(BasicFunctionality, Power) {
    auto ident = factory.CreateObject("power", 2);
    EXPECT_DOUBLE_EQ((*ident)(2), 4);
    EXPECT_DOUBLE_EQ(ident->DerivativeValue(3), 6);
}

TEST_F(BasicFunctionality, Exp) {
    auto ident = factory.CreateObject("exp");
    EXPECT_DOUBLE_EQ((*ident)(2), 7.3890560989306504);
    EXPECT_DOUBLE_EQ(ident->DerivativeValue(2), 7.3890560989306504);
}

TEST_F(BasicFunctionality, Polynomial) {
    auto ident = factory.CreateObject("polynomial", {1, 2, 3});
    EXPECT_DOUBLE_EQ((*ident)(2), 17);
    EXPECT_DOUBLE_EQ(ident->DerivativeValue(2), 14);
}


class BasicOps: public Test{};


TEST_F(BasicOps, Add) {
    auto f1 = factory.CreateObject("polynomial", {15, 6, 0, 1});
    auto f2 = factory.CreateObject("power", 0.5);

    auto s = f1 + f2;

    EXPECT_EQ(s->ToString(), "15 + 6x + x^3 + x^0.500000");
    EXPECT_DOUBLE_EQ((*s)(10), 1078.16227766016837);
}

TEST_F(BasicOps, Sub) {
    auto f1 = factory.CreateObject("const", 11);
    auto f2 = factory.CreateObject("exp");

    auto s = f1 - f2;

    EXPECT_EQ(s->ToString(), "11.000000 - (e^x)");
    EXPECT_DOUBLE_EQ((*s)(8.5), -4903.7688402991344);
}

TEST_F(BasicOps, Mult) {
    auto f1 = factory.CreateObject("ident");
    auto f2 = factory.CreateObject("polynomial", {10,15,63,45});

    auto s = f1 * f2;

    EXPECT_EQ(s->ToString(), "(x) * (10 + 15x + 63x^2 + 45x^3)");
    EXPECT_DOUBLE_EQ((*s)(6), 72528);
}

TEST_F(BasicOps, Divis) {
    auto f1 = factory.CreateObject("const", 46);
    auto f2 = factory.CreateObject("exp");

    auto s = f1 / f2;

    EXPECT_EQ(s->ToString(), "(46.000000) / (e^x)");
    EXPECT_DOUBLE_EQ((*s)(4.6), 0.4623844442531449);
}

TEST(Deriv_and_Eval, ComplexFunc) {
    auto f1 = factory.CreateObject("polynomial", {-1, 4, 0, 0.9});
    auto f2 = factory.CreateObject("exp");
    auto f3 = factory.CreateObject("power", 2);

    auto c_sum = f1 + f2 + f3;
    auto c_mult = f1 * f2 * f3;
    auto mixed = f1 - f2 / f3;

    EXPECT_DOUBLE_EQ(c_sum->DerivativeValue(3), 54.385536923187672);
    EXPECT_DOUBLE_EQ(c_mult->DerivativeValue(3), 15751.078055163769);
    EXPECT_DOUBLE_EQ(mixed->DerivativeValue(3), 27.556091225067124);

    EXPECT_DOUBLE_EQ((*c_sum)(3), 64.385536923187658);
    EXPECT_DOUBLE_EQ((*c_mult)(3), 6381.1750804967214);
    EXPECT_DOUBLE_EQ((*mixed)(3), 33.068273675201368);
}

//Exceptions

TEST(Exceptions, WrongType) {
    auto f1 = factory.CreateObject("aaa");
    auto f2 = factory.CreateObject("exp");

    EXPECT_THROW(f1 + f2, std::logic_error);
    EXPECT_THROW(f1 - f2, std::logic_error);
    EXPECT_THROW(f1 * f2, std::logic_error);
    EXPECT_THROW(f1 / f2, std::logic_error);
}

TEST(Exceptions, ZeroDivision) {
    auto f1 = factory.CreateObject("exp");
    auto f2 = factory.CreateObject("const", 0);

    EXPECT_THROW((*(f1 / f2))(1), std::invalid_argument);

    auto f3 = factory.CreateObject("power", -1);

    EXPECT_THROW((*f3)(0), std::invalid_argument);
}

// Newton method

TEST(NewtonMethods, BasicNewtonMethod) {
    auto f1 = factory.CreateObject("polynomial", {3, 0, 1, -2.3});

    EXPECT_NEAR(FuncRoot(f1), 1.258425044, 1e-6);
}

TEST(NewtonMethods, ComplexNewtonMethod) {
    auto f1 = factory.CreateObject("const", 2);
    auto f2 = factory.CreateObject("exp");

    EXPECT_NEAR(FuncRoot(f1 - f2), 0.69314766963599039, 1e-6);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

