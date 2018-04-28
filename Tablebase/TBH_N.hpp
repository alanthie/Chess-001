#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBH_N   : Generic handler of N pieces TB (W to play or B to play)
//
//
#ifndef _AL_CHESS_TABLEBASE_TBH_N_HPP
#define _AL_CHESS_TABLEBASE_TBH_N_HPP

namespace chess
{
    // TBH_N
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    class TBH_N : public TBH<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TBH_N(const PieceSet<PieceID, _BoardSize>& ps, TB_TYPE t, TBH_IO_MODE iomode, TBH_OPTION option) : TBH(t, ps, NPIECE, iomode, option)
        {
            Tablebase<PieceID, _BoardSize, NPIECE>* _tb_W;
            Tablebase<PieceID, _BoardSize, NPIECE>* _tb_B;
            Tablebase<PieceID, _BoardSize, NPIECE>* tb_w = (Tablebase<PieceID, _BoardSize, NPIECE>*)TB_Manager<PieceID, _BoardSize>::instance()->find_N(NPIECE, ps.name(PieceColor::W));
            Tablebase<PieceID, _BoardSize, NPIECE>* tb_b = (Tablebase<PieceID, _BoardSize, NPIECE>*)TB_Manager<PieceID, _BoardSize>::instance()->find_N(NPIECE, ps.name(PieceColor::B));
            std::vector<PieceID> v = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);

            if (tb_w == nullptr)
            {
                // Not in memory
                _tb_W = new TB_N<PieceID, _BoardSize, NPIECE>(v, PieceColor::W);
                TB_Manager<PieceID, _BoardSize>::instance()->add_N(ps.name(PieceColor::W), _tb_W);
            } 
            else 
            { 
                _tb_W = tb_w; 
            }

            if (tb_b == nullptr)
            {
                // Not in memory
                _tb_B = new TB_N<PieceID, _BoardSize, NPIECE>(v, PieceColor::B);
                TB_Manager<PieceID, _BoardSize>::instance()->add_N(ps.name(PieceColor::B), _tb_B);
            } 
            else 
            { 
                _tb_B = tb_b; 
            }

            // TB
            set_TB_W(_tb_W);
            set_TB_B(_tb_B);

            // Children handlers
            TBH_IO_MODE childmode = iomode;
            if (iomode == TBH_IO_MODE::tb_and_child) childmode = TBH_IO_MODE::tb_only;
            if (iomode != TBH_IO_MODE::tb_only)
            {
                _tb_children_info = TB_Manager<PieceID, _BoardSize>::instance()->make_all_child_TBH(ps, childmode, option);
                for (size_t i = 0; i < _tb_children_info.size(); i++)
                {
                    if (_tb_children_info[i]._tbh != nullptr)
                    {
                        _tbh_children.push_back(_tb_children_info[i]._tbh);
                        _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                    }
                    else
                        assert(false);
                }
            }
        }     

        virtual ~TBH_N() {}
        bool build(TBH_IO_MODE mode, char verbose = 0) override;

    protected:
        bool     build_base(char verbose = 0);
        uint64_t set_mate_score(PieceColor color_to_play);
        uint64_t set_marker(PieceColor color_to_play);
        uint64_t process_marker(PieceColor color_to_play);
    };

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool TBH_N<PieceID, _BoardSize, NPIECE>::build(TBH_IO_MODE mode, char verbose)
    {
        TBH_IO_MODE childmode = mode;
        if (mode == TBH_IO_MODE::tb_and_child) childmode = TBH_IO_MODE::tb_only;
        if (mode != TBH_IO_MODE::tb_only)
        {
            for (size_t i = 0; i < _tbh_children.size(); i++)
            {
                _tbh_children[i]->build(childmode, verbose);
            }
        }
        return build_base(verbose);
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline uint64_t TBH_N<PieceID, _BoardSize, NPIECE>::set_mate_score(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W)
            return set_mate_score_vv(color_to_play, _TB_W);
        else
            return set_mate_score_vv(color_to_play, _TB_B);
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline uint64_t TBH_N<PieceID, _BoardSize, NPIECE>::set_marker(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W)
            setup_marker_vv<PieceID, _BoardSize, NPIECE>(this, color_to_play, _TB_W, _TB_B);
        else
            return setup_marker_vv<PieceID, _BoardSize, NPIECE>(this, color_to_play, _TB_B, _TB_W);
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline uint64_t TBH_N<PieceID, _BoardSize, NPIECE>::process_marker(PieceColor color_to_play)
    {
        if (color_to_play == PieceColor::W)
            process_marker_vv<PieceID, _BoardSize, NPIECE>(this, color_to_play, _TB_W, _TB_B);
        else
            return process_marker_vv<PieceID, _BoardSize, NPIECE>(this, color_to_play, _TB_B, _TB_W);
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool TBH_N<PieceID, _BoardSize, NPIECE>::build_base(char verbose)
    {
        return build_base_vv<PieceID, _BoardSize, NPIECE>(this, _TB_W, _TB_B, verbose);
    }
};
#endif

