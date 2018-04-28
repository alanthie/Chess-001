#pragma once
//=================================================================================================
//                  Copyright (C) 2017 Alain Lanthier - All Rights Reserved  
//                  License: MIT License    See LICENSE.md for the full license.
//=================================================================================================
//
// Piece<PieceID, _BoardSize>
//
// Piece instance represent a chess piece
// Piece class own and manage all chess pieces
//
// PieceID type is the piece identifier type
// PieceID type must be integral type
//
//
#ifndef _AL_CHESS_CORE_PIECE_HPP
#define _AL_CHESS_CORE_PIECE_HPP

namespace chess
{
    struct PieceKey
    {
        PieceName  _n;
        PieceColor _c;
    };
    struct PieceKeyLess
    {
        bool operator()(const PieceKey& lhs, const PieceKey& rhs) const 
        { 
            if (lhs._n < rhs._n) return true;
            if (lhs._n == rhs._n) return (lhs._c < rhs._c);
            return false;
        }
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    class Piece
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        friend class Board<PieceID, _BoardSize>;

    public:
        Piece(PieceName n = PieceName::none, PieceColor	c = PieceColor::none, PieceMoveStyle m = PieceMoveStyle::none);
        ~Piece() = default;

        Piece(const Piece&)                 = delete;
        Piece & operator=(const Piece &)    = delete;
        Piece(Piece&&)                      = delete;

        static const PieceID empty_id();
        static const PieceID get_id(PieceName _name, PieceColor _c);
        static const _Piece* get(PieceID id);
        static void init();
        static void clear(); // test...
        static const std::string to_str(PieceID id, bool with_space = true);
        static const std::string to_str2(PieceID id);
        static const uint8_t Piece<PieceID, _BoardSize>::to_uint8(PieceID id);
        static size_t pieces_size() { return pieces.size(); }

        const PieceID           get_id()        const;
        const PieceName         get_name()      const { return name; }
        const PieceColor        get_color()     const { return color; }
        const PieceMoveStyle    get_movestyle() const { return move_style; }
        const PieceName         get_moves()     const { return moves; }

    private:
        PieceName               name;
        PieceColor              color;
        PieceMoveStyle          move_style;
        std::vector<MoveUnit>   moves;

        static bool             is_init;
        static std::vector<std::unique_ptr<_Piece>> pieces;
        static std::map<PieceKey, PieceID, PieceKeyLess> _keyToID;
    };

    // static is_init
    template <typename PieceID, typename uint8_t _BoardSize>
    bool Piece<PieceID, _BoardSize>::is_init = false;

    // static _keyToID
    template <typename PieceID, typename uint8_t _BoardSize>
    std::map<PieceKey, PieceID, PieceKeyLess> Piece<PieceID, _BoardSize>::_keyToID;
    
    // static pieces
    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<std::unique_ptr<Piece<PieceID, _BoardSize>>> Piece<PieceID, _BoardSize>::pieces;

    // Piece()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline Piece<PieceID, _BoardSize>::Piece(PieceName n, PieceColor c, PieceMoveStyle m)
        : name(n), color(c), move_style(m) 
    {
        if (!Piece::is_init) init();
    }
    
    // empty_id()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const PieceID Piece<PieceID, _BoardSize>::empty_id() { return 0; }

    // get_id()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const PieceID Piece<PieceID, _BoardSize>::get_id(PieceName _name, PieceColor _c)
    {
        if (!Piece::is_init) init();

        auto &it = _keyToID.find(PieceKey{ _name,_c });
        if (it != _keyToID.end()) return it->second;
        return empty_id();
    }

    // get_id()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const PieceID Piece<PieceID, _BoardSize>::get_id() const 
    { 
        if (!Piece::is_init) init();
        return get_id(this->name, this->color); 
    }

    // get()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const Piece<PieceID, _BoardSize>* Piece<PieceID, _BoardSize>::get(PieceID id)
    {
        if (!Piece::is_init) init();

        assert(pieces.size() > 0);
        assert(id >= 0);
        assert(id < pieces.size());
        return pieces[id].get();
    }

    // to_str()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const std::string Piece<PieceID, _BoardSize>::to_str(PieceID id, bool with_space)
    {
        if (!Piece::is_init) init();

        std::string s;
        for (const auto &p : pieces)
        {
            if (_Piece::get_id(p->name, p->color) == id)
            {
                if (p->color == PieceColor::W) s += "w";
                else if (p->color == PieceColor::B) s += "b";
                else s += "-";

                if (p->name == PieceName::R) s += "R";
                else if (p->name == PieceName::N) s += "N";
                else if (p->name == PieceName::B) s += "B";
                else if (p->name == PieceName::Q) s += "Q";
                else if (p->name == PieceName::K) s += "K";
                else if (p->name == PieceName::P) s += "P";
                else s += "-";

                if (with_space) s += " ";
                return s;
            }
        }
        if (with_space) return "-- ";
        return "--";
    }

    // to_str2()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const std::string Piece<PieceID, _BoardSize>::to_str2(PieceID id)
    {
        if (!Piece::is_init) init();

        std::string s;
        for (const auto &p : pieces)
        {
            if (_Piece::get_id(p->name, p->color) == id)
            {
                if (p->color == PieceColor::W)
                {
                    if (p->name == PieceName::R) s += "R";
                    else if (p->name == PieceName::N) s += "N";
                    else if (p->name == PieceName::B) s += "B";
                    else if (p->name == PieceName::Q) s += "Q";
                    else if (p->name == PieceName::K) s += "K";
                    else if (p->name == PieceName::P) s += "P";
                    else s += "?";
                }
                else if (p->color == PieceColor::B)
                {
                    if (p->name == PieceName::R) s += "r";
                    else if (p->name == PieceName::N) s += "n";
                    else if (p->name == PieceName::B) s += "b";
                    else if (p->name == PieceName::Q) s += "q";
                    else if (p->name == PieceName::K) s += "k";
                    else if (p->name == PieceName::P) s += "p";
                    else s += "?";
                }
                else
                {
                    s += "-";
                }
                return s;
            }
        }
        return "-";
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline const uint8_t Piece<PieceID, _BoardSize>::to_uint8(PieceID id)
    {
        return (uint8_t)id; //...
    }

    // clear()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Piece<PieceID, _BoardSize>::clear()
    {
        _keyToID.clear();
        pieces.clear();
        pieces.shrink_to_fit();
        is_init = false;
    }

    // init()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Piece<PieceID, _BoardSize>::init()
    {
        if (is_init) return;
        is_init = true; // prevent re-entry

        uint8_t id = 0; 
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::none, PieceColor::none }, id++)); // implicit cast PieceID(id)
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::R, PieceColor::B }, id++));
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::N, PieceColor::B }, id++));
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::B, PieceColor::B }, id++));
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::Q, PieceColor::B }, id++));
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::K, PieceColor::B }, id++));
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::P, PieceColor::B }, id++));
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::R, PieceColor::W }, id++));
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::N, PieceColor::W }, id++));
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::B, PieceColor::W }, id++));
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::Q, PieceColor::W }, id++));
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::K, PieceColor::W }, id++));
        _keyToID.insert(std::pair<PieceKey, PieceID>(PieceKey{ PieceName::P, PieceColor::W }, id++));

        pieces.clear();
        pieces.push_back(std::make_unique<_Piece>(PieceName::none, PieceColor::none, PieceMoveStyle::none));
        pieces.push_back(std::make_unique<_Piece>(PieceName::R, PieceColor::B, PieceMoveStyle::Sliding));
        pieces.push_back(std::make_unique<_Piece>(PieceName::N, PieceColor::B, PieceMoveStyle::Jumping));
        pieces.push_back(std::make_unique<_Piece>(PieceName::B, PieceColor::B, PieceMoveStyle::Sliding));
        pieces.push_back(std::make_unique<_Piece>(PieceName::Q, PieceColor::B, PieceMoveStyle::Sliding));
        pieces.push_back(std::make_unique<_Piece>(PieceName::K, PieceColor::B, PieceMoveStyle::Sliding));
        pieces.push_back(std::make_unique<_Piece>(PieceName::P, PieceColor::B, PieceMoveStyle::SlidingDiagonalCapturePromo));
        pieces.push_back(std::make_unique<_Piece>(PieceName::R, PieceColor::W, PieceMoveStyle::Sliding));
        pieces.push_back(std::make_unique<_Piece>(PieceName::N, PieceColor::W, PieceMoveStyle::Jumping));
        pieces.push_back(std::make_unique<_Piece>(PieceName::B, PieceColor::W, PieceMoveStyle::Sliding));
        pieces.push_back(std::make_unique<_Piece>(PieceName::Q, PieceColor::W, PieceMoveStyle::Sliding));
        pieces.push_back(std::make_unique<_Piece>(PieceName::K, PieceColor::W, PieceMoveStyle::Sliding));
        pieces.push_back(std::make_unique<_Piece>(PieceName::P, PieceColor::W, PieceMoveStyle::SlidingDiagonalCapturePromo));

        for(size_t i = 0; i< pieces.size();i++)
            assert(i == pieces[i]->get_id());

        uint8_t n = _BoardSize - 1;
        std::vector<MoveUnit> mv_R = { { 0,1, n },{ 0,-1, n },{ 1,0, n },{ -1,0, n } };
        std::vector<MoveUnit> mv_N = { { 1,2, 1 },{ 1,-2, 1 },{ 2,1, 1 },{ 2,-1, 1 },{ -1,-2, 1 },{ -1,2, 1 },{ -2,1, 1 },{ -2,-1, 1 } };
        std::vector<MoveUnit> mv_B = { { 1,1, n },{ -1,-1, n },{ 1,-1, n },{ -1,1, n } };
        std::vector<MoveUnit> mv_Q = { { 0,1, n },{ 0,-1, n },{ 1,0, n },{ -1,0, n },{ 1,1, n },{ -1,-1, n },{ 1,-1, n },{ -1,1, n } };
        std::vector<MoveUnit> mv_K = { { 0,1, 1 },{ 0,-1, 1 },{ 1,0, 1 },{ -1,0, 1 },{ 1,1, 1 },{ -1,-1, 1 },{ 1,-1, 1 },{ -1,1, 1 } ,{ 3, 0, 1, MoveUnit::FLAG::conditional, "castlingK" } ,{ -2, 0, 1, MoveUnit::FLAG::conditional, "castlingQ" } };
        std::vector<MoveUnit> mv_PW = { { 0,1, 1 } ,{ 1,1, 1 },{ -1,1, 1 } ,   { 0, 2, 1, MoveUnit::FLAG::conditional, "y1" } ,{ -1, 1,1, MoveUnit::FLAG::conditional, "y5_ep" } ,{ 1, 1,1, MoveUnit::FLAG::conditional, "y5_ep" } };
        std::vector<MoveUnit> mv_PB = { { 0,-1, 1 } ,{ -1,-1, 1 },{ 1,-1, 1 } ,{ 0,-2, 1, MoveUnit::FLAG::conditional, "y6" } ,{ -1,-1,1, MoveUnit::FLAG::conditional, "y2_ep" } ,{ 1,-1,1, MoveUnit::FLAG::conditional, "y2_ep" } };

        pieces.at(get_id(PieceName::R, PieceColor::W))->moves = mv_R;
        pieces.at(get_id(PieceName::N, PieceColor::W))->moves = mv_N;
        pieces.at(get_id(PieceName::B, PieceColor::W))->moves = mv_B;
        pieces.at(get_id(PieceName::Q, PieceColor::W))->moves = mv_Q;
        pieces.at(get_id(PieceName::K, PieceColor::W))->moves = mv_K;
        pieces.at(get_id(PieceName::P, PieceColor::W))->moves = mv_PW;

        pieces.at(get_id(PieceName::R, PieceColor::B))->moves = mv_R;
        pieces.at(get_id(PieceName::N, PieceColor::B))->moves = mv_N;
        pieces.at(get_id(PieceName::B, PieceColor::B))->moves = mv_B;
        pieces.at(get_id(PieceName::Q, PieceColor::B))->moves = mv_Q;
        pieces.at(get_id(PieceName::K, PieceColor::B))->moves = mv_K;
        pieces.at(get_id(PieceName::P, PieceColor::B))->moves = mv_PB;

        is_init = true;
    }
};
#endif
