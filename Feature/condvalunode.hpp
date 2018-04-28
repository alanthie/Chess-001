#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// ConditionValuationNode  : Binary tree of ConditionFeature and their attached ValuationFeature formula
//                           Brain of player to evolve
//
// Model
//  Fc = conditional term = [Fcond() and/or Fcond() and/or ...] supported by cond_product_boolean algo
//  Fv =   valuation term = Scoring classifier/predictor algorithms
//                          valu_weight_sum algo = Sigmoid([Fval()*w + Fval()*w + ...]) range(0 to 1) 0==Black_winning 1==white_winning
//                          binary classifier: rvm_trainer
//                          binary classifier: svm
//                          multi class classifier: one_vs_all_trainer
//                          ...
//
//
#ifndef _AL_CHESS_FEATURE_CONDVALNODE_HPP
#define _AL_CHESS_FEATURE_CONDVALNODE_HPP

namespace chess
{
    inline float  sigmoid(float x, float   a = 0.001f)  { return (1 / (1 + std::exp(-a * x))); }
    inline double sigmoid(double x, double a = 0.001)   { return (1 / (1 + std::exp(-a * x))); }

    // ConditionValuationNode - Brain nodes of a player
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ConditionValuationNode
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _BaseFeature = BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PlayerFactory = PlayerFactory<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _CondValuNodeChanger = CondValuNodeChanger<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        friend class _PlayerFactory;
        friend class _DomainPlayer;
        friend class _CondValuNodeChanger;

        using _FeatureAlgo = FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureCondAlgo = FeatureCondAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureValuAlgo = FeatureValuAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgoConfig = FeatureAlgoConfig<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureValuAlgo_weight_sum = FeatureValuAlgo_weight_sum<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureCondAlgo_cond_product_boolean = FeatureCondAlgo_cond_product_boolean<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        const std::string stringNULLKEY = "NULLKEY";

    protected:
        bool                            _is_positive_node;
        ConditionValuationNode*         _parent;
        ConditionValuationNode*         _positive_child;
        ConditionValuationNode*         _negative_child;                // Miror node - reverse the condition value
        ConditionValuationNode*         _link_to_mirror;
        std::string                     _persist_key;

        bool                            _use_current_cond_algo;         // else use _test_cond_algo
        bool                            _use_current_valu_algo;         // else use _test_valu_algo
        _FeatureCondAlgo*               _current_cond_algo;
        _FeatureValuAlgo*               _current_valu_algo;
        _FeatureCondAlgo*               _test_cond_algo;                // not persisting - for training/testing
        _FeatureValuAlgo*               _test_valu_algo;                // not persisting - for training/testing

        // Fcond(0)/Fvalu(0) + !Fcond(0)/Fvalu(1) 
        std::list<_FeatureAlgoConfig>   _history_positive_child_cond_algo_cfg;
        std::list<_FeatureAlgoConfig>   _history_negative_child_cond_algo_cfg;
        std::list<_FeatureAlgoConfig>   _history_positive_child_valu_algo_cfg;
        std::list<_FeatureAlgoConfig>   _history_negative_child_valu_algo_cfg;

        // Fcond(0)/Fvalu(0)
        std::list<_FeatureAlgoConfig>   _history_cond_algo_cfg;
        std::list<_FeatureAlgoConfig>   _history_valu_algo_cfg;

    public:
        ConditionValuationNode(ConditionValuationNode* parent, bool _is_positive_node, bool create_children) :
            _is_positive_node(_is_positive_node),
            _parent(parent),
            _positive_child(nullptr),
            _negative_child(nullptr),
            _link_to_mirror(nullptr),
            _persist_key(""),
            _use_current_cond_algo(true),
            _use_current_valu_algo(true),
            _current_cond_algo(nullptr),
            _current_valu_algo(nullptr),
            _test_cond_algo(nullptr),
            _test_valu_algo(nullptr)
        {
            _persist_key = PersistManager<PieceID, _BoardSize>::instance()->create_persist_key();
            if (create_children)
            {
                this->_positive_child = new ConditionValuationNode(this, true, false);
                this->_negative_child = new ConditionValuationNode(this, false, false);
                this->_positive_child->_link_to_mirror = _negative_child;
                this->_negative_child->_link_to_mirror = _positive_child;

                if (_parent != nullptr)
                {
                    if (_is_positive_node)
                    {
                        if (_parent->_positive_child != nullptr)
                        {
                            delete _parent->_positive_child;
                        }
                        _parent->_positive_child = this;
                    }
                    else
                    {
                        if (_parent->_negative_child != nullptr)
                        {
                            delete _parent->_negative_child;
                        }
                        _parent->_negative_child = this;
                    }
                }
            }
            else if (_parent != nullptr)
            {
                if (_is_positive_node)
                {
                    _parent->_positive_child = this;
                    if (_parent->_negative_child != nullptr)
                    {
                        _parent->_positive_child->_link_to_mirror = _parent->_negative_child;
                        _parent->_negative_child->_link_to_mirror = _parent->_positive_child;
                    }
                }
                else
                {
                    _parent->_negative_child = this;
                    if (_parent->_positive_child != nullptr)
                    {
                        _parent->_negative_child->_link_to_mirror = _parent->_positive_child;
                        _parent->_positive_child->_link_to_mirror = _parent->_negative_child;
                    }
                }
            }
        }

        virtual ~ConditionValuationNode()
        {
            // recursion to delete full branch
            try
            {
                if (_current_cond_algo != nullptr) { delete _current_cond_algo;    _current_cond_algo = nullptr; }
                if (_current_valu_algo != nullptr) { delete _current_valu_algo;    _current_valu_algo = nullptr; }
                if (_test_cond_algo != nullptr) { delete _test_cond_algo;       _test_cond_algo = nullptr; }
                if (_test_valu_algo != nullptr) { delete _test_valu_algo;       _test_valu_algo = nullptr; }

                if (_positive_child != nullptr)
                {
                    delete _positive_child;
                    _positive_child = nullptr;
                }
                if (_negative_child != nullptr)
                {
                    delete _negative_child;
                    _negative_child = nullptr;
                }
            }
            catch (...)
            {
                assert(false);
            }
        }

        ConditionValuationNode*  parent() const { return _parent; }
        bool is_positive_node() { return _is_positive_node;  }

        // isRoot
        bool isRoot() const {return (_parent == nullptr);}

        // get_condition_value - eval conditions on a board position
        bool get_condition_value(const _Board& position, const std::vector<_Move>& m) const
        {
            bool use_mirror_node = false;
            if (!_is_positive_node)
            {
                if (_link_to_mirror != nullptr)  use_mirror_node = true;
                else
                {
                    assert(false);
                    return false;
                }
            }

            if (_use_current_cond_algo)
            {
                assert(_current_cond_algo != nullptr);

                if (use_mirror_node)
                    return _link_to_mirror->_current_cond_algo->get_condition_value(isRoot(), _is_positive_node, position, m);
                else
                    return _current_cond_algo->get_condition_value(isRoot(), _is_positive_node, position, m);
            }
            else
            {
                assert(_test_cond_algo != nullptr);

                if (use_mirror_node)
                    return _link_to_mirror->_test_cond_algo->get_condition_value(isRoot(), _is_positive_node, position, m);
                else
                    return _test_cond_algo->get_condition_value(isRoot(), _is_positive_node, position, m);
            }
        }

        // get_terminal_node
        _ConditionValuationNode* get_terminal_node(const _Board& position, const std::vector<_Move>& m) const
        {
            // if on a node it means the composite conditions to reach this node was true;
            if ((_positive_child != nullptr) && (_negative_child != nullptr))
            {
                bool cond_child = _positive_child->get_condition_value(position, m);
                if (cond_child) return _positive_child->get_terminal_node(position, m); // recursion - following conditional path until terminal
                else            return _negative_child->get_terminal_node(position, m); // recursion
            }
            else if ((_positive_child == nullptr) && (_negative_child == nullptr))
            {
                return (_ConditionValuationNode*)this;
            }

            // error
            assert(false);
            return nullptr;
        }

        // get_path
        std::vector<_ConditionValuationNode*> get_path() 
        {
            _ConditionValuationNode* p = this;
            std::vector<_ConditionValuationNode*> v;
            if (p == nullptr) return v;

            v.push_back(p);
            while (p != nullptr)
            {
                if (p->parent() != nullptr)
                {
                    v.push_back(p->parent());
                    p = p->parent();
                }
                else
                {
                    break;
                }
            }
            return v;
        }

        bool save_root() const;
        bool save(std::ofstream& is) const;
        bool load_root();
        bool load(std::ifstream& is);

        // get_valuations_value - compute a valuation on the board position
        TYPE_PARAM get_valuations_value(const _Board& position, const std::vector<_Move>& m, char verbose, std::stringstream& verbose_stream) const
        {
            if (_use_current_valu_algo)
            {
                if (_current_valu_algo == nullptr) assert(false);
                return _current_valu_algo->get_valuations_value(position, m, verbose, verbose_stream);
            }
            else
            {
                if (_test_valu_algo == nullptr) assert(false);
                return _test_valu_algo->get_valuations_value(position, m, verbose, verbose_stream);
            }
        }

        // eval_position - find the assoicate terminal node (of a board position) and compute the valuation
        bool eval_position(const _Board& position, const std::vector<_Move>& m, TYPE_PARAM& ret_eval, char verbose, std::stringstream& verbose_stream) const
        {
            _ConditionValuationNode* terminal_node = get_terminal_node(position, m);
            if (terminal_node == nullptr)
            {
                return false;
            }
            ret_eval = terminal_node->get_valuations_value(position, m, verbose, verbose_stream);
            return true;
        }

        std::string persist_key() const { return _persist_key; }
        void set_persist_key(std::string s) { _persist_key = s; }

        ConditionValuationNode* positive_child() const { return _positive_child; }
        ConditionValuationNode* negative_child() const { return _negative_child; }

        // get_term_nodes - Ex: GA algo will modify weights of the player terminal nodes
        void get_term_nodes(std::vector<_ConditionValuationNode*>& v)
        {
            if ((_positive_child == nullptr) && (_negative_child == nullptr))
            {
                v.push_back(this);
            }
            else
            {
                if (_positive_child != nullptr)
                {
                    _positive_child->get_term_nodes(v);
                }
                if (_negative_child != nullptr)
                {
                    _negative_child->get_term_nodes(v);
                }
            }
        }

        _FeatureCondAlgo*   current_cond_algo() { return _current_cond_algo; }
        _FeatureValuAlgo*   current_valu_algo() { return _current_valu_algo; }
        _FeatureCondAlgo*   test_cond_algo()    { return _test_cond_algo; }
        _FeatureValuAlgo*   test_valu_algo()    { return _test_valu_algo; }

        void update_child_cond_valu_algo(bool only_hist = false)
        {
            if (_positive_child != nullptr)
            {
                if (_positive_child->_test_cond_algo != nullptr)
                    _history_positive_child_cond_algo_cfg.push_back(_positive_child->_test_cond_algo->cfg());
                if (_positive_child->_test_cond_algo != nullptr)
                    _history_negative_child_cond_algo_cfg.push_back(_positive_child->_test_cond_algo->cfg()) // mirror
            }
            if (_negative_child != nullptr)
            {
                if (_positive_child->_test_valu_algo != nullptr)
                    _history_positive_child_valu_algo_cfg.push_back(_positive_child->_test_valu_algo->cfg());
                if (_negative_child->_test_valu_algo != nullptr)
                    _history_negative_child_valu_algo_cfg.push_back(_negative_child->_test_valu_algo->cfg());
            }

            if (!only_hist)
            {
                if (_positive_child != nullptr)
                {
                    _positive_child->update_cond_algo();
                    _positive_child->update_valu_algo();
                }
                if (_negative_child != nullptr)
                {
                    _negative_child->update_cond_algo();
                    _negative_child->update_valu_algo();
                }
            }
        }

        void clear_child_cond_valu_algo()
        {
            if (_positive_child != nullptr)
            {
                _positive_child->clear_test_cond_algo();
                _positive_child->clear_test_valu_algo();
            }
            if (_negative_child != nullptr)
            {
                _negative_child->clear_test_cond_algo();
                _negative_child->clear_test_valu_algo();
            }
        }

        void update_cond_algo()
        {
            if (_test_cond_algo == nullptr)
            {
                _use_current_cond_algo = true;
                return;
            }

            if (_current_cond_algo != nullptr) {
                delete _current_cond_algo; 
                _current_cond_algo = nullptr;
            }
            _current_cond_algo = _test_cond_algo; //std::move()...

            _use_current_cond_algo = true;
            _history_cond_algo_cfg.push_back(_test_cond_algo->cfg());
            _test_cond_algo = nullptr;
        }

        void clear_test_cond_algo()
        {
            if (_test_cond_algo != nullptr)
            {
                delete _test_cond_algo;
                _test_cond_algo = nullptr;
            }
            _use_current_cond_algo = true;
        }

        void set_test_cond_algo(_FeatureCondAlgo* algo)
        {
            if (algo == nullptr) return;
            if (_test_cond_algo != nullptr)
            {
                delete _test_cond_algo;
                _test_cond_algo = nullptr;
            }
            _test_cond_algo = algo;
            _use_current_cond_algo = false;
        }

        void update_valu_algo()
        {
            if (_test_valu_algo == nullptr)
            {
                _use_current_valu_algo = true;
                return;
            }

            if (_current_valu_algo != nullptr) {
                delete _current_valu_algo; 
                _current_valu_algo = nullptr;
            }
            _current_valu_algo = _test_valu_algo;   //std::move()...

            _use_current_valu_algo = true;
            _history_valu_algo_cfg.push_back(_test_valu_algo->cfg());
            _test_valu_algo = nullptr;
        }

        void clear_test_valu_algo()
        {
            if (_test_valu_algo != nullptr)
            {
                delete _test_valu_algo;
                _test_valu_algo = nullptr;
            }
            _use_current_valu_algo = true;
        }

        void set_test_valu_algo(_FeatureValuAlgo* algo)
        {
            if (algo == nullptr) return;

            if (_test_valu_algo != nullptr)
            {
                delete _test_valu_algo;
                _test_valu_algo = nullptr;
            }
            _test_valu_algo = algo;
            _use_current_valu_algo = false;
        }

    };

    // save_root()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save_root() const
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("ConditionValuationNode", _persist_key);
        std::ofstream   is;
        is.open(f.c_str(), std::fstream::out | std::fstream::trunc);
        if (save(is))
        {
            is.close();
            return true;
        }
        is.close();
        return false;
    }

    // save()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save(std::ofstream& is) const
    {
        if (is.good())
        {
            is << _persist_key;                         is << " ";
            is << bool_to_int(_is_positive_node);       is << " ";

            if (_current_cond_algo != nullptr)
            {
                is << _current_cond_algo->cfg()._persist_key; is << " ";
                _current_cond_algo->save_detail(is);
            }
            else { is << stringNULLKEY; is << " "; }

            if (_current_valu_algo != nullptr)
            {
                is << _current_valu_algo->cfg()._persist_key; is << " ";
                _current_valu_algo->save_detail(is);
            }
            else { is << stringNULLKEY; is << " "; }

            is << _history_positive_child_cond_algo_cfg.size();        is << " ";
            for (auto& v : _history_positive_child_cond_algo_cfg)      v.save_detail(is);
            is << _history_negative_child_cond_algo_cfg.size();        is << " ";
            for (auto& v : _history_negative_child_cond_algo_cfg)      v.save_detail(is);
            is << _history_positive_child_valu_algo_cfg.size();        is << " ";
            for (auto& v : _history_positive_child_valu_algo_cfg)      v.save_detail(is);
            is << _history_negative_child_valu_algo_cfg.size();        is << " ";
            for (auto& v : _history_negative_child_valu_algo_cfg)      v.save_detail(is);

            is << _history_cond_algo_cfg.size();        is << " ";
            for (auto& v : _history_cond_algo_cfg)      v.save_detail(is);
            is << _history_valu_algo_cfg.size();        is << " ";
            for (auto& v : _history_valu_algo_cfg)      v.save_detail(is);

            if (_parent != nullptr)                     { is << _parent->_persist_key;          is << " "; } else { is << stringNULLKEY; is << " "; }
            if (_positive_child != nullptr)             { is << _positive_child->_persist_key;  is << " "; } else { is << stringNULLKEY; is << " "; }
            if (_negative_child != nullptr)             { is << _negative_child->_persist_key;  is << " "; } else { is << stringNULLKEY; is << " "; }
            if (_link_to_mirror != nullptr)             { is << _link_to_mirror->_persist_key;  is << " "; } else { is << stringNULLKEY; is << " "; }            
            if (_positive_child != nullptr)             _positive_child->save(is);  // recursion
            if (_negative_child != nullptr)             _negative_child->save(is);  // recursion

            is.close();
            return true;
        }
        is.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load_root()
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("ConditionValuationNode", _persist_key);
        std::ifstream   is;
        is.open(f.c_str(), std::fstream::in);
        if (load(is))
        {
            is.close();
            return true;
        }
        is.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load(std::ifstream& is)
    {
        if (is.good())
        {
            std::string persist_key;    
            is >> persist_key;
            assert(persist_key == _persist_key);

            int c; 
            is >> c; 
            _is_positive_node = int_to_bool(c);

            std::string algo_cfg_persist_key;

            {
                is >> algo_cfg_persist_key;
                if (algo_cfg_persist_key != stringNULLKEY)
                {
                    _FeatureAlgoConfig cfg;
                    cfg.load_detail(is);
                    if (_current_cond_algo != nullptr) { delete _current_cond_algo; _current_cond_algo = nullptr; }
                    _current_cond_algo = (_FeatureCondAlgo*)_FeatureAlgo::create(cfg);
                    if (_current_cond_algo == nullptr) return false;

                    assert(algo_cfg_persist_key == cfg._persist_key);
                }
                _current_cond_algo->load_detail(is);
            }

            {
                is >> algo_cfg_persist_key;
                if (algo_cfg_persist_key != stringNULLKEY)
                {
                    _FeatureAlgoConfig cfg;
                    cfg.load_detail(is);
                    if (_current_valu_algo != nullptr) { delete _current_valu_algo; _current_valu_algo = nullptr;}
                    _current_valu_algo = (_FeatureValuAlgo*)_FeatureAlgo::create(cfg);
                    if (_current_valu_algo == nullptr) return false;
                    _current_valu_algo->load_detail(is);

                    assert(algo_cfg_persist_key == cfg._persist_key);
                }
            }
            size_t n;

            is >> n;
            for (size_t i = 0; i< n; i++)
            {
                _FeatureAlgoConfig cfg;
                cfg.load_detail(is);
                _history_positive_child_cond_algo_cfg.push_back(cfg);
            }
            is >> n;
            for (size_t i = 0; i< n; i++)
            {
                _FeatureAlgoConfig cfg;
                cfg.load_detail(is);
                _history_negative_child_cond_algo_cfg.push_back(cfg);
            }
            is >> n;
            for (size_t i = 0; i< n; i++)
            {
                _FeatureAlgoConfig cfg;
                cfg.load_detail(is);
                _history_positive_child_valu_algo_cfg.push_back(cfg);
            }
            is >> n;
            for (size_t i = 0; i< n; i++)
            {
                _FeatureAlgoConfig cfg;
                cfg.load_detail(is);
                _history_negative_child_valu_algo_cfg.push_back(cfg);
            }

            is >> n;
            for (size_t i = 0; i< n; i++)
            {
                _FeatureAlgoConfig cfg;
                cfg.load_detail(is);
                _history_cond_algo_cfg.push_back(cfg);
            }

            is >> n;
            for (size_t i = 0; i< n; i++)
            {
                _FeatureAlgoConfig cfg;
                cfg.load_detail(is);
                _history_valu_algo_cfg.push_back(cfg);
            }

            std::string _parent_persist_key;
            std::string _positive_child_persist_key;
            std::string _negative_child_persist_key;
            std::string _link_to_mirror_persist_key;

            is >> _parent_persist_key;
            is >> _positive_child_persist_key;
            is >> _negative_child_persist_key;
            is >> _link_to_mirror_persist_key;

            std::map<std::string, ConditionValuationNode*> map_nodes;
            if (_parent_persist_key == stringNULLKEY)
            {
                _parent = nullptr;
            }
            else
            {
                if (map_nodes[_parent_persist_key] != nullptr)
                {
                    _parent = map_nodes[_parent_persist_key];
                }
            }

            if (_positive_child_persist_key == stringNULLKEY)
            {
                _positive_child = nullptr;
            }
            else
            {
                _positive_child = new ConditionValuationNode(_parent, true, false);
                map_nodes[_positive_child_persist_key] = _positive_child;
                _positive_child->_persist_key = _positive_child_persist_key;
                if (map_nodes[_parent_persist_key] != nullptr)
                {
                    _positive_child->_parent = map_nodes[_parent_persist_key];
                    _positive_child->_parent->_positive_child = _positive_child;
                }
            }

            if (_negative_child_persist_key == stringNULLKEY)
            {
                _negative_child = nullptr;
            }
            else
            {
                _negative_child = new ConditionValuationNode(_parent, false, false);
                map_nodes[_negative_child_persist_key] = _negative_child;
                _negative_child->_persist_key = _negative_child_persist_key;
                if (map_nodes[_parent_persist_key] != nullptr)
                {
                    _negative_child->_parent = map_nodes[_parent_persist_key];
                    _negative_child->_parent->_negative_child = _negative_child;
                }
            }

            if (_link_to_mirror_persist_key == stringNULLKEY)
            {
                _link_to_mirror = nullptr;
            }
            else
            {
                if (map_nodes[_link_to_mirror_persist_key] != nullptr)
                {
                    _link_to_mirror = map_nodes[_link_to_mirror_persist_key];
                }
            }

            if (_positive_child != nullptr)
            {
                _positive_child->load(is);
            }
            if (_negative_child != nullptr)
            {
                _negative_child->load(is);
            }

            // recheck
            if ((_parent_persist_key != stringNULLKEY) && (_parent == nullptr))
            {
                if (map_nodes[_parent_persist_key] != nullptr)
                {
                    _parent = map_nodes[_parent_persist_key];
                }
            }
            if ((_positive_child_persist_key != stringNULLKEY) && (_positive_child == nullptr))
            {
                if (map_nodes[_positive_child_persist_key] != nullptr)
                {
                    _positive_child = map_nodes[_positive_child_persist_key];
                }
            }
            if ((_negative_child_persist_key != stringNULLKEY) && (_negative_child == nullptr))
            {
                if (map_nodes[_negative_child_persist_key] != nullptr)
                {
                    _negative_child = map_nodes[_negative_child_persist_key];
                }
            }
            if ((_link_to_mirror_persist_key != stringNULLKEY) && (_link_to_mirror == nullptr))
            {
                if (map_nodes[_link_to_mirror_persist_key] != nullptr)
                {
                    _link_to_mirror = map_nodes[_link_to_mirror_persist_key];
                }
            }

            return true;
        }
        return false;
    }

};

#endif

