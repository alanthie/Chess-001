#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// FeatureAlgoConfig<...>     :
//
//
#ifndef _AL_CHESS_FEATURE_FeatureAlgoConfig_HPP
#define _AL_CHESS_FEATURE_FeatureAlgoConfig_HPP

namespace chess
{
    enum class FeatureAlgoStatus { none, empty, building, pending, ready, failed };

    int FeatureAlgoStatus_to_int(FeatureAlgoStatus c)
    {
        if (c == FeatureAlgoStatus::empty) return 1;
        else if (c == FeatureAlgoStatus::building)  return 2;
        else if (c == FeatureAlgoStatus::pending)   return 3;
        else if (c == FeatureAlgoStatus::ready)     return 4;
        else if (c == FeatureAlgoStatus::failed)    return 5;
        return 0;
    }
    FeatureAlgoStatus int_to_FeatureAlgoStatus(int t)
    {
        if (t == 1) return FeatureAlgoStatus::empty;
        else if (t == 2) return FeatureAlgoStatus::building;
        else if (t == 3) return FeatureAlgoStatus::pending;
        else if (t == 4) return FeatureAlgoStatus::ready;
        else if (t == 5) return FeatureAlgoStatus::failed;
        return FeatureAlgoStatus::none;
    }

    // FeatureAlgoConfig  - Config param of feature based algorithm
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    struct FeatureAlgoConfig
    {
        using _FeatureManager   = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgoConfig = FeatureAlgoConfig<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        mutable std::string             _persist_key;
        FeatureAlgoStatus               _status; 
        bool                            _is_cond;                       // cond or valu
        FeatureBasedAlgoName            _name;
        std::string                     _param_key_signature;
        std::vector<std::string>        _cond_primitive_features;       // list of primitive features used by the algo
        std::vector<std::string>        _valu_primitive_features;
        double                          _train_dataset_average_error;   // clasifier performance (error) after algo build()
        double                          _test_dataset_average_error;

        FeatureAlgoConfig() : _persist_key(""), _status(FeatureAlgoStatus::empty), _is_cond(true), _name(FeatureBasedAlgoName::none), _param_key_signature(""),
            _train_dataset_average_error(1.00), _test_dataset_average_error(1.00) // worst case 100% error
        {
        }

        bool load()
        {
            std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("FeatureAlgoConfig", _persist_key);
            std::ifstream is;
            is.open(f.c_str(), std::fstream::in);
            if (load_detail(is))
            {
                is.close();
                return true;
            }
            is.close();
            return false;
        }

        bool load_detail(std::ifstream& is)
        {
            if (is.good())
            {
                is >> _persist_key;

                int c;
                is >> c;
                _status = int_to_FeatureAlgoStatus(c);
                _status = FeatureAlgoStatus::empty;     // initial state

                is >> c;
                _is_cond = int_to_bool(c);

                std::string s;
                is >> s;
                _name = string_FeatureBasedAlgoName(s);
                is >> _param_key_signature;

                size_t n;
                is >> n;
                for (size_t i = 0; i < n; i++)
                {
                    is >> s;
                    _ConditionFeature* f = _FeatureManager::instance()->get_cond_feature_by_fullname(s);
                    if (f == nullptr)
                    {
                        //...
                    }
                    _cond_primitive_features.push_back(s);
                }
                is >> n;
                for (size_t i = 0; i < n; i++)
                {
                    is >> s;
                    _ValuationFeature* f = _FeatureManager::instance()->get_valu_feature_by_fullname(s);
                    if (f == nullptr)
                    {
                        //...
                    }
                    _valu_primitive_features.push_back(s);
                }

                is >> _train_dataset_average_error;
                is >> _test_dataset_average_error;

                return true;
            }
            return false;
        }

        bool save()
        {
            if (_persist_key.size()==0)
                _persist_key = PersistManager<PieceID, _BoardSize>::instance()->create_persist_key();
            std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("FeatureAlgoConfig", _persist_key);
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
                os << _persist_key << " ";
                os << FeatureAlgoStatus_to_int(_status) << " ";
                os << bool_to_int(_is_cond); os << " ";
                os << FeatureBasedAlgoName_to_string(_name) << " ";
                os << _param_key_signature << " ";
                os << _cond_primitive_features.size() << " ";
                for (auto& v : _cond_primitive_features)
                {
                    os << v << " ";
                }
                os << _valu_primitive_features.size() << " ";
                for (auto& v : _valu_primitive_features)
                {
                    os << v << " ";
                }
                os << _train_dataset_average_error << " ";
                os << _test_dataset_average_error << " ";
                return true;
            }
            return false;
        }

        bool is_same(_FeatureAlgoConfig& rhs)
        {
            if (_is_cond != rhs._is_cond) return false;
            if (_name != rhs._name) return false;
            if (_param_key_signature != rhs._param_key_signature) return false;
            if (_is_cond)
            {
                if (_cond_primitive_features.size() != rhs._cond_primitive_features.size()) return false;
                for (size_t i = 0; i < _cond_primitive_features.size(); i++)
                {
                    if (_cond_primitive_features[i] != rhs._cond_primitive_features[i]) return false;
                }
            }
            else
            {
                if (_valu_primitive_features.size() != rhs._valu_primitive_features.size()) return false;
                for (size_t i = 0; i < _valu_primitive_features.size(); i++)
                {
                    if (_valu_primitive_features[i] != rhs._valu_primitive_features[i]) return false;
                }
            }
            return true;
        }
    };

};

#endif
