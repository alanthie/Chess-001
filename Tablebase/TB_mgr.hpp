#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TB_Manager : Track TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TB_MANAGER_HPP
#define _AL_CHESS_TABLEBASE_TB_MANAGER_HPP

namespace chess
{
    // TB_Manager
    template <typename PieceID, typename uint8_t _BoardSize>
    class TB_Manager
    {
        using _Piece = Piece<PieceID, _BoardSize>;

    private:
        TB_Manager() { _mutex = new std::recursive_mutex; }
        TB_Manager(const TB_Manager&)               = delete;
        TB_Manager & operator=(const TB_Manager &)  = delete;
        TB_Manager(TB_Manager&&)                    = delete;

    public:
        ~TB_Manager()
        {
            if (_instance.operator bool())
            {
                _instance.release();
            }
            //...insure no more lock
            delete _mutex;
        }
        static const TB_Manager* instance();
        void clear() const;

        bool add_N(  const std::string& name, TablebaseBase<PieceID, _BoardSize>* tb) const;
        bool add_sym(const std::string& name, TablebaseBase<PieceID, _BoardSize>* tb) const;
 
        TablebaseBase<PieceID, _BoardSize>*  find_N(  const std::string& name) const; 
        TablebaseBase<PieceID, _BoardSize>*  find_sym(const std::string& name) const;
        TablebaseBase<PieceID, _BoardSize>*  find_N(uint8_t N, const std::string& name) const { return find_N(name); }

        std::vector<STRUCT_TBH<PieceID, _BoardSize>> make_all_child_TBH(const PieceSet<PieceID, _BoardSize>& set, TBH_IO_MODE iomode, TBH_OPTION option) const;

    public:
        static std::string name_pieces(const std::vector<PieceID>& v, PieceColor color_to_play)
        {
            // assume sorted
            std::string str_pieces;
            if (color_to_play == PieceColor::W) str_pieces = "W_";
            else if (color_to_play == PieceColor::B) str_pieces = "B_";
            else if (color_to_play == PieceColor::none) str_pieces = "";
            for (auto& vv : v) str_pieces += Piece<PieceID, _BoardSize>::to_str(vv, false); // Filesystem not case sensitive!
            return str_pieces;
        }

        static uint64_t _TB_MAX_SIZE;       // TB size limit for RAM
        static uint16_t _TB_MINMAX_DEPTH;   // Partial TB iterative minmax search limit
        static uint8_t  _TB_MAX_DTC;
        static uint64_t new_TB_setup_size(uint64_t boardsize, uint64_t N) { return std::min<uint64_t>(_TB_MAX_SIZE, TB_FULL_SIZE(boardsize, N)); }

    private:
        // owner... unique_ptr/weak_ptr
        mutable std::map<std::string, TablebaseBase<PieceID, _BoardSize>*> _tbN;
        mutable std::map<std::string, TablebaseBase<PieceID, _BoardSize>*> _tbsym;

        static std::unique_ptr<TB_Manager>  _instance;
        mutable std::recursive_mutex*       _mutex;

        bool check_tbh_exist(std::vector<STRUCT_TBH<PieceID, _BoardSize>>& v, TB_TYPE t, PieceSet<PieceID, _BoardSize>& ps, size_t& ret_idx) const;
    };

    //_instance
    template <typename PieceID, typename uint8_t _BoardSize>
    std::unique_ptr<TB_Manager<PieceID, _BoardSize>>
    TB_Manager<PieceID, _BoardSize>::_instance = nullptr;

    // _TB_MAX_SIZE
    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TB_Manager<PieceID, _BoardSize>::_TB_MAX_SIZE = 8000000;

    // _TB_MINMAX_DEPTH
    template <typename PieceID, typename uint8_t _BoardSize>
    uint16_t TB_Manager<PieceID, _BoardSize>::_TB_MINMAX_DEPTH = 2;

    // _TB_MAX_DTC
    template <typename PieceID, typename uint8_t _BoardSize>
    uint8_t TB_Manager<PieceID, _BoardSize>::_TB_MAX_DTC = 255;   //0..255;

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TB_Manager<PieceID, _BoardSize>::clear() const
    {
        _tbN.clear();
        _tbsym.clear();
    }

    // add_sym
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TB_Manager<PieceID, _BoardSize>::add_sym(const std::string& name, TablebaseBase<PieceID, _BoardSize>* tb) const
    {
        if (_instance == nullptr) return false;

        // LOCK
        std::lock_guard<std::recursive_mutex> lock(*_mutex);

        auto iter = _tbsym.find(name);
        if (iter == _tbsym.end())
        {
            _tbsym[name] = tb;
        }
        return true;
    }

    // add_N
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TB_Manager<PieceID, _BoardSize>::add_N(const std::string& name, TablebaseBase<PieceID, _BoardSize>* tb) const
    {
        if (_instance == nullptr) return false;

        // LOCK
        std::lock_guard<std::recursive_mutex> lock(*_mutex);

        auto iter = _tbN.find(name);
        if (iter == _tbN.end())
        {
            _tbN[name] = tb;
        }
        return true;
    }

    // find_sym
    template <typename PieceID, typename uint8_t _BoardSize>
    inline TablebaseBase<PieceID, _BoardSize>*  TB_Manager<PieceID, _BoardSize>::find_sym(const std::string& name) const
    {
        if (_instance == nullptr) return nullptr;

        auto iter = _tbsym.find(name);
        if (iter != _tbsym.end())
        {
            return _tbsym[name];
        }
        return nullptr;
    }

    // find_N
    template <typename PieceID, typename uint8_t _BoardSize>
    inline TablebaseBase<PieceID, _BoardSize>*  TB_Manager<PieceID, _BoardSize>::find_N(const std::string& name) const
    {
        if (_instance == nullptr) return nullptr;

        auto iter = _tbN.find(name);
        if (iter != _tbN.end())
        {
            return _tbN[name];
        }
        return nullptr;
    }

    // instance()
    template <typename PieceID, typename uint8_t _BoardSize>
    const TB_Manager<PieceID, _BoardSize>* 
    TB_Manager<PieceID, _BoardSize>::instance()
    {
        if (_instance == nullptr)
        {
            _instance = std::unique_ptr<TB_Manager>(new TB_Manager);
            return _instance.get();
        }
        return  _instance.get();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TB_Manager<PieceID, _BoardSize>::check_tbh_exist(std::vector<STRUCT_TBH<PieceID, _BoardSize>>& v, TB_TYPE t, PieceSet<PieceID, _BoardSize>& ps, size_t& ret_idx) const
    {
        ret_idx = 0;
        for (size_t k = 0; k < v.size(); k++)
        {
            if ((v[k]._t == t) && (ps.is_same(v[k]._ps)))
            {
                ret_idx = k;
                return true;
            }
        }
        return false;
    }

    // make_all_child_TBH() - Create all TB children handler of a TB - called on new TBH_N()
    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::vector<STRUCT_TBH<PieceID, _BoardSize>> TB_Manager<PieceID, _BoardSize>::make_all_child_TBH(const PieceSet<PieceID, _BoardSize>& set, TBH_IO_MODE iomode, TBH_OPTION option) const
    {
        std::vector<STRUCT_TBH<PieceID, _BoardSize>> v;
        uint16_t nw; 
        uint16_t nb;
        uint16_t nwK;
        uint16_t nbK;

        // LOCK
        std::lock_guard<std::recursive_mutex> lock(*_mutex);

        set.make_all_child_set(); // BUILD it (while in LOCK)
        std::vector<PieceSet<PieceID, _BoardSize>> vz = set.get_all_child();

        TBH_IO_MODE childmode = iomode;
        if (iomode == TBH_IO_MODE::tb_and_child) childmode = TBH_IO_MODE::tb_only;

        for (size_t i = 0; i < vz.size(); i++)
        {
            nw = vz[i].count_all_piece(PieceColor::W);
            nb = vz[i].count_all_piece(PieceColor::B);
            nwK = vz[i].count_one_piecename(PieceColor::W, PieceName::K);
            nbK = vz[i].count_one_piecename(PieceColor::B, PieceName::K);
            
            STRUCT_TBH<PieceID, _BoardSize> struct_tbh(vz[i]);
            struct_tbh._t = TB_TYPE::tb_unknown;
            struct_tbh._nw = nw;
            struct_tbh._nb = nb;
            struct_tbh._tbh = nullptr;

            if ((nb == 0) || (nbK == 0)) 
            { 
                PieceSet<PieceID, _BoardSize> ps(vz[i].wset(), vz[i].bset()); ps.collapse_to_one_piece();
                struct_tbh._t = TB_TYPE::tb_Xv0;
                size_t ret_idx;
                bool found = check_tbh_exist(v, struct_tbh._t, ps, ret_idx);
                if (found == false)
                {
                    struct_tbh._tbh = TBH_Manager<PieceID, _BoardSize>::instance()->add(TB_TYPE::tb_Xv0, ps, childmode, option);
                    v.push_back(struct_tbh);
                }
            } 
            else if ((nw == 0) || (nwK == 0))
            { 
                PieceSet<PieceID, _BoardSize> ps(vz[i].wset(), vz[i].bset()); ps.collapse_to_one_piece();
                struct_tbh._t = TB_TYPE::tb_0vX;
                size_t ret_idx; 
                bool found = check_tbh_exist(v, struct_tbh._t, ps, ret_idx);
                if (found == false)
                {
                    struct_tbh._tbh = TBH_Manager<PieceID, _BoardSize>::instance()->add(TB_TYPE::tb_0vX, ps, childmode, option);
                    v.push_back(struct_tbh);
                }
            }
            else if (nw > nb) // normal
            {
                struct_tbh._t = tb_type_NM(nw, nb);
                size_t ret_idx; 
                bool found = check_tbh_exist(v, struct_tbh._t, vz[i], ret_idx);
                if (found == false)
                {
                    struct_tbh._tbh = TBH_Manager<PieceID, _BoardSize>::instance()->add(struct_tbh._t, vz[i], childmode, option);
                    v.push_back(struct_tbh);
                }
            }
            else if (nw == nb) // promo NvN
            {
                struct_tbh._t = tb_type_NM(nw, nb);
                size_t ret_idx; 
                bool found = check_tbh_exist(v, struct_tbh._t, vz[i], ret_idx);
                if (found == false)
                {
                    struct_tbh._tbh = TBH_Manager<PieceID, _BoardSize>::instance()->add(struct_tbh._t, vz[i], childmode, option);
                    v.push_back(struct_tbh);
                }
            }
            else if (nw < nb) // sym
            {
                // Find master TB
                struct_tbh._t = tb_type_NM(nb, nw); // reversed
                PieceSet<PieceID, _BoardSize> copy(vz[i].wset(), vz[i].bset());
                PieceSet<PieceID, _BoardSize> r({ PieceSet<PieceID, _BoardSize>::reverse_color_set(copy.bset()), PieceSet<PieceID, _BoardSize>::reverse_color_set(copy.wset()) });
                size_t ret_idx; 
                bool found = check_tbh_exist(v, struct_tbh._t, r, ret_idx);
                if (found == false)
                {
                    struct_tbh._tbh = TBH_Manager<PieceID, _BoardSize>::instance()->add(struct_tbh._t, r, childmode, option);
                    assert(r.count_all_piece(PieceColor::W) >= r.count_all_piece(PieceColor::B));
                    struct_tbh._ps = r;
                    struct_tbh._nw = nb;    // reversed
                    struct_tbh._nb = nw;
                    v.push_back(struct_tbh);

                    assert(struct_tbh._tbh != nullptr);
                }
                else
                {
                    struct_tbh._tbh = v[ret_idx]._tbh;
                }

                // sym TB
                STRUCT_TBH<PieceID, _BoardSize> struct_tbh_sym(vz[i]);
                // check_tbh_exist...
                struct_tbh_sym._t = tb_type_NM(nw, nb);
                struct_tbh_sym._nw = nw;
                struct_tbh_sym._nb = nb;
                struct_tbh_sym._tbh = TBH_Manager<PieceID, _BoardSize>::instance()->add_sym(struct_tbh_sym._t, struct_tbh._tbh);
                v.push_back(struct_tbh_sym);
            }
        }

        return v;
    }

};

#endif
