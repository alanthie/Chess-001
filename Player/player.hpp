#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Player<...> : Chess Player capable of learning (Genetical Algorithm)
//
// The chromosomes of the player are in ConditionValuationNode x (tree size) x number of domains:
//      std::vector<_ConditionFeature*> _conditions;
//      std::vector<bool>               _conditions_and_or;
//      std::vector<_ValuationFeature*> _valuations;
//      std::vector<TYPE_PARAM>         _weights;
//
//
#ifndef _AL_CHESS_PLAYER_PLAYER_HPP
#define _AL_CHESS_PLAYER_PLAYER_HPP

namespace chess
{
    // BasePlayer interface
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class BasePlayer
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _BasePlayer = BasePlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        BasePlayer(const std::string& playername) : _playername(playername) , _PARAM_NBIT(PARAM_NBIT){}
        virtual ~BasePlayer() {}

        // Genetic Algo would play games between population members by calling select_move_algo() for fitness evolution
        virtual size_t      select_move_algo(    _Board& board, std::vector<_Move>& m, size_t max_num_position_per_move, uint16_t max_depth_per_move, uint16_t max_game_ply, bool verbose = false) = 0;
        virtual TYPE_PARAM  eval_position_algo(  _Board& board, std::vector<_Move>& m, bool verbose = false) = 0;

        // Persistence
        virtual bool save() const = 0;
        virtual bool load() = 0;

        const std::string playername()          const   { return _playername; }
        const size_t param_NBIT()               const   { return _PARAM_NBIT; }

    protected:
        std::string             _playername;
        size_t                  _PARAM_NBIT;
    };

    // DomainPlayer
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class DomainPlayer : public BasePlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PartitionManager = PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _BasePlayer = BasePlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        DomainPlayer(   PieceColor color_player,
                        const std::string playername,       const std::string partition_key,
                        const std::string domainname_key,    const std::string instance_key);

        virtual ~DomainPlayer();

        virtual GameDB<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* get_game_db()
        {
            if (_domain != nullptr) 
                return _domain->get_game_db(); 
            else 
                return nullptr; 
        }

        virtual size_t      select_move_algo(   _Board& pos, std::vector<_Move>& m, size_t max_num_position_per_move, uint16_t max_depth_per_move, uint16_t max_game_ply, bool verbose)  override;
        virtual TYPE_PARAM  eval_position_algo( _Board& pos, std::vector<_Move>& m, bool verbose = false)  override;

        const std::string persist_key() const;
        virtual bool save() const override;
        virtual bool load() override;

        std::string get_partition_key()     const { return _partition_key; }
        std::string get_domainname_key()    const { return _domainname_key; }
        std::string get_instance_key()      const { return _instance_key; }
        _Domain*    get_domain()            const { return _domain; }

        _ConditionValuationNode* get_root() { return _root; }

    protected:
        TYPE_PARAM minimax(_Board& board, uint16_t depth, TYPE_PARAM a, TYPE_PARAM b,
            bool isMaximizing, size_t max_num_node, uint16_t max_game_ply,
            size_t& ret_mv_idx, size_t& num_pos_eval,
            bool is_recursive_entry, bool verbose);

        PieceColor              _color_player;
        std::string             _partition_key;
        std::string             _domainname_key;    // domain key part
        std::string             _instance_key;      // domain key part
        _Domain*                _domain;
        _ConditionValuationNode* _root;              // The brain of the player!
    };


    // Exemple
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class NullPlayer : public BasePlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;;

    public:
        NullPlayer() : BasePlayer("NULLPlayer"){}

        size_t      select_move_algo(   _Board& board, std::vector<_Move>& m, size_t max_position, uint16_t max_depth_per_move, uint16_t max_game_ply, bool verbose) override { return 0; }
        TYPE_PARAM  eval_position_algo( _Board& board, std::vector<_Move>& m, bool verbose = false) override { return 0; }

        bool save() const           override { return false; }
        bool load()                 override { return false; }
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    ::DomainPlayer( PieceColor color_player,
                    const std::string playername,       const std::string partition_key,
                    const std::string domainname_key,    const std::string instance_key)
            :   _BasePlayer(playername),
                _color_player(color_player),
                _partition_key(partition_key),
                _domainname_key(domainname_key),
                _instance_key(instance_key),
                _domain(nullptr),
                _root(nullptr) // initially empty, must be fill or load
    {
        _root = new _ConditionValuationNode(nullptr, true, false);

        _Partition* partition_ptr = _PartitionManager::instance()->find_partition(_partition_key);
        if (partition_ptr != nullptr)
        {
            _domain = partition_ptr->find_domain(_Domain::domain_key(_domainname_key, instance_key));
            if (_domain != nullptr)
            {
                if (_color_player == PieceColor::W)
                {
                    if (_domain->_attached_domain_playerW == nullptr)
                     _domain->attach_domain_playerW(this);
                }
                else
                {
                    if (_domain->_attached_domain_playerB == nullptr)
                        _domain->attach_domain_playerB(this);
                }

                _Domain* p_lookup_child_domain;
                for (size_t i = 0; i < _domain->_children.size(); i++)
                {
                    p_lookup_child_domain = partition_ptr->find_domain(_Domain::domain_key(_domain->_children[i]->_domainname_key, _domain->_children[i]->_instance_key));
                    if (p_lookup_child_domain != nullptr)
                    {
                        if (_color_player == PieceColor::W)
                        {
                            if (_domain->_children[i]->_attached_domain_playerW == nullptr)
                            {
                                _domain->_children[i]->_attached_domain_playerW = new DomainPlayer(_color_player, playername, _partition_key, _domain->_children[i]->_domainname_key, _domain->_children[i]->_instance_key);
                                // recursion
                            }
                        }
                        else
                        {
                            if (_domain->_children[i]->_attached_domain_playerB == nullptr)
                            {
                                _domain->_children[i]->_attached_domain_playerB = new DomainPlayer(_color_player, playername, _partition_key, _domain->_children[i]->_domainname_key, _domain->_children[i]->_instance_key);
                                // recursion
                            }
                        }
                    }
                }
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    ::~DomainPlayer()
    {
        if (_domain != nullptr)
        {
            for (size_t i = 0; i < _domain->_children.size(); i++)
            {
                if (_domain->_children[i] != nullptr)
                {
                    if (_color_player == PieceColor::W)
                    {
                        if (_domain->_children[i]->_attached_domain_playerW != nullptr)
                        {
                            delete _domain->_children[i]->_attached_domain_playerW;
                            _domain->_children[i]->_attached_domain_playerW = nullptr;
                            // recursion
                        }
                    }
                    else
                    {
                        if (_domain->_children[i]->_attached_domain_playerB != nullptr)
                        {
                            delete _domain->_children[i]->_attached_domain_playerB;
                            _domain->_children[i]->_attached_domain_playerB = nullptr;
                            // recursion
                        }
                    }
                }
            }
        }
        delete _root;
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    const std::string DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    ::persist_key() const
    {
        return playername() + "_" + _partition_key + "_" + _domainname_key + "_" + _instance_key;
    }


    // DomainPlayer::save()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save() const
    {
        //std::string f = PersistManager::instance()->get_stream_name("player", persist_key(), playername());
        std::string f = PersistManager::instance()->get_stream_name("player", persist_key());
        std::ofstream   filestream;
        filestream.open(f.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (filestream.good())
        {
            filestream <<  playername();  filestream << std::endl;
            filestream << _partition_key; filestream << std::endl;
            filestream << _domainname_key; filestream << std::endl;
            filestream << _instance_key;  filestream << std::endl;

            filestream << _root->persist_key();  filestream << std::endl;
            _root->save_root();  // independant stream

            _Partition* partition_ptr = _PartitionManager::instance()->find_partition(_partition_key);
            if (partition_ptr != nullptr)
            {
                if (_domain != nullptr)
                {
                    if (_color_player == PieceColor::W)
                    {
                        if (_domain->_attached_domain_playerW != nullptr)
                        {
                            for (size_t i = 0; i < _domain->_children.size(); i++)
                            {
                                if (_domain->_children[i] != nullptr)
                                {
                                    if (_domain->_children[i]->_attached_domain_playerW != nullptr)
                                    {
                                        _domain->_children[i]->_attached_domain_playerW->save();
                                        // recursion
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (_domain->_attached_domain_playerB != nullptr)
                        {
                            for (size_t i = 0; i < _domain->_children.size(); i++)
                            {
                                if (_domain->_children[i] != nullptr)
                                {
                                    if (_domain->_children[i]->_attached_domain_playerB != nullptr)
                                    {
                                        _domain->_children[i]->_attached_domain_playerB->save();
                                        // recursion
                                    }
                                }
                            }
                        }
                    }
                }
            }

            filestream.close();
            return true;
        }
        filestream.close();
        return false;
    }

    // DomainPlayer::load()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load()
    {
        std::string f = PersistManager::instance()->get_stream_name("player", persist_key());
        std::ifstream   filestream;
        filestream.open(f.c_str(), std::fstream::in);
        if (filestream.good())
        {
            std::string playername;
            std::string partition_key;
            std::string domainname_key;
            std::string instance_key;
            std::string root_persist_key;

            filestream >> playername;
            filestream >> partition_key;
            filestream >> domainname_key;
            filestream >> instance_key;
            filestream >> root_persist_key;

            assert(playername    == _playername);
            assert(partition_key == _partition_key);
            assert(domainname_key == _domainname_key);
            assert(instance_key  == _instance_key);

            _root->set_persist_key(root_persist_key);
            _root->load_root(); // root data was not load from DomainPlayer() constructor

            _Partition* p_partition = _PartitionManager::instance()->find_partition(partition_key);
            if (p_partition == nullptr) return false;

            for (size_t i = 0; i < _domain->_children.size(); i++)
            {
                if (_domain->_children[i] != nullptr)
                {
                    if (_color_player == PieceColor::W)
                    {
                        if (_domain->_children[i]->_attached_domain_playerW != nullptr)
                        {
                            _domain->_children[i]->_attached_domain_playerW->load();
                            // recursion
                        }
                    }
                    else
                    {
                        if (_domain->_children[i]->_attached_domain_playerB != nullptr)
                        {
                            _domain->_children[i]->_attached_domain_playerB->load();
                            // recursion
                        }
                    }
                }
            }

            filestream.close();
            return true;
        }
        filestream.close();
        return false;
    }

    // minimax
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    TYPE_PARAM DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::
    minimax(_Board& board, uint16_t depth, TYPE_PARAM a, TYPE_PARAM b, bool isMaximizing, 
            size_t max_num_node, uint16_t max_game_ply,
            size_t& ret_mv_idx, size_t& num_pos_eval, bool is_recursive_entry, bool verbose)
    {
        size_t best_a_idx = 0;
        size_t best_b_idx = 0;

        std::vector<_Move> m = board.generate_moves();
        if ((depth == 0) || (board.is_final(m)) || (num_pos_eval >= max_num_node) || (board.get_histo_size() >= max_game_ply))
        {
            num_pos_eval++;
            if (verbose)
            {
                _Move m = board.last_history_move();
                std::cout << "[" << std::to_string(m.src_x) << std::to_string(m.src_y) << std::to_string(m.dst_x) << std::to_string(m.dst_y) << "]";
            }
            return eval_position_algo(board, m, verbose);
        }

        if (isMaximizing)
        {
            for (size_t i = 0; i < m.size(); i++)
            {
                board.apply_move(m[i]);
                TYPE_PARAM temp = this->minimax(board, depth - 1, a, b, false, max_num_node, max_game_ply, ret_mv_idx, num_pos_eval, true, verbose);
                if (temp >= a) best_a_idx = i;
                a = std::max<TYPE_PARAM>(a, temp);
                board.undo_move();
                if (b <= a)
                {
                    return b; // b cutoff.
                }
            }
            ret_mv_idx = best_a_idx;
            return a;
        }
        else
        {
            for (size_t i = 0; i < m.size(); i++)
            {
                board.apply_move(m[i]);
                TYPE_PARAM temp = this->minimax(board, depth - 1, a, b, true, max_num_node, max_game_ply, ret_mv_idx, num_pos_eval, true, verbose);
                if (temp <= b) best_b_idx = i;
                b = std::min(b, temp);
                board.undo_move();
                if (b <= a)
                {
                    return a; // a cutoff.
                }
            }
            ret_mv_idx = best_b_idx;
            return b;
        }
    }

    // select_move_algo
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    size_t DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::
    select_move_algo(_Board& pos, std::vector<_Move>& m, size_t max_num_position_per_move, uint16_t max_depth_per_move, uint16_t max_game_ply, bool verbose)
    {
        if (_domain != nullptr)
        {
            if (_domain->has_known_score_move())
            {
                size_t ret_mv_idx;
                ExactScore sc = _domain->get_known_score_move(pos, m, ret_mv_idx);
                if (sc != ExactScore::UNKNOWN)
                    return ret_mv_idx;
            }
        }
        // classic minmax
        TYPE_PARAM  a = -std::numeric_limits<TYPE_PARAM>::max();    // eval is only in (0..1) currently
        TYPE_PARAM  b = std::numeric_limits<TYPE_PARAM>::max();
        size_t      ret_best_move_index = 0;
        size_t      num_pos_eval = 0;
        TYPE_PARAM  e = minimax(pos, max_depth_per_move, a, b, pos.get_color() == PieceColor::W, max_num_position_per_move, max_game_ply, ret_best_move_index, num_pos_eval,false, verbose);
        return ret_best_move_index;
    }

    // eval_position_algo
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    TYPE_PARAM DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::
    eval_position_algo(_Board& pos, std::vector<_Move>& m, bool verbose)
    {
        TYPE_PARAM ret_eval = 0.5;
        bool ret = _root->eval_position(pos, m, ret_eval, verbose);
        if (ret == true) return ret_eval;

        // This is first layer of children domain (should make sure it cover all sub domains)
        for (size_t i = 0; i < _domain->_children.size(); i++)
        {
            if (_color_player == PieceColor::W)
            {
                ret = _domain->_children[i]->_attached_domain_playerW->_root->eval_position(pos, m, ret_eval, verbose);
            }
            else
            {
                ret = _domain->_children[i]->_attached_domain_playerB->_root->eval_position(pos, m, ret_eval, verbose);
            }
            if (ret == true) return ret_eval;
        }

        // Failure if hole in the hiearchy of the domains of the partition
        // throw...
        return ret_eval;
    }

};

#endif
