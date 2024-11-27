#include "func.h"
#include <sstream>
#include <cmath>


double TFunction::DerivativeValue(double value) const { return deriv(value); }
double TFunction::operator()(double value) const { return culc(value); }


double IdentFunction::culc(double value) const { return value; }
double IdentFunction::deriv(double value) const { return 1.0; }
std::string IdentFunction::ToString() const { return "x"; }


double ConstFunction::culc(double value) const { return value_; }
double ConstFunction::deriv(double value) const { return 0.0; }
std::string ConstFunction::ToString() const { return std::to_string(value_); }


double PowerFunction::culc(double value) const {
    if (exponent_ < 0 && value == 0){
        throw std::invalid_argument("Attempting to divide by zero");
    }else{
        return std::pow(value, exponent_);
    }
}

double PowerFunction::deriv(double value) const {
    if (exponent_ - 1.0 < 0 && value == 0){
        throw std::invalid_argument("Attempting to devide by zero");
    }else{
        return exponent_ * std::pow(value, exponent_ - 1.0);
    }
}

std::string PowerFunction::ToString() const {
    return "x^" + std::to_string(exponent_);
}


double ExpFunction::culc(double value) const { return std::exp(value); }
double ExpFunction::deriv(double value) const { return culc(value); }
std::string ExpFunction::ToString() const { return "e^x"; }


double PolinomialFunction::culc(double value) const {
    double res = 0.0;
    double powered_value = 1.0;

    for (auto coef: coefs_){
        res += coef * powered_value;
        powered_value *= value;
    }

    return res;
}

double PolinomialFunction::deriv(double value) const {
    double res = 0.0;
    double powered_value = 1.0;

    for (int i = 1; i < coefs_.size(); ++i){
        res += coefs_[i] * i * powered_value;
        powered_value *= value;
    }

    return res;
}

std::string PolinomialFunction::ToString() const {
    std::ostringstream res;
    
    for (int i = 0; i < coefs_.size(); ++i){
        if (coefs_[i] != 0){
            if (i > 0){
                if (res.tellp() == std::streampos(0)){
                    if (coefs_[i] < 0){
                        res << "-";
                    }
                }else{
                    if (coefs_[i] < 0){
                        res << " - ";
                    }else{
                        res << " + ";
                    }
                }

                if (coefs_[i] != 1){
                    res << std::abs(coefs_[i]);
                }

                if (i != 1){
                    res << "x^" << std::to_string(i);
                }else{
                    res << "x";
                }
            }else{
                res << coefs_[i];
            }
        }
    }
    return (res.tellp() == std::streampos(0) ? "0" : res.str()); 
}


double TFuncAdd::culc(double value) const {
    return (*lhs_)(value) + (*rhs_)(value);
}

double TFuncAdd::deriv(double value) const {
    return lhs_->DerivativeValue(value) + rhs_->DerivativeValue(value); 
}

std::string TFuncAdd::ToString() const {
    return lhs_->ToString() + " + " + rhs_->ToString();
}


double TFuncSub::culc(double value) const {
    return (*lhs_)(value) - (*rhs_)(value);
}

double TFuncSub::deriv(double value) const {
    return lhs_->DerivativeValue(value) - rhs_->DerivativeValue(value);
}

std::string TFuncSub::ToString() const {
    return lhs_->ToString() + " - (" + rhs_->ToString() + ")";
}


double TFuncMult::culc(double value) const {
    return (*lhs_)(value) * (*rhs_)(value);
}

double TFuncMult::deriv(double value) const {
    return lhs_->DerivativeValue(value) * (*rhs_)(value)
        + (*lhs_)(value) * rhs_->DerivativeValue(value);
}

std::string TFuncMult::ToString() const {
    return "(" + lhs_->ToString() + ") * (" + rhs_->ToString() + ")";
}


double TFuncDiv::culc(double value) const {
    double rhs_value = (*rhs_)(value);

    if (rhs_value != 0){
        return (*lhs_)(value) / rhs_value;
    }else{
        throw std::invalid_argument("Attempting to devide by zero");
    } 
}

double TFuncDiv::deriv(double value) const {
    double rhs_value = (*rhs_)(value);

    if (rhs_value != 0){
        return (lhs_->DerivativeValue(value) * (*rhs_)(value)
            - (*lhs_)(value) * rhs_->DerivativeValue(value)) / (rhs_value * rhs_value);
    }else{
        throw std::invalid_argument("Attempting to devide by zero");
    }
}

std::string TFuncDiv::ToString() const {
    return "(" + lhs_->ToString() + ") / (" + rhs_->ToString() + ")";
}


TFunctionPtr operator+(TFunctionPtr lhs, TFunctionPtr rhs) {
    if (lhs == nullptr || rhs == nullptr){
        throw std::logic_error("Pessed nullptr");
    }

    return std::make_shared<TFuncAdd>(lhs, rhs);
}

TFunctionPtr operator-(TFunctionPtr lhs, TFunctionPtr rhs) {
    if (lhs == nullptr || rhs == nullptr){
        throw std::logic_error("Pessed nullptr");
    }

    return std::make_shared<TFuncSub>(lhs, rhs);
}

TFunctionPtr operator*(TFunctionPtr lhs, TFunctionPtr rhs) {
    if (lhs == nullptr || rhs == nullptr){
        throw std::logic_error("Pessed nullptr");
    }

    return std::make_shared<TFuncMult>(lhs, rhs);
}

TFunctionPtr operator/(TFunctionPtr lhs, TFunctionPtr rhs) {
    if (lhs == nullptr || rhs == nullptr){
        throw std::logic_error("Pessed nullptr");
    }

    return std::make_shared<TFuncDiv>(lhs, rhs);
}

double FuncRoot(
    TFunctionPtr funcPtr,
    double startValue,
    double learningRate,
    double threshold,
    int maxIterations
) {
    double x = startValue;

    for (int i = 0; i < maxIterations; ++i) {
        double fx = (*funcPtr)(x);
        double dfx = funcPtr->DerivativeValue(x);

        if (std::abs(fx) < threshold) {
            return x;
        }

        if (std::abs(dfx) < 1e-10) {
            break;
        }

        x -= learningRate * fx / dfx; 
    }

    return x;
}
