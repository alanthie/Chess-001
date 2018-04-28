#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// PlayerFactory    : Handle modification to player brain
//
//
#ifndef _AL_CHESS_PLAYER_PLAYERFACTORY_HPP
#define _AL_CHESS_PLAYER_PLAYERFACTORY_HPP

namespace chess
{
    // REPLACED by CondValuNodeChanger...
    //
    // Conditions now in cond_product_boolean algorithm
    // GA weight now in FeatureValuAlgo_weight_sum algorithm

    // Ways to modify player root tree
    enum class CondFeatureSelection { none, one_random, one_at_index, all };    // ex: add one random cond feature to a player node
    enum class ValuFeatureSelection { none, one_random, one_at_index, all };

    // PlayerFactoryConfig
    struct PlayerFactoryConfig
    {
        CondFeatureSelection cond_s;
        ValuFeatureSelection valu_s;
        bool cascading;
        std::string node_persist_key;   // Node ID to modify - on player root
        // if cascading, only modify root node of child players [Cascading mostly for initial root node creation]
    };

    // PlayerFactory
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class PlayerFactory
    {
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PlayerFactory = PlayerFactory<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureManager = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PartitionManager = PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    private:
        PlayerFactory() {}
        PlayerFactory(const PlayerFactory&) = delete;
        PlayerFactory & operator=(const PlayerFactory &) = delete;
        PlayerFactory(PlayerFactory&&) = delete;

    public:
        ~PlayerFactory()
        {
            if (_instance.operator bool())
            {
                _instance.release();
            }
        }

        static const PlayerFactory* instance()
        {
            if (!_instance.operator bool())
            {
                _instance = std::unique_ptr<PlayerFactory>(new PlayerFactory);
            }
            return _instance.get();
        }

        // TODO: Generalize to any node , not just root ...
        // reconfigPlayerRootNode
        bool reconfigPlayerRootNode(_DomainPlayer* player, const PlayerFactoryConfig& cfg) const
        {
            if (player == nullptr) return true;

            size_t nc = _FeatureManager::instance()->count_cond_features();
            size_t nv = _FeatureManager::instance()->count_valu_features();
            if ((cfg.cond_s != CondFeatureSelection::none) && (nc == 0)) return false;
            if ((cfg.valu_s != ValuFeatureSelection::none) && (nv == 0)) return false;

            // _ConditionValuationNode* node_to_modify = player->get_node(cfg.node_persist_key)...
            if (player->get_root()->_positive_child != nullptr)
            {
                delete player->get_root()->_positive_child;
                player->get_root()->_positive_child = nullptr;
            }
            if (player->get_root()->_negative_child != nullptr)
            {
                delete player->get_root()->_positive_child;
                player->get_root()->_positive_child = nullptr;
            }

            //if (cfg.cond_s == CondFeatureSelection::one_random) // Get a random CondFeature
            //{
            //    _ConditionValuationNode* child1 = new _ConditionValuationNode(player->get_root(), true, false);
            //    _ConditionValuationNode* child2 = new _ConditionValuationNode(player->get_root(), false, false);

            //    if (cfg.valu_s == ValuFeatureSelection::all)    // Get all ValuFeature
            //    {
            //        _ValuationFeature* v;
            //        size_t nv = _FeatureManager::instance()->count_valu_features();
            //        for (size_t i = 0; i < nv; i++)
            //        {
            //            v = _FeatureManager::instance()->get_valu_feature(i);
            //            if (player->domain()->is_valu_feature_valid(*v))
            //            {
            //                child1->add_valuations(_FeatureManager::instance()->get_valu_feature(i));
            //                child1->add_weights(1.0);
            //            }
            //        }
            //        for (size_t i = 0; i < nv; i++)
            //        {
            //            v = _FeatureManager::instance()->get_valu_feature(i);
            //            if (player->domain()->is_valu_feature_valid(*v))
            //            {
            //                child2->add_valuations(_FeatureManager::instance()->get_valu_feature(i));
            //                child2->add_weights(1.0);
            //            }
            //        }
            //        if (nc == 1)
            //        {
            //            child1->add_conditions(_FeatureManager::instance()->get_cond_feature(0));
            //            child1->add_conditions_and_or(true);
            //        }
            //        else
            //        {
            //            child1->add_conditions(_FeatureManager::instance()->get_cond_feature(std::rand() % nc));
            //            child1->add_conditions_and_or(true);
            //        }
            //        // child2 mirror node reuse child1 cond

            //        if (cfg.cascading)
            //        {
            //            if (!reconfig_children_players(player, cfg))    // recursion
            //            {
            //                //...
            //                return false;
            //            }
            //        }
            //        return true;
            //    }
            //    else
            //    {
            //        //...if (cfg.valu_s == ValuFeatureSelection::xyz)
            //    }
            //}
            //else if (cfg.cond_s == CondFeatureSelection::none)      // No extra CondFeature - Node will only have valuations - sum(Fv*w)
            //{
            //    if (cfg.valu_s == ValuFeatureSelection::all)        // Get all VF
            //    {
            //        _ValuationFeature* v;
            //        for (size_t i = 0; i < nv; i++)
            //        {
            //            v = _FeatureManager::instance()->get_valu_feature(i);
            //            if (player->domain()->is_valu_feature_valid(*v))
            //            {
            //                player->get_root()->add_valuations(_FeatureManager::instance()->get_valu_feature(i));
            //                player->get_root()->add_weights(1.0);
            //            }
            //        }
            //        if (cfg.cascading)
            //        {
            //            if (!reconfig_children_players(player, cfg))    // recursion
            //            {
            //                //... (undo)
            //                return false;
            //            }
            //        }
            //        return true;
            //    }
            //    else
            //    {
            //        //...if (cfg.valu_s == ValuFeatureSelection::xyz)
            //    }
            //}
            //else
            //{
            //    //... if (cfg.cond_s == CondFeatureSelection::xyz)
            //}         
            return false;
        }

        // reconfig_children_players
        bool reconfig_children_players(_DomainPlayer* player, const PlayerFactoryConfig& cfg) const
        {
            if (player == nullptr) return true;
            for (auto& v : player->_children_players)
            {
                reconfigPlayerRootNode(v, cfg);
                // recursion
            }
            return true;
        }

    private:
        static std::unique_ptr<PlayerFactory> _instance;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::unique_ptr<PlayerFactory<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>>
    PlayerFactory<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_instance = nullptr;
}
#endif
