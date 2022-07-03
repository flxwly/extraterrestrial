#ifndef PVECTOR_HPP
#define PVECTOR_HPP

#include <cmath>
#include <string>


/** a representation of both a point and a vector in 2D space
 * that includes some methods to manipulate and compare it.
 *
*/
class PVector {
public:

    /// creates a 2D Point-Vector at (NAN | NAN)
    PVector() : x(NAN), y(NAN) {};


    /** creates a 2D Point-Vector at the given coordinates
     *
     * @tparam _x x-Position of the PVector.
     * @tparam _y y-Position of the PVector.
     *
    */
    PVector(double _x, double _y) : x(_x), y(_y) {};

    /// the x-Position of the PVector
    double x;
    /// the x-Position of the PVector
    double y;

    /** sets the position to the given coordinates
     *
     * @tparam _x new x-Position of the PVector.
     * @tparam _y new y-Position of the PVector.
     *
     * @returns a copy of itself after executing the method
     *
    */
    PVector set(double _x, double _y);

    /** normalizes the PVector's magnitude
     *
     * @returns a copy of itself after executing the method
     *
    */
    PVector normalize();

    /** calculates the magnitude of the PVector
     *
     * @returns magnitude of the PVector
     *
    */
    [[nodiscard]] double getMag() const;

    /** sets the magnitude of the PVector
     *
     * @param mag the new magnitude the PVector should have
     * @returns a copy of itself after executing the method
     *
    */
    PVector setMag(double mag);

    /** sets the magnitude of a given PVector
     *
     * @param pVector the PVector the magnitude should be changed of
     * @param mag the new magnitude pVector should have
     *
     * @note this is the static version of PVector::setMag();
     *
     * @returns a copy of pVector after executing the method
     *
    */
    static PVector setMag(PVector &pVector, double mag);

    /** rotates the PVector counter-clockwise by a given angle
     *
     * @param angle the angle the PVector should be rotated by
     *
     * @note angle must be in radians
     *
     * @returns a copy of pVector after executing the method
     *
    */
    PVector rotate(double angle);

    /** rounds the coordinates of the PVector
     *
     * @returns a copy this PVector after executing the method
     *
    */

    static PVector rotate(PVector &pVector, double angle);

    /** rotates a PVector by an angle (in radians)
     *
     * @returns the rotated PVector
     *
     * @note this is the static version of PVector::rotate();
     *
    */
    PVector round();

    static double dot(PVector v1, PVector v2);

    /** rounds the coordinates of a given pVector
     *
     * @param pVector the PVector the coordinates should be rounded of
     *
     * @note this is the static version of PVector::round();
     *
     * @returns a copy of pVector after executing the method
     *
    */
    static PVector round(const PVector &pVector);

    /** binary operator to compare two pVectors with each other for equal coordinates
     *
     * @param lhs the other PVector that should be compared to this
     *
     * @returns true if both the x and the y coordinate of each PVector match
     *
    */
    bool operator==(const PVector &lhs) const;

    /** binary operator to compare this PVector with a double for unequal coordinates
     *
     * @param lhs the other PVector that should be compared to this
     *
     * @returns true if either the x or the y coordinate doesn't match
     *
    */
    bool operator==(const double &d) const;

    /** binary operator to compare a pVectors with a double for unequal coordinates
     *
     * @param d the double that should be compared to this
     *
     * @returns true if either the x or the y coordinate of this PVector doesn't match with d
     *
    */
    bool operator!=(const PVector &lhs) const;

    /** binary operator to compare this PVector with a double for equal coordinates
     *
     * @param d the double that should be compared to this
     *
     * @returns true if both the x and the y coordinate of this PVector match with lhs
     *
    */
    bool operator!=(const double &lhs) const;

    PVector &operator+=(const PVector &rhs);

    PVector operator+(const PVector &rhs);

    PVector &operator-=(const PVector &rhs) ;

    PVector operator-(const PVector &rhs);

    PVector &operator*=(const double &m);

    PVector operator*(const double &m);

    PVector &operator/=(const double &m);

    PVector operator /(const double &m);

    /** binary operator to check whether a PVectors position is defined or not
     *
     * @returns true if both the x and the y coordinate are not NAN
     *
    */
    explicit operator bool() const;

	friend std::ostream& operator<<(std::ostream& os, const PVector& vec);

    [[nodiscard]] std::string str() const;

    static std::string str(PVector pVector);
};


#endif //PVECTOR_HPP
