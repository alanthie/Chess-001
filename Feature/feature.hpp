#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Condition feature:   Detect if something is on/off on the board 
// Valuation feature:   Compute some value
//
// Model
//  Fc = conditional term = [Fcond() and/or Fcond() and/or ...]
//  Fv =   valuation term = Sigmoid([Fval()*w + Fval()*w + ...]) range(0 to 1) 0==Black_winning 1==white_winning
//
//
#ifndef _AL_CHESS_FEATURE_FEATURE_HPP
#define _AL_CHESS_FEATURE_FEATURE_HPP

namespace chess
{
    // ConditionFeature
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ConditionFeature : public BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board    = Board<PieceID, _BoardSize>;
        using _Move     = Move<PieceID>;
        using _FeatureManager = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        ConditionFeature() : BaseFeature(FeatureType::condition)  { }
        virtual ~ConditionFeature() {};
        virtual bool check(const _Board& position, const std::vector<_Move>& m) const = 0;
        CondFeatureName name() { return _name; }

    protected:
        CondFeatureName _name;
    };

    // ConditionFeature_isOppositeKingCheck
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ConditionFeature_isOppositeKinCheck : public ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _FeatureManager = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        ConditionFeature_isOppositeKinCheck() : ConditionFeature()
        {
            _name = CondFeatureName::eConditionFeature_isOppositeKinCheck;
            set_classtype( _FeatureManager::get_cond_feature_name(CondFeatureName::eConditionFeature_isOppositeKinCheck) );
        }
        virtual ~ConditionFeature_isOppositeKinCheck() {};
        virtual bool check(const _Board& position, const std::vector<_Move>& m) const override
        {
            size_t ret_mv;
            return position.can_capture_opposite_king(m, ret_mv);
        }
    };

    // ValuationFeature
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ValuationFeature : public BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _FeatureManager = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        ValuationFeature() : BaseFeature(FeatureType::valuation), _PARAM_NBIT(PARAM_NBIT){ }
        virtual ~ValuationFeature() {};
        virtual TYPE_PARAM compute(const _Board& position, const std::vector<_Move>& m) const = 0;
        ValuFeatureName name() { return _name; }

    protected:
        size_t  _PARAM_NBIT;
        ValuFeatureName _name;
    };

    // ValuationFeature_numberMoveForPiece
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ValuationFeature_numberMoveForPiece : public ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _Piece = Piece<PieceID, _BoardSize>;

    public:
        ValuationFeature_numberMoveForPiece(const PieceName p, const PieceColor c) : _ValuationFeature() , _p(p), _c(c)
        { 
            _name = ValuFeatureName::eValuationFeature_numberMoveForPiece;
            set_classtype(_FeatureManager::get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece));
            PieceID id = _Piece::get_id(_p, _c);
            set_classtype_argument(std::to_string(id));
            if (_is_init == false)
            {
                for (size_t i = 1; i < _Piece::pieces_size(); i++) // skip empty piece
                {
                    _arg_index_value[i - 1] = std::to_string(i - 1);
                }
                _is_init = true;
            }
        }

        virtual ~ValuationFeature_numberMoveForPiece() 
        {
            _arg_index_value.clear();
        }

        virtual TYPE_PARAM compute(const _Board& position, const std::vector<_Move>& m) const override
        {
            if (_c == position.get_color())
                return (TYPE_PARAM)position.cnt_move(_p, _c, m);
            else
                return (TYPE_PARAM)position.cnt_move_oppo(_p, _c, m);
        }

        PieceName  piecename()  { return _p; }
        PieceColor piececolor() { return _c; }

    private:
        PieceName   _p;
        PieceColor  _c;
        static bool _is_init;
        static std::map<size_t, std::string> _arg_index_value;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::map<size_t, std::string> ValuationFeature_numberMoveForPiece<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_arg_index_value;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool ValuationFeature_numberMoveForPiece<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_is_init = false;


    // ValuationFeature_countCaptureKing
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ValuationFeature_countCaptureKing : public ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _Piece = Piece<PieceID, _BoardSize>;

    public:
        ValuationFeature_countCaptureKing(const PieceColor color): _c(color)
        {
            _name = ValuFeatureName::eValuationFeature_countCaptureKing;
            set_classtype(_FeatureManager::get_valu_feature_name(ValuFeatureName::eValuationFeature_countCaptureKing));

            if (_c == PieceColor::W) set_classtype_argument("W");
            else set_classtype_argument("B");
        }

        virtual ~ValuationFeature_countCaptureKing()
        {
        }

        virtual TYPE_PARAM compute(const _Board& position, const std::vector<_Move>& m) const override
        {
            if (position.get_color() == _c)
                return (TYPE_PARAM) position.count_capture_king();
            else
                return (TYPE_PARAM) position.count_capture_opposite_king(m);
        }
    private:
        PieceColor  _c;
    };

};

#endif
