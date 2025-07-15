/* Copyright 2025 Karlsruhe Institute of Technology
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include <vector>
#include <iostream>
#include <cstdint>

template<typename T>
class ParamPack {
protected:
    std::vector<T> values_;
    size_t size_;
public:
    template<typename... U>
    ParamPack( U&&... u ) :
      // values_{ static_cast<T>(u), ... },
      size_( sizeof...(U) ) {
        for (auto i : std::initializer_list<T>{u... })
          values_.push_back(static_cast<T>(i));

      }

    template<typename ... U>
    ParamPack( std::initializer_list<std::is_same<T, U...>( U...)> il ) :
      values_( il ), size_( il.size() ) {}

    std::vector<T>& operator()() { return values_; }

    size_t size() const { return size_; }
};

template<typename T>
class Array : public ParamPack<T> {
private:
    T* items_;
    std::string test;
public:
    template<typename... U>
    Array( U... u ) : ParamPack<T>::ParamPack( u... ) {
        items_ = this->values_.data();
        test = false;
    }
    template<typename... U>
    Array( std::string t, U... u ) : ParamPack<T>::ParamPack( u... ) {
        items_ = this->values_.data();
        test = t;
        std::cout << "std::string " << test << std::endl;
    }
    template<typename... U>
    Array( std::string t, std::initializer_list<U...> il ) : ParamPack<T>::ParamPack( il ) {
        items_ = this->values_.data();
        test = t;
        std::cout << "std::string " << test << std::endl;
    }

    // template<typename... U>
    // Array( std::initializer_list<U...> il ) : ParamPack<T>::ParamPack( il ) {
    //     items_ = this->values_.data();
    // }

    T& operator[]( size_t idx ) {
        return items_[idx];
    }

    T operator[]( size_t idx ) const {
        return items_[idx];
    }

    T* data() const { return items_; }
};

int main() {
    try {
        // Parameter Pack Examples:
        // Variadic Constructor { ... }
        std::cout << "ParamPack<T> Examples:\n";
        std::cout << "Using ParamPack<T>'s Variadic Constructor\n";
        ParamPack<int> pp1( 1, 2, 3, 4  );
        std::cout << "Size: " << pp1.size() << " | Elements: ";
        for ( auto& v : pp1() ) {
            std::cout << v << " ";
        }
        std::cout << '\n';

        std::cout << "Using ParamPack<T>'s Variadic Constructor with an Initializer List\n";
        ParamPack<int> pp2( { 5, 6, 7, 8 } );
        std::cout << "Size: " << pp2.size() << " | Elements: ";
        for ( auto& v : pp2() ) {
            std::cout << v << " ";
        }
        std::cout << '\n';

        std::cout << "Using ParamPack<T>'s initializer_list constructor\n";
        std::initializer_list<int> il{ 9,10,11,12 };
        ParamPack<int> pp3( il );
        std::cout << "Size: " << pp3.size() << " | Elements: ";
        for ( auto& v : pp3() ) {
            std::cout << v << " ";
        }
        std::cout << "\n\n";

        // Array Examples:
        std::cout << "Array<T> Examples:\n";
        std::cout << "Using Array<T>'s initializer_list Constructor\n";
        Array<int> arr( il );
        for ( size_t i = 0; i < arr.size(); i++ ) {
            std::cout << arr[i] << " ";
        }
        std::cout << "\n";

        // Using Variadic Constructor
        std::cout << "Using Array<T>'s Variadic Constructor\n";
        Array<int> testA( 9, 8, 7, 6 );
        for ( size_t i = 0; i < testA.size(); i++ ) {
            std::cout << testA[i] << " ";
        }
        std::cout << '\n';

        Array<std::string> testB( "Hello", " World" );
        for ( size_t i = 0; i < testB.size(); i++ ) {
            std::cout << testB[i] << " ";
        }
        std::cout << "\n\n";

        Array<std::string> testB2(  std::string(" "), { " ", "Hello", " World" } );
        for ( size_t i = 0; i < testB2.size(); i++ ) {
            std::cout << testB2[i] << " ";
        }
        std::cout << "\n\n";

        // Using Constructor w/ Initializer List
        std::cout << "Using Array<T>'s Variadic Constructor with Initializer List\n";
        Array<int> testC( { 105, 210, 420 } );
        for ( size_t i = 0; i < testC.size(); i++ ) {
            std::cout << testC[i] << " ";
        }
        std::cout << "\n\n";

        // Using Initializer List with =
        std::cout << "Using Array<T>'s Initializer List with =\n";
        Array<int> a = { 1, 2, 3, 4 };
        for ( size_t i = 0; i < a.size(); i++ ) {
            std::cout << a[i] << " ";
        }
        std::cout << '\n';

        Array<char> b = { 'a', 'b', 'c', 'd' };
        for ( size_t i = 0; i < b.size(); i++ ) {
            std::cout << b[i] << " ";
        }
        std::cout << '\n';

        Array<double> c = { 1.2, 3.4, 4.5, 6.7 };
        for ( size_t i = 0; i < c.size(); i++ ) {
            std::cout << c[i] << " ";
        }
        std::cout << "\n\n";

        // Using Initializer List directly
        std::cout << "Using Array<T>'s Initalizer List directly\n";
        Array<uint32_t> a1{ 3, 6, 9, 12 };
        for ( size_t i = 0; i < a1.size(); i++ ) {
            std::cout << a1[i] << " ";
        }
        std::cout << "\n\n";

        // Using user defined data type
        struct Point {
            int x_, y_;
            Point( int x, int y ) : x_( x ), y_( y ) {}
        };
        Point p1( 1, 2 ), p2( 3, 4 ), p3( 5, 6 );

        // Variadic Constructor
        std::cout << "Using Array<T>'s Variadic Consturctor with user data type\n";
        Array<Point> d1( p1, p2, p3 );
        for ( size_t i = 0; i < d1.size(); i++ ) {
            std::cout << "(" << d1[i].x_ << "," << d1[i].y_ << ") ";
        }
        std::cout << '\n';

        // Initializer List Construtor (reversed order)
        std::cout << "Using Array<T>'s Initializer List Constructor with user data type\n";
        Array<Point> d2( { p3, p2, p1 } );
        for ( size_t i = 0; i < d2.size(); i++ ) {
            std::cout << "(" << d2[i].x_ << "," << d2[i].y_ << ") ";
        }
        std::cout << '\n';

        // Initializer List Version = {...} p2 first
        std::cout << "Using Array<T>'s  = Initializer List with user data type\n";
        Array<Point> d3 = { p2, p1, p3 };
        for ( size_t i = 0; i < d3.size(); i++ ) {
            std::cout << "(" << d3[i].x_ << "," << d3[i].y_ << ") ";
        }
        std::cout << '\n';

        // Initializer List Directly p2 first p1 & p3 swapped
        std::cout << "Using Array<T>'s Initializer List directly with user data type\n";
        Array<Point> d4{ p2, p3, p1 };
        for ( size_t i = 0; i < d4.size(); i++ ) {
            std::cout << "(" << d4[i].x_ << "," << d4[i].y_ << ") ";
        }
        std::cout << '\n';

        std::initializer_list<Point> ilPoints{ p1, p2, p3 };
        std::cout << "Using Array<T>'s initializer_list Constructor with user data type\n";
        Array<Point> d5( ilPoints );
        for ( size_t i = 0; i < d5.size(); i++ ) {
            std::cout << "(" << d5[i].x_ << "," << d5[i].y_ << ") ";
        }
        std::cout << "\n\n";

        // Need a local copy of the vector instead?
        std::cout << "Using Array<T>'s base class's operator()() to retrieve vector\n";
        std::vector<Point> points = d4(); // using operator()()
        for ( auto& p : points ) {
            std::cout << "(" << p.x_ << "," << p.y_ << ") ";
        }
        std::cout << '\n';

        // Need a local copy of the pointer instead?
        std::cout << "Using Array<T>'s data() to get the contents of its internal pointer\n";
        Point* pPoint = nullptr;
        pPoint = d4.data();
        for ( size_t i = 0; i < d4.size(); i++ ) {
            std::cout << "(" << pPoint[i].x_ << "," << pPoint[i].y_ << ") ";
        }
        std::cout << '\n';

    } catch( const std::runtime_error& e ) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
