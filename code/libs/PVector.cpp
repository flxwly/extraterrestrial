#include "PVector.hpp"

PVector PVector::set(double _x, double _y) {
    x = _x, y = _y;
    return *this;
}

PVector PVector::normalize() {
    double mag = getMag();
    return (mag == 0) ? *this : set(x / mag, y / mag);
}

double PVector::getMag() const {
    return sqrt(x * x + y * y);
}

PVector PVector::setMag(double mag) {
    normalize();
    return set(x * mag, y * mag);
}

PVector PVector::setMag(PVector &pVector, double mag) {
    pVector.normalize();
    return {pVector.x * mag, pVector.y * mag};
}

/// counter-clockwise rotation
PVector PVector::rotate(double angle) {
    return set(x * cos(angle) - y * sin(angle), x * sin(angle) + y * cos(angle));
}

PVector PVector::round() {
    return set(std::round(x), std::round(y));
}

PVector PVector::round(const PVector &pVector) {
    return {std::round(pVector.x), std::round(pVector.y)};
}

bool PVector::operator==(const PVector &lhs) const {
    return x == lhs.x && y == lhs.y;
}

bool PVector::operator==(const double &d) const {
    return x == d && y == d;
}

bool PVector::operator!=(const PVector &lhs) const {
    return x != lhs.x || y != lhs.y;
}

bool PVector::operator!=(const double &lhs) const {
    return x != lhs || y != lhs;
}

PVector &PVector::operator+=(const PVector &rhs) {
    x += rhs.x, y += rhs.y;
    return *this;
}

PVector PVector::operator+(const PVector &rhs) {
    return PVector(*this) += rhs;
}

PVector &PVector::operator-=(const PVector &rhs) {
    x -= rhs.x, y -= rhs.y;
    return *this;
}

PVector PVector::operator-(const PVector &rhs) {
    return PVector(*this) -= rhs;
}

PVector &PVector::operator*=(const double &m) {
    x *= m, y *= m;
    return *this;
}

PVector PVector::operator*(const double &m) {
    return PVector(*this) *= m;
}

PVector &PVector::operator/=(const double &m) {
    if (m != 0) x /= m, y /= m;
    return *this;
}

PVector PVector::operator/(const double &m) {
    return PVector(*this) /= m;
}

PVector::operator bool() const {
    return !_isnan(x) && !_isnan(y);
}

std::string PVector::str() const {
    return std::to_string(x) + " | " + std::to_string(y);
}

std::string PVector::str(PVector pVector) {
    return std::to_string(pVector.x) + " | " + std::to_string(pVector.y);
}

std::ostream &operator<<(std::ostream &os, const PVector &vec) {
    os << std::to_string(vec.x) + " | " + std::to_string(vec.y);
    return os;
}

double PVector::dot(PVector v1, PVector v2) {
    return v1.x * v2.x + v1.y * v2.y;
}
