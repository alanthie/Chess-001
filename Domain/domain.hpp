#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Domain<...>      : Base chess domain interface
// DomainKQvK       : KQvK chess domain
// DomainKvK        : KvK  chess domain
//
//
#ifndef _AL_CHESS_DOMAIN_DOMAIN_HPP
#define _AL_CHESS_DOMAIN_DOMAIN_HPP

namespace chess
{
    //  Partition create Domain (hiearchical)
    //  Domain dont need to match a specific pieces set like TB(Tablebase) (EX: Material difference=0,1,..., TBwithDistanceToConversion=0,1...)
    //  Domain best historical score data may be accumulated either explicitly [_GameDB (estimate score)] or implicitly [TB (exact score and estimate score)]
    //  isInDomain(const _Board& position) will determine if a position belong to a domain or not
    //
    //  DomainPlayer (Specific white or black player) link to one domain and can be evolved ( _ConditionValuationNode* _root is what is evolving)
    //

    enum class eDomainName {KvK, KQvK};

    // Domain interface
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Domain 
    {
        using _Board            = Board<PieceID, _BoardSize>;
        using _Piece            = Piece<PieceID, _BoardSize>;
        using _Move             = Move<PieceID>;
        using _Domain           = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainTB         = DomainTB<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Partition        = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PartitionManager = PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer     = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _GameDB           = GameDB<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PlayerFactory    = PlayerFactory<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PieceSet = PieceSet<PieceID, _BoardSize>;

        friend class _DomainPlayer;
        friend class _PlayerFactory;
        friend class _Partition;
        friend class _PartitionManager;
        friend class _DomainTB;

    protected:
        std::string             _partition_key;             // Identification 3 keys
        std::string             _domainname_key;
        std::string             _instance_key;    
        std::vector<_Domain*>   _children;
        _DomainPlayer*          _attached_domain_playerW;
        _DomainPlayer*          _attached_domain_playerB;
        std::string             _gamedb_key;
        _GameDB*                _gameDB;

    public:
        Domain( const std::string partition_key,
                const std::string domainname_key,
                const std::string instance_key) :
            _partition_key(partition_key), _domainname_key(domainname_key), _instance_key(instance_key),
            _attached_domain_playerW(nullptr),
            _attached_domain_playerB(nullptr),
            _gameDB(nullptr)
        {
            // Would load _gameDB on demand (after domain is created and linked to a partition)
        }

        _GameDB* get_game_db()
        {
            if (_gameDB == nullptr)
            {
                _gamedb_key = persist_key();
                _gameDB = new _GameDB(this, _gamedb_key);
                if (_gameDB->status() != 0)
                {
                    delete _gameDB;
                    _gameDB = nullptr;
                }
            }
            return _gameDB;
        }

        virtual ~Domain()
        {
            if (_gameDB != nullptr)
                delete _gameDB;
        }

        // = 0
        virtual bool        is_cond_feature_valid(const _ConditionFeature& f) const = 0;
        virtual bool        is_valu_feature_valid(const _ValuationFeature& f) const = 0;
        virtual bool        has_known_score_move()  const = 0;
        virtual ExactScore  get_known_score_move(const _Board& position, const std::vector<_Move>& m, size_t& ret_mv_idx) const = 0;
        virtual bool        isInDomain(const _Board& position) const = 0;
        virtual bool        save() const = 0;
        virtual bool        load() = 0;
        virtual _Board      get_random_position(bool no_check) const = 0;
     
        virtual _Board*     first_position(PieceColor c) const { return nullptr; }
        virtual _Board*     next_position()  const { return nullptr; }

        const std::vector<_Domain*>& children() const { return _children; }
        const std::string partition_key()       const { return _partition_key; }
        const std::string domainname_key()      const { return _domainname_key; }
        const std::string instance_key()        const { return _instance_key; }
        const std::string domain_key()          const { return _domainname_key + "_" + _instance_key; }
        static const std::string domain_key(const std::string& k1, const std::string& k2) { return k1 + "_" + k2; }
        const std::string persist_key()         const { return _partition_key + "_" + domain_key(); }

        // Factory
        static _Domain* make(const std::string& partition_key, const std::string& domainname_key, const std::string& instance_key)
        {
            _Partition* p = _PartitionManager::instance()->find_partition(partition_key);
            if (p == nullptr)
            {
                //cerr...
                return nullptr;
            }

            if (p->partition_type() == PartitionType::lookup_eDomainName)
            {
                if (domainname_key == getDomainName(eDomainName::KvK))  return new DomainKvK <PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(partition_key);
                else if (domainname_key == getDomainName(eDomainName::KQvK)) return new DomainKQvK<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(partition_key);
            }
            else if (p->partition_type() == PartitionType::lookup_PieceSet)
            {
                // reverse name to ps
                std::vector<PieceID> v_wp;
                std::vector<PieceID> v_bp;

                size_t i = 0;
                char c;
                PieceName pn;

                while ( (i < domainname_key.length() - 1) && (domainname_key[i] == 'w') )
                {
                    c = domainname_key[i + 1];
                    pn = PieceName::none;
                    if (c == 'K') pn = PieceName::K;
                    else if (c == 'P') pn = PieceName::P;
                    else if (c == 'Q') pn = PieceName::Q;
                    else if (c == 'R') pn = PieceName::R;
                    else if (c == 'N') pn = PieceName::N;
                    else if (c == 'B') pn = PieceName::B;
                    if (pn != PieceName::none)
                        v_wp.push_back(_Piece::get_id(pn, PieceColor::W));
                    i += 2;
                }

                while ((i < domainname_key.length() - 1) && (domainname_key[i] == 'b'))
                {
                    c = domainname_key[i + 1];
                    pn = PieceName::none;
                    if (c == 'K') pn = PieceName::K;
                    else if (c == 'P') pn = PieceName::P;
                    else if (c == 'Q') pn = PieceName::Q;
                    else if (c == 'R') pn = PieceName::R;
                    else if (c == 'N') pn = PieceName::N;
                    else if (c == 'B') pn = PieceName::B;
                    if (pn != PieceName::none)
                        v_bp.push_back(_Piece::get_id(pn, PieceColor::B));
                    i += 2;
                }

                std::vector<std::pair<PieceID, uint8_t>> v_white_piece = _PieceSet::to_set(v_wp);
                std::vector<std::pair<PieceID, uint8_t>> v_black_piece = _PieceSet::to_set(v_bp);
                _PieceSet ps(v_white_piece, v_black_piece);
                return new DomainTB< PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(partition_key, ps);
            }
            return nullptr;
        }

        static const std::string getDomainName(eDomainName e)
        {
            if (e == eDomainName::KvK)  return "DomainKvK";
            else if (e == eDomainName::KQvK) return "DomainKQvK";
            return "";
        }

        void attach_domain_playerW(_DomainPlayer* player) { _attached_domain_playerW = player; }
        void attach_domain_playerB(_DomainPlayer* player) { _attached_domain_playerB = player; }

    protected:
        bool save_root() const
        {
            std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("domain", persist_key());
            std::ofstream   is;
            is.open(f.c_str(), std::ofstream::out | std::ofstream::trunc);
            if (save_detail(is))
            {
                is.close();
                return true;
            }
            is.close();
            return false;
        }

        virtual bool save_detail(std::ofstream& os) const
        {
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
                return true;
            }
            return false;
        }

        bool load_root() const
        {
            std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("domain", persist_key());
            std::ifstream   is;
            is.open(f.c_str(), std::fstream::in);
            if (load_detail(is))
            {
                is.close();
                return true;
            }
            is.close();
            return false;
        }

        virtual bool load_detail(std::ifstream& is) const
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
                        _Domain* p_dom_child = p_partition->find_domain(domain_key(domainname_key,instance_key));
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
                return true;
            }
            return false;
        }


    public:
        bool add_child(_Domain* p)
        {
            for (size_t i = 0; i < _children.size(); i++)
            {
                // check instance...
                if (_children[i]->_domainname_key == p->_domainname_key) 
                    return true;
            }
            _children.push_back(p);
            return true;
        }

        Domain* get_child(size_t idx) const
        {
            if ((idx>=0) && (idx < _children.size())) return _children.at(idx);
            return nullptr;
        }

        size_t size_children() { return _children.size(); }
    };

    // DomainKvK
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class DomainKvK : public Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board  = Board<PieceID, _BoardSize>;
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Move = Move<PieceID>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature_isOppositeKinCheck = ConditionFeature_isOppositeKinCheck<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_numberMoveForPiece = ValuationFeature_numberMoveForPiece<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        friend class _DomainPlayer;

    public:
        DomainKvK(const std::string partition_key) 
            : _Domain(partition_key, _Domain::getDomainName(eDomainName::KvK), "0") {}

        bool isInDomain(const _Board& position) const override 
        { 
            if ( (position.cnt_piece(PieceName::K, PieceColor::W) == 1) &&
                 (position.cnt_piece(PieceName::K, PieceColor::B) == 1) &&
                 (position.cnt_all_piece() == 2))
                return true;
            return false;
        }

        bool is_cond_feature_valid(const _ConditionFeature& f) const override
        {
            //...
            return true;
        }
        bool is_valu_feature_valid(const _ValuationFeature& f) const override
        {
            if (f.name() == ValuFeatureName::eValuationFeature_distKK) return true;

            if ((f.name() == ValuFeatureName::eValuationFeature_numberMoveForPiece) ||
                (f.name() == ValuFeatureName::eValuationFeature_onEdge))
            {
                _ValuationFeature_numberMoveForPiece& r = (_ValuationFeature_numberMoveForPiece&)f;
                if (r.piecename() == PieceName::K) return true;
                return false;
            }
            return true;
        }

        virtual bool has_known_score_move() const { return true; }
        virtual ExactScore get_known_score_move(const _Board& position, const std::vector<_Move>& m, size_t& ret_mv_idx) const
        { 
            if (position.allow_self_check())
            {
                if (position.can_capture_opposite_king(m, ret_mv_idx))
                {
                    if (position.get_color() == PieceColor::W) return ExactScore::WIN;
                    else if (position.get_color() == PieceColor::B) return ExactScore::LOSS;
                }
               // else  return ExactScore::DRAW; // must know also the move
            }
            // Lookup score in TB if exist...
            return ExactScore::UNKNOWN;
        }

        virtual bool save() const override
        {
            return save_root();
        }
        virtual bool load() override 
        {
            return load_root();
        }

        _Board get_random_position(bool no_check) const override
        {     
            uint8_t wK = 0; 
            uint8_t bK = 0;
            while (wK == bK)
            {
                // TODO: replace rand() by unbiases rnd...
                wK = (std::rand() % _BoardSize*_BoardSize);
                bK = (std::rand() % _BoardSize*_BoardSize);
            }
            _Board b; 
            b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::W), wK % _BoardSize, ((uint8_t)(wK / _BoardSize)));
            b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::B), bK % _BoardSize, ((uint8_t)(bK / _BoardSize)));
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
                    if (b.can_capture_opposite_king(m, mv))
                        return get_random_position(no_check);
                }
            }
            return b;
        }
    };

    // DomainKQvK
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class DomainKQvK : public Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Move = Move<PieceID>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature_isOppositeKinCheck = ConditionFeature_isOppositeKinCheck<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_numberMoveForPiece = ValuationFeature_numberMoveForPiece<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        friend class _DomainPlayer;

    public:
        DomainKQvK(const std::string partition_key) 
            : _Domain(partition_key, _Domain::getDomainName(eDomainName::KQvK), "0")
        {
        }

        bool isInDomain(const _Board& position) const override
        {
            if ((position.cnt_piece(PieceName::K, PieceColor::W) == 1) &&
                (position.cnt_piece(PieceName::K, PieceColor::B) == 1) &&
                (position.cnt_piece(PieceName::Q, PieceColor::W) == 1) &&
                (position.cnt_all_piece() == 3))
                return true;
            return false;
        }

        bool is_cond_feature_valid(const _ConditionFeature& f) const override
        {
            //...
            return true;
        }
        bool is_valu_feature_valid(const _ValuationFeature& f) const override
        {
            if (f.name() == ValuFeatureName::eValuationFeature_distKK) return true;

            if ( (f.name() == ValuFeatureName::eValuationFeature_numberMoveForPiece)||
                 (f.name() == ValuFeatureName::eValuationFeature_onEdge))
            {
                _ValuationFeature_numberMoveForPiece& r = (_ValuationFeature_numberMoveForPiece&)f;
                if      (r.piecename() == PieceName::K) return true;
                else if ((r.piecename() == PieceName::Q) && (r.piececolor() == PieceColor::W)) return true;
                return false;
            }
            return true;
        }

        virtual bool has_known_score_move() const { return true; }
        virtual ExactScore get_known_score_move(const _Board& position, const std::vector<_Move>& m, size_t& ret_mv_idx) const
        {
            if (position.allow_self_check())
            {
                if (position.can_capture_opposite_king(m, ret_mv_idx))
                {
                    if (position.get_color() == PieceColor::W) return ExactScore::WIN;
                    else if (position.get_color() == PieceColor::B) return ExactScore::LOSS;
                }
            }
            // Lookup score in TB if exist...
            return ExactScore::UNKNOWN;
        }

        virtual bool save() const override  
        {
            return save_root();
        }
        virtual bool load() override
        {
            return load_root();
        }

        _Board get_random_position(bool no_check) const override
        {
            uint8_t wQ = 0;
            uint8_t wK = 0;
            uint8_t bK = 0;
            while ((wK == bK)||(wK==wQ)||(bK==wQ))
            {
                wQ = (std::rand() % (_BoardSize*_BoardSize));
                wK = (std::rand() % (_BoardSize*_BoardSize));
                bK = (std::rand() % (_BoardSize*_BoardSize));
            }
            _Board b;
            b.set_pieceid_at(_Piece::get_id(PieceName::Q, PieceColor::W), wQ % _BoardSize, ((uint8_t)(wQ / _BoardSize)));
            b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::W), wK % _BoardSize, ((uint8_t)(wK / _BoardSize)));
            b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::B), bK % _BoardSize, ((uint8_t)(bK / _BoardSize)));
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
    };

};

#endif
