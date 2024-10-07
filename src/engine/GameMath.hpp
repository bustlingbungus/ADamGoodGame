#ifndef GAME_MATH_HPP
#define GAME_MATH_HPP

#include <cmath>
#include <iostream>

// mathematical constants
#define PI              3.141592654f
#define ROOT2           1.414213562f
#define SIN45           0.7071067812f
#define EULERS_NUMBER   2.7182818285f



namespace Math 
{




    // a two dimensional vector
    struct Vector2 {
        float x;
        float y;


        // constructors
        Vector2( float X, float Y );
        Vector2();



        // used for finding length and getting unit vectors
        float length();
        void normalise(); // makes itself a unit vector
        Vector2 normalised(); // returns itself as a unit vector

        // prints x and y
        void print();



        // overload operators
        Vector2 operator+ ( Vector2 const& other );
        Vector2 operator- ( Vector2 const& other );
        Vector2 operator* ( float const& k );
        Vector2 operator/ ( float const& k );
        float   operator* ( Vector2 const& other );
        bool    operator==( Vector2 const& other );
        bool    operator!=( Vector2 const& other );
        void    operator+=( Vector2 const& other );
        void    operator-=( Vector2 const& other );
        void    operator/=( float const& k );
        void    operator*=( float const& k );


    };

    // global 2D vectors, Vector2_Zero = zero magnitude vector, Vector2_One = (1, 1)
    extern Vector2 Vector2_Zero;
    extern Vector2 Vector2_One;
    extern Vector2 Vector2_Up;
    extern Vector2 Vector2_Down;
    extern Vector2 Vector2_Left;
    extern Vector2 Vector2_Right;










    // a Vector2, but with integer entries
    struct Vector2Int {
        int x;
        int y;


        // contructors
        Vector2Int( int X, int Y );
        Vector2Int();

        // prints x and y
        void print();


        // overload operators
        Vector2Int operator+ ( Vector2Int const& other );
        Vector2Int operator- ( Vector2Int const& other );
        Vector2Int operator* ( float const& k );
        Vector2Int operator/ ( float const& k );
        int        operator* ( Vector2Int const& other );
        bool       operator==( Vector2Int const& other );
        bool       operator!=( Vector2Int const& other );
        void       operator+=( Vector2Int const& other );
        void       operator-=( Vector2Int const& other );
        void       operator/=( float const& k );
        void       operator*=( float const& k );


    };

    // global Vector2Ints
    extern Vector2Int Vector2Int_Zero;
    extern Vector2Int Vector2Int_One;
    extern Vector2Int Vector2Int_Up;
    extern Vector2Int Vector2Int_Down;
    extern Vector2Int Vector2Int_Left;
    extern Vector2Int Vector2Int_Right;





    // finds the unit vector pointing from p0 to p1
    Vector2 getUnitVector( Vector2 p0, Vector2 p1 );

    // finds the hadamard product of two vectors
    Vector2 hadamard( Vector2 v0, Vector2 v1 );



    // math functions

    float absf( float x );
    double absd( double x );
    int Abs( int x );

    // rounds up/down a float to the nearest integer
    float ceil( float x );
    float floor( float x );
    // returns it as an int
    int ceilToInt( float x );
    int floorToInt ( float x );

    int signf( float a );
    int signd( double a );
    int Sign( int a );

    int fact( int x ); // finds x!



    // generic math functions (using template)
    template <typename T> T min(T a, T b) { return (a<b)? a : b; }
    template <typename T> T max(T a, T b) { return (a>b)? a : b; }

    template <typename T> T clamp(T min, T max, T x) {
        if (x < min) return min;
        return (x>max)? max : x;
    }

    template <typename T> void clamp(T min, T max, T *x) {
        if (*x < min) *x = min;
        else if (*x>max) *x = max;
    }

    template <typename T> T pow(T x, int n) 
    {
        T res = 1;
        bool flag = n < 0;
        for (n = Abs(n); n > 0; n--) res *= x;
        return (flag)? 1.0/res : res;
    }   

    // counts the number of active bits in a variable
    template <typename T> int numBits(T var) {
        int count = 0;
        while (var) {
            if (var%2) count ++;
            var >>= 1;
        }
        return count;
    }
}

#endif