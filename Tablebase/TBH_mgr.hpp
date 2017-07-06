#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBH_Manager : Track TBH
//
//
#ifndef _AL_CHESS_TABLEBASE_TBH_MANAGER_HPP
#define _AL_CHESS_TABLEBASE_TBH_MANAGER_HPP

namespace chess
{
    // TBH_Manager
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBH_Manager
    {
    private:
        TBH_Manager() {}
        TBH_Manager(const TBH_Manager&)               = delete;
        TBH_Manager & operator=(const TBH_Manager &)  = delete;
        TBH_Manager(TBH_Manager&&)                    = delete;

    public:
        ~TBH_Manager()
        {
            if (_instance.operator bool())
            {
                _instance.release();
            }
        }
        static const TBH_Manager* instance();
        void clear() const;

        TBH<PieceID, _BoardSize>* add(TB_TYPE t, const PieceSet<PieceID, _BoardSize>& ps, TBH_OPTION option) const;
        TBH<PieceID, _BoardSize>* find(TB_TYPE t, const PieceSet<PieceID, _BoardSize>& ps) const;

        TBH<PieceID, _BoardSize>* add_sym( TB_TYPE t, TBH<PieceID, _BoardSize>* ref) const;
        TBH<PieceID, _BoardSize>* find_sym(TB_TYPE t, TBH<PieceID, _BoardSize>* ref) const;

    private:
        // Would be owner of TBH... unique_ptr/weak_ptr
        mutable std::map<std::string, TBH<PieceID, _BoardSize>*>  _map_tbh;
        mutable std::map<std::string, TBH<PieceID, _BoardSize>*>  _map_tbh_sym;
        static std::unique_ptr<TBH_Manager> _instance;
    };

    //_instance
    template <typename PieceID, typename uint8_t _BoardSize>
    std::unique_ptr<TBH_Manager<PieceID, _BoardSize>>
    TBH_Manager<PieceID, _BoardSize>::_instance = nullptr;

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TBH_Manager<PieceID, _BoardSize>::clear() const
    {
        // owner
        _map_tbh_sym.clear();
        _map_tbh.clear();
    }

    // add 
    template <typename PieceID, typename uint8_t _BoardSize>
    TBH<PieceID, _BoardSize>* TBH_Manager<PieceID, _BoardSize>::add(TB_TYPE t, const PieceSet<PieceID, _BoardSize>& ps, TBH_OPTION option) const
    {
        if (_instance == nullptr) return false;

        std::string name = TB_TYPE_to_string(t) + ps.name(PieceColor::W) + ps.name(PieceColor::B);
        auto iter = _map_tbh.find(name);
        if (iter == _map_tbh.end())
        {
            if (t == TB_TYPE::tb_Xv0)
            {
                _map_tbh[name] = (TBH<PieceID, _BoardSize>*)new TBH_N<PieceID, _BoardSize, 1>(ps, TB_TYPE::tb_Xv0, option);
            }
            else if (t == TB_TYPE::tb_0vX)
            {
                _map_tbh[name] = (TBH<PieceID, _BoardSize>*)new TBH_N<PieceID, _BoardSize, 1>(ps, TB_TYPE::tb_0vX, option);
            }
            else if (t == TB_TYPE::tb_1v1)
            {
                _map_tbh[name] = (TBH<PieceID, _BoardSize>*)new TBH_N<PieceID, _BoardSize, 2>(ps, TB_TYPE::tb_1v1, option);
            }
            else if (t == TB_TYPE::tb_2v1)
            {
                _map_tbh[name] = (TBH<PieceID, _BoardSize>*)new TBH_N<PieceID, _BoardSize, 3>(ps, TB_TYPE::tb_2v1, option);
            }
            else if (t == TB_TYPE::tb_2v2)
            {
                _map_tbh[name] = (TBH<PieceID, _BoardSize>*)new TBH_N<PieceID, _BoardSize, 4>(ps, TB_TYPE::tb_2v2, option);
            }
            else if (t == TB_TYPE::tb_3v1)
            {
                _map_tbh[name] = (TBH<PieceID, _BoardSize>*)new TBH_N<PieceID, _BoardSize, 4>(ps, TB_TYPE::tb_3v1, option);
            }
            else
            {
                assert(false);
                return nullptr;
            }
        }
        return _map_tbh[name];
    }

    // add_sym
    template <typename PieceID, typename uint8_t _BoardSize>
    TBH<PieceID, _BoardSize>* TBH_Manager<PieceID, _BoardSize>::add_sym(TB_TYPE t, TBH<PieceID, _BoardSize>* ref) const
    {
        if (_instance == nullptr) return false;

        std::string name = TB_TYPE_to_string(t) + ref->name(PieceColor::W) + ref->name(PieceColor::B);
        auto iter = _map_tbh_sym.find(name);
        if (iter == _map_tbh_sym.end())
        {
            if      (t == TB_TYPE::tb_2v1_sym) _map_tbh_sym[name] = (TBH<PieceID, _BoardSize>*)new TBH_Symmetry<PieceID, _BoardSize, 3>(ref, TB_TYPE::tb_2v1_sym);
            else if (t == TB_TYPE::tb_3v1_sym) _map_tbh_sym[name] = (TBH<PieceID, _BoardSize>*)new TBH_Symmetry<PieceID, _BoardSize, 4>(ref, TB_TYPE::tb_3v1_sym);
            else
            {
                assert(false);
                return nullptr;
            }
        }
        return _map_tbh_sym[name];
    }

    // find
    template <typename PieceID, typename uint8_t _BoardSize>
    TBH<PieceID, _BoardSize>* TBH_Manager<PieceID, _BoardSize>::find(TB_TYPE t, const PieceSet<PieceID, _BoardSize>& ps) const
    {
        if (_instance == nullptr) return nullptr;

        std::string name = TB_TYPE_to_string(t) + ps.name(PieceColor::W) + ps.name(PieceColor::B);
        auto iter = _map_tbh.find(name);
        if (_map_tbh != _map_tbh.end())
        {
            return _map_tbh[name];
        }
        assert(false);
        return nullptr;
    }

    // find_sym
    template <typename PieceID, typename uint8_t _BoardSize>
    TBH<PieceID, _BoardSize>* TBH_Manager<PieceID, _BoardSize>::find_sym(TB_TYPE t, TBH<PieceID, _BoardSize>* ref) const
    {
        if (_instance == nullptr) return nullptr;

        std::string name = TB_TYPE_to_string(t) + ref->name(PieceColor::W) + ref->name(PieceColor::B);
        auto iter = _map_tbh_sym.find(name);
        if (_map_tbh_sym != _map_tbh_sym.end())
        {
            return _map_tbh_sym[name];
        }
        assert(false);
        return nullptr;
    }

    // instance()
    template <typename PieceID, typename uint8_t _BoardSize>
    const TBH_Manager<PieceID, _BoardSize>* 
    TBH_Manager<PieceID, _BoardSize>::instance()
    {
        if (_instance == nullptr)
        {
            _instance = std::unique_ptr<TBH_Manager>(new TBH_Manager);
            return _instance.get();
        }
        return  _instance.get();
    }

};

#endif
