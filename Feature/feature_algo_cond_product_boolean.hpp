#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// FeatureCondAlgo<...>     :
//
//
#ifndef _AL_CHESS_FEATURE_FeatureCondAlgo_cond_product_boolean_HPP
#define _AL_CHESS_FEATURE_FeatureCondAlgo_cond_product_boolean_HPP

namespace chess
{
    // FeatureCondAlgo_cond_product_boolean
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class FeatureCondAlgo_cond_product_boolean : public FeatureCondAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board    = Board<PieceID, _BoardSize>;
        using _Move     = Move<PieceID>;
        using _FeatureManager   = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionValuationNode   = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgo              = FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgoConfig        = FeatureAlgoConfig<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
    
    protected:
        std::vector<_ConditionFeature*> _conditions;
        std::vector<bool>               _conditions_and_or;

    public:
        FeatureCondAlgo_cond_product_boolean(const _FeatureAlgoConfig& cfg) : FeatureCondAlgo(cfg)
        {
        }
        ~FeatureCondAlgo_cond_product_boolean() {}

        virtual bool get_condition_value(bool is_root, bool is_positive_node, const _Board& position, const std::vector<_Move>& m) const 
        {
            if (is_root) return true;

            assert(_conditions_and_or.size() == _conditions.size());

            bool cond = true;
            for (size_t i = 0; i < _conditions.size(); i++)
            {
                if (i == 0) cond = _conditions[0]->check(position, m); //_conditions_and_or[0] ignored (assume true)
                else
                {
                    if (_conditions_and_or[i] == true)  cond = cond && _conditions[i]->check(position, m);
                    else                                cond = cond || _conditions[i]->check(position, m);
                }
            }
            if (!is_positive_node) cond = !cond;
            return cond;
        }

        bool load()
        {
            std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("FeatureCondAlgo", cfg()._persist_key);
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
                return cfg().load_detail(is);
            }
            return false;
        }

        bool load_detail(std::ifstream& is) override
        {
            if (is.good())
            {               
                for (size_t i = 0; i < _conditions.size(); i++) _conditions[i] = nullptr; // not owner
                _conditions.clear();
                size_t n;

                is >> n;
                for (size_t i = 0; i < n; i++)
                {
                    _ConditionFeature* feature = read_cond(is);
                    if (feature == nullptr) return false;
                    _conditions.push_back(feature);
                }

                int c;
                _conditions_and_or.clear(); 
                is >> n;
                for (size_t i = 0; i < n; i++)
                {
                    is >> c;
                    _conditions_and_or.push_back(int_to_bool(c));
                }
                return true;
            }
            return false;
        }

        bool save() const override
        {
            if (cfg()._persist_key.size() == 0)
                cfg()._persist_key = PersistManager<PieceID, _BoardSize>::instance()->create_persist_key();
            std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("FeatureCondAlgo", cfg()._persist_key);
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

        bool save_detail(std::ofstream& os) const
        {
            if (os.good())
            {
                cfg().save_detail(os);

                os << _conditions.size();           os << " ";
                for (auto& v : _conditions)         v->save(os);
                os << _conditions_and_or.size();    os << " ";
                for (auto v : _conditions_and_or)   { os << bool_to_int(v); os << " "; }
                return true;
            }
            return false;
        }

        bool prepare(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override 
        { 
            // Load features given
            _conditions.clear();
            _conditions_and_or.clear();

            _ConditionFeature* f;
            for (size_t i = 0; i < cfg()._cond_primitive_features.size(); i++)
            {
                f = _FeatureManager::instance()->get_cond_feature_by_fullname(cfg()._cond_primitive_features[i]);
                if (f!=nullptr)
                {
                    _conditions.push_back(f);
                    _conditions_and_or.push_back(true); // default AND
                }
            }

            // No optimization - not producing data set
            return true; 
        }

        bool train(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset, char verbose) override
        { 
            // produce random _conditions_and_or - No optimization
            cfg()._param_key_signature = "";
            for (size_t i = 0; i < _conditions.size(); i++)
            {
                if (i == 0) _conditions_and_or[0] = true;
                else if (uniform_double(rnd_generator) >= 0.50)
                {
                    _conditions_and_or[i] = false;  // OR
                }
                cfg()._param_key_signature += bool_to_string(_conditions_and_or[i]);
            }
            return true; 
        }

        bool test(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override 
        { 
            // no testing required
            return true; 
        }

        bool compare(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override
        {
            // no compare required
            return true; // can change
        }

        void cleanup(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override 
        { 
            // no stuff to clean
        }

    };

 };

#endif
