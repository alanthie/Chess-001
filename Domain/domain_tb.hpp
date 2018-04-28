#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// DomainTB<...>
//
//
#ifndef _AL_CHESS_DOMAIN_DomainTB_HPP
#define _AL_CHESS_DOMAIN_DomainTB_HPP

namespace chess
{
    // DomainTB (Domain matching a TB (Tablebase) space)
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class DomainTB : public Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _STRUCT_PIECE_RANK = STRUCT_PIECE_RANK<PieceID, _BoardSize>;
        
        using _Board    = Board<PieceID, _BoardSize>;
        using _Piece    = Piece<PieceID, _BoardSize>;
        using _Domain   = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Move     = Move<PieceID>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature_isOppositeKinCheck = ConditionFeature_isOppositeKinCheck<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_numberMoveForPiece = ValuationFeature_numberMoveForPiece<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PieceSet = PieceSet<PieceID, _BoardSize>;
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        friend class _DomainPlayer;

    private:
        _PieceSet   _ps;

        mutable PieceColor  _tb_color;
        mutable uint64_t    _next_position_index;               // iterative position index in TB(_tb_color)
        mutable TablebaseBase<PieceID, _BoardSize>* _TB_W;      // owner is TB_Manager
        mutable TablebaseBase<PieceID, _BoardSize>* _TB_B;
        mutable _Board*     _work_board;
        std::map<size_t, _STRUCT_PIECE_RANK>& _map_piece_rank;

    public:
        DomainTB(const std::string partition_key, const _PieceSet& ps)
            : _Domain(partition_key, ps.name(PieceColor::none), "0"),
            _ps(ps.wset(), ps.bset()), _next_position_index(0), _tb_color(PieceColor::W), _TB_W(nullptr), _TB_B(nullptr), _map_piece_rank(_ps.map_piece_rank())
        {
            _work_board = new Board<PieceID, _BoardSize>();

            _TB_W = TB_Manager<PieceID, _BoardSize>::instance()->find_N(_ps.name(PieceColor::W));
            _TB_B = TB_Manager<PieceID, _BoardSize>::instance()->find_N(_ps.name(PieceColor::B));

            if ((_TB_W == nullptr) || (_TB_B == nullptr))
            {
                // try disk load
                TBH<PieceID, _BoardSize>* _TBH = TBH_Manager<PieceID, _BoardSize>::instance()->create(ps, TBH_IO_MODE::tb_only, TBH_OPTION::none);
                if (_TBH != nullptr)
                {
                    _TBH->load(TBH_IO_MODE::tb_only);
                    _TB_W = TB_Manager<PieceID, _BoardSize>::instance()->find_N(_ps.name(PieceColor::W));
                    _TB_B = TB_Manager<PieceID, _BoardSize>::instance()->find_N(_ps.name(PieceColor::B));
                }
            }
        }

        ~DomainTB()
        {
            delete _work_board;
        }

        bool has_TB(PieceColor c)
        {
            if (c == PieceColor::none) return false;
            TablebaseBase<PieceID, _BoardSize>* _TB = (c == PieceColor::W) ? _TB_W : _TB_B;
            if (_TB == nullptr) return false;
            return true;
        }

        TablebaseBase<PieceID, _BoardSize>* TB(PieceColor c)
        {
            if (c == PieceColor::none) return nullptr;
            return (c == PieceColor::W) ? _TB_W : _TB_B;
        }

        _PieceSet& ps()             { return _ps; }
        const _PieceSet& ps() const { return _ps; }

        bool isInDomain(const _Board& position) const override
        {
            if (position.cnt_all_piece() != _ps.count_all_piece(PieceColor::W) + _ps.count_all_piece(PieceColor::B)) return false;
            else if (position.cnt_piece(PieceName::K, PieceColor::W) != _ps.count_one_piecename(PieceColor::W, PieceName::K)) return false;
            else if (position.cnt_piece(PieceName::K, PieceColor::B) != _ps.count_one_piecename(PieceColor::B, PieceName::K)) return false;
            else if (position.cnt_piece(PieceName::P, PieceColor::W) != _ps.count_one_piecename(PieceColor::W, PieceName::P)) return false;
            else if (position.cnt_piece(PieceName::P, PieceColor::B) != _ps.count_one_piecename(PieceColor::B, PieceName::P)) return false;
            else if (position.cnt_piece(PieceName::Q, PieceColor::W) != _ps.count_one_piecename(PieceColor::W, PieceName::Q)) return false;
            else if (position.cnt_piece(PieceName::Q, PieceColor::B) != _ps.count_one_piecename(PieceColor::B, PieceName::Q)) return false;
            else if (position.cnt_piece(PieceName::R, PieceColor::W) != _ps.count_one_piecename(PieceColor::W, PieceName::R)) return false;
            else if (position.cnt_piece(PieceName::R, PieceColor::B) != _ps.count_one_piecename(PieceColor::B, PieceName::R)) return false;
            else if (position.cnt_piece(PieceName::N, PieceColor::W) != _ps.count_one_piecename(PieceColor::W, PieceName::N)) return false;
            else if (position.cnt_piece(PieceName::N, PieceColor::B) != _ps.count_one_piecename(PieceColor::B, PieceName::N)) return false;
            else if (position.cnt_piece(PieceName::B, PieceColor::W) != _ps.count_one_piecename(PieceColor::W, PieceName::B)) return false;
            else if (position.cnt_piece(PieceName::B, PieceColor::B) != _ps.count_one_piecename(PieceColor::B, PieceName::B)) return false;
            return true;
        }

        bool is_cond_feature_valid(const _ConditionFeature& f) const override
        {
            return true;
        }

        bool is_valu_feature_valid(const _ValuationFeature& f) const override
        {
            if (f.name() == ValuFeatureName::eValuationFeature_distKK) return true;
            if ( (f.name() == ValuFeatureName::eValuationFeature_numberMoveForPiece)||
                 (f.name() == ValuFeatureName::eValuationFeature_onEdge))
            {
                _ValuationFeature_numberMoveForPiece& r = (_ValuationFeature_numberMoveForPiece&)f;
                if (_ps.count_one_piecename(r.piececolor(), r.piecename()) > 0) return true;
                return false;
            }
            return true;
        }

        bool has_known_score_move() const override { return true; }

        ExactScore get_known_score_move(const _Board& position, const std::vector<_Move>& m, size_t& ret_mv_idx) const override
        {
            assert( isInDomain(position) );
            if (position.is_final(m))
            {
                ret_mv_idx = 0; // move is irrevelant here
                return position.final_score(m);
            }
            else if (position.allow_self_check())
            {
                if (position.can_capture_opposite_king(m, ret_mv_idx))
                {
                    if (position.get_color() == PieceColor::W) return ExactScore::WIN;
                    else if (position.get_color() == PieceColor::B) return ExactScore::LOSS;
                }
            }

            // Lookup score in TB
            _tb_color = position.get_color();
            if (_tb_color == PieceColor::none) return ExactScore::UNKNOWN;
            TablebaseBase<PieceID, _BoardSize>* _TB = (_tb_color == PieceColor::W) ? _TB_W : _TB_B;
            if (_TB == nullptr) return ExactScore::UNKNOWN;

            std::vector<PieceID> v_id = position.get_piecesID();        // sorted
            // check
            std::vector<PieceID> this_v_id = PieceSet<PieceID, _BoardSize>::ps_to_pieces(_ps);
            assert(this_v_id.size() == v_id.size());
            for (size_t i=0;i<v_id.size();i++)
            {
                assert(v_id[i] == this_v_id[i]);
            }

            std::vector<uint16_t> v_sq;
            v_sq.assign(v_id.size(), 0);
            for (size_t z = 0; z < v_id.size(); z++)
            {
                v_sq[z] = position.get_square_ofpiece_instance(
                    Piece<PieceID, _BoardSize>::get(_map_piece_rank[z].ret_id)->get_name(),
                    Piece<PieceID, _BoardSize>::get(_map_piece_rank[z].ret_id)->get_color(),
                    _map_piece_rank[z].ret_instance);
            }
            _TB->order_sq_v(v_sq, v_id);
            return _TB->score_v(v_sq);
        }

        bool save_detail(std::ofstream& os) const override
        {
            std::vector<PieceID> v_id = _PieceSet::ps_to_pieces(_ps);

            if (os.good())
            {
                os << _partition_key;   os << " ";
                os << _domainname_key;  os << " ";
                os << _instance_key;    os << " ";

                os << _children.size(); os << " ";
                for (auto& v : _children)
                {
                    os << v->partition_key();   os << " ";
                    os << v->domainname_key();  os << " ";
                    os << v->instance_key();    os << " ";
                }

                os << v_id.size(); os << " ";
                for (auto& v : v_id)
                {
                    os << (int)v; os << " ";
                }
                return true;
            }
            return false;
        }

        bool save() const override  
        {
            return save_root();
        }

        bool load_detail(std::ifstream& is) const override
        {
            if (is.good())
            {
                size_t n_child;
                std::string partition_key;
                std::string domainname_key;
                std::string instance_key;

                is >> partition_key;
                is >> domainname_key;
                is >> instance_key;

                // check
                assert(partition_key == _partition_key);
                assert(domainname_key == _domainname_key);
                assert(instance_key == _instance_key);

                is >> n_child;

                _Partition* p_partition = _PartitionManager::instance()->find_partition(partition_key);
                if (p_partition == nullptr) return false;

                _Domain* p_dom = p_partition->find_domain(domain_key(domainname_key, instance_key));
                if (p_dom == nullptr) return false;

                // reloading
                p_dom->_children.clear();

                bool ok = true;
                for (size_t i = 0; i < n_child; i++)
                {
                    ok = false;
                    is >> partition_key;
                    is >> domainname_key;
                    is >> instance_key;
                    {
                        _Domain* p_dom_child = p_partition->find_domain(domain_key(domainname_key, instance_key));
                        if (p_dom_child != nullptr)
                        {
                            if (p_dom->add_child(p_dom_child))
                            {
                                ok = true;
                            }
                        }
                    }
                    if (!ok)
                    {
                        return false;
                    }
                }

                is >> n_child;
                std::vector<PieceID> v_id;
                int id;
                for (size_t i = 0; i < n_child; i++)
                {
                    is >> id;
                    v_id.push_back((PieceID)id);
                }
                return true;
            }
            return false;
        }

        bool load() override
        {
            return load_root();
        }

        _Board get_random_position(bool no_check) const override
        {
            uint16_t n = _ps.count_all_piece(PieceColor::W) + _ps.count_all_piece(PieceColor::B);
            std::vector<uint16_t> v_sq;
            v_sq.assign(n, 0);
            for (size_t i = 0; i < n; i++) v_sq[i] = (uint16_t)(uniform_double(rnd_generator) * (_BoardSize*_BoardSize));

            bool ok = false;
            while (!ok)
            {
                ok = true; 
                for (size_t i = 0; i < n; i++)
                {
                    if (std::count(v_sq.begin(), v_sq.end(), v_sq[i]) > 1)
                    {
                        ok = false;
                        break;
                    }
                }
                if (!ok)
                {
                    for (size_t i = 0; i < n; i++)
                        v_sq[i] = (uint16_t)(uniform_double(rnd_generator) * (_BoardSize*_BoardSize));
                }
            }

            _Board b;
            STRUCT_PIECE_RANK<PieceID, _BoardSize> r;
            for (size_t z = 0; z < n; z++)
            {
                _ps.find_rank_index(z, r.ret_id, r.ret_count, r.ret_instance);
                if (r.ret_count > 0)
                {
                    b.set_pieceid_at(r.ret_id, v_sq[z] % _BoardSize, ((uint8_t)(v_sq[z] / _BoardSize)));
                }
            }
            b.set_color(PieceColor::W);

            if (no_check)
            {
                if (b.is_in_check())
                {
                    return get_random_position(no_check);
                }
                else
                {
                    std::vector<_Move> m = b.generate_moves();
                    size_t mv;
                    if ( b.can_capture_opposite_king(m, mv) )
                        return get_random_position(no_check); 
                }
            }
            return b;
        }

        _Board* first_position(PieceColor c) const override 
        {
            if (c == PieceColor::none) return nullptr;
            _tb_color = c;  // keep it for next_position()
            TablebaseBase<PieceID, _BoardSize>* _TB = (_tb_color == PieceColor::W) ? _TB_W : _TB_B;
            if (_TB == nullptr) return nullptr;

            _next_position_index = 0;
            return next_position();
        }

        _Board* next_position() const override 
        { 
            if (_tb_color == PieceColor::none) return nullptr;
            TablebaseBase<PieceID, _BoardSize>* _TB = (_tb_color == PieceColor::W) ? _TB_W : _TB_B;
            if (_TB == nullptr)  return nullptr;

            std::vector<uint16_t> sq;
            sq.assign(_TB->num_piece(), 0);

            uint64_t m;
            if (_TB->is_full_type())
                m = _TB->size_tb();
            else
                m = _TB->size_full_tb();

            ExactScore sc;
            while (true)
            {
                if (_next_position_index >= m) return nullptr;

                if (!_TB->valid_index(_next_position_index, *_work_board, sq))
                {
                    _next_position_index++;
                    continue;
                }
                sc = _TB->score_v(sq);
                if (sc == ExactScore::UNKNOWN)
                {
                    _next_position_index++;
                    continue;
                }
            }
            _next_position_index++;
            return _work_board;
        }
    };

};

#endif
