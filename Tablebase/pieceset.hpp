#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// PieceSet - To keep track of W/B pieces and their combinations when capture/promo
//
//
#ifndef _AL_CHESS_TABLEBASE_PIECESET_HPP
#define _AL_CHESS_TABLEBASE_PIECESET_HPP

namespace chess
{
    // PieceSet
    template <typename PieceID, typename uint8_t _BoardSize>
    class PieceSet
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

    public:
        // EX: KQPPvKRPP - w_set is KQP(2), b_set is KRP(2)
        PieceSet(const std::vector<std::pair<PieceID, uint8_t>>& w_set, const std::vector<std::pair<PieceID, uint8_t>>& b_set);
        PieceSet(std::vector<const _Piece*>& v);
        PieceSet(std::vector<PieceID>& v);
        ~PieceSet() {  }

        bool is_valid()  const { return _is_valid; }
        std::vector<std::pair<PieceID, uint8_t>> wset() const { return _wset; }
        std::vector<std::pair<PieceID, uint8_t>> bset() const { return _bset; }
        const std::vector<std::pair<PieceID, uint8_t>>& wset_ref() const  { return _wset; }
        const std::vector<std::pair<PieceID, uint8_t>>& bset_ref() const  { return _bset; }

        bool is_same(const PieceSet<PieceID, _BoardSize>& v) const;
        std::string name(PieceColor color_toplay) const;
        uint16_t find_rank_index(size_t rank, PieceID& ret_id, uint16_t& ret_count, uint16_t& ret_instance)  const;
        uint16_t count_one_piece(PieceColor c, PieceID id)  const;
        uint16_t count_all_piece(PieceColor c) const;
        uint16_t count_one_piecename(PieceColor c, PieceName n)  const;
        void add_promo_capture(PieceColor color, PieceName n);

        void collapse_to_one_piece();
        std::vector<std::pair<PieceID, uint8_t>> capture_only_set(PieceColor c, size_t children_index) const;

        // All combination of 1 piece less either white or black + promo
        std::vector<PieceSet> get_all_child() const;
        bool find_all_child_index(PieceColor c, const _Board& pos, bool isPromo, bool isCapture, size_t& ret_all_child_index) const;

        std::map<size_t, STRUCT_PIECE_RANK<PieceID, _BoardSize>>& map_piece_rank();

        std::vector<std::pair<PieceID, uint8_t>> merge() const;
        static std::vector<PieceID> to_pieces(const std::vector<std::pair<PieceID, uint8_t>>& v);
        static std::vector<PieceID> ps_to_pieces(const PieceSet<PieceID, _BoardSize>& v);
        static std::vector<std::pair<PieceID, uint8_t>> to_set(const std::vector<PieceID>& v);
        static std::vector<std::pair<PieceID, uint8_t>> to_set(const std::vector<PieceID>& v, PieceColor c);
        static std::vector<std::pair<PieceID, uint8_t>> to_set_p(std::vector<const _Piece*>& v, PieceColor c);
        static std::vector<std::pair<PieceID, uint8_t>> reverse_color_set(std::vector<std::pair<PieceID, uint8_t>>& v);

        void make_all_child_set() const;
        const PieceSet<PieceID, _BoardSize>& all_child_set_at(size_t idx) const {return _all_child_set[idx];}

        bool PieceSet<PieceID, _BoardSize>::is_sym() const;
        bool PieceSet<PieceID, _BoardSize>::is_collapse_to_one_piece() const;

    protected:
        bool _is_valid;
        std::vector<std::pair<PieceID, uint8_t>>                _wset;          // white pieces and count
        std::vector<std::pair<PieceID, uint8_t>>                _bset;          // black pieces and count
        std::vector<std::vector<std::pair<PieceID, uint8_t>>>   _wchildren;     // all white combination with 1 piece less + promo (not both)
        std::vector<std::vector<std::pair<PieceID, uint8_t>>>   _bchildren;     // all black combination with 1 piece less + promo (not both)
        mutable std::vector<PieceSet<PieceID, _BoardSize>>      _all_child_set; 

        mutable std::map<size_t, STRUCT_PIECE_RANK<PieceID, _BoardSize>> _map_piece_rank;

        bool validate();
        void remove_one_piecename(PieceColor c, PieceName n);
        void add_one_Q(PieceColor c);
        void make_capture_only_set();
        static void add_to_v(PieceID id, std::vector<std::pair<PieceID, uint8_t>>& v);
    };

    // ct
    template <typename PieceID, typename uint8_t _BoardSize>
    PieceSet<PieceID, _BoardSize>::PieceSet(const  std::vector<std::pair<PieceID, uint8_t>>& w_set, const  std::vector<std::pair<PieceID, uint8_t>>& b_set)
            : _is_valid(true), _wset(w_set), _bset(b_set)
    {
        if (validate())
            make_capture_only_set();
    }

    // ct
    template <typename PieceID, typename uint8_t _BoardSize>
    PieceSet<PieceID, _BoardSize>::PieceSet(std::vector<const _Piece*>& v)
        : _is_valid(true), 
        _wset(PieceSet<PieceID, _BoardSize>::to_set_p(v, PieceColor::W)),
        _bset(PieceSet<PieceID, _BoardSize>::to_set_p(v, PieceColor::B))
    {
        if (validate())
            make_capture_only_set();
    }

    // ct
    template <typename PieceID, typename uint8_t _BoardSize>
    PieceSet<PieceID, _BoardSize>::PieceSet(std::vector<PieceID>& v)
        : _is_valid(true),
        _wset(PieceSet<PieceID, _BoardSize>::to_set(v, PieceColor::W)),
        _bset(PieceSet<PieceID, _BoardSize>::to_set(v, PieceColor::B))
    {
        if (validate())
            make_capture_only_set();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::string  PieceSet<PieceID, _BoardSize>::name(PieceColor color_toplay)  const
    {
        std::vector<PieceID> v = PieceSet<PieceID, _BoardSize>::ps_to_pieces(*this);
        return TB_Manager<PieceID, _BoardSize>::instance()->name_pieces(v, color_toplay);
    }

    // find_rank_index
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t PieceSet<PieceID, _BoardSize>::find_rank_index(size_t rank, PieceID& ret_pieceid, uint16_t& ret_count, uint16_t& ret_instance)  const
    {
        uint16_t n = 0; ret_count = 0;

        for (uint16_t i = 0; i < _wset.size(); i++)
        {
            for (uint16_t j = 0; j < _wset[i].second; j++)
            {
                if (n == rank)
                {
                    ret_count   = _wset[i].second;
                    ret_pieceid = _wset[i].first;
                    ret_instance = j;
                    return i;
                }
                n++;
            }
        }

        for (uint16_t i = 0; i < _bset.size(); i++)
        {
            for (uint16_t j = 0; j < _bset[i].second; j++)
            {
                if (n == rank)
                {
                    ret_count   = _bset[i].second;
                    ret_pieceid = _bset[i].first;
                    ret_instance = j;
                    return i;
                }
                n++;
            }
        }

        return -1; // ret_count is 0
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t PieceSet<PieceID, _BoardSize>::count_one_piece(PieceColor c, PieceID id)  const
    {
        uint16_t n = 0;
        if (c == PieceColor::W)
        {
            for (size_t i = 0; i < _wset.size(); i++)
                if (_wset[i].first == id)
                    n += _wset[i].second;
        }
        else if (c == PieceColor::B)
        {
            for (size_t i = 0; i < _bset.size(); i++)
                if (_bset[i].first == id)
                    n += _bset[i].second;
        }
        return n;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint16_t PieceSet<PieceID, _BoardSize>::count_one_piecename(PieceColor c, PieceName name)  const
    {
        uint16_t n = 0;
        if (c == PieceColor::W)
        {
            for (size_t i = 0; i < _wset.size(); i++)
                if (_wset[i].first == Piece<PieceID, _BoardSize>::get_id(name, c))
                    n += _wset[i].second;
        }
        else if (c == PieceColor::B)
        {
            for (size_t i = 0; i < _bset.size(); i++)
                if (_bset[i].first == Piece<PieceID, _BoardSize>::get_id(name, c))
                    n += _bset[i].second;
        }
        return n;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void PieceSet<PieceID, _BoardSize>::remove_one_piecename(PieceColor c, PieceName n)
    {
        // not an atomic operation
        if (c == PieceColor::W)
        {
            for (size_t i = 0; i < _wset.size(); i++)
            {
                if (_wset[i].first == Piece<PieceID, _BoardSize>::get_id(n, c))
                {
                    if (_wset[i].second > 0)
                    {
                        _wset[i].second--;
                    }
                }
            }
        }
        else if (c == PieceColor::B)
        {
            for (size_t i = 0; i < _bset.size(); i++)
            {
                if (_bset[i].first == Piece<PieceID, _BoardSize>::get_id(n, c))
                {
                    if (_bset[i].second > 0)
                    {
                        _bset[i].second--;
                    }
                }
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void PieceSet<PieceID, _BoardSize>::add_one_Q(PieceColor c)
    {
        // not an atomic operation
        if (c == PieceColor::W)
        {
            for (size_t i = 0; i < _wset.size(); i++)
            {
                if (_wset[i].first == Piece<PieceID, _BoardSize>::get_id(PieceName::Q, c))
                {
                    _wset[i].second++;
                    return;
                }
            }

            // new Q entry required
            std::pair<PieceID, uint8_t> pair_piece_count;
            pair_piece_count.first = Piece<PieceID, _BoardSize>::get_id(PieceName::Q, PieceColor::W);
            pair_piece_count.second = 1;
            _wset.push_back(pair_piece_count);
            
            // re-sort
            std::vector<PieceID> v = ps_to_pieces(*this);
            sorter_less_pieces<PieceID, _BoardSize> srt;
            std::sort(v.begin(), v.end(), srt);

            _wset = to_set(v, PieceColor::W);
            _bset = to_set(v, PieceColor::B);
        }
        else if (c == PieceColor::B)
        {
            for (size_t i = 0; i < _bset.size(); i++)
            {
                if (_bset[i].first == Piece<PieceID, _BoardSize>::get_id(PieceName::Q, c))
                {
                    _bset[i].second++;
                    return;
                }
            }

            // new entry
            std::pair<PieceID, uint8_t> pair_piece_count;
            pair_piece_count.first = Piece<PieceID, _BoardSize>::get_id(PieceName::Q, PieceColor::B);
            pair_piece_count.second = 1;
            _bset.push_back(pair_piece_count);

            // reorder
            std::vector<PieceID> v = ps_to_pieces(*this);
            sorter_less_pieces<PieceID, _BoardSize> srt;
            std::sort(v.begin(), v.end(), srt);

            _wset = to_set(v, PieceColor::W);
            _bset = to_set(v, PieceColor::B);
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize> 
    inline uint16_t PieceSet<PieceID, _BoardSize>::count_all_piece(PieceColor c)  const
    {
        uint16_t n = 0;
        if (c == PieceColor::W)
        {
            for (size_t i = 0; i < _wset.size(); i++)
                n += _wset[i].second;
        }
        else if (c == PieceColor::B)
        {
            for (size_t i = 0; i < _bset.size(); i++)
                n += _bset[i].second;
        }
        return n;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::vector<std::pair<PieceID, uint8_t>> PieceSet<PieceID, _BoardSize>::capture_only_set(PieceColor c, size_t children_index)  const
    {
        if (c == PieceColor::W)
        {
            if (children_index < _wchildren.size())
                return _wchildren[children_index];
        }
        else if (c == PieceColor::B)
        {
            if (children_index < _bchildren.size())
                return _bchildren[children_index];
        }
        std::vector<std::pair<PieceID, uint8_t>> v;
        return v;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool PieceSet<PieceID, _BoardSize>::validate()
    {
        // todo check sorting...
        return _is_valid;
    }

    // make_children
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void PieceSet<PieceID, _BoardSize>::make_capture_only_set()
    {
        std::vector<std::pair<PieceID, uint8_t>> workset;
        std::pair<PieceID, uint8_t>              pair_piece_count;

        _wchildren.clear();
        for (size_t z = 0; z < _wset.size(); z++)
        {
            workset = _wset;
            pair_piece_count = workset[z];
            if (pair_piece_count.second > 0)
            {
                // Capture - remove 1 piece at ith position
                pair_piece_count.second--;
                workset[z] = pair_piece_count;
                _wchildren.push_back(workset);
            }
        }

        _bchildren.clear();
        for (size_t z = 0; z < _bset.size(); z++)
        {
            workset = _bset;
            pair_piece_count = workset[z];
            if (pair_piece_count.second > 0)
            {
                // Capture remove 1 ith piece
                pair_piece_count.second--;
                workset[z] = pair_piece_count;
                _bchildren.push_back(workset);
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    void  PieceSet<PieceID, _BoardSize>::make_all_child_set() const
    {
        if (_all_child_set.size() == 0)
        {
            // external lock - see make_all_child_TBH
            if (_all_child_set.size() == 0)
            {
                _all_child_set = get_all_child();
            }
        }
    }

    // called by TBH<PieceID, _BoardSize>::find_score_children_tb (THREAD SPLITTING) - initiated by TBH_N::build() (main thread)
    template <typename PieceID, typename uint8_t _BoardSize>
    bool PieceSet<PieceID, _BoardSize>::find_all_child_index(PieceColor color_pos, const _Board& pos, bool isPromo, bool isCapture, size_t& ret_all_child_index) const
    {
        uint16_t cnt_piece_c;
        uint16_t cnt_piece_c_oppo;
        uint16_t cnt_piece_board_c;
        uint16_t cnt_piece_board_c_oppo;
        PieceColor c_oppo = (color_pos == PieceColor::W) ? PieceColor::B : PieceColor::W;

        cnt_piece_board_c       = pos.cnt_all_piece(color_pos);
        cnt_piece_board_c_oppo  = pos.cnt_all_piece(c_oppo);

        bool child_ok;

        if (_all_child_set.size() == 0)
        {
            // external lock - see make_all_child_TBH
            make_all_child_set();
        }

        for (size_t j = 0; j < _all_child_set.size(); j++)
        {
            child_ok = true;
            const PieceSet& child_set = _all_child_set[j];

            cnt_piece_c         = child_set.count_all_piece(color_pos);
            cnt_piece_c_oppo    = child_set.count_all_piece(c_oppo);

            if (cnt_piece_c         != cnt_piece_board_c)       { child_ok = false; continue; }
            if (cnt_piece_c_oppo    != cnt_piece_board_c_oppo)  { child_ok = false; continue; }

            for (size_t i = 0; i < child_set.wset_ref().size(); i++)
            {
                if (pos.cnt_piece(child_set.wset_ref()[i].first) != child_set.wset_ref()[i].second) { child_ok = false; break; }
            }
            if (child_ok == false) continue;

            for (size_t i = 0; i < child_set.bset_ref().size(); i++)
            {
                if (pos.cnt_piece(child_set.bset_ref()[i].first) != child_set.bset_ref()[i].second) { child_ok = false; break; }
            }
            if (child_ok == false) continue;

            if (child_ok)
            {
                ret_all_child_index = j;
                return true;
            }
        }
        return false;
    }


    // All combination of 1 piece capture/promo, either white or black
    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::vector<PieceSet<PieceID, _BoardSize>>  PieceSet<PieceID, _BoardSize>::get_all_child() const
    {
        // Reject if not at least wK or bK

        // 1 piece capture
        std::vector<PieceSet<PieceID, _BoardSize>> v;
        for (size_t i = _wchildren.size() ; i > 0; i--)
        {
            PieceSet<PieceID, _BoardSize> ps({ capture_only_set(PieceColor::W, i - 1), _bset });
            if ((ps.count_one_piecename(PieceColor::W, PieceName::K)==1) || (ps.count_one_piecename(PieceColor::B, PieceName::K) == 1))
                v.push_back(ps);
        }
        for (size_t i = _bchildren.size() ; i > 0; i--)
        {
            PieceSet<PieceID, _BoardSize> ps({ _wset, capture_only_set(PieceColor::B, i - 1) });
            if ((ps.count_one_piecename(PieceColor::W, PieceName::K) == 1) || (ps.count_one_piecename(PieceColor::B, PieceName::K) == 1))
                v.push_back(ps);
        }

        // 1 pawn promo
        for (int i = 0; i<2; i++)
        {
            PieceColor color = (i == 0) ? PieceColor::W : PieceColor::B;
            if (count_one_piecename(color, PieceName::P) > 0)
            {
                PieceSet<PieceID, _BoardSize> ps(_wset, _bset);
                ps.add_one_Q(color);
                ps.remove_one_piecename(color, PieceName::P);
                if ((ps.count_one_piecename(PieceColor::W, PieceName::K) == 1) || (ps.count_one_piecename(PieceColor::B, PieceName::K) == 1))
                {
                    ps.make_capture_only_set();
                    v.push_back(ps);
                }
            }
        }

        // simultaneous promo+capture
        for(int i=0;i<2;i++)
        {
            PieceColor color = (i == 0) ? PieceColor::W : PieceColor::B;
            PieceColor c_oppo = (color == PieceColor::W) ? PieceColor::B : PieceColor::W;
            if (count_one_piecename(color, PieceName::P) > 0)
            {
                if (count_one_piecename(c_oppo, PieceName::K) > 0)
                {
                    PieceSet<PieceID, _BoardSize> ps(_wset, _bset);
                    ps.add_promo_capture(color, PieceName::K);
                    if ((ps.count_one_piecename(PieceColor::W, PieceName::K) == 1) || (ps.count_one_piecename(PieceColor::B, PieceName::K) == 1))
                    {
                        ps.make_capture_only_set();
                        v.push_back(ps);
                    }
                }
                if (count_one_piecename(c_oppo, PieceName::Q) > 0)
                {
                    PieceSet<PieceID, _BoardSize> ps(_wset, _bset);
                    ps.add_promo_capture(color, PieceName::Q);
                    if ((ps.count_one_piecename(PieceColor::W, PieceName::K) == 1) || (ps.count_one_piecename(PieceColor::B, PieceName::K) == 1))
                    {
                        ps.make_capture_only_set();
                        v.push_back(ps);
                    }
                }
                if (count_one_piecename(c_oppo, PieceName::P) > 0)
                {
                    PieceSet<PieceID, _BoardSize> ps(_wset, _bset);
                    ps.add_promo_capture(color, PieceName::P);
                    if ((ps.count_one_piecename(PieceColor::W, PieceName::K) == 1) || (ps.count_one_piecename(PieceColor::B, PieceName::K) == 1))
                    {
                        ps.make_capture_only_set();
                        v.push_back(ps);
                    }
                }
            }
        }
        return v;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void PieceSet<PieceID, _BoardSize>::add_promo_capture(PieceColor color, PieceName name)
    {
        PieceColor c_oppo = (color == PieceColor::W) ? PieceColor::B : PieceColor::W;
        add_one_Q(color);
        remove_one_piecename(color, PieceName::P);
        remove_one_piecename(c_oppo, name);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<std::pair<PieceID, uint8_t>> PieceSet<PieceID, _BoardSize>::merge() const
    {
        std::vector<std::pair<PieceID, uint8_t>> v = _wset;
        for (size_t i = 0; i < _bset.size(); i++)
        {
            v.push_back(_bset[i]);
        }
        return v;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<PieceID> PieceSet<PieceID, _BoardSize>::to_pieces(const std::vector<std::pair<PieceID, uint8_t>>& v)
    {
        std::vector<PieceID> r;
        for (size_t i = 0; i < v.size(); i++)
        {
            for (size_t j = 0; j < v[i].second; j++)
                r.push_back(v[i].first);
        }
        return r;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<PieceID> PieceSet<PieceID, _BoardSize>::ps_to_pieces(const PieceSet<PieceID, _BoardSize>& v)
    {
        std::vector<PieceID> r;
        for (size_t i = 0; i < v._wset.size(); i++)
        {
            for (size_t j = 0; j < v._wset[i].second; j++)
                r.push_back(v._wset[i].first);
        }
        for (size_t i = 0; i < v._bset.size(); i++)
        {
            for (size_t j = 0; j < v._bset[i].second; j++)
                r.push_back(v._bset[i].first);
        }
        return r;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    void PieceSet<PieceID, _BoardSize>::add_to_v(PieceID id, std::vector<std::pair<PieceID, uint8_t>>& v)
    {
        uint8_t n = 0;
        bool found = false;
        for (size_t i = 0; i < v.size(); i++)
        {
            if (v[i].first == id)
            { 
                n += v[i].second;
                found = true;
            }
        }
        if (found == false) v.push_back(std::pair<PieceID, uint8_t>({ id, 1 }));
        else
        {
            for (size_t i = 0; i < v.size(); i++)
            {
                if (v[i].first == id)
                {
                    v[i].second = n+1;
                    break;
                }
            }
        }
    }
    
    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<std::pair<PieceID, uint8_t>> PieceSet<PieceID, _BoardSize>::to_set(const std::vector<PieceID>& v)
    {
        std::vector<std::pair<PieceID, uint8_t>> r;
        for (size_t i = 0; i < v.size(); i++)
        {
            add_to_v(v[i], r);
        }
        return r;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<std::pair<PieceID, uint8_t>> PieceSet<PieceID, _BoardSize>::to_set(const std::vector<PieceID>& v, PieceColor c)
    {
        std::vector<std::pair<PieceID, uint8_t>> r;
        for (size_t i = 0; i < v.size(); i++)
        {
            if (_Piece::get(v[i])->get_color() == c)
                add_to_v(v[i], r);
        }
        return r;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<std::pair<PieceID, uint8_t>> PieceSet<PieceID, _BoardSize>::reverse_color_set(std::vector<std::pair<PieceID, uint8_t>>& v)
    {
        PieceColor c;
        for (size_t i = 0; i < v.size(); i++)
        {
            c = _Piece::get(v[i].first)->get_color();
            v[i].first = _Piece::get_id(_Piece::get(v[i].first)->get_name(), (c == PieceColor::W) ? PieceColor::B : PieceColor::W );
        }
        return v;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<std::pair<PieceID, uint8_t>> PieceSet<PieceID, _BoardSize>::to_set_p(std::vector<const _Piece*>& v, PieceColor c)
    {
        std::vector<std::pair<PieceID, uint8_t>> r;
        for (size_t i = 0; i < v.size(); i++)
        {
            if (v[i]->get_color() == c)
                add_to_v(v[i]->get_id(), r);
        }
        return r;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    void PieceSet<PieceID, _BoardSize>::collapse_to_one_piece()
    {
        PieceID WKid = _Piece::get_id(PieceName::K, PieceColor::W);
        PieceID BKid = _Piece::get_id(PieceName::K, PieceColor::B);
        bool w_king = false;
        bool b_king = false;

        for (size_t i = 0; i < _wset.size(); i++)
        {
            if ((_wset[i].first == WKid) && (_wset[i].second >= 1))
            {
                _wset[i].second = 1;
                w_king = true;
                break;
            }
        }
        for (size_t i = 0; i < _bset.size(); i++)
        {
            if ((_bset[i].first == BKid) && (_bset[i].second >= 1))
            {
                _bset[i].second = 1;
                b_king = true;
                break;
            }
        }

        if (w_king)
        {
            for (size_t i = 0; i < _wset.size(); i++)
            {
                if ((_wset[i].first != WKid) && (_wset[i].second >= 1))
                {
                    _wset[i].second = 0;
                }
            }
            for (size_t i = 0; i < _bset.size(); i++)
            {
                 _bset[i].second = 0;
            }
        }
        else if (b_king)
        {
            for (size_t i = 0; i < _bset.size(); i++)
            {
                if ((_bset[i].first != BKid) && (_bset[i].second >= 1))
                {
                    _bset[i].second = 0;
                }
            }
            for (size_t i = 0; i < _wset.size(); i++)
            {
                _wset[i].second = 0;
            }
        }
        else
        {
            bool found = false;
            for (size_t i = 0; i < _wset.size(); i++)
            {
                if ((!found) && (_wset[i].second >= 1))
                {
                    _wset[i].second = 1;
                    found = true;
                }
                else if ((found) && (_wset[i].second > 0))
                {
                    _wset[i].second = 0;
                }
            }

            for (size_t i = 0; i < _bset.size(); i++)
            {
                if ((!found) && (_bset[i].second >= 1))
                {
                    _bset[i].second = 1;
                    found = true;
                }
                else if ((found) && (_bset[i].second > 0))
                {
                    _bset[i].second = 0;
                }
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool PieceSet<PieceID, _BoardSize>::is_same(const PieceSet<PieceID, _BoardSize>& v) const
    {
        if (count_all_piece(PieceColor::W) != v.count_all_piece(PieceColor::W)) return false;
        if (count_all_piece(PieceColor::B) != v.count_all_piece(PieceColor::B)) return false;

        for (size_t i = 0; i < _wset.size(); i++)
        {
            if (_wset[i].second > 0)
            {
                if (_wset[i].second != v.count_one_piece(PieceColor::W, _wset[i].first))
                    return false;
            }
        }

        for (size_t i = 0; i < _bset.size(); i++)
        {
            if (_bset[i].second > 0)
            {
                if (_bset[i].second != v.count_one_piece(PieceColor::B, _bset[i].first))
                    return false;
            }
        }

        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::map<size_t, STRUCT_PIECE_RANK<PieceID, _BoardSize>>& PieceSet<PieceID, _BoardSize>::map_piece_rank()
    {
        if (_map_piece_rank.size() == 0)
        {
            std::vector<PieceID> v_id = PieceSet<PieceID, _BoardSize>::ps_to_pieces(*this);
            for (size_t z = 0; z < v_id.size(); z++)
            {
                STRUCT_PIECE_RANK<PieceID, _BoardSize> r;
                r.rank_ret_id = this->find_rank_index(z, r.ret_id, r.ret_count, r.ret_instance);
                if (r.ret_count > 0)
                {
                    _map_piece_rank[z] = r;
                }
            }
        }
        return _map_piece_rank;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool PieceSet<PieceID, _BoardSize>::is_sym() const
    {
        uint16_t nw;
        uint16_t nb;
        uint16_t nwK;
        uint16_t nbK;
        nw = this->count_all_piece(PieceColor::W);
        nb = this->count_all_piece(PieceColor::B);
        nwK = this->count_one_piecename(PieceColor::W, PieceName::K);
        nbK = this->count_one_piecename(PieceColor::B, PieceName::K);

        bool is_sym = false;
        if ((nw < nb) && (nw > 0) && (nb > 0) && (nbK > 0) && (nwK > 0))
        {
            is_sym = true;
        }
        return is_sym;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool PieceSet<PieceID, _BoardSize>::is_collapse_to_one_piece() const
    {
        uint16_t nw;
        uint16_t nb;
        uint16_t nwK;
        uint16_t nbK;
        nw = this->count_all_piece(PieceColor::W);
        nb = this->count_all_piece(PieceColor::B);
        nwK = this->count_one_piecename(PieceColor::W, PieceName::K);
        nbK = this->count_one_piecename(PieceColor::B, PieceName::K);

        if ((nw < nb) && (nw > 0) && (nb > 0) && (nbK > 0) && (nwK > 0))
        {
        }
        else if ((nb == 0) || (nbK == 0) || (nw == 0) || (nwK == 0))
        {
            return true;            
        }
        return false;
    }
};
#endif

