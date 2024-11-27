#ifndef FUNC_HEADER
#define FUNC_HEADER

#include <memory>
#include <string>
#include <vector>
#include <initializer_list>


class TOptions{
    double value_;
    std::vector<double> coefs_;

public:
    TOptions() {}
    TOptions(double value) : value_(value) {} 
    TOptions(const std::vector<double>& coefs) : coefs_(coefs) {}
    TOptions(std::initializer_list<double> coefs) : coefs_(coefs) {}

    double GetValue() const { return value_; }
    std::vector<double> GetCoefs() const { return coefs_; }

    operator double() const { return value_; }
    operator std::vector<double>() const { return coefs_; }
};


class TFunction;
using TFunctionPtr = std::shared_ptr<TFunction>;


class TFunction{
    virtual double culc(double value) const = 0;
    virtual double deriv(double value) const = 0;

public:
    virtual std::string ToString() const = 0;
    
    double DerivativeValue(double value) const;
    double operator()(double value) const; 

    virtual ~TFunction() = default;
};


class IdentFunction: public TFunction {
    double culc(double value) const override;
    double deriv(double value) const override; 

public:
    explicit IdentFunction(const TOptions opts) {}

    std::string ToString() const override;
};


class ConstFunction: public TFunction {
    double value_;

    double culc(double value = 0.0) const override;
    double deriv(double value = 0.0) const override; 

public: 
    explicit ConstFunction(double value): value_(value) {}

    std::string ToString() const override;
};


class PowerFunction: public TFunction {
    double exponent_;

    double culc(double value) const override;
    double deriv(double value) const override;

public:
    explicit PowerFunction(double exponent): exponent_(exponent) {}

    std::string ToString() const override; 
};


class ExpFunction: public TFunction {
    double culc(double value) const override; 
    double deriv(double value) const override; 

public:
    explicit ExpFunction(const TOptions opts) {}

    std::string ToString() const override;
};


class PolinomialFunction: public TFunction {
    std::vector<double> coefs_;

    double culc(double value) const override; 
    double deriv(double value) const override;

public:
    explicit PolinomialFunction(const std::vector<double>& coefs): coefs_(coefs) {}

    std::string ToString() const override; 
};


class TFuncAdd: public TFunction {
    TFunctionPtr lhs_, rhs_;

    double culc(double value) const override;
    double deriv(double value) const override;

public:
    explicit TFuncAdd(TFunctionPtr lhs, TFunctionPtr rhs): lhs_(lhs), rhs_(rhs) {} 

    std::string ToString() const override;
};


class TFuncSub: public TFunction {
    TFunctionPtr lhs_, rhs_;

    double culc(double value) const override;
    double deriv(double value) const override;

public:
    explicit TFuncSub(TFunctionPtr lhs, TFunctionPtr rhs): lhs_(lhs), rhs_(rhs) {} 

    std::string ToString() const override;
};


class TFuncMult: public TFunction {
    TFunctionPtr lhs_, rhs_;

    double culc(double value) const override;
    double deriv(double value) const override;

public:
    explicit TFuncMult(TFunctionPtr lhs, TFunctionPtr rhs): lhs_(lhs), rhs_(rhs) {} 

    std::string ToString() const override;
};


class TFuncDiv: public TFunction {
    TFunctionPtr lhs_, rhs_;

    double culc(double value) const override;
    double deriv(double value) const override;

public:
    explicit TFuncDiv(TFunctionPtr lhs, TFunctionPtr rhs): lhs_(lhs), rhs_(rhs) {} 

    std::string ToString() const override;
};


TFunctionPtr operator+(TFunctionPtr lhs, TFunctionPtr rhs);
TFunctionPtr operator-(TFunctionPtr lhs, TFunctionPtr rhs);
TFunctionPtr operator*(TFunctionPtr lhs, TFunctionPtr rhs);
TFunctionPtr operator/(TFunctionPtr lhs, TFunctionPtr rhs);

double FuncRoot(
    TFunctionPtr funcPtr,
    double startValue = 10.0,
    double learningRate = 0.1,
    double threshold = 1e-6,
    int maxIterations = 1000);

#endif

