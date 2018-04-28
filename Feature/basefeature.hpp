#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// BaseFeature<...>     :   Primitive features (a simple function on a board position)
//
//
#ifndef _AL_CHESS_FEATURE_BASEFEATURE_HPP
#define _AL_CHESS_FEATURE_BASEFEATURE_HPP

namespace chess
{
    // CondFeatureName (primitive feature)
    enum class CondFeatureName 
    {
        none,
        eConditionFeature_isOppositeKinCheck
    };
   
    // ValuFeatureName (primitive feature)
    enum class ValuFeatureName 
    {
        none,
        eValuationFeature_numberMoveForPiece,
        eValuationFeature_countCaptureKing,
        eValuationFeature_onEdge,
        eValuationFeature_distKK
    };

    enum class ScoreBinaryClassifier
    {
        none, WIN_LOSS, WIN_DRAW, DRAW_LOSS, WIN_OTHER, DRAW_OTHER, LOSS_OTHER
        // MultiClass may do 2 hiaerchical binary: WIN_OTHER => (If OTHER) DRAW_LOSS
        // one-vs-all: WIN_OTHER, DRAW_OTHER, LOSS_OTHER
        // One-vs-one: WIN_LOSS, WIN_DRAW, DRAW_LOSS
        // Error Correcting Output Codes : WIN_OTHER, DRAW_OTHER, LOSS_OTHER
    };

    // FeatureBasedAlgoName (complex algorithmic classifier)
    enum class FeatureBasedAlgoName 
    {      
        none,

        cond_product_boolean,
        valu_weight_sum,

        // Valu - binary classifier
        rvm_trainer,
        svm_c_ekm_trainer,
        svm_c_linear_dcd_trainer,
        svm_c_linear_trainer,
        svm_c_trainer,
        svm_nu_trainer,
        svm_pegasos,

        // Valu - composite classifier
        one_vs_all_trainer,
        one_vs_one_trainer,
        svm_multiclass_linear_trainer
    };

    std::string FeatureBasedAlgoName_to_string(FeatureBasedAlgoName c)
    {
        if (c == FeatureBasedAlgoName::cond_product_boolean)            return "cond_product_boolean";
        else if (c == FeatureBasedAlgoName::valu_weight_sum)            return "valu_weight_sum";
        else if (c == FeatureBasedAlgoName::rvm_trainer)                return "rvm_trainer";
        else if (c == FeatureBasedAlgoName::svm_c_ekm_trainer)          return "svm_c_ekm_trainer";
        else if (c == FeatureBasedAlgoName::svm_c_linear_dcd_trainer)   return "svm_c_linear_dcd_trainer";
        else if (c == FeatureBasedAlgoName::svm_c_linear_trainer)       return "svm_c_linear_trainer";
        else if (c == FeatureBasedAlgoName::svm_c_trainer)              return "svm_c_trainer";
        else if (c == FeatureBasedAlgoName::svm_nu_trainer)             return "svm_nu_trainer";
        else if (c == FeatureBasedAlgoName::svm_pegasos)                return "svm_pegasos";
        else if (c == FeatureBasedAlgoName::one_vs_all_trainer)         return "one_vs_all_trainer";
        else if (c == FeatureBasedAlgoName::one_vs_one_trainer)         return "one_vs_one_trainer";
        else if (c == FeatureBasedAlgoName::svm_multiclass_linear_trainer)  return "svm_multiclass_linear_trainer";
        else return "UNKNOWN";
    }

    FeatureBasedAlgoName string_FeatureBasedAlgoName(std::string c)
    {
        if (c == "cond_product_boolean")    return FeatureBasedAlgoName::cond_product_boolean;
        else if (c == "valu_weight_sum")    return FeatureBasedAlgoName::valu_weight_sum;
        else if (c == "rvm_trainer")        return FeatureBasedAlgoName::rvm_trainer;
        //...
        else return FeatureBasedAlgoName::none;
    }

    // BaseFeature
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class BaseFeature
    {
        using _FeatureManager   = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
    
    private:
        FeatureType     _featureType;
        std::string     _classtype;
        std::string     _classtype_argument;

    public:
        BaseFeature(const FeatureType atype) : _featureType(atype) 
        {
            _classtype = "";
            _classtype_argument = "";
        }
        virtual ~BaseFeature() {}

        const std::string classtype()       const { return _classtype; }
        const std::string classtype_arg()   const { return _classtype_argument; }

        void set_classtype(const std::string& ct)           { _classtype = ct; }
        void set_classtype_argument(const std::string& arg) { _classtype_argument = arg; }

        virtual bool                save(std::ofstream& is)  const;
        static _ConditionFeature*   read_cond_feature(std::ifstream& is);
        static _ValuationFeature*   read_valu_feature(std::ifstream& is);
    };

    // save()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline bool BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save(std::ofstream& is)  const
    {
        if (is.good())
        {
            is << FeatureType_to_int(_featureType);    is << " ";
            is << toNULLSTR(_classtype);           is << " ";
            is << toNULLSTR(_classtype_argument);  is << " ";
            return true;
        }
        return false;
    }

    // read_cond_feature()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>*
    BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::read_cond_feature(std::ifstream& is)
    {
        if (is.good())
        {
            FeatureType featureType;
            std::string classtype;
            std::string classtype_argument;

            int c; is >> c;
            int_to_FeatureType(c);
            featureType = int_to_FeatureType(c);
            is >> classtype;            fromNULLSTR(classtype);
            is >> classtype_argument;   fromNULLSTR(classtype_argument);

            return _FeatureManager::instance()->get_cond_feature(classtype, classtype_argument);
        }
        return nullptr;
    }

    // read_valu_feature()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* 
    BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::read_valu_feature(std::ifstream& is)
    {
        if (is.good())
        {
            FeatureType featureType;
            std::string classtype;
            std::string classtype_argument;

            int c; is >> c;
            int_to_FeatureType(c);
            featureType = int_to_FeatureType(c);
            is >> classtype; fromNULLSTR(classtype);
            is >> classtype_argument; fromNULLSTR(classtype_argument);

            return _FeatureManager::instance()->get_valu_feature(classtype, classtype_argument);
        }
        return nullptr;
    }
};

#endif
