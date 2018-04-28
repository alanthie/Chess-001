#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// FeatureValuAlgo<...>     :
//
//
#ifndef _AL_CHESS_FEATURE_FeatureValuAlgo_weight_sum_HPP
#define _AL_CHESS_FEATURE_FeatureValuAlgo_weight_sum_HPP

namespace chess
{
    // FeatureValuAlgo_weight_sum
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class FeatureValuAlgo_weight_sum : public FeatureValuAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board    = Board<PieceID, _BoardSize>;
        using _Move     = Move<PieceID>;
        using _FeatureManager   = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgo = FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureValuAlgo = FeatureValuAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgoConfig = FeatureAlgoConfig<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
    
    protected:
        std::vector<_ValuationFeature*> _valuations;
        std::vector<TYPE_PARAM>         _weights;

    public:
        FeatureValuAlgo_weight_sum(const _FeatureAlgoConfig& cfg) : _FeatureValuAlgo(cfg)
        {
        }
        ~FeatureValuAlgo_weight_sum() {}

        TYPE_PARAM get_valuations_value(const _Board& position, const std::vector<_Move>& m, char verbose, std::stringstream& verbose_stream) const override
        { 
            assert(_weights.size() >= _valuations.size());
            TYPE_PARAM c = 0;
            TYPE_PARAM v = 0;
            for (size_t i = 0; i < _valuations.size(); i++)
            {
                v = _valuations[i]->compute(position, m);
                c += (v * _weights[i]);
                if (verbose > 2)
                {
                    verbose_stream << v << "*" << _weights[i] << " ";
                }
            }
            if (verbose > 2) verbose_stream << "(" << c << "," << sigmoid(c) << ")" << std::endl;
            return sigmoid(c);
        }

        bool load() override
        {
            std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("ConditionValuationNode", cfg()._persist_key);
            std::ifstream is;
            is.open(f.c_str(), std::fstream::in);
            if (!load_cfg(is))
            {
                is.close();
                return true;
            }
            if (!load_detail(is))
            {
                is.close();
                return true;
            }
            is.close();
            return false;
        }

        bool load_cfg(std::ifstream& is) override
        {
            if (is.good())
            {
                cfg().load_detail(is);
                return true;
            }
            return false;
        }

        bool load_detail(std::ifstream& is) override
        {
            for (size_t i = 0; i < _valuations.size(); i++) _valuations[i] = nullptr; // not owner
            _valuations.clear(); 
            size_t n;

            is >> n;
            for (size_t i = 0; i < n; i++)
            {
                _ValuationFeature* feature = read_valu(is);
                if (feature == nullptr) return false;
                _valuations.push_back(feature);
            }

            _weights.clear(); 
            is >> n;
            for (size_t i = 0; i < n; i++)
            {
                TYPE_PARAM v; is >> v;
                _weights.push_back(v);
            }

            return true;
        }

        bool save() const override
        {
            if (cfg()._persist_key.size() == 0)
                cfg()._persist_key = PersistManager<PieceID, _BoardSize>::instance()->create_persist_key();
            std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("ConditionValuationNode", cfg()._persist_key);
            std::ofstream os;
            os.open(f.c_str(), std::fstream::out | std::fstream::trunc);
            if (save_detail(os))
            {
                os.close();
                return true;
            }
            os.close();
            return false;
        }

        bool save_detail(std::ofstream& os) const override
        {
            if (os.good())
            {  
                cfg().save_detail(os);

                os << _valuations.size();   os << " ";
                for (auto& v : _valuations) v->save(os);
                os << _weights.size();      os << " ";
                for (auto& v : _weights)    { os << v; os << " "; }
                return true;
            }
            return false;
        }

        bool prepare(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override
        {
            // Load features given
            _valuations.clear();

            _ValuationFeature* f;
            for (size_t i = 0; i < cfg()._valu_primitive_features.size(); i++)
            {
                f = _FeatureManager::instance()->get_valu_feature_by_fullname(cfg()._valu_primitive_features[i]);
                if (f != nullptr)
                {
                    _valuations.push_back(f);
                }
            }

            // Weights optimization - prepare data set ...
            return true;
        }

        bool train(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset, char verbose) override
        {
            //... GA
            return true;
        }

        bool test(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override
        {
            //...
            return true;
        }

        bool compare(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override
        {
            //...
            return true; // can change
        }

        void cleanup(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override
        {
            //...
        }

    };

};

#endif
