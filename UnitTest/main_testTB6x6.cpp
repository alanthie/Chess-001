//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Generating/reading some TB (chess endgame tablebase) with boardsize of 6x6
//
//
#include "core/chess.hpp"

using namespace chess;

const uint8_t _BoardSize = 6;           // Board size is 6x6    (can be 2 to 255)
using _PieceID = uint8_t;               // PieceID type is uint8_t

// Useful define
using _Piece = Piece<_PieceID, _BoardSize>; 
using _Board = Board<_PieceID, _BoardSize>;
using _Move  = Move<_PieceID>;
using _PieceSet = PieceSet<_PieceID, _BoardSize>;
using _TBHandler_3 = TBH_N<_PieceID, _BoardSize, 3>;
using _TB_Manager = TB_Manager<_PieceID, _BoardSize>;

#define DO_TB4
#define DO_TB5
#define DO_TB6
#define DO_TB7

int main(int argc, char* argv[])
{
    // SPACE/TIME constraints for TB generation
    TB_Manager<_PieceID, _BoardSize>::instance()->_TB_MAX_SIZE      = 100000;      // Max number positions for partial TB (so it fit in RAM)
    TB_Manager<_PieceID, _BoardSize>::instance()->_TB_MINMAX_DEPTH  = 1;           // Max minmax_search_depth for partial TB

    FeatureAlgoConfig<uint8_t, 6, double, 16>  cfg;
    FeatureValuAlgo_rvm_trainer<uint8_t, 6, double, 16> a(cfg);

    // TEST TB partition
    {
        const std::string TestPartition = "TB_N4_B6";
        chess::Partition<uint8_t, 6, double, 16>* p_tb = chess::PartitionManager<uint8_t, 6, double, 16>::instance()->load_partition(TestPartition);
        if (p_tb == nullptr)
        {
            // Not found: create partition and save
            chess::PartitionManager<uint8_t, 6, double, 16>::instance()->make_tb_partition(4);
            p_tb = chess::PartitionManager<uint8_t, 6, double, 16>::instance()->find_partition(TestPartition);
            assert(p_tb != nullptr);
            p_tb->save();
        }
    }

    bool DO_BUILD = true; 

    _TB_Manager::instance()->clear();
    _Board::reset_to_default_option();
    srand((unsigned int)time(NULL)); // use std::mt19937_64 ...

    // KQvK
    if (_BoardSize >= 2)
    {
        std::cout << "KQvK" << std::endl;

        // Making the KQvK tablebases (white or black to play) and all their children as necessary.
        // Saving in persistence_root_folder\\CFG_6unsignedchar\\tablebase
        std::vector<_PieceID> white_piece_set;
        std::vector<_PieceID> black_piece_set;
        white_piece_set.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
        white_piece_set.push_back(_Piece::get_id(PieceName::Q, PieceColor::W));
        black_piece_set.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
        std::vector<std::pair<_PieceID, uint8_t>> white_piece_count = _PieceSet::to_set(white_piece_set);
        std::vector<std::pair<_PieceID, uint8_t>> black_piece_count = _PieceSet::to_set(black_piece_set);
        _PieceSet ps(white_piece_count, black_piece_count);
        _TBHandler_3 TBH_KQvK(ps, TB_TYPE::tb_2v1, TBH_IO_MODE::tb_hiearchy, TBH_OPTION::try_load_on_build);    // Handler of TB (dont rebuild if already exist)
        if (DO_BUILD) TBH_KQvK.build(TBH_IO_MODE::tb_hiearchy, 1);
        if (DO_BUILD) TBH_KQvK.save();
        TBH_KQvK.load(TBH_IO_MODE::tb_hiearchy);    // KQvK and all children loaded in memory
        TBH_KQvK.print();

        // Testing KQvK
        {
            // check the (internal) symmetrical KvKQ tablebase
            TablebaseBase<_PieceID, _BoardSize>* sym_tb_w;
            TablebaseBase<_PieceID, _BoardSize>* sym_tb_b;
            {
                // reference TB (for the symmetrcal TB) is Black to play
                _PieceSet master(((Tablebase<_PieceID, _BoardSize, 3>*)(TBH_KQvK.TB_B()))->piecesID());
                _PieceSet reverse({ _PieceSet::reverse_color_set(master.bset()), _PieceSet::reverse_color_set(master.wset()) });
                sym_tb_w = (TablebaseBase<_PieceID, _BoardSize>*)new SymmetryTablebase<_PieceID, _BoardSize, 3>(( Tablebase<_PieceID, _BoardSize, 3>*)TBH_KQvK.TB_B_const());
                TB_Manager<_PieceID, _BoardSize>::instance()->add_sym(reverse.name(PieceColor::W), sym_tb_w);
                sym_tb_w->load();
                std::cout << "check_score B KQvK vs sym W kVKQ: " << ((SymmetryTablebase<_PieceID, _BoardSize, 3>*)(sym_tb_w))->check_score() << std::endl;
            }
            {
                // reference TB is White to play
                _PieceSet master(((Tablebase<_PieceID, _BoardSize, 3>*)(TBH_KQvK.TB_W()))->piecesID());
                _PieceSet reverse({ _PieceSet::reverse_color_set(master.bset()), _PieceSet::reverse_color_set(master.wset()) });
                sym_tb_b = (TablebaseBase<_PieceID, _BoardSize>*)new SymmetryTablebase<_PieceID, _BoardSize, 3>((Tablebase<_PieceID, _BoardSize, 3>*)TBH_KQvK.TB_W_const());
                TB_Manager<_PieceID, _BoardSize>::instance()->add_sym(reverse.name(PieceColor::B), sym_tb_b);
                sym_tb_b->load();
                assert(sym_tb_b->is_build_and_loaded());
                std::cout << "check_score W KQvK vs sym B kVKQ: " << ((SymmetryTablebase<_PieceID, _BoardSize, 3>*)(sym_tb_b))->check_score() << std::endl;
            }

            // check a position score in the symmetrical TB
            if (_BoardSize >= 6)
            {
                std::vector<uint16_t> squares = { 26, 10, 31 }; // B to play in KvKQ (sym)
                std::cout << "B to play in KvKQ (sym) " << ExactScore_to_string(sym_tb_b->score_v(squares)) << " vs LOSS ";
                for (size_t z = 0; z < squares.size(); z++) std::cout << (int)squares[z] << " "; std::cout << std::endl;
            }
        }

    }

    // KPvK
    if (_BoardSize >= 2)
    {
        std::cout << "KPvK" << std::endl;
        std::vector<_PieceID> ws1;
        std::vector<_PieceID> bs1;
        ws1.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
        ws1.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        bs1.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
        std::vector<std::pair<_PieceID, uint8_t>> sw1 = _PieceSet::to_set(ws1);
        std::vector<std::pair<_PieceID, uint8_t>> sb1 = _PieceSet::to_set(bs1);
        _PieceSet ps1(sw1, sb1);
        TBH_N<_PieceID, _BoardSize, 3> TBH_KPvK(ps1, TB_TYPE::tb_2v1,TBH_IO_MODE::tb_hiearchy, TBH_OPTION::try_load_on_build);
        if (DO_BUILD) TBH_KPvK.build(TBH_IO_MODE::tb_hiearchy, 1);
        if (DO_BUILD) TBH_KPvK.save();
        TBH_KPvK.load(TBH_IO_MODE::tb_hiearchy); 
        TBH_KPvK.print();
    }

#ifdef DO_TB4
    // KQvKQ
    if (_BoardSize >= 2)
    {
        std::cout << "KQvKQ" << std::endl;
        std::vector<_PieceID> w4;
        std::vector<_PieceID> b4;
        w4.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
        w4.push_back(_Piece::get_id(PieceName::Q, PieceColor::W));
        b4.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
        b4.push_back(_Piece::get_id(PieceName::Q, PieceColor::B));
        std::vector<std::pair<_PieceID, uint8_t>> sw4 = _PieceSet::to_set(w4);
        std::vector<std::pair<_PieceID, uint8_t>> sb4 = _PieceSet::to_set(b4);
        _PieceSet ps4(sw4, sb4);
        TBH_N<_PieceID, _BoardSize, 4> TBH_4X(ps4, TB_TYPE::tb_2v2,TBH_IO_MODE::tb_hiearchy, TBH_OPTION::try_load_on_build);
        if (DO_BUILD) TBH_4X.build(TBH_IO_MODE::tb_hiearchy, 1);
        if (DO_BUILD) TBH_4X.save();
        TBH_4X.load(TBH_IO_MODE::tb_hiearchy);
        TBH_4X.print();
    }

    // KPvKQ
    if (_BoardSize >= 2)
    {
        std::cout << "KPvKQ" << std::endl;
        std::vector<_PieceID> w4;
        std::vector<_PieceID> b4;
        w4.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
        w4.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        b4.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
        b4.push_back(_Piece::get_id(PieceName::Q, PieceColor::B));
        std::vector<std::pair<_PieceID, uint8_t>> sw4 = _PieceSet::to_set(w4);
        std::vector<std::pair<_PieceID, uint8_t>> sb4 = _PieceSet::to_set(b4);
        _PieceSet ps4(sw4, sb4);
        TBH_N<_PieceID, _BoardSize, 4> TBH_4X(ps4, TB_TYPE::tb_2v2, TBH_IO_MODE::tb_hiearchy,TBH_OPTION::try_load_on_build);
        if (DO_BUILD) TBH_4X.build(TBH_IO_MODE::tb_hiearchy, 1);
        if (DO_BUILD) TBH_4X.save();
        TBH_4X.load(TBH_IO_MODE::tb_hiearchy);
        TBH_4X.print();
    }

    // KPPvK
    if (_BoardSize >= 3)
    {
        std::cout << "KPPvK" << std::endl;
        std::vector<_PieceID> w4;
        std::vector<_PieceID> b4;
        w4.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
        w4.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        w4.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        b4.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
        std::vector<std::pair<_PieceID, uint8_t>> sw4 = _PieceSet::to_set(w4);
        std::vector<std::pair<_PieceID, uint8_t>> sb4 = _PieceSet::to_set(b4);
        _PieceSet ps4(sw4, sb4);
        TBH_N<_PieceID, _BoardSize, 4> TBH_KPPvK(ps4, TB_TYPE::tb_3v1,TBH_IO_MODE::tb_hiearchy, TBH_OPTION::try_load_on_build);
        if (DO_BUILD) TBH_KPPvK.build(TBH_IO_MODE::tb_hiearchy, 1);
        if (DO_BUILD) TBH_KPPvK.save();
        TBH_KPPvK.load(TBH_IO_MODE::tb_hiearchy);
        TBH_KPPvK.print();
    }

    // KQvKP
    if (_BoardSize >= 2)
    {
        std::cout << "KQvKP" << std::endl;
        std::vector<_PieceID> w4;
        std::vector<_PieceID> b4;
        w4.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
        w4.push_back(_Piece::get_id(PieceName::Q, PieceColor::W));
        b4.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
        b4.push_back(_Piece::get_id(PieceName::P, PieceColor::B));
        std::vector<std::pair<_PieceID, uint8_t>> sw4 = _PieceSet::to_set(w4);
        std::vector<std::pair<_PieceID, uint8_t>> sb4 = _PieceSet::to_set(b4);
        _PieceSet ps4(sw4, sb4);
        TBH_N<_PieceID, _BoardSize, 4> TBH_4X(ps4, TB_TYPE::tb_2v2, TBH_IO_MODE::tb_hiearchy,TBH_OPTION::try_load_on_build);
        if (DO_BUILD) TBH_4X.build(TBH_IO_MODE::tb_hiearchy, 1);
        if (DO_BUILD) TBH_4X.save();
        TBH_4X.load(TBH_IO_MODE::tb_hiearchy);
        TBH_4X.print();

        if (_BoardSize >= 6)
        {
            std::vector<uint16_t> sq = { 35, 11, 16, 15 }; // B to play on KQvKP
            std::cout << "B to play on KQvKP " << ExactScore_to_string(TBH_4X.TB_B()->score_v(sq)) << " vs LOSS ";
            for (size_t z = 0; z < sq.size(); z++) std::cout << (int)sq[z] << " "; std::cout << std::endl;

            // Testing and expand a position on screen if error
            if (TBH_4X.TB_B()->score_v(sq) != ExactScore::LOSS)
            {
                Board<_PieceID, _BoardSize> work_board;
                work_board.clear();
                work_board.set_color(PieceColor::B);
                for (size_t z = 0; z < sq.size(); z++) work_board.set_pieceid_at(_PieceSet::ps_to_pieces(ps4)[z], sq[z]);
                std::cout << work_board.to_str() << std::endl;

                // expand a position
                {
                    Board<_PieceID, _BoardSize> work_board;
                    work_board.clear();
                    work_board.set_color(PieceColor::B);
                    for (size_t z = 0; z < sq.size(); z++) work_board.set_pieceid_at(_PieceSet::ps_to_pieces(ps4)[z], sq[z]);
                    std::cout << work_board.to_str() << std::endl;

                    Board<_PieceID, _BoardSize> b = work_board;
                    Board<_PieceID, _BoardSize> bt = work_board;
                    TablebaseUtil<_PieceID, _BoardSize> tbu;
                    std::cout << "===================================" << std::endl;
                    tbu.expand_position(bt, 30, 1);
                }
            }

            sq = { 35, 6, 16, 15 }; // W to play on KQvKP
            std::cout << "W to play on KQvKP " << ExactScore_to_string(TBH_4X.TB_W()->score_v(sq)) << " vs WIN ";
            for (size_t z = 0; z < sq.size(); z++) std::cout << (int)sq[z] << " "; std::cout << std::endl;
        }
    }

    // KPvKP
    if (_BoardSize >= 2)
    {
        std::cout << "KPvKP" << std::endl;
        std::vector<_PieceID> w4;
        std::vector<_PieceID> b4;
        w4.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
        w4.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        b4.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
        b4.push_back(_Piece::get_id(PieceName::P, PieceColor::B));
        std::vector<std::pair<_PieceID, uint8_t>> sw4 = _PieceSet::to_set(w4);
        std::vector<std::pair<_PieceID, uint8_t>> sb4 = _PieceSet::to_set(b4);
        _PieceSet ps4(sw4, sb4);
        TBH_N<_PieceID, _BoardSize, 4> TBH_KPvKP(ps4, TB_TYPE::tb_2v2, TBH_IO_MODE::tb_hiearchy,TBH_OPTION::try_load_on_build);
        if (DO_BUILD) TBH_KPvKP.build(TBH_IO_MODE::tb_hiearchy, 1);
        if (DO_BUILD) TBH_KPvKP.save();
        TBH_KPvKP.load(TBH_IO_MODE::tb_hiearchy);
        TBH_KPvKP.print();

        // expand a KPKP position on screen
        for (int i = 0; i < 1; i++)
        {
            Board<_PieceID, _BoardSize> b;
            Board<_PieceID, _BoardSize> bcopy;
            Board<_PieceID, _BoardSize> bt = _Board::get_random_position_KPKP(true);
            bcopy = b = bt; // save initial pos
            TablebaseUtil<_PieceID, _BoardSize> tbu;

            std::cout << "===================================" << std::endl;
            std::cout << bt.to_str() << std::endl << std::endl;
            tbu.expand_position(bt, 30, 1);
        }
    }

    // KPPvK
    if (_BoardSize >= 3)
    {
        std::cout << "KPPvK" << std::endl;
        std::vector<_PieceID> w4;
        std::vector<_PieceID> b4;
        w4.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
        w4.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        w4.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        b4.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
        std::vector<std::pair<_PieceID, uint8_t>> sw4 = _PieceSet::to_set(w4);
        std::vector<std::pair<_PieceID, uint8_t>> sb4 = _PieceSet::to_set(b4);
        _PieceSet ps4(sw4, sb4);
        TBH_N<_PieceID, _BoardSize, 4> TBH_KPPvK(ps4, TB_TYPE::tb_3v1, TBH_IO_MODE::tb_hiearchy,TBH_OPTION::try_load_on_build);
        if (DO_BUILD) TBH_KPPvK.build(TBH_IO_MODE::tb_hiearchy, 1);
        if (DO_BUILD) TBH_KPPvK.save();
        TBH_KPPvK.load(TBH_IO_MODE::tb_hiearchy);
        TBH_KPPvK.print();
    }
#endif

#ifdef DO_TB5
    // KPPvKP
    if (_BoardSize >= 3)
    {
        std::cout << "KPPvKP" << std::endl;
        std::vector<_PieceID> w5;
        std::vector<_PieceID> b5;
        w5.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
        w5.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        w5.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        b5.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
        b5.push_back(_Piece::get_id(PieceName::P, PieceColor::B));
        std::vector<std::pair<_PieceID, uint8_t>> sw5 = _PieceSet::to_set(w5);
        std::vector<std::pair<_PieceID, uint8_t>> sb5 = _PieceSet::to_set(b5);
        _PieceSet ps5(sw5, sb5);
        TBH_N<_PieceID, _BoardSize, 5> TBH_KPPvKP(ps5, TB_TYPE::tb_3v2, TBH_IO_MODE::tb_hiearchy,TBH_OPTION::try_load_on_build);
        if (DO_BUILD) TBH_KPPvKP.build(TBH_IO_MODE::tb_hiearchy, 1);
        if (DO_BUILD) TBH_KPPvKP.save();
        TBH_KPPvKP.load(TBH_IO_MODE::tb_hiearchy);
        TBH_KPPvKP.print();
    }
#endif

#ifdef DO_TB6
    // KPPvKPP
    if (_BoardSize >= 3)
    {
        std::cout << "KPPvKPP" << std::endl;
        std::vector<_PieceID> w6;
        std::vector<_PieceID> b6;
        w6.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
        w6.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        w6.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        b6.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
        b6.push_back(_Piece::get_id(PieceName::P, PieceColor::B));
        b6.push_back(_Piece::get_id(PieceName::P, PieceColor::B));
        std::vector<std::pair<_PieceID, uint8_t>> sw6 = _PieceSet::to_set(w6);
        std::vector<std::pair<_PieceID, uint8_t>> sb6 = _PieceSet::to_set(b6);
        _PieceSet ps6(sw6, sb6);
        TBH_N<_PieceID, _BoardSize, 6> TBH_KPPvKPP(ps6, TB_TYPE::tb_3v3, TBH_IO_MODE::tb_hiearchy,TBH_OPTION::try_load_on_build);
        if (DO_BUILD)
        {
            std::cout << "KPPvKPP build..." << std::endl;
            TBH_KPPvKPP.build(TBH_IO_MODE::tb_hiearchy, 1);
        }
        if (DO_BUILD)
        {
            std::cout << "KPPvKPP save..." << std::endl;
            TBH_KPPvKPP.save();
        }
        std::cout << "KPPvKPP load..." << std::endl;
        TBH_KPPvKPP.load(TBH_IO_MODE::tb_hiearchy);
        TBH_KPPvKPP.print();
    }
#endif

#ifdef DO_TB7
    // KPPPvKPP
    if (_BoardSize >= 4)
    {
        std::cout << "KPPPvKPP" << std::endl;
        std::vector<_PieceID> w7;
        std::vector<_PieceID> b7;
        w7.push_back(_Piece::get_id(PieceName::K, PieceColor::W));
        w7.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        w7.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        w7.push_back(_Piece::get_id(PieceName::P, PieceColor::W));
        b7.push_back(_Piece::get_id(PieceName::K, PieceColor::B));
        b7.push_back(_Piece::get_id(PieceName::P, PieceColor::B));
        b7.push_back(_Piece::get_id(PieceName::P, PieceColor::B));
        std::vector<std::pair<_PieceID, uint8_t>> sw7= _PieceSet::to_set(w7);
        std::vector<std::pair<_PieceID, uint8_t>> sb7 = _PieceSet::to_set(b7);
        _PieceSet ps7(sw7, sb7);
        std::cout << "KPPPvKPP creating hiearchy..." << std::endl;
        TBH_N<_PieceID, _BoardSize, 7> KPPPvKPP(ps7, TB_TYPE::tb_4v3, TBH_IO_MODE::tb_hiearchy, TBH_OPTION::try_load_on_build);
        if (DO_BUILD)
        {
            std::cout << "KPPPvKPP build..." << std::endl;
            KPPPvKPP.build(TBH_IO_MODE::tb_hiearchy, 1);
        }
        if (DO_BUILD)
        {
            std::cout << "KPPPvKPP save..." << std::endl;
            KPPPvKPP.save();
        }
        std::cout << "KPPPvKPP load..." << std::endl;
        KPPPvKPP.load(TBH_IO_MODE::tb_hiearchy);
        KPPPvKPP.print();
    }
#endif

    std::cout << "Press enter..." << std::endl;
    int c = getchar();
    return 0;
}
