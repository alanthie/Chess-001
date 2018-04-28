//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// 
//
#define CATCH_CONFIG_RUNNER
#include "ExternLib/Catch/single_include/catch.hpp"
#include "core/chess.hpp"

using namespace chess;

unsigned int Factorial(unsigned int number) {
    return number > 1 ? Factorial(number - 1)*number : 1;
}
TEST_CASE("Factorials are computed", "[factorial]") {
    REQUIRE(Factorial(0) == 2); // will fail
    REQUIRE(Factorial(1) == 1);
    REQUIRE(Factorial(2) == 2);
    REQUIRE(Factorial(3) == 6);
    REQUIRE(Factorial(10) == 3628800);
}

template <typename PieceID, typename uint8_t _BoardSize>
class TestClass
{
    using _Piece = Piece<PieceID, _BoardSize>;
    using _Move = Move<PieceID>;
    using _Board = Board<PieceID, _BoardSize>;

public:
    bool func() 
    {
        _Board::reset_to_default_option();
        _Board board(true);

        std::vector<_Move> m = board.generate_moves();

        _Piece::clear(); // test static leak

        if (_BoardSize == 8) return (m.size() == 20);
        return false;
    }
};
TEST_CASE("TestClass", "[func]") {

    TestClass<uint8_t, 8> a;
    REQUIRE(a.func() == true);
}

int main(int argc, char* argv[])
{
    {
        Catch::Session session;
        int returnCode = session.applyCommandLine(argc, argv);
        if (returnCode != 0)
            return returnCode;
        int numFailed = session.run();
        std::cout << "numFailed:" << (numFailed < 0xff ? numFailed : 0xff) << std::endl;
    }
    return 0;
}
