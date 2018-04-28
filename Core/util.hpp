#pragma once
//=================================================================================================
//                  Copyright (C) 2017 Alain Lanthier - All Rights Reserved  
//                  License: MIT License    See LICENSE.md for the full license.
//=================================================================================================
//
//
#ifndef _AL_CHESS_CORE_UTIL_HPP
#define _AL_CHESS_CORE_UTIL_HPP

namespace chess
{
    enum class PieceName        { none, R, N, B, Q, K, P };
    enum class PieceColor       { none, W, B };
    enum class PieceMoveStyle   { none, Sliding, Jumping, SlidingDiagonalCapturePromo };
    enum class ExactScore       { LOSS, DRAW, WIN, UNKNOWN }; // white win is WIN, black win is LOSS
    enum class FeatureType      { condition = 0, valuation = 1 };

    std::string  toNULLSTR(const std::string& s) { return (s.size() == 0) ? "NullSTR" : s; }
    void         fromNULLSTR(std::string& s)     { if (s == "NullSTR") s = ""; }

    std::random_device  rnd_dev;                    // uniformly - distributed integer random number generator
    std::mt19937_64     rnd_generator(rnd_dev());   // Mersenne Twister 19937 pseudo-random number generator [0, pow(2,64))
    std::uniform_real_distribution<double> uniform_double(0, 1);    // uniform random probability in range [0,1)     

    void reverse_sq(uint16_t& sq, uint8_t _BoardSize)
    {
        uint8_t x = sq%_BoardSize;
        uint8_t y = (uint8_t) (sq / _BoardSize);
        sq = (_BoardSize - (y+1))*_BoardSize + (_BoardSize - (x+1));
    }
    void reverse_sq_v(std::vector<uint16_t>& sq, uint8_t _BoardSize)
    {
        for (size_t i = 0; i < sq.size(); i++) 
            reverse_sq(sq[i], _BoardSize);
    }

    std::vector<uint16_t> reverse_order_sq(const std::vector<uint16_t>& sq, bool is_ref_to_sym, uint16_t nw, uint16_t nb, uint8_t _BoardSize)
    {
        std::vector<uint16_t> sq_copy = sq;
        reverse_sq_v(sq_copy, _BoardSize);
        //Re-order piece square
        std::vector<uint16_t> sq_reverse;
        if (!is_ref_to_sym)
        {
            for (size_t i = 0; i < nw; i++) sq_reverse.push_back(sq_copy[nb + i]);
            for (size_t i = 0; i < nb; i++) sq_reverse.push_back(sq_copy[0 + i]);
        }
        else
        {
            for (size_t i = 0; i < nb; i++) sq_reverse.push_back(sq_copy[nw + i]);
            for (size_t i = 0; i < nw; i++) sq_reverse.push_back(sq_copy[0 + i]);
        }
        return sq_reverse;
    }
    
    ExactScore reverse_score(ExactScore sc)
    {
        if (sc == ExactScore::WIN) return ExactScore::LOSS;
        else if (sc == ExactScore::LOSS) return ExactScore::WIN;
        else if (sc == ExactScore::DRAW) return ExactScore::DRAW;
        return sc; // UNKNOWN
    }

    int ExactScore_to_int(ExactScore c)
    {
        if (c == ExactScore::LOSS) return 1;
        else if (c == ExactScore::DRAW) return 2;
        else if (c == ExactScore::WIN)  return 3;
        else return 0;
    }

    ExactScore int_to_ExactScore(int t)
    {
        if (t == 1) return ExactScore::LOSS;
        else if (t == 2) return ExactScore::DRAW;
        else if (t == 3) return ExactScore::WIN;
        return ExactScore::UNKNOWN;
    }

    std::string ExactScore_to_string(ExactScore c)
    {
        if (c == ExactScore::LOSS) return "LOSS";
        else if (c == ExactScore::DRAW) return "DRAW";
        else if (c == ExactScore::WIN)  return "WIN";
        else return "UNKNOWN";
    }

    int FeatureType_to_int(FeatureType c)
    {
        if (c == FeatureType::valuation) return 1;
        else return 0;
    }
    FeatureType int_to_FeatureType(int t)
    {
        if (t == 1) return FeatureType::valuation;
        return FeatureType::condition;
    }

    int PieceColor_to_int(PieceColor c)
    {
        if (c == PieceColor::W) return 1;
        else if (c == PieceColor::B) return 2;
        return 0;
    }
    PieceColor int_to_PieceColor(int t)
    {
        if (t == 1) return PieceColor::W;
        else if (t == 2) return PieceColor::B;
        return PieceColor::none;
    }

    int bool_to_int(bool c)
    {
        if (c) return 1;
        return 0;
    }
    bool int_to_bool(int t)
    {
        if (t == 1) return true;
        return false;
    }
    std::string bool_to_string(bool c)
    {
        if (c) return "1";
        return "0";
    }
    bool string_to_bool(std::string t)
    {
        if (t == "1") return true;
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    struct sorter_less_pieces
    {
        using _Piece = Piece<PieceID, _BoardSize>;

        bool operator() (const PieceID& lhs, const PieceID& rhs)
        {
            const _Piece* a = _Piece::get(lhs);
            const _Piece* b = _Piece::get(rhs);
            if ((a->get_color() == PieceColor::W) && (b->get_color() == PieceColor::B)) return true;
            else if ((a->get_color() == PieceColor::B) && (b->get_color() == PieceColor::W)) return false;
            else
            {
                // KQRBNP 
                if ((a->get_name() == PieceName::K) && (b->get_name() != PieceName::K)) return true;
                if ((a->get_name() == PieceName::Q) && (b->get_name() == PieceName::R)) return true;
                if ((a->get_name() == PieceName::Q) && (b->get_name() == PieceName::B)) return true;
                if ((a->get_name() == PieceName::Q) && (b->get_name() == PieceName::N)) return true;
                if ((a->get_name() == PieceName::Q) && (b->get_name() == PieceName::P)) return true;
                if ((a->get_name() == PieceName::R) && (b->get_name() == PieceName::B)) return true;
                if ((a->get_name() == PieceName::R) && (b->get_name() == PieceName::N)) return true;
                if ((a->get_name() == PieceName::R) && (b->get_name() == PieceName::P)) return true;
                if ((a->get_name() == PieceName::B) && (b->get_name() == PieceName::N)) return true;
                if ((a->get_name() == PieceName::B) && (b->get_name() == PieceName::P)) return true;
                return false;
            }
        }
    };

}
#endif
