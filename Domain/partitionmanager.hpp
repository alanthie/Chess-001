#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// PartitionManger  : Manage the multiple partitions
//
//
#ifndef _AL_CHESS_DOMAIN_PARTITIONMANAGER_HPP
#define _AL_CHESS_DOMAIN_PARTITIONMANAGER_HPP

namespace chess
{
    // PartitionManager
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class PartitionManager
    {
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PieceSet = PieceSet<PieceID, _BoardSize>;

    private:
        PartitionManager() {}

        PartitionManager(const PartitionManager&) = delete;
        PartitionManager & operator=(const PartitionManager &) = delete;
        PartitionManager(PartitionManager&&) = delete;

    public:
        ~PartitionManager()
        {
            if (_instance.operator bool())
            {
                _partitions.clear();
                _instance.release();
            }
        }

        static const PartitionManager* instance();

    public:
        _Partition*     load_partition(const std::string& name) const;
        bool            add_partition(_Partition* p)const;
        _Partition*     find_partition(const std::string partition_name) const;
        void            remove_partition(const std::string partition_name) const;

        bool            make_classic_partition() const;       // Exemple
        bool            make_tb_partition(uint8_t maxNpiece) const;

    private:
        std::map<std::string, _Partition*>          _partitions;    // TODO std_unique<> ...
        static std::unique_ptr<PartitionManager>    _instance;

        bool make_tb_domain_from_ps(std::string& partition_name, _PieceSet& ps, _Partition* p_tb) const;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::unique_ptr<PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>>
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_instance = nullptr;

    // make_classic_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::make_classic_partition() const
    {
        std::stringstream ss_partion_name;
        ss_partion_name << "classic" << std::to_string(_BoardSize);

        // Check if in memory
        _Partition* p_classic = PartitionManager::instance()->find_partition(ss_partion_name.str());
        if (p_classic == nullptr)
        {
            _Partition* p = new _Partition(ss_partion_name.str());
            if (!PartitionManager::instance()->add_partition(p))
            {
                remove_partition(ss_partion_name.str());
                return false;
            }

            // Try loading from disk
            if (p->load() == true)
            {
                return true;
            }
            else
            {
                // Continu to create it
            }
        }
        else
        {
            // Reload from disk
            if (p_classic->load() == false)
            {
                return false;
            }
        }

        // Check if in memory
        p_classic = PartitionManager::instance()->find_partition(ss_partion_name.str());
        if (p_classic == nullptr)
            return false;

        // Create domains
        _Domain* dom_KK   = new DomainKvK< PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(ss_partion_name.str());
        _Domain* dom_KQvK = new DomainKQvK<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(ss_partion_name.str());

        if (!p_classic->add_domain(dom_KK))     return false;
        if (!p_classic->add_domain(dom_KQvK))   return false;

        // Just checking alternate method
        _Domain* ptr_dom_KQvK = p_classic->find_domain(_Domain::getDomainName(eDomainName::KQvK), "0"); if (ptr_dom_KQvK == nullptr) return false;
        _Domain* ptr_dom_KK   = p_classic->find_domain(_Domain::getDomainName(eDomainName::KvK) , "0"); if (ptr_dom_KK   == nullptr) return false;

        // set children relationship
        if (!ptr_dom_KQvK->add_child(ptr_dom_KK)) return false;

        ptr_dom_KQvK->save();
        ptr_dom_KK->save();
        p_classic->save();

        return true;
    }

    // find_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* 
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::find_partition(const std::string partition_name) const
    {
        if (_instance == nullptr) return nullptr;

        auto& iter = _instance->_partitions.find(partition_name);
        if (iter != _instance->_partitions.end())
        {
            return (iter->second);
        }
        return nullptr;
    }

    // remove_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    void PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::remove_partition(const std::string partition_name) const
    {
        if (_instance == nullptr) return;

        auto& iter = _instance->_partitions.find(partition_name);
        if (iter != _instance->_partitions.end())
        {
            _instance->_partitions.erase(iter);
        }
    }

    // add_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::add_partition(_Partition* p) const
    {
        if (_instance == nullptr) return false;

        auto iter = _instance->_partitions.find(p->name());
        if (iter == _instance->_partitions.end())
        {
            _instance->_partitions.insert(std::pair<std::string, _Partition*>(p->name(), p));
            return true;
        }
        return false;
    }

    // instance()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    const PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* 
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::instance()
    {
        if (_instance == nullptr)
        {
            _instance = std::unique_ptr<PartitionManager>(new PartitionManager);
            return _instance.get();
        }
        return  _instance.get();
    }

    // load_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT> * 
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load_partition(const std::string& name) const
    {
        // Check if in memory
        _Partition* pp = PartitionManager::instance()->find_partition(name);
        if (pp == nullptr)
        {
            _Partition* p = new _Partition(name);
            if (!PartitionManager::instance()->add_partition(p))
            {
                remove_partition(name);
                delete p;
                return nullptr;
            }

            // Try loading from disk
            if (p->load() == true)  return p;
   
            remove_partition(name);
            delete p;
            return nullptr;
        }
        else
        {
            // Reload from disk
            if (pp->load() == false)
            {
                return nullptr;
            }
            return pp;
        }
        return nullptr;
    }

    // make_tb_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::make_tb_partition(uint8_t maxNpiece) const
    {
        if (maxNpiece > 2 + 2 * _BoardSize) // only KP(N)vKP(M) for now (P promote to Q)
        {
            // cerr...
            return false;
        }

        std::stringstream ss_partion_name;
        ss_partion_name << "TB_N" << (int)maxNpiece << "_B" << std::to_string(_BoardSize);

        // Check if in memory
        _Partition* p_tb = PartitionManager::instance()->find_partition(ss_partion_name.str());
        if (p_tb == nullptr)
        {
            _Partition* p = new _Partition(ss_partion_name.str(), PartitionType::lookup_PieceSet);
            if (!PartitionManager::instance()->add_partition(p))
            {
                remove_partition(ss_partion_name.str());
                return false;
            }

            // Try loading from disk
            if (p->load() == true)
            {
                return true;
            }
            else
            {
                // Continu to create it
            }
        }
        else
        {
            // Reload from disk
            if (p_tb->load() == false)
            {
                // cerr...
                return false;
            }
        }

        // Check if in memory
        p_tb = PartitionManager::instance()->find_partition(ss_partion_name.str());
        if (p_tb == nullptr)
        {
            // cerr...
            return false;
        }

        // Create domains
        // pieceset (nw >= nb) gives all childs - Only doing pawn/queen for now
        // 2: KK
        // N: KP(N-2)K, KP(N-3)KP(1), KP(N-4)KP(2), ... while[(n-i) >= i]

        std::vector<_Domain*> v_dom;
        std::vector<PieceID> white_piece_set;
        std::vector<PieceID> black_piece_set;
        std::vector<std::pair<PieceID, uint8_t>> v_white_piece;
        std::vector<std::pair<PieceID, uint8_t>> v_black_piece;

        for (uint8_t N = 2; N <= maxNpiece; N++)
        {
            for (uint8_t i = 0; i < _BoardSize; i++) // P white
            {
                for (uint8_t j = 0; j < _BoardSize; j++) // P black
                {
                    if ( ((2 + i + j) == N) && (i >= j) )
                    {
                        v_dom.clear();
                        white_piece_set.clear();
                        black_piece_set.clear();
                        v_white_piece.clear();
                        v_black_piece.clear();

                        white_piece_set.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
                        black_piece_set.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
                        for (uint8_t z = 0; z < i; z++)
                        {
                            white_piece_set.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
                        }
                        for (uint8_t z = 0; z < j; z++)
                        {
                            black_piece_set.push_back(_Piece::get_id(PieceName::P, PieceColor::B));
                        }
                        v_white_piece = _PieceSet::to_set(white_piece_set);
                        v_black_piece = _PieceSet::to_set(black_piece_set);

                        _PieceSet ps(v_white_piece, v_black_piece);
                        make_tb_domain_from_ps(ss_partion_name.str(), ps, p_tb); // recursion
                    }
                }
            }
        }
        if (!p_tb->save())
        {
            //...
            assert(false);
        }
        return true;
    }

    // make_tb_domain_from_ps
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::make_tb_domain_from_ps(std::string& partition_name, _PieceSet& ps, _Partition* p_tb) const
    {
        std::vector<_Domain*> v_dom;
        uint16_t nw;
        uint16_t nb;
        uint16_t nwK;
        uint16_t nbK;
        nw = ps.count_all_piece(PieceColor::W);
        nb = ps.count_all_piece(PieceColor::B);
        nwK = ps.count_one_piecename(PieceColor::W, PieceName::K);
        nbK = ps.count_one_piecename(PieceColor::B, PieceName::K);

        PieceSet<PieceID, _BoardSize> copy(ps.wset(), ps.bset());
        PieceSet<PieceID, _BoardSize> ps_sym({ PieceSet<PieceID, _BoardSize>::reverse_color_set(copy.bset()), PieceSet<PieceID, _BoardSize>::reverse_color_set(copy.wset()) });

        bool is_sym = false;
        if ( (nw < nb) && (nw > 0) && (nb > 0) && (nbK > 0) && (nwK > 0))
        {
            is_sym = true;
        }
        else if ((nb == 0) || (nbK == 0) || (nw == 0) || (nwK == 0))
        {
            ps.collapse_to_one_piece();
        }
        else
        {
        }

        {
            _Domain* ptr_dom_parent;
            if (!is_sym)
            {
                ptr_dom_parent = p_tb->find_domain(ps.name(PieceColor::none), "0");
                if (ptr_dom_parent == nullptr)
                {
                    ptr_dom_parent = new DomainTB< PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(partition_name, ps);
                    v_dom.push_back(ptr_dom_parent);
                    p_tb->add_domain(ptr_dom_parent);
                }
            }
            else
            {
                ptr_dom_parent = p_tb->find_domain(ps_sym.name(PieceColor::none), "0");
                if (ptr_dom_parent == nullptr)
                {
                    ptr_dom_parent = new DomainTB< PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(partition_name, ps_sym);
                    v_dom.push_back(ptr_dom_parent);
                    p_tb->add_domain(ptr_dom_parent);
                }
            }

            // using make_all_child_TBH
            std::vector<STRUCT_TBH<PieceID, _BoardSize>> _tb_children_info;
            if (!is_sym)
                _tb_children_info = TB_Manager<PieceID, _BoardSize>::instance()->make_all_child_TBH(ps, TBH_IO_MODE::tb_hiearchy, TBH_OPTION::none);
            else
                _tb_children_info = TB_Manager<PieceID, _BoardSize>::instance()->make_all_child_TBH(ps_sym, TBH_IO_MODE::tb_hiearchy, TBH_OPTION::none);

            for (size_t z = 0; z < _tb_children_info.size(); z++)
            {
                nw  = _tb_children_info[z]._ps.count_all_piece(PieceColor::W);
                nb  = _tb_children_info[z]._ps.count_all_piece(PieceColor::B);
                nwK = _tb_children_info[z]._ps.count_one_piecename(PieceColor::W, PieceName::K);
                nbK = _tb_children_info[z]._ps.count_one_piecename(PieceColor::B, PieceName::K);

                if ((nb == 0) || (nbK == 0) || (nw == 0) || (nwK == 0) || (nw >= nb))
                {
                    make_tb_domain_from_ps(partition_name, _tb_children_info[z]._ps, p_tb);

                    _Domain* ptr_dom = p_tb->find_domain(_tb_children_info[z]._ps.name(PieceColor::none), "0");
                    if (ptr_dom == nullptr)
                    {
                        ptr_dom = new DomainTB< PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(partition_name, _tb_children_info[z]._ps);
                        v_dom.push_back(ptr_dom);
                        p_tb->add_domain(ptr_dom);
                    }
                    ptr_dom_parent->add_child(ptr_dom); // link
                }
                else if ((nw < nb) && (nw > 0) && (nb > 0) && (nbK > 0) && (nwK > 0))
                {
                    // sym
                    PieceSet<PieceID, _BoardSize> copy(_tb_children_info[z]._ps.wset(), _tb_children_info[z]._ps.bset());
                    PieceSet<PieceID, _BoardSize> ps_sym({ PieceSet<PieceID, _BoardSize>::reverse_color_set(copy.bset()), PieceSet<PieceID, _BoardSize>::reverse_color_set(copy.wset()) });

                    make_tb_domain_from_ps(partition_name, ps_sym, p_tb);

                    _Domain* ptr_dom = p_tb->find_domain(ps_sym.name(PieceColor::none), "0");
                    if (ptr_dom == nullptr)
                    {
                        ptr_dom = new DomainTB< PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(partition_name, ps_sym);
                        v_dom.push_back(ptr_dom);
                        p_tb->add_domain(ptr_dom);
                    }
                    ptr_dom_parent->add_child(ptr_dom); // link
                }
            }
            for (size_t z = 0; z < v_dom.size(); z++)
            {
                v_dom[z]->save();
            }

        }
        return true;
    }
};

#endif
