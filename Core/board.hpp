#pragma once
//=================================================================================================
//                  Copyright (C) 2017 Alain Lanthier - All Rights Reserved  
//                  License: MIT License    See LICENSE.md for the full license.
//=================================================================================================
//
// Board<PieceID, _BoardSize>
//
// Board represent a chess board of size [_BoardSize*_BoardSize]
// Board internal representation is std::vector<PieceID>
// _BoardSize maximum is 255
//
// PieceID type is the piece identifier type  
// PieceID type must be convertible to integral type
//
//
#ifndef _AL_CHESS_CORE_BOARD_HPP
#define _AL_CHESS_CORE_BOARD_HPP

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize>
    class Board
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        Board(bool set_classic = false);

        Board(const Board&)                 = default;
        Board & operator=(const Board &)    = default;
        Board(Board&&)                      = default;
        Board & operator=(Board&&)          = default;
        ~Board()                            = default;

        bool legal_pos(char option = 0) const;  // 0==just check pawn rows (add more option...)

        const uint8_t index_at(uint8_t x, uint8_t y) const { return _BoardSize * y + x; }
        const _Piece* piece_at(uint8_t x, uint8_t y) const { return _Piece::get(_cells.at(index_at(x, y))); }

        const PieceID get_pieceid_at(uint8_t x, uint8_t y) const { return _cells.at(index_at(x, y)); }
        void          set_pieceid_at(PieceID id, uint8_t x, uint8_t y) { _cells.at(index_at(x, y)) = id; }
        void          set_pieceid_at(PieceID id, uint16_t sq) { _cells.at(index_at((uint8_t)(sq%_BoardSize), (uint8_t)(sq/_BoardSize))) = id; }

        const PieceColor    get_color() const;
        void                set_color(PieceColor c) { _color_toplay = c; }
        const PieceColor    get_opposite_color() const;
        void                set_opposite_color();

        void apply_move(const _Move& m);
        void undo_move();

        bool can_capture_opposite_king(const std::vector<_Move>& m, size_t& ret_index) const;
        bool is_in_check() const;
        uint16_t count_capture_opposite_king(const std::vector<_Move>& m) const;
        uint16_t count_capture_king() const;
        void set_classic_pos();
        bool has_piece(PieceName n, PieceColor c) const;
        uint16_t cnt_piece(PieceName n, PieceColor c) const;
        uint16_t cnt_piece(PieceID)  const;
        std::vector<PieceID> get_piecesID() const;
        std::vector<uint16_t> get_square_of_pieces() const;
        uint8_t on_edge(PieceName n, PieceColor c) const;
        uint8_t dist(PieceName n, PieceColor c, PieceName n2, PieceColor c2) const;
        uint16_t cnt_all_piece() const;
        uint16_t cnt_all_piece(PieceColor c) const;
        uint16_t cnt_move(PieceName n, PieceColor c, const std::vector<_Move>& m) const;
        uint16_t cnt_move_oppo(PieceName n, PieceColor c, const std::vector<_Move>& m) const;
        bool is_final(const std::vector<_Move>& m) const;
        ExactScore final_score(const std::vector<_Move>& m) const;
        ExactScore minmax_score(const std::vector<_Move>& m);

        bool is_last_move_promo() const;
        bool is_last_move_capture() const;
        bool is_last_move_pawn() const;

        const std::vector<_Move>    generate_moves(bool is_recursive_call = false);
        const std::list<_Move>      get_history_moves() const { return _history_moves; }
        size_t get_histo_size()     const { return _history_moves.size(); }
        _Move last_history_move()   const { return _history_moves.back(); }


        const std::string to_str()  const;

        static void reset_to_default_option();
        static bool allow_self_check()              { return _allow_self_check;}
        static bool check_repeating_move_draw()     { return _check_repeating_move_draw; }
        static bool check_50_moves_draw()           { return _check_50_moves_draw;}
        static bool promo_Q_only()                  { return _promo_Q_only; }
        static void set_allow_self_check(bool v)            { _allow_self_check = v; }
        static void set_check_repeating_move_draw(bool v)   { _check_repeating_move_draw = v; }
        static void set_check_50_moves_draw(bool v)         { _check_50_moves_draw = v; }
        static void set_promo_Q_only(bool v)                { _promo_Q_only = v; }

        uint16_t get_square_ofpiece(PieceName n, PieceColor c, bool check_2nd_instance = false) const;
        uint16_t get_square_ofpiece_instance(PieceName n, PieceColor c, uint16_t instance) const;

        void clear()
        {
            _color_toplay = PieceColor::W;
            for (auto& v : _cells) v = Piece<PieceID, _BoardSize>::empty_id();
            _history_moves.clear();
        }

        static Board<PieceID, _BoardSize> get_random_position_KQK(bool no_check = false, uint64_t ncall = 0);
        static Board<PieceID, _BoardSize> get_random_position_KPK(bool no_check = false, uint64_t ncall = 0);
        static Board<PieceID, _BoardSize> get_random_position_KPKP(bool no_check = false, uint64_t ncall = 0);

    private:
        PieceColor              _color_toplay;
        std::vector<PieceID>    _cells;
        std::list<_Move>        _history_moves;     // History

        // Features
        static bool _allow_self_check;
        static bool _check_repeating_move_draw;
        static bool _check_50_moves_draw;
        static bool _promo_Q_only;                 // Promo Queen only or all
    };

    template <typename PieceID, typename uint8_t _BoardSize> bool Board<PieceID, _BoardSize>::_allow_self_check = true;
    template <typename PieceID, typename uint8_t _BoardSize> bool Board<PieceID, _BoardSize>::_check_repeating_move_draw = false;
    template <typename PieceID, typename uint8_t _BoardSize> bool Board<PieceID, _BoardSize>::_check_50_moves_draw = false;
    template <typename PieceID, typename uint8_t _BoardSize> bool Board<PieceID, _BoardSize>::_promo_Q_only = true;

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Board<PieceID, _BoardSize>::reset_to_default_option()
    {
        Board<PieceID, _BoardSize>::_allow_self_check = true;
        Board<PieceID, _BoardSize>::_check_repeating_move_draw = false;
        Board<PieceID, _BoardSize>::_check_50_moves_draw = false;
        Board<PieceID, _BoardSize>::_promo_Q_only = true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool Board<PieceID, _BoardSize>::legal_pos(char option) const
    {
        if (option == 0)
        {
            // No pawn at first/last row
            PieceID WPid = _Piece::get_id(PieceName::P, PieceColor::W);
            PieceID BPid = _Piece::get_id(PieceName::P, PieceColor::B);
            PieceID id;

            for (uint8_t y = 0; y < _BoardSize; y++)
            {
                for (uint8_t x = 0; x < _BoardSize; x++)
                {
                    id = _cells.at(index_at(x, y));
                    if ((id == WPid) && (y == 0)) return false;
                    if ((id == BPid) && (y == 0)) return false;
                    if ((id == WPid) && (y == _BoardSize-1)) return false;
                    if ((id == BPid) && (y == _BoardSize-1)) return false;
                }
            }
        }
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::ofstream& operator<<(std::ofstream& os, const Board<PieceID, _BoardSize>& play_board)
    {
        std::vector<PieceID>  _pieces;
        std::vector<std::pair<uint8_t, uint8_t>>  _xy;

        for (uint8_t x = 0; x < _BoardSize; x++)
        {
            for (uint8_t y = 0; y < _BoardSize; y++)
            {
                if (play_board.get_pieceid_at(x, y) != Piece<PieceID, _BoardSize>::empty_id())
                {
                    _pieces.push_back(play_board.get_pieceid_at(x, y));
                    _xy.push_back(std::pair<uint8_t, uint8_t>({ x, y }));
                }
            }
        }

        os << (int)_BoardSize << " ";
        os << PieceColor_to_int(play_board.get_color());  os << " ";

        os << bool_to_int(play_board.allow_self_check());           os << " ";
        os << bool_to_int(play_board.check_repeating_move_draw());  os << " ";
        os << bool_to_int(play_board.check_50_moves_draw());        os << " ";
        os << bool_to_int(play_board.promo_Q_only());               os << " ";

        os << (int)_pieces.size() << " ";
        for (auto &v : _pieces) { os << (int)v; os << " "; } // ...

        os << (int)_xy.size() << " ";
        for (auto &v : _xy)
        {
            os << (int)v.first;     os << " ";
            os << (int)v.second;    os << " ";
        }

        const std::list<Move<PieceID>>  hm = play_board.get_history_moves();
        for (auto &v : hm) { os << v; os << " "; }

        return os;
    }

    // generate_moves_no_self_check()
    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<Move<PieceID>> generate_moves_no_self_check(Board<PieceID, _BoardSize>&, std::vector<Move<PieceID>>& m, size_t from, size_t to);

    // Board() - ct()
    template <typename PieceID, typename uint8_t _BoardSize>
    Board<PieceID, _BoardSize>::Board(bool set_classic)
        : _cells(_BoardSize*_BoardSize)
    {
        _color_toplay = PieceColor::none;
        PieceID emptyid = _Piece::empty_id();
        for (auto &v : _cells) v = emptyid;
        if (set_classic) set_classic_pos();
    }

    // get_color()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const PieceColor Board<PieceID, _BoardSize>::get_color() const
    { 
        return _color_toplay;
    }

    // get_opposite_color()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const PieceColor Board<PieceID, _BoardSize>::get_opposite_color() const
    {
        if (_color_toplay == PieceColor::B) return PieceColor::W;
        else if (_color_toplay == PieceColor::W) return PieceColor::B;
        return PieceColor::none;
    }

    // set_opposite_color()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Board<PieceID, _BoardSize>::set_opposite_color() {
        if (_color_toplay == PieceColor::B) _color_toplay = PieceColor::W;
        else if (_color_toplay == PieceColor::W) _color_toplay = PieceColor::B;
    }

    // set_classic_pos()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Board<PieceID, _BoardSize>::set_classic_pos()
    {
        if (_BoardSize < 8) return;	// throw...

        _color_toplay = PieceColor::W;
        _history_moves.clear();

        PieceID emptyid = _Piece::empty_id();
        for (auto &v : _cells) v = emptyid;

        set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::W), 0, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::W), 1, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::W), 2, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::Q, chess::PieceColor::W), 3, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::K, chess::PieceColor::W), 4, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::W), 5, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::W), 6, 0);
        set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::W), 7, 0);
        for (uint8_t i = 0; i<8; i++) set_pieceid_at(_Piece::get_id(chess::PieceName::P, chess::PieceColor::W), i, 1);

        set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::B), 0, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::B), 1, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::B), 2, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::Q, chess::PieceColor::B), 3, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::K, chess::PieceColor::B), 4, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::B, chess::PieceColor::B), 5, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::N, chess::PieceColor::B), 6, 7);
        set_pieceid_at(_Piece::get_id(chess::PieceName::R, chess::PieceColor::B), 7, 7);
        for (uint8_t i = 0; i<8; i++) set_pieceid_at(_Piece::get_id(chess::PieceName::P, chess::PieceColor::B), i, 6);
    }

    // get_square_ofpiece
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t Board<PieceID, _BoardSize>::get_square_ofpiece(PieceName n, PieceColor c, bool check_2nd_instance) const
    {
        uint16_t cnt = 0;  uint16_t xy_1; uint16_t xy_next;
        PieceID id = _Piece::get_id(n, c);
        // Lowest first
        for (uint8_t y = 0; y < _BoardSize; y++)
        {
            for (uint8_t x = 0; x < _BoardSize; x++)
            {
                if (_cells.at(index_at(x, y)) == id)
                {
                    if (cnt == 0) xy_1 = (y*_BoardSize) + x;
                    if (!check_2nd_instance) return xy_1;

                    if (cnt != 0) xy_next = (y*_BoardSize) + x;
                    cnt++; 
                }
            }
        }
        if (cnt  > 1) return xy_next;
        if (cnt == 1) return xy_1;

        // assert(false);
        return -1; // invalid
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t Board<PieceID, _BoardSize>::get_square_ofpiece_instance(PieceName n, PieceColor c, uint16_t instance) const
    {
        uint16_t cnt = 0;
        PieceID id = _Piece::get_id(n, c);
        // Lowest first
        for (uint8_t y = 0; y < _BoardSize; y++)
        {
            for (uint8_t x = 0; x < _BoardSize; x++)
            {
                if (_cells.at(index_at(x, y)) == id)
                {
                    if (cnt == instance) return (y*_BoardSize) + x;
                    cnt++;
                }
            }
        }
        assert(false);
        return -1; // invalid
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::vector<uint16_t> Board<PieceID, _BoardSize>::get_square_of_pieces() const
    {
        std::vector<uint16_t> v;
        // Lowest first
        for (uint8_t y = 0; y < _BoardSize; y++)
        {
            for (uint8_t x = 0; x < _BoardSize; x++)
            {
                if (_cells.at(index_at(x, y)) != _Piece::empty_id())
                {
                    v.push_back(y*_BoardSize+x);
                }
            }
        }
        return v;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<PieceID> Board<PieceID, _BoardSize>::get_piecesID() const
    {
        struct
        {
            bool operator() (const PieceID& lhs, const PieceID& rhs)
            {
                const _Piece* a = _Piece::get(lhs);
                const _Piece* b = _Piece::get(rhs);
                if      ((a->get_color() == PieceColor::W) && (b->get_color() == PieceColor::B)) return true;
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
        } sorter_less_pieces;

        std::vector<PieceID> v;
        for (const auto& m : _cells)
        {
            if (m != _Piece::empty_id())
                v.push_back(m);
        }
        std::sort(v.begin(), v.end(), sorter_less_pieces);
        return v;
    }

    // has_piece()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool Board<PieceID, _BoardSize>::has_piece(PieceName n, PieceColor c) const
    {
        PieceID id = _Piece::get_id(n, c);
        for (const auto &m : _cells)
            if (m == id) return true;
        return false;
    }

    // cnt_piece
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t Board<PieceID, _BoardSize>::cnt_piece(PieceName n, PieceColor c) const
    {
        uint16_t cnt = 0;
        PieceID id = _Piece::get_id(n, c);
        for (const auto &v : _cells)
        {
            if (v == id) cnt++;
        }
        return cnt;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t Board<PieceID, _BoardSize>::cnt_piece(PieceID id) const
    {
        uint16_t cnt = 0;
        //PieceID id = _Piece::get_id(n, c);
        for (const auto &v : _cells)
        {
            if (v == id) cnt++;
        }
        return cnt;
    }
    
    // cnt_all_piece
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t Board<PieceID, _BoardSize>::cnt_all_piece() const
    {
        uint16_t cnt = 0;
        for (const auto &v : _cells)
        {
            if (v != _Piece::empty_id())
             cnt++;
        }
        return cnt;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t Board<PieceID, _BoardSize>::cnt_all_piece(PieceColor c) const
    {
        uint16_t cnt = 0;
        const _Piece* a;
        for (const auto &v : _cells)
        {
            if (v != _Piece::empty_id())
            {
                a = _Piece::get(v);
                if (a->get_color() == c)
                    cnt++;
            }
        }
        return cnt;
    }

    //cnt_move
    template <typename PieceID, typename uint8_t _BoardSize>
    uint16_t Board<PieceID, _BoardSize>::cnt_move(PieceName n, PieceColor c, const std::vector<_Move>& m) const
    {
        uint16_t cnt = 0;
        PieceID id = _Piece::get_id(n, c);
        for (const auto &v : m)
        {
            PieceID id = _Piece::get_id(n, c);
            if (v.prev_src_id == id) cnt++;
        }
        return cnt;
    }

    // cnt_move_oppo
    template <typename PieceID, typename uint8_t _BoardSize>
    uint16_t Board<PieceID, _BoardSize>::cnt_move_oppo(PieceName n, PieceColor c, const std::vector<_Move>& m) const
    {
        Board<PieceID, _BoardSize> b = *this;
        b.set_opposite_color();
        std::vector<_Move> mm = b.generate_moves();
        return b.cnt_move(n, c, mm);
    }

    // can_capture_opposite_king()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool Board<PieceID, _BoardSize>::can_capture_opposite_king(const std::vector<_Move>& m, size_t& ret_index) const
    {
        size_t index = 0;
        PieceID K_id = _Piece::get_id(PieceName::K, get_opposite_color());
        for (const auto &mv : m)
        {
            if (mv.prev_dst_id == K_id)
            {
                ret_index = index;
                return true;
            }
            index++;
        }
        return false;
    }

    // count_capture_opposite_king()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t Board<PieceID, _BoardSize>::count_capture_opposite_king(const std::vector<_Move>& m) const
    {
        uint16_t n = 0;
        PieceID K_id = _Piece::get_id(PieceName::K, get_opposite_color());
        for (const auto &mv : m)
        {
            if (mv.prev_dst_id == K_id) n++;
        }
        return n;
    }

    // count_capture_king()
    template <typename PieceID, typename uint8_t _BoardSize>
    uint16_t Board<PieceID, _BoardSize>::count_capture_king() const
    {
        Board<PieceID, _BoardSize> b = *this;
        b.set_opposite_color();
        std::vector<_Move> m = b.generate_moves();
        return b.count_capture_opposite_king(m);
    }

    // is_in_check()
    template <typename PieceID, typename uint8_t _BoardSize>
    bool Board<PieceID, _BoardSize>::is_in_check() const
    {
        Board<PieceID, _BoardSize> b = *this;
        b.set_opposite_color();
        std::vector<_Move> m = b.generate_moves();
        size_t mv;
        return b.can_capture_opposite_king(m, mv);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline ExactScore Board<PieceID, _BoardSize>::minmax_score(const std::vector<_Move>& m)
    {
        ExactScore sc = final_score(m);
        if (sc != ExactScore::UNKNOWN) return sc;

        ExactScore max_score = ExactScore::UNKNOWN;
        std::vector<_Move> m_child;

        for (const auto &mv : m)
        {
            apply_move(mv);
            m_child = generate_moves();
            sc = final_score(m_child);

            if (get_color() == PieceColor::B)
            {
                if (sc == ExactScore::WIN) max_score = ExactScore::WIN;
                else if (sc == ExactScore::DRAW)
                {
                    if (max_score != ExactScore::WIN)  max_score = ExactScore::DRAW;
                }
                else max_score = ExactScore::LOSS;
            }
            else
            {
                if (sc == ExactScore::LOSS) max_score = ExactScore::LOSS;
                else if (sc == ExactScore::DRAW)
                {
                    if (max_score != ExactScore::LOSS)  max_score = ExactScore::DRAW;
                }
                else max_score = ExactScore::WIN;
            }

            undo_move();
        }
        return max_score;
    }

    // final_score()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline ExactScore Board<PieceID, _BoardSize>::final_score(const std::vector<_Move>& m) const
    {
        if (!has_piece(PieceName::K, PieceColor::B)) return ExactScore::WIN;
        if (!has_piece(PieceName::K, PieceColor::W)) return ExactScore::LOSS;
        if (m.size() == 0) return ExactScore::DRAW;
        return ExactScore::UNKNOWN;
    }

    // is_final()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool Board<PieceID, _BoardSize>::is_final(const std::vector<_Move>& m) const
    {
        if (!has_piece(PieceName::K, PieceColor::W)) return true;
        if (!has_piece(PieceName::K, PieceColor::B)) return true;
        if (m.size() == 0) return true;

        if (!Board<PieceID, _BoardSize>::_allow_self_check)
        {
            // No extra processing required:
            // 1 - K checkmated then (m.size() == 0) - already checked
            // 2 - Opponent_K checkmated then on next move would be case 1 - ok
            // 3 - Draw then (m.size() == 0) - already checked
        }

        if (Board<PieceID, _BoardSize>::_check_repeating_move_draw)
        {
            // ...
        }

        if (Board<PieceID, _BoardSize>::_check_50_moves_draw)
        {
            // ...
        }

        return false;
    }

    // apply_move
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Board<PieceID, _BoardSize>::apply_move(const _Move& m)
    {
        _cells.at(index_at(m.dst_x, m.dst_y)) = _cells.at(index_at(m.src_x, m.src_y));
        _cells.at(index_at(m.src_x, m.src_y)) = _Piece::empty_id();
        if ((m.mu.flag_spec == "y5_ep") || (m.mu.flag_spec == "y2_ep"))
        {
            _cells.at(index_at(m.src_x + m.mu.x * 1, m.src_y - m.mu.y * 1)) = _Piece::empty_id();
        }
        else if (m.mu.context_extra == "Q") _cells.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::Q, _color_toplay);
        else if (m.mu.context_extra == "R") _cells.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::R, _color_toplay);
        else if (m.mu.context_extra == "B") _cells.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::B, _color_toplay);
        else if (m.mu.context_extra == "N") _cells.at(index_at(m.dst_x, m.dst_y)) = _Piece::get_id(PieceName::N, _color_toplay);
        else if (m.mu.context_extra == "castlingK_R") _cells.at(index_at(_BoardSize-1, m.dst_y)) = _Piece::get_id(PieceName::R, _color_toplay);
        else if (m.mu.context_extra == "castlingQ_R") _cells.at(index_at(3, m.dst_y)) = _Piece::get_id(PieceName::R, _color_toplay);
        _history_moves.push_back(m);
        set_opposite_color();
    }

    // undo_move()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void Board<PieceID, _BoardSize>::undo_move()
    {
        if (_history_moves.size() == 0) return;

        _Move m = _history_moves.back();
        _history_moves.pop_back();

        _cells.at(index_at(m.src_x, m.src_y)) = m.prev_src_id;
        _cells.at(index_at(m.dst_x, m.dst_y)) = m.prev_dst_id;
        if ((m.mu.flag_spec == "y5_ep") || (m.mu.flag_spec == "y2_ep"))
        {
            if (_history_moves.size() > 0)
            {
                _Move mh = _history_moves.back();
                _cells.at(index_at(mh.dst_x, mh.dst_y)) = mh.prev_src_id;
            }
        }
        // castling...
        set_opposite_color();
    }

    // generate_moves()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline const std::vector<Move<PieceID>> Board<PieceID, _BoardSize>::generate_moves(bool is_recursive_call)
    {
        using _Board = Board<PieceID, _BoardSize>;

        const _Piece*  p_src;
        const _Piece*  p_dst;
        std::vector<_Move> m;

        if (!has_piece(PieceName::K, PieceColor::W)) return m;
        if (!has_piece(PieceName::K, PieceColor::B)) return m;

        for (uint8_t i = 0; i < _BoardSize; i++)
        {
            for (uint8_t j = 0; j < _BoardSize; j++)
            {
                p_src = this->piece_at(i, j);

                if (p_src->color != this->_color_toplay) continue;
                if (p_src->name != PieceName::none) // valid src piece
                {
                    for (auto &mu : p_src->moves)
                    {
                        bool prev_path_empty = true;
                        for (uint8_t n = 1; n <= mu.len; n++) // follow mu direction
                        {
                            if ((prev_path_empty) &&
                                (i + mu.x*n >= 0) && (i + mu.x*n < _BoardSize) &&
                                (j + mu.y*n >= 0) && (j + mu.y*n < _BoardSize)
                                )
                            {
                                p_dst = this->piece_at((uint8_t)(i + mu.x*n), (uint8_t)(j + mu.y*n));

                                // Move
                                _Move mv( i, j, (uint8_t)(i + mu.x*n), (uint8_t)(j + mu.y*n),  p_src->get_id(), p_dst->get_id(), mu );
                                if (_history_moves.size() > 0)
                                {
                                    mv.copy_state(_history_moves.back());
                                }

                                // castling
                                if (    (mu.flag == MoveUnit::FLAG::conditional) &&
                                        (p_src->get_name() == PieceName::K) &&
                                        ((mu.flag_spec == "castlingK") || (mu.flag_spec == "castlingQ"))
                                    )
                                {
                                    if (_BoardSize >= 8) // ...
                                    {
                                        uint8_t rank_y = (p_src->get_color() == PieceColor::W) ? 0 : _BoardSize-1;
                                        if ((mu.flag_spec == "castlingK") && (mv.get_state_castling(p_src->get_color(), PieceName::K)))
                                        {
                                            if ((get_pieceid_at(_BoardSize - 4, rank_y) == _Piece::get_id(PieceName::K, p_src->get_color())) &&
                                                (get_pieceid_at(_BoardSize - 3, rank_y) == _Piece::empty_id()) &&
                                                (get_pieceid_at(_BoardSize - 2, rank_y) == _Piece::empty_id()) &&
                                                (get_pieceid_at(_BoardSize - 1, rank_y) == _Piece::get_id(PieceName::R, p_src->get_color()))
                                                )
                                            {
                                                mv.mu.context_extra = "castlingK_R";
                                                mv.set_state_castling(p_src->get_color(), PieceName::K, false);
                                                m.push_back(mv);
                                            }
                                        }
                                        else if ((mu.flag_spec == "castlingQ") && (mv.get_state_castling(p_src->get_color(), PieceName::Q)))
                                        {
                                            if ((get_pieceid_at(0, rank_y) == _Piece::get_id(PieceName::R, p_src->get_color())) &&
                                                (get_pieceid_at(1, rank_y) == _Piece::empty_id()) &&
                                                (get_pieceid_at(2, rank_y) == _Piece::empty_id()) &&
                                                (get_pieceid_at(3, rank_y) == _Piece::empty_id()) &&
                                                (get_pieceid_at(4, rank_y) == _Piece::get_id(PieceName::K, p_src->get_color()))
                                                )
                                            {
                                                mv.mu.context_extra = "castlingQ_R";
                                                mv.set_state_castling(p_src->get_color(), PieceName::Q, false);
                                                m.push_back(mv);
                                            }
                                        }
                                    }
                                }
                                else if (p_dst->name == PieceName::none)
                                {
                                    if ((p_src->move_style == PieceMoveStyle::Sliding) || (p_src->move_style == PieceMoveStyle::Jumping))
                                    {
                                        if (p_src->get_name() == PieceName::K)
                                            mv.set_state_castling(p_src->get_color(), PieceName::K, false);
                                        m.push_back(mv);
                                    }
                                    else if (p_src->move_style == PieceMoveStyle::SlidingDiagonalCapturePromo)
                                    {
                                        if (mu.flag == MoveUnit::FLAG::conditional)
                                        {
                                            if ( ((mu.flag_spec == "y1") && (j == 1)) ||
                                                 ((mu.flag_spec == "y6") && (j == _BoardSize-2)) )
                                            {
                                                if (_BoardSize >= 5)    // otherwise pawn can promo in 1 move
                                                {
                                                    uint8_t prev_x = (mu.x > 1) ? 1 : mu.x;
                                                    uint8_t prev_y = (mu.y > 1) ? 1 : mu.y;
                                                    uint8_t prev_mv_dst_x = (uint8_t)(i + prev_x * 1);
                                                    uint8_t prev_mv_dst_y = (uint8_t)(j + prev_y * 1);
                                                    if (get_pieceid_at(prev_mv_dst_x, prev_mv_dst_y) == _Piece::empty_id()) // empty path
                                                    {
                                                        mv.set_state_ep(get_color(), i, false);
                                                        m.push_back(mv);
                                                    }
                                                }
                                            }
                                            // en passant
                                            else if ( ((mu.flag_spec == "y5_ep") && (j == _BoardSize - 3) && (get_color() == PieceColor::W) && (_history_moves.size() > 0)) ||
                                                      ((mu.flag_spec == "y2_ep") && (j == 2) && (get_color() == PieceColor::B) && (_history_moves.size() > 0)) )
                                            {
                                                {
                                                    if (mv.get_state_ep(get_color(), i))
                                                    {
                                                        _Move mh = _history_moves.back();
                                                        if (mh.prev_src_id == _Piece::get_id(PieceName::P, get_opposite_color()))
                                                        {
                                                            if ((mh.src_x == i + mu.x) && (mh.src_y == j + mu.y) && (abs(mh.dst_y - mh.src_y) == 2))
                                                            {
                                                                mv.set_state_ep(get_color(), i, false);
                                                                m.push_back(mv);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else 
                                        {
                                            if (std::abs(mu.x) != std::abs(mu.y)) // not diago
                                            {
                                                // promo
                                                if ( ((p_src->color == PieceColor::W) && (j == _BoardSize-2)) || ((p_src->color == PieceColor::B) && (j == 1)))
                                                {
                                                    if (Board<PieceID, _BoardSize>::_promo_Q_only)
                                                    {
                                                        _Move mvQ = mv; mvQ.mu.context_extra = "Q"; m.push_back(mvQ);
                                                    }
                                                    else
                                                    {
                                                        _Move mvQ = mv; mvQ.mu.context_extra = "Q"; m.push_back(mvQ);
                                                        _Move mvR = mv; mvR.mu.context_extra = "R"; m.push_back(mvR);
                                                        _Move mvN = mv; mvN.mu.context_extra = "N"; m.push_back(mvN);
                                                        _Move mvB = mv; mvB.mu.context_extra = "B"; m.push_back(mvB);
                                                    }
             
                                                }
                                                else
                                                {
                                                    m.push_back(mv);
                                                }
                                            }
                                            else
                                            {
                                            }
                                        }
                                    }
                                }
                                // capture dst
                                else if (p_dst->color != p_src->color) 
                                {
                                    if ((p_src->move_style == PieceMoveStyle::Sliding) || (p_src->move_style == PieceMoveStyle::Jumping))
                                    {
                                        if (p_src->get_name() == PieceName::K)
                                            mv.set_state_castling(p_src->get_color(), PieceName::K, false);
                                        m.push_back(mv);
                                    }
                                    else if (p_src->move_style == PieceMoveStyle::SlidingDiagonalCapturePromo)
                                    {
                                        if (mu.flag == MoveUnit::FLAG::conditional)
                                        {
                                        }
                                        else
                                        {
                                            if (std::abs(mu.x) != std::abs(mu.y)) // not diago
                                            {
                                            }
                                            else
                                            {
                                                // promo
                                                if ( ((p_src->color == PieceColor::W) && (j == _BoardSize-2)) || ((p_src->color == PieceColor::B) && (j == 1)))
                                                {
                                                    if (Board<PieceID, _BoardSize>::_promo_Q_only)
                                                    {
                                                        _Move mvQ = mv; mvQ.mu.context_extra = "Q"; m.push_back(mvQ);
                                                    }
                                                    else
                                                    {
                                                        _Move mvQ = mv; mvQ.mu.context_extra = "Q"; m.push_back(mvQ);
                                                        _Move mvR = mv; mvR.mu.context_extra = "R"; m.push_back(mvR);
                                                        _Move mvN = mv; mvN.mu.context_extra = "N"; m.push_back(mvN);
                                                        _Move mvB = mv; mvB.mu.context_extra = "B"; m.push_back(mvB);
                                                    }
                                                }
                                                else
                                                {
                                                    m.push_back(mv);
                                                }
                                            }
                                        }
                                    }
                                    prev_path_empty = false;
                                }
                                else if (p_dst->color == p_src->color) // same color dst
                                {
                                    prev_path_empty = false;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (is_recursive_call)
        {
            // ok no more recursive
        }
        else if (!Board<PieceID, _BoardSize>::_allow_self_check)  // Splitting computation into multi thread
        {
            unsigned concurrenty = std::thread::hardware_concurrency();
            if (concurrenty < 2) concurrenty = 2;

            _Board*                          bb  = new _Board[concurrenty];
            std::vector<_Move>*              bm  = new std::vector<_Move>[concurrenty];
            std::future<std::vector<_Move>>* fut = new std::future<std::vector<_Move>>[concurrenty];

            size_t n = (size_t)m.size() / concurrenty;
            if (n < 1) n = 1;
            for (size_t i = 0; i < concurrenty; i++)
            {
                bb[i] = *this; // Get a copy of board per thread
                if (i<concurrenty-1)
                    fut[i] = std::async(generate_moves_no_self_check<PieceID, _BoardSize>, bb[i], m, i*n, (i+1)*n-1);
                else
                    fut[i] = std::async(generate_moves_no_self_check<PieceID, _BoardSize>, bb[i], m, i*n, m.size()-1);
            }

            for (size_t i = 0; i < concurrenty; i++)
                bm[i]=fut[i].get();

            // reuse bm[0]
            bm[0].reserve(m.size()+1);
            for (size_t i = 1; i < concurrenty; i++)
                bm[0].insert(bm[0].end(), bm[i].begin(), bm[i].end());

            std::vector<_Move> mm = bm[0];
            delete[]bb;
            delete[]fut;
            delete[]bm;
            return mm;
        }

        return m;
    }

    // generate_moves_no_self_check()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::vector<Move<PieceID>> generate_moves_no_self_check(Board<PieceID, _BoardSize>& bb, std::vector<Move<PieceID>>& m, size_t from, size_t to)
    {
        std::vector<Move<PieceID>> mm;
        if (to < from) return mm;

        size_t ret_idx;
        for (size_t i=from;i<=to;i++)
        {
            bb.apply_move(m[i]);
            {
                std::vector<Move<PieceID>> m_oppon = bb.generate_moves(true);
                if (!bb.can_capture_opposite_king(m_oppon, ret_idx))
                {
                    mm.push_back(m[i]);
                }
            }
            bb.undo_move();
        }
        return mm;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline const std::string Board<PieceID, _BoardSize>::to_str() const
    {
        std::string s;
        PieceID id;
        for (uint8_t j = _BoardSize; j >= 1; j--)
        {
            for (uint8_t i = 0; i < _BoardSize; i++)
            {
                id = get_pieceid_at(i, j - 1);
                s += _Piece::to_str(id);
            }
            if (j > 1)  s += "\n";
        }
        return s;
    }

    // on_edge
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint8_t Board<PieceID, _BoardSize>::on_edge(PieceName n, PieceColor c) const
    {
        uint8_t cnt = 0;
        PieceID id = _Piece::get_id(n, c);

        for (uint8_t x = 0; x < _BoardSize; x++)
        {
            for (uint8_t y = 0; y < _BoardSize; y++)
            {
                if ((x == 0) || (x == _BoardSize - 1) || (y == 0) || (y == _BoardSize - 1))
                {
                    if (get_pieceid_at(x, y) == id)
                    {
                        cnt++;
                    }
                }
            }
        }
        return cnt;
    }

    // dist
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint8_t Board<PieceID, _BoardSize>::dist(PieceName n, PieceColor c, PieceName n2, PieceColor c2) const
    {
        size_t cnt = 0;
        PieceID id1 = _Piece::get_id(n, c);
        PieceID id2 = _Piece::get_id(n2, c2);
        uint8_t x1;
        uint8_t y1;
        uint8_t x2;
        uint8_t y2;
        bool found1 = false;
        bool found2 = false;

        for (uint8_t x = 0; x < _BoardSize; x++)
        {
            for (uint8_t y = 0; y < _BoardSize; y++)
            {
                if (get_pieceid_at(x, y) == id1)
                {
                    x1 = x; y1 = y;
                    found1 = true;
                }
                else if (get_pieceid_at(x, y) == id2)
                {
                    x2 = x; y2 = y;
                    found2 = true;
                }
            }
        }
        if (found1 && found2)
        {
            return std::max( std::abs(x2 - x1), std::abs(y2 - y1) );
        }
        return 0;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    Board<PieceID, _BoardSize> Board<PieceID, _BoardSize>::get_random_position_KQK(bool no_check, uint64_t ncall)
    {
        ncall++;
        uint64_t n = 0;
        uint8_t wQ = 0;
        uint8_t wK = 0;
        uint8_t bK = 0;
        while ((wK == bK) || (wK == wQ) || (bK == wQ))
        {
            wQ = (std::rand() % (_BoardSize*_BoardSize));
            wK = (std::rand() % (_BoardSize*_BoardSize));
            bK = (std::rand() % (_BoardSize*_BoardSize));
            n++;
            if (n > 100)
            {
                no_check = false;
                break;
            }
        }
        Board<PieceID, _BoardSize>  b;
        b.set_pieceid_at(_Piece::get_id(PieceName::Q, PieceColor::W), wQ % _BoardSize, ((uint8_t)(wQ / _BoardSize)));
        b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::W), wK % _BoardSize, ((uint8_t)(wK / _BoardSize)));
        b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::B), bK % _BoardSize, ((uint8_t)(bK / _BoardSize)));
        b.set_color(PieceColor::W);

        if (ncall > 100) no_check = false;
        if (no_check)
        {
            if (b.is_in_check())
            {
                return get_random_position_KQK(no_check, ncall);
            }
            else
            {
                std::vector<_Move> m = b.generate_moves();
                size_t mv;
                if (b.can_capture_opposite_king(m, mv))
                    return get_random_position_KQK(no_check, ncall);
            }
        }
        return b;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    Board<PieceID, _BoardSize> Board<PieceID, _BoardSize>::get_random_position_KPK(bool no_check, uint64_t ncall)
    {
        ncall++;
        uint64_t n = 0;
        uint8_t wP = 0;
        uint8_t wK = 0;
        uint8_t bK = 0;
        while ((wK == bK) || (wK == wP) || (bK == wP) || (((uint8_t)(wP / _BoardSize)) == (_BoardSize - 1)) || (((uint8_t)(wP / _BoardSize)) == (0)))
        {
            // use std::mt19937_64 ...
            wP = (std::rand() % (_BoardSize*_BoardSize));
            wK = (std::rand() % (_BoardSize*_BoardSize));
            bK = (std::rand() % (_BoardSize*_BoardSize));
            n++;
            if (n > 100)
            {
                no_check = false;
                break;
            }
        }
        Board<PieceID, _BoardSize>  b;
        b.set_pieceid_at(_Piece::get_id(PieceName::P, PieceColor::W), wP % _BoardSize, ((uint8_t)(wP / _BoardSize)));
        b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::W), wK % _BoardSize, ((uint8_t)(wK / _BoardSize)));
        b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::B), bK % _BoardSize, ((uint8_t)(bK / _BoardSize)));
        b.set_color(PieceColor::W);

        if (ncall > 100) no_check = false;
        if (no_check)
        {
            if (b.is_in_check())
            {
                return get_random_position_KPK(no_check, ncall);
            }
            else
            {
                std::vector<_Move> m = b.generate_moves();
                size_t mv;
                if (b.can_capture_opposite_king(m, mv))
                    return get_random_position_KPK(no_check, ncall);
            }
        }
        return b;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    Board<PieceID, _BoardSize> Board<PieceID, _BoardSize>::get_random_position_KPKP(bool no_check, uint64_t ncall)
    {
        ncall++;
        uint64_t n = 0;
        uint8_t wP = 0;
        uint8_t wK = 0;
        uint8_t bK = 0;
        uint8_t bP = 0;
        while ( (wK == bK) || (wK == wP) || (wK == bP) || (bK == wP) || (bK == bP) || (wP == bP) ||
                (((uint8_t)(wP / _BoardSize)) == (_BoardSize - 1)) || (((uint8_t)(wP / _BoardSize)) == (0)) ||
                (((uint8_t)(bP / _BoardSize)) == (_BoardSize - 1)) || (((uint8_t)(bP / _BoardSize)) == (0)))
        {
            wP = (std::rand() % (_BoardSize*_BoardSize));
            wK = (std::rand() % (_BoardSize*_BoardSize));
            bK = (std::rand() % (_BoardSize*_BoardSize));
            bP = (std::rand() % (_BoardSize*_BoardSize));
            n++;
            if (n > 100)
            {
                no_check = false;
                break;
            }

        }
        Board<PieceID, _BoardSize>  b;
        b.set_pieceid_at(_Piece::get_id(PieceName::P, PieceColor::W), wP % _BoardSize, ((uint8_t)(wP / _BoardSize)));
        b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::W), wK % _BoardSize, ((uint8_t)(wK / _BoardSize)));
        b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::B), bK % _BoardSize, ((uint8_t)(bK / _BoardSize)));
        b.set_pieceid_at(_Piece::get_id(PieceName::P, PieceColor::B), bP % _BoardSize, ((uint8_t)(bP / _BoardSize)));
        b.set_color(PieceColor::W);

        if (ncall > 100) no_check = false;
        if (no_check)
        {
            if (b.is_in_check())
            {
                return get_random_position_KPKP(no_check, ncall);
            }
            else
            {
                std::vector<_Move> m = b.generate_moves();
                size_t mv;
                if (b.can_capture_opposite_king(m, mv))
                {
                    return get_random_position_KPKP(no_check, ncall);
                }
            }
        }
        return b;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Board<PieceID, _BoardSize>::is_last_move_promo() const
    {
        if (_history_moves.size() == 0) return false;
        _Move mv = last_history_move();
        if (mv.mu.context_extra == "Q")     // only Q for now..
            return true;
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Board<PieceID, _BoardSize>::is_last_move_capture() const
    {
        if (_history_moves.size() == 0) return false;
        _Move mv = last_history_move();
        if (mv.prev_dst_id != Piece<PieceID, _BoardSize>::empty_id())
            return true;
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Board<PieceID, _BoardSize>::is_last_move_pawn() const
    {
        if (_history_moves.size() == 0) return false;
        _Move mv = last_history_move();
        PieceID P_id = _Piece::get_id(PieceName::P, get_opposite_color());
        if (mv.prev_src_id == P_id)
            return true;
        return false;
    }
};
#endif
