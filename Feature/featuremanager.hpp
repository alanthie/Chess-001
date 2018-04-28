#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//
#ifndef _AL_CHESS_FEATURE_FEATUREMANAGER_HPP
#define _AL_CHESS_FEATURE_FEATUREMANAGER_HPP

namespace chess
{   
    // FeatureManager
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class FeatureManager
    {
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        using _ConditionFeature_isOppositeKinCheck  = ConditionFeature_isOppositeKinCheck<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_numberMoveForPiece  = ValuationFeature_numberMoveForPiece<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_countCaptureKing    = ValuationFeature_countCaptureKing<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_onEdge              = ValuationFeature_onEdge<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_distKK              = ValuationFeature_distKK<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Piece                                = Piece<PieceID, _BoardSize>;

    private:
        // Primitive features list
        static std::map<CondFeatureName, std::string>       _cond_features_name;
        static std::map<ValuFeatureName, std::string>       _valu_features_name;
        static std::map<std::string, _ConditionFeature*>    _cond_features_instance;
        static std::map<std::string, _ValuationFeature*>    _valu_features_instance;

        // Algo list
        static std::vector<FeatureBasedAlgoName>            _features_algo_name;
        static double _WIN_THRESHOLD_IN_PROB_01;
        static double _LOSS_THRESHOLD_IN_PROB_01;

        // _instance
        static std::unique_ptr<FeatureManager>              _instance;

        // Other method of access
        static std::vector<_ConditionFeature*>              _v_cond_features_instance;
        static std::vector<_ValuationFeature*>              _v_valu_features_instance;
        static std::vector<std::string>                     _v_cond_features_instance_fullname;
        static std::vector<std::string>                     _v_valu_features_instance_fullname;

    private:
        FeatureManager()
        {
        }

    public:
        CondFeatureName get_cond_feature_name(const std::string s) const
        {
            if (s == "ConditionFeature_isOppositeKinCheck") return CondFeatureName::eConditionFeature_isOppositeKinCheck;
            return CondFeatureName::none;
        }

        ValuFeatureName get_valu_feature_name(const std::string s) const
        {
            if (s == "ValuationFeature_numberMoveForPiece") return ValuFeatureName::eValuationFeature_numberMoveForPiece;
            else if (s == "ValuationFeature_countCaptureKing") return ValuFeatureName::eValuationFeature_countCaptureKing;
            else if (s == "ValuationFeature_onEdge") return ValuFeatureName::eValuationFeature_onEdge;
            else if (s == "ValuationFeature_distKK") return ValuFeatureName::eValuationFeature_distKK;
            return ValuFeatureName::none;
        }

        // setters...
        double WIN_THRESHOLD_IN_PROB_01() const     { return _WIN_THRESHOLD_IN_PROB_01; }
        double LOSS_THRESHOLD_IN_PROB_01() const    { return _LOSS_THRESHOLD_IN_PROB_01; }

    private:
        void load()
        {
            // Primitive features
            _cond_features_name[CondFeatureName::eConditionFeature_isOppositeKinCheck] = "ConditionFeature_isOppositeKinCheck";         
            _valu_features_name[ValuFeatureName::eValuationFeature_numberMoveForPiece] = "ValuationFeature_numberMoveForPiece";
            _valu_features_name[ValuFeatureName::eValuationFeature_countCaptureKing]   = "ValuationFeature_countCaptureKing";
            _valu_features_name[ValuFeatureName::eValuationFeature_onEdge]             = "ValuationFeature_onEdge";
            _valu_features_name[ValuFeatureName::eValuationFeature_distKK]             = "ValuationFeature_distKK";

            // cond set
            _cond_features_instance[get_cond_feature_name(CondFeatureName::eConditionFeature_isOppositeKinCheck)] = (_ConditionFeature*)new _ConditionFeature_isOppositeKinCheck();
           
            // valu set
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::K, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::K, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::R, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::R, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::N, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::N, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::B, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::B, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::Q, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::Q, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::P, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::P, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::K, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::K, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::R, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::R, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::N, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::N, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::B, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::B, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::Q, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::Q, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::P, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::P, PieceColor::B);
            
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_countCaptureKing) + "W"] = (_ValuationFeature*)new _ValuationFeature_countCaptureKing(PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_countCaptureKing) + "B"] = (_ValuationFeature*)new _ValuationFeature_countCaptureKing(PieceColor::B);

            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::K, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::K, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::R, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::R, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::N, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::N, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::B, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::B, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::Q, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::Q, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::P, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::P, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::K, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::K, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::R, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::R, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::N, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::N, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::B, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::B, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::Q, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::Q, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_onEdge) + std::to_string(_Piece::get_id(PieceName::P, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_onEdge(PieceName::P, PieceColor::B);

            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_distKK)] = (_ValuationFeature*)new _ValuationFeature_distKK();

            for (auto it = _cond_features_instance.begin(); it != _cond_features_instance.end(); ++it)
            {
                _v_cond_features_instance_fullname.push_back(it->first);
                _v_cond_features_instance.push_back(it->second);
            }
            for (auto it = _valu_features_instance.begin(); it != _valu_features_instance.end(); ++it)
            {
                _v_valu_features_instance_fullname.push_back(it->first);
                _v_valu_features_instance.push_back(it->second);
            }

            // ALGO
            _features_algo_name.push_back(FeatureBasedAlgoName::cond_product_boolean);
            _features_algo_name.push_back(FeatureBasedAlgoName::valu_weight_sum);
            _features_algo_name.push_back(FeatureBasedAlgoName::rvm_trainer);
            _features_algo_name.push_back(FeatureBasedAlgoName::svm_c_ekm_trainer);
            _features_algo_name.push_back(FeatureBasedAlgoName::svm_c_linear_dcd_trainer);
            _features_algo_name.push_back(FeatureBasedAlgoName::svm_c_trainer);
            _features_algo_name.push_back(FeatureBasedAlgoName::svm_nu_trainer);
            _features_algo_name.push_back(FeatureBasedAlgoName::svm_pegasos);
            _features_algo_name.push_back(FeatureBasedAlgoName::one_vs_all_trainer);
            _features_algo_name.push_back(FeatureBasedAlgoName::one_vs_one_trainer);
            _features_algo_name.push_back(FeatureBasedAlgoName::svm_multiclass_linear_trainer);
        }

        FeatureManager(const FeatureManager&) = delete;
        FeatureManager & operator=(const FeatureManager &) = delete;
        FeatureManager(FeatureManager&&) = delete;

    public:
        ~FeatureManager()
        {
            if (_instance.operator bool())
            {
                for (size_t i=0;i < _v_cond_features_instance.size(); i++)
                {
                    _v_cond_features_instance[i] = nullptr; // not owner
                }
                for (size_t i = 0; i < _v_valu_features_instance.size(); i++)
                {
                    _v_valu_features_instance[i] = nullptr; // not owner
                }
                _v_cond_features_instance.clear();
                _v_valu_features_instance.clear();

                _cond_features_name.clear();
                _valu_features_name.clear();
                _cond_features_instance.clear();
                _valu_features_instance.clear();
                _instance.release();
            }
        }

    public:
        size_t count_cond_features() const { return _cond_features_instance.size(); }
        size_t count_valu_features() const { return _valu_features_instance.size(); }
        _ConditionFeature* get_cond_feature(size_t idx)  const { return _v_cond_features_instance[idx]; }
        _ValuationFeature* get_valu_feature(size_t idx)  const { return _v_valu_features_instance[idx]; }
        std::string get_cond_feature_fullname(size_t idx)  const { return _v_cond_features_instance_fullname[idx]; }
        std::string get_valu_feature_fullname(size_t idx)  const { return _v_valu_features_instance_fullname[idx]; }

        size_t count_algo() const { return _features_algo_name.size(); }
        FeatureBasedAlgoName get_algo_name(size_t idx)  const { return _features_algo_name[idx]; }

        std::string get_cond_feature_name(CondFeatureName n) const
        {
            auto it = _cond_features_name.find(n);
            if (it == _cond_features_name.end()) return "";
            return _cond_features_name[n];
        }

        std::string get_valu_feature_name(ValuFeatureName n) const
        {
            auto it = _valu_features_name.find(n);
            if (it == _valu_features_name.end()) return "";
            return _valu_features_name[n];
        }

        _ConditionFeature* get_cond_feature(const std::string name, const std::string arg) const
        {
            auto it = _cond_features_instance.find(name + arg);
            if (it == _cond_features_instance.end()) return nullptr;
            return _cond_features_instance[name+arg];
        }
        _ConditionFeature* get_cond_feature_by_fullname(const std::string namearg) const
        {
            auto it = _cond_features_instance.find(namearg);
            if (it == _cond_features_instance.end()) return nullptr;
            return _cond_features_instance[namearg];
        }

        _ValuationFeature* get_valu_feature(const std::string name, const std::string arg) const
        {
            auto it = _valu_features_instance.find(name + arg);
            if (it == _valu_features_instance.end()) return nullptr;
            return _valu_features_instance[name+arg];
        }
        _ValuationFeature* get_valu_feature_by_fullname(const std::string namearg) const
        {
            auto it = _valu_features_instance.find(namearg);
            if (it == _valu_features_instance.end()) return nullptr;
            return _valu_features_instance[namearg];
        }

    public:
        static const FeatureManager* instance()
        {
            if (_instance == nullptr)
            {
                _instance = std::unique_ptr<FeatureManager>(new FeatureManager);
                _instance->load();
                return _instance.get();
            }
            return  _instance.get();
        }
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::unique_ptr<FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_instance = nullptr;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::map<CondFeatureName, std::string>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_cond_features_name;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::map<ValuFeatureName, std::string>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_valu_features_name;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::map<std::string, ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>*>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_cond_features_instance;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::map<std::string, ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>*>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_valu_features_instance;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::vector<ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>*>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_v_cond_features_instance;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::vector<std::string>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_v_cond_features_instance_fullname;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::vector<ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>*>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_v_valu_features_instance;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::vector<std::string>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_v_valu_features_instance_fullname;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::vector<FeatureBasedAlgoName>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_features_algo_name;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    double FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_WIN_THRESHOLD_IN_PROB_01 = 0.75;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    double FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_LOSS_THRESHOLD_IN_PROB_01 = 0.25;
};

#endif
