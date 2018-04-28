#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// FeatureAlgo<...>     :
//
//
#ifndef _AL_CHESS_FEATURE_FeatureAlgo_HPP
#define _AL_CHESS_FEATURE_FeatureAlgo_HPP

namespace chess
{
    // FeatureAlgo
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class FeatureAlgo
    {
        using _Board            = Board<PieceID, _BoardSize>;
        using _Move             = Move<PieceID>;
        using _FeatureManager   = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer     = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _CondValuNodeChanger = CondValuNodeChanger<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgoConfig = FeatureAlgoConfig<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    private:
        _FeatureAlgoConfig     _cfg;

    public:
        FeatureAlgo(const _FeatureAlgoConfig& cfg) : _cfg(cfg)
        {
            _cfg._status = FeatureAlgoStatus::empty;
        }
        virtual ~FeatureAlgo() {}

        _FeatureAlgoConfig& cfg()               { return _cfg; }
        const _FeatureAlgoConfig& cfg() const   { return _cfg; }
        FeatureAlgoStatus& status()             { return _cfg._status; } 
        const FeatureAlgoStatus& status() const { return _cfg._status; }

        virtual bool prepare(   _DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) = 0;
        virtual bool train(     _DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset, char verbose) = 0;
        virtual bool test(      _DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) = 0;
        virtual bool compare(   _DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) = 0;
        virtual void cleanup(   _DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) = 0;

        virtual bool train_test_compare(_DomainPlayer& player,  _ConditionValuationNode* node, size_t max_size_dataset, char verbose, bool& ret_node_can_change)
        { 
            ret_node_can_change = false;

            if      (_cfg._status == FeatureAlgoStatus::none)   return false;
            else if (_cfg._status == FeatureAlgoStatus::ready)  return true;
            else if (_cfg._status == FeatureAlgoStatus::failed) return false;
            else if ((_cfg._status != FeatureAlgoStatus::empty) && (_cfg._status != FeatureAlgoStatus::pending))   return false;
            
            _cfg._status = FeatureAlgoStatus::building;
            try 
            {
                if (prepare(player, node, max_size_dataset))
                {
                    if (train(player, node, max_size_dataset, verbose))
                    {
                        if (test(player, node, max_size_dataset))
                        {
                            ret_node_can_change = compare(player, node, max_size_dataset);
                            if (ret_node_can_change)
                            {
                                _cfg._status = FeatureAlgoStatus::ready;
                                cleanup(player, node, max_size_dataset);
                                return true;
                            }
                            else
                            {
                                _cfg._status = FeatureAlgoStatus::pending;
                                cleanup(player, node, max_size_dataset);
                                return true;
                            }
                        }
                    }
                }
            }
            catch (std::exception& re)
            {
                std::stringstream ss_detail;
                ss_detail << re.what() << "\n";
                std::cerr << ss_detail.str();             
            }
            catch (...)
            {
            }
            _cfg._status = FeatureAlgoStatus::failed;
            cleanup(player, node, max_size_dataset);
            return false;
        }

        virtual bool save()                             const { return false; }
        virtual bool save_detail(std::ofstream& os)     const { return false; }
        virtual bool load()                             { return false; }
        virtual bool load_cfg(std::ifstream& is)        { return false; }
        virtual bool load_detail(std::ifstream& is)     { return false; }
  
        static bool is_algo_cond(FeatureBasedAlgoName n)
        {
            if (n == FeatureBasedAlgoName::cond_product_boolean) return true;
            return false;
        }
        static bool is_algo_valu(FeatureBasedAlgoName n)
        {
            if (n == FeatureBasedAlgoName::none) return false;
            return !is_algo_cond(n);
        }

        static FeatureAlgo* create(_FeatureAlgoConfig& cfg);

    protected:
        _ValuationFeature* read_valu(std::ifstream& is)
        {
            if (is.good())
            {
                FeatureType featureType;
                std::string classtype;
                std::string classtype_argument;

                int c; 
                is >> c;
                featureType = int_to_FeatureType(c);
                is >> classtype; fromNULLSTR(classtype);
                is >> classtype_argument; fromNULLSTR(classtype_argument);

                return _FeatureManager::instance()->get_valu_feature(classtype, classtype_argument);
            }
            return nullptr;
        }

        _ConditionFeature* read_cond(std::ifstream& is)
        {
            if (is.good())
            {
                FeatureType featureType;
                std::string classtype;
                std::string classtype_argument;

                int c; 
                is >> c;
                featureType = int_to_FeatureType(c);
                is >> classtype;            fromNULLSTR(classtype);
                is >> classtype_argument;   fromNULLSTR(classtype_argument);

                return _FeatureManager::instance()->get_cond_feature(classtype, classtype_argument);
            }
            return nullptr;
        }

    };

    // FeatureCondAlgo
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class FeatureCondAlgo : public FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _FeatureAlgo = FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgoConfig = FeatureAlgoConfig<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        FeatureCondAlgo(const _FeatureAlgoConfig& cfg) : _FeatureAlgo(cfg)
        {
        }
        ~FeatureCondAlgo() {}

        virtual bool get_condition_value(bool is_root, bool is_positive_node, const _Board& position, const std::vector<_Move>& m) const = 0;
    };

    // FeatureValuAlgo
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class FeatureValuAlgo : public FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _FeatureManager = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgo = FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgoConfig = FeatureAlgoConfig<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        FeatureValuAlgo(const _FeatureAlgoConfig& cfg) : FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(cfg)
        {
        }
        ~FeatureValuAlgo() {}

        virtual TYPE_PARAM get_valuations_value(const _Board& position, const std::vector<_Move>& m, char verbose, std::stringstream& verbose_stream) const = 0;
    };

    // FeatureAlgo::create
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::create(_FeatureAlgoConfig& cfg)
    {
        if      (cfg._name == FeatureBasedAlgoName::cond_product_boolean)   return (FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* )new FeatureCondAlgo_cond_product_boolean<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(cfg);
        else if (cfg._name == FeatureBasedAlgoName::valu_weight_sum)        return (FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* )new FeatureValuAlgo_weight_sum<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(cfg);
        //...
        return nullptr;
    }
};

#endif
