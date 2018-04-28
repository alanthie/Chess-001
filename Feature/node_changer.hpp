#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// NodeChanger - Automating the discovery of better score classifier/predictor
//
//
#ifndef _AL_CHESS_FEATURE_CondValuNodeChanger_HPP
#define _AL_CHESS_FEATURE_CondValuNodeChanger_HPP

namespace chess
{
    // CondValuNodeChanger
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class CondValuNodeChanger
    {
        using _DomainPlayer     = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureManager   = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer     = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgo      = FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureCondAlgo  = FeatureCondAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureValuAlgo  = FeatureValuAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgoConfig = FeatureAlgoConfig<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        // Possible change:
        //  1 terminal node change Fvalu() - 1 test algo to train
        //  1 terminal node expand into 2 new childs :  Fcond(0)/Fvalu(0) + !Fcond(0)/Fvalu(1)  - 3 test algo to train simultaneously!
        //
    public:
        CondValuNodeChanger() {}
        virtual ~CondValuNodeChanger() {}

    private:
        // try_expand_terminal_node_cond stuff - not owner
        _ConditionValuationNode*    node_parent = nullptr;
        _ConditionValuationNode*    node_positive_child = nullptr;
        _ConditionValuationNode*    node_negative_child = nullptr;
        _FeatureCondAlgo*           algo_cond_positive_child = nullptr;
        _FeatureValuAlgo*           algo_valu_positive_child = nullptr;
        _FeatureValuAlgo*           algo_valu_negative_child = nullptr;
    
    public:
        // Try changing a player terminal node with 2 new child nodes[Fcond(0)/Fvalu(0) + !Fcond(0)/Fvalu(1)]
        bool try_expand_terminal_node_cond(_DomainPlayer& player, size_t max_size_dataset, bool& ret_node_changed);

        // Try changing a player terminal node[Fcond(0)]
        bool try_change_terminal_node_cond(_DomainPlayer& player, size_t max_size_dataset, bool commit_undo, bool& ret_node_changed);

        // Try changing a player terminal node[Fvalu(0)]
        bool try_change_terminal_node_valu(_DomainPlayer& player, size_t max_size_dataset, bool commit_undo, bool& ret_node_changed);

    protected:
        _ConditionValuationNode*    suggest_next_node_for_change(_DomainPlayer& player, bool changing_cond);
        _FeatureAlgo*               suggest_algo_for_change(_DomainPlayer& player, _ConditionValuationNode* node, bool changing_cond);

        // Cond changer
        _ConditionValuationNode* suggest_next_node_for_cond_change(_DomainPlayer& player)
        {
            return suggest_next_node_for_change(player, true);
        }

        _FeatureCondAlgo* suggest_algo_for_cond_change(_DomainPlayer& player, _ConditionValuationNode* node)
        {
            _FeatureAlgo* algo = suggest_algo_for_change(player, node, true);
            if (algo == nullptr) return nullptr;
            else return (_FeatureCondAlgo*)algo;
        }

        bool eval_algo_for_cond_change(_DomainPlayer& player, _ConditionValuationNode* node, _FeatureCondAlgo* algo, size_t max_size_dataset, bool& ret_node_can_change)
        {
            if (!node->set_test_cond_algo(algo))  return false;
            return algo->train_test_compare(player, node, max_size_dataset, ret_node_can_change);
        }

        // Valu changer
        _ConditionValuationNode* suggest_next_node_for_valu_change(_DomainPlayer& player)
        {
            return suggest_next_node_for_change(player, false);
        }

        _FeatureValuAlgo* suggest_algo_for_valu_change(_DomainPlayer& player, _ConditionValuationNode* node)
        {
            _FeatureAlgo* algo = suggest_algo_for_change(player, node, false);
            if (algo == nullptr) return nullptr;
            else return (_FeatureValuAlgo*)algo;
        }

        bool eval_algo_for_valu_change(_DomainPlayer& player, _ConditionValuationNode* node, _FeatureValuAlgo* algo, size_t max_size_dataset)
        {
            if (!node->set_test_valu_algo(algo))  return false;
            return algo->train_test_compare(player, node, max_size_dataset, ret_node_to_change);
        }

        void cleanup_try_expand_terminal_node_cond(_ConditionValuationNode* node_parent, bool undo = true)
        {
            if (undo)
            {
                clear_child_cond_valu_algo();

                if (node_parent->_positive_child != nullptr)
                {
                    delete node_parent->_positive_child;
                    node_parent->_positive_child = nullptr;
                }
                if (node_parent->_negative_child != nullptr)
                {
                    delete node_parent->_negative_child;
                    node_parent->_negative_child = nullptr;
                }
            }
            else
            {
            }
        }
    };

    // try_expand_terminal_node_cond
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool CondValuNodeChanger<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::
    try_expand_terminal_node_cond(_DomainPlayer& player, size_t max_size_dataset, bool& ret_node_changed)
    {
        node_parent = suggest_next_node_for_cond_change(player);
        if (node_parent == nullptr) return false;

        // expand
        node_positive_child = new _ConditionValuationNode(node_parent, true, false);
        node_negative_child = new _ConditionValuationNode(node_parent, false, false);

        // Fcond(0)
        algo_cond_positive_child = suggest_algo_for_cond_change(player, node_positive_child);
        if (algo_cond_positive_child == nullptr) { cleanup_try_expand_terminal_node_cond(node_parent); return false; }
        node_positive_child->set_test_cond_algo(algo_cond_positive_child);

        // Fvalu(0)
        algo_valu_positive_child = suggest_algo_for_valu_change(player, node_positive_child);
        if (algo_valu_positive_child == nullptr) { cleanup_try_expand_terminal_node_cond(node_parent); return false; }
        node_positive_child->set_test_valu_algo(algo_valu_positive_child);

        // Fvalu(1)
        algo_valu_negative_child = suggest_algo_for_valu_change(player, node_negative_child);
        if (algo_valu_negative_child == nullptr) { cleanup_try_expand_terminal_node_cond(node_parent); return false; }
        node_negative_child->set_test_valu_algo(algo_valu_negative_child);

        bool r[3];
        bool ret_node_can_change[3];

        // make multi-threads..
        r[0] = eval_algo_for_cond_change(player, node_positive_child, algo_cond_positive_child, max_size_dataset, ret_node_can_change[0]); // Fcond(0)
        if (!r[0]) { cleanup_try_expand_terminal_node_cond(node_parent); return false; }
        r[1] = eval_algo_for_valu_change(player, node_positive_child, algo_valu_positive_child, max_size_dataset, ret_node_can_change[1]); // Fvalu(0)
        if (!r[1]) { cleanup_try_expand_terminal_node_cond(node_parent); return false; }
        r[2] = eval_algo_for_valu_change(player, node_negative_child, algo_valu_negative_child, max_size_dataset, ret_node_can_change[2]); // Fvalu(1)
        if (!r[2]) { cleanup_try_expand_terminal_node_cond(node_parent); return false; }

        // compare if less error, commit_undo
        if (node_parent->current_cond_algo()->cfg()._test_dataset_average_error >=                          // may compute split error before algo testing... 0.10 == .08+.12/2
            std::max<double>(node_positive_child->test_cond_algo()->cfg()._test_dataset_average_error,      // if avg_better  [0.10 == .08+.12/2]  => .09+.10/2 (possible degrade in one child)
                node_negative_child->test_cond_algo()->cfg()._test_dataset_average_error))                  // if both_better [0.10 == .08+.12/2]  => .08+.11/2 ok all better
        {
            ret_node_changed = true;
            update_child_cond_valu_algo();

            node_positive_child->save_root();
            node_negative_child->save_root();
            node_parent->save_root();
            cleanup_try_expand_terminal_node_cond(node_parent, false);
        }
        else
        {
            // no improvement
            update_child_cond_valu_algo(true);

            node_parent->save_root(); // keep histo
            cleanup_try_expand_terminal_node_cond(node_parent, true);
        }
        return true;
    }

    // try_change_terminal_node_cond
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool CondValuNodeChanger<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::
    try_change_terminal_node_cond(_DomainPlayer& player, size_t max_size_dataset, bool commit_undo, bool& ret_node_changed)
    {
        ret_node_changed = false;
        _ConditionValuationNode* node = suggest_next_node_for_cond_change(player);
        if (node == nullptr) return false;

        _FeatureCondAlgo* algo = suggest_algo_for_cond_change(player, node);
        if (algo == nullptr) return false;
        if (!node->set_test_cond_algo(algo)) return false;

        bool ret_node_to_change;
        bool r = eval_algo_for_cond_change(player, node, algo, max_size_dataset, ret_node_to_change);
        if (r)
        {
            if (commit_undo)
            {
                if (ret_node_can_change)
                {
                    node->update_cond_algo();
                    ret_node_changed = true;
                    node->save_root();
                }
                else
                {
                    node->clear_test_cond_algo();
                }
            }
        }
        // extra cleanup... 
        return r;
    }

    // try_change_terminal_node_valu
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool CondValuNodeChanger<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::
    try_change_terminal_node_valu(_DomainPlayer& player, size_t max_size_dataset, bool commit_undo, bool& ret_node_changed)
    {
        ret_node_changed = false;
        _ConditionValuationNode* node = suggest_next_node_for_valu_change(player);  // Link - not owner
        if (node == nullptr) return false;

        _FeatureValuAlgo* algo = suggest_algo_for_valu_change(player, node);       // node owner
        if (algo == nullptr) return false;
        if (!node->set_test_valu_algo(algo)) return false;

        bool ret_node_can_change;
        bool r = eval_algo_for_valu_change(player, node, algo, max_size_dataset, ret_node_can_change);
        if (r)
        {
            if (commit_undo)
            {
                if (ret_node_can_change)
                {
                    node->update_valu_algo();
                    ret_node_changed = true;
                    node->save_root();
                }
                else
                {
                    node->clear_test_valu_algo();
                }
            }
        }
        // extra cleanup...            
        return r;
    }

    // suggest_next_node_for_change
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* CondValuNodeChanger<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::
    suggest_next_node_for_change(_DomainPlayer& player, bool changing_cond)
    {
        if ((player.get_root()->_positive_child == nullptr) && (player.get_root()->_negative_child == nullptr))
            return player.get_root();

        // select randomly 1 terminal node with above avg_error (so need improvement)
        std::vector<_ConditionValuationNode*> player_terminal_nodes;
        player.get_root()->get_term_nodes(player_terminal_nodes);
        if (player_terminal_nodes.size() == 0) return nullptr;
        if (player_terminal_nodes.size() == 1) return player_terminal_nodes[0];

        double sum_err = 0;
        double avg_err = 0;
        for (size_t i = 0; i < player_terminal_nodes.size(); i++)
        {
            if (changing_cond)
                sum_err += player_terminal_nodes[i]->current_cond_algo()->cfg()._test_dataset_average_error;
            else
                sum_err += player_terminal_nodes[i]->current_valu_algo()->cfg()._test_dataset_average_error;
        }
        avg_err = sum_err / (double)(player_terminal_nodes.size());

        size_t nlower = 0;
        size_t nabove = 0;
        for (size_t i = 0; i < player_terminal_nodes.size(); i++)
        {
            if (changing_cond)
            {
                if (player_terminal_nodes[i]->current_cond_algo()->cfg()._test_dataset_average_error >= avg_err) nabove++;
                else nlower++;
            }
            else
            {
                if (player_terminal_nodes[i]->current_valu_algo()->cfg()._test_dataset_average_error >= avg_err) nabove++;
                else nlower++;
            }
        }

        size_t cnt = 0;
        size_t idx = 0;
        bool found = false;
        size_t attempt = 0;
        while (true)
        {
            size_t r = (size_t)uniform_double(rnd_generator) * nabove;
            size_t cnt = 0;
            for (size_t i = 0; i < player_terminal_nodes.size(); i++)
            {
                if (changing_cond)
                {
                    if (player_terminal_nodes[i]->current_cond_algo()->cfg()._test_dataset_average_error >= avg_err) cnt++;
                }
                else
                {
                    if (player_terminal_nodes[i]->current_valu_algo()->cfg()._test_dataset_average_error >= avg_err) cnt++;
                }

                if (cnt == r)
                {
                    idx = i;
                    found = true;
                    break;
                }
            }
            attempt++;
            if (attempt > 200) break;
        }
        if (found)
        {
            return player_terminal_nodes[idx];
        }
        return nullptr;
    }

    // suggest_algo_for_change
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* CondValuNodeChanger<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::
    suggest_algo_for_change(_DomainPlayer& player, _ConditionValuationNode* node, bool changing_cond)
    {
        size_t nc;
        if (changing_cond)  nc = _FeatureManager::instance()->count_cond_features();
        else                nc = _FeatureManager::instance()->count_valu_features();
        if (nc == 0) return nullptr;

        std::vector<FeatureBasedAlgoName> v_algo_name;              // List of available algo
        size_t n = _FeatureManager::instance()->count_algo();
        for (size_t i = 0; i < n; i++)
        {
            if (changing_cond)
            {
                if (_FeatureAlgo::is_algo_cond(_FeatureManager::instance()->get_algo_name(i)))
                {
                    v_algo_name.push_back(_FeatureManager::instance()->get_algo_name(i));
                }
            }
            else
            {
                if (_FeatureAlgo::is_algo_valu(_FeatureManager::instance()->get_algo_name(i)))
                {
                    v_algo_name.push_back(_FeatureManager::instance()->get_algo_name(i));
                }
            }
        }
        if (v_algo_name.size() == 0) return nullptr;

        bool                            exist;
        size_t                          attempt = 0;
        _FeatureAlgoConfig              cfg;
        std::vector<std::string>        v_primitive_features;
        std::vector<_FeatureAlgoConfig> v_cfg;

        while (true)
        {
            attempt++;
            if (attempt > 200) break;

            // Select a feature set
            v_primitive_features.clear();
            for (size_t i = 0; i < nc; i++)
            {
                if (changing_cond)
                {
                    _ConditionFeature* f = _FeatureManager::instance()->get_cond_feature(i);
                    if (player.domain()->is_cond_feature_valid(*f))
                    {
                        if (uniform_double(rnd_generator) >= 0.50)
                        {
                            v_primitive_features.push_back(_FeatureManager::instance()->get_cond_feature_fullname(i));
                        }
                    }
                }
                else
                {
                    _ValuationFeature* f = _FeatureManager::instance()->get_valu_feature(i);
                    if (player.domain()->is_valu_feature_valid(*f))
                    {
                        if (uniform_double(rnd_generator) >= 0.50)
                        {
                            v_primitive_features.push_back(_FeatureManager::instance()->get_valu_feature_fullname(i));
                        }
                    }
                }
            }
            if (v_primitive_features.size() == 0) continue;

            v_cfg.clear();
            std::sort(v_primitive_features.begin(), v_primitive_features.end());

            for (size_t i = 0; i < v_algo_name.size(); i++)
            {
                cfg._is_cond = true;
                cfg._name = v_algo_name[i];
                cfg._param_key_signature = "";
                if (changing_cond)  cfg._cond_primitive_features = v_primitive_features;
                else                cfg._valu_primitive_features = v_primitive_features;

                // lookup not already in history
                exist = false;
                for (auto &p : node->_history_cond_algo_cfg)
                {
                    if (cfg.is_same(p))
                    {
                        exist = true;
                        break;
                    }
                }
                if (!exist)
                {
                    v_cfg.push_back(cfg);
                }
            }
            if (v_cfg.size() == 0) continue;

            size_t r = (size_t)uniform_double(rnd_generator) * v_cfg.size();
            if ((r >= 0) && (r < v_cfg.size()))
            {
                return _FeatureAlgo::create(v_cfg[r]);
            }
        }
        return nullptr;
    }

};

#endif
