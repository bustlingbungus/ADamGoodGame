#ifndef GAME_MATH_CPP
#define GAME_MATH_CPP

#include "GameMath.hpp"

namespace Math
{
    // initialise the global vectors
    Vector2 Vector2_Zero  (  0.0f,  0.0f );
    Vector2 Vector2_One   (  1.0f,  1.0f );
    Vector2 Vector2_Up    (  0.0f,  1.0f );
    Vector2 Vector2_Down  (  0.0f, -1.0f );
    Vector2 Vector2_Left  ( -1.0f,  0.0f );
    Vector2 Vector2_Right (  1.0f,  0.0f );

    Vector2Int Vector2Int_Zero  (  0,  0 );
    Vector2Int Vector2Int_One   (  1,  1 );
    Vector2Int Vector2Int_Up    (  0,  1 );
    Vector2Int Vector2Int_Down  (  0, -1 );
    Vector2Int Vector2Int_Left  ( -1,  0 );
    Vector2Int Vector2Int_Right (  1,  0 );



    // constructors: assign x and y
    Vector2::Vector2(float X, float Y) : x(X), y(Y) {}
    // make x and y both 0 by default
    Vector2::Vector2() : x(0.0f), y(0.0f) {}


    // magnitude of the vector
    // ||v|| = sqrt(x^2 + y^2)
    float Vector2::length() {
        return sqrtf(x*x + y*y);
    }


    void Vector2::print() {
        std::cout << x <<' '<< y <<'\n';
    }



    // unit vector = vector with magnitude of 1 = v / ||v||
    // just divide x and y by length()
    // when the vector has zero magnitude, x = y = 0, issues arise since both terms are divided by zero
    // when normalising a zero magnitude vector, do nothing

    // makes itself a unit vector
    void Vector2::normalise() {
        float len = length();
        if (len != 0.0f) {
            // divide both terms by the magnitude
            x /= len; 
            y /= len;
        }
    }


    // returns itself as a unit vector
    Vector2 Vector2::normalised() {
        float len = length();
        if (len == 0.0f) return Vector2_Zero; // reutrn a zero magnitude vector
        return *this / len; // return itself divided by its magnitude
    }



    // overload operators for vector arithmetic


    // vector addition/subtraction: a + b = (ax + bx, ay + by)
    Vector2 Vector2::operator+(Vector2 const& other) { return Vector2(x+other.x, y+other.y); }

    Vector2 Vector2::operator-(Vector2 const& other) { return Vector2(x-other.x, y-other.y); }

    // scalar multiplication/division: k * a = (k * ax, k * ay)
    Vector2 Vector2::operator*(float const& k) { return Vector2(x*k, y*k); }

    Vector2 Vector2::operator/(float const& k) { return Vector2(x/k, y/k); }


    // dot product (retuns a scalar): a * b = ||a||*||b||*cos(theta)
    // a * b = (ax * bx) + (ay * by)
    float Vector2::operator*(Vector2 const& other) { return (x*other.x) + (y*other.y); }


    // boolean comparisons

    // vectors a and b are equal if ax = bx AND ay = by
    bool Vector2::operator==(Vector2 const& other) { return x==other.x && y==other.y; }
    // vectors a and b are NOT equal if ax != bx OR ay != by
    bool Vector2::operator!=(Vector2 const& other) { return x!=other.x || y!=other.y; }


    void Vector2::operator+=(Vector2 const& other) { x += other.x; y += other.y; }
    void Vector2::operator-=(Vector2 const& other) { x -= other.x; y -= other.y; }
    void Vector2::operator/=(float const& k) { x /= k; y /= k; }
    void Vector2::operator*=(float const& k) { x *= k; y *= k; }




    








    // constructors: assign x and y
    Vector2Int::Vector2Int(int X, int Y) : x(X), y(Y) {}
    // make x and y both 0 by default
    Vector2Int::Vector2Int() : x(0), y(0) {}


    void Vector2Int::print() {
        std::cout << x <<' '<< y <<'\n';
    }




    // overload operators for vector arithmetic

    // vector addition/subtraction: a + b = (ax + bx, ay + by)
    Vector2Int Vector2Int::operator+(Vector2Int const& other) { return Vector2Int(x+other.x, y+other.y); }

    Vector2Int Vector2Int::operator-(Vector2Int const& other) { return Vector2Int(x-other.x, y-other.y); }

    // scalar multiplication/division: k * a = (k * ax, k * ay)
    Vector2Int Vector2Int::operator*(float const& k) { return Vector2Int(x*k, y*k); }

    Vector2Int Vector2Int::operator/(float const& k) { return Vector2Int(x/k, y/k); }


    // dot product (retuns a scalar): a * b = ||a||*||b||*cos(theta)
    // a * b = (ax * bx) + (ay * by)
    int Vector2Int::operator*(Vector2Int const& other) { return (x*other.x) + (y*other.y); }


    // boolean comparisons

    // vectors a and b are equal if ax = bx AND ay = by
    bool Vector2Int::operator==(Vector2Int const& other) { return x==other.x && y==other.y; }
    // vectors a and b are NOT equal if ax != bx OR ay != by
    bool Vector2Int::operator!=(Vector2Int const& other) { return x!=other.x || y!=other.y; }


    void Vector2Int::operator+=(Vector2Int const& other) { x += other.x; y += other.y; }
    void Vector2Int::operator-=(Vector2Int const& other) { x -= other.x; y -= other.y; }
    void Vector2Int::operator/=(float const& k) { x /= k; y /= k; }
    void Vector2Int::operator*=(float const& k) { x *= k; y *= k; }











    // gets unit vector from p0 to p1
    Vector2 getUnitVector(Vector2 p0, Vector2 p1)
    {
        // displacement from p0 to p1 = p1 - p0
        Vector2 disp = p1 - p0;
        // normalise the displacement vector to get a unit vector
        return disp.normalised();
    }




    // finds the hadamard product of two vectors
    Vector2 hadamard(Vector2 v0, Vector2 v1) {
        return Vector2(v0.x*v1.x, v0.y*v1.y);
    }




    // MATH FUNCTIONS

    float absf( float x ) { return (x<0.0f)? -x : x; }
    double absf( double x ) { return (x<0.0)? -x : x; }
    int Abs( int x ) { return (x<0)? -x : x; }
    
    float ceil( float x ) {
        int n = (int)x;
        return ( x-n != 0.0f )? float(n+1) : (float)n;
    }
    float floor( float x ) {
        int n = (int)x;
        return (float)n;
    }
    
    int ceilToInt( float x ) {
        int n = (int)x;
        return ( x-n != 0.0f )? n+1 : n;
    }
    int floorToInt ( float x ) { return (int)x; }


    int signf( float a ) {
        if (a == 0.0f) return 0;
        return (a < 0.0f)? -1 : 1;
    } 
    int signd(double a) {
        if (a == 0.0) return 0;
        return (a < 0.0)? -1 : 1;
    }
    int Sign( int a ) {
        if (a == 0) return 0;
        return (a < 0)? -1 : 1;
    }    

    int fact( int x ) {
        int res = 1;
        while (x > 0) res *= x--;
        return res;
    }
}

#endif