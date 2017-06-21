#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TablebaseHandler_2v1
// Tablebase_2v1
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_2v1_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_2v1_HPP


namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize> class Tablebase_2v0;

    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_2v1 : public Tablebase<PieceID, _BoardSize, 3>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

        friend class TablebaseHandler_2v1<PieceID, _BoardSize>;

    public:
        Tablebase_2v1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 3>(v, c)
        {
        }

        bool load() override
        {
            _is_build = read_tb();
            if (_is_build) TablebaseManager<PieceID, _BoardSize>::instance()->add(name(), this);
            return _is_build;
        }

        bool save() const override { return save_tb(); }
        bool build(char verbose = 0) override { return false; }
        bool isPiecesMatch(const _Board& pos) override;

    protected:
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Tablebase_2v1<PieceID, _BoardSize>::isPiecesMatch(const _Board& pos)
    {
        if ((pos.cnt_all_piece() == 3) &&
            (pos.has_piece(_Piece::get(_piecesID[0])->get_name(), PieceColor::W)) &&
            (pos.has_piece(_Piece::get(_piecesID[1])->get_name(), PieceColor::W)) &&
            (pos.has_piece(_Piece::get(_piecesID[2])->get_name(), PieceColor::B)))
        {
            return true;
        }
        return false;
    }


    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_2v1 : public TablebaseBaseHandler<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

        friend class TablebaseHandler_1v1<PieceID, _BoardSize>;

    public:
        TablebaseHandler_2v1(std::vector<PieceID>& v) : TablebaseBaseHandler<PieceID, _BoardSize>(v)
        {
            _tb_W = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::W);
            _tb_B = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::B);

            _p_2v0.push_back(v[0]);     _p_2v0.push_back(v[1]);
            _p0_1v1.push_back(v[0]);    _p0_1v1.push_back(v[2]);
            _p1_1v1.push_back(v[1]);    _p1_1v1.push_back(v[2]);

            // children TB
            _tb_2v0_w   = new Tablebase_2v0<PieceID, _BoardSize>(_p_2v0, PieceColor::W);
            _tb_2v0_b   = new Tablebase_2v0<PieceID, _BoardSize>(_p_2v0, PieceColor::B);
            _tbh_0_1v1  = new TablebaseHandler_1v1<PieceID, _BoardSize>(_p0_1v1);
            _tbh_1_1v1  = new TablebaseHandler_1v1<PieceID, _BoardSize>(_p1_1v1);
        }

        ~TablebaseHandler_2v1()
        {
            delete _tb_W;
            delete _tb_B;

            delete  _tb_2v0_w;
            delete  _tb_2v0_b;
            delete _tbh_0_1v1;
            delete _tbh_1_1v1;;
        }

        bool build(char verbose = 0) override;
        bool is_build() const override { return _tb_W->is_build() && _tb_B->is_build(); }
        bool load() override;
        bool save() const override;

        Tablebase_2v1<PieceID, _BoardSize>* tb_W() { return _tb_W; }
        Tablebase_2v1<PieceID, _BoardSize>* tb_B() { return _tb_B; }

    protected:
        uint64_t set_mate_score(PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb);
        uint64_t set_marker(    PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo);
        uint64_t process_marker(PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo);
        ExactScore minmax_score(const uint64_t& idx_item, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo, uint8_t& ret_dtc, size_t& ret_child);
        bool find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const;
        Tablebase_2v0<PieceID, _BoardSize>* locate_children_2v0(const _Board& pos, PieceColor c, uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const;
        Tablebase_1v1<PieceID, _BoardSize>* locate_children_1v1(const _Board& pos, PieceColor c, uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const;
        void print() const;

        std::vector<PieceID>                _piecesID;
        Tablebase_2v1<PieceID, _BoardSize>* _tb_W;
        Tablebase_2v1<PieceID, _BoardSize>* _tb_B;

        std::vector<PieceID> _p_2v0;
        std::vector<PieceID> _p0_1v1;
        std::vector<PieceID> _p1_1v1;

        // children TB
        Tablebase_2v0<PieceID, _BoardSize>*         _tb_2v0_w;
        Tablebase_2v0<PieceID, _BoardSize>*         _tb_2v0_b;
        TablebaseHandler_1v1<PieceID, _BoardSize>* _tbh_0_1v1;
        TablebaseHandler_1v1<PieceID, _BoardSize>* _tbh_1_1v1;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_2v1<PieceID, _BoardSize>::save()  const
    {
        if (!_tb_W->save()) return false;
        if (!_tb_B->save()) return false;
        if (!_tb_2v0_w->save()) return false;
        if (!_tb_2v0_b->save()) return false;
        if (!_tbh_0_1v1->save()) return false;
        if (!_tbh_1_1v1->save()) return false;
        return true;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_2v1<PieceID, _BoardSize>::load()
    {
        if (!_tb_2v0_w->load()) return false;
        if (!_tb_2v0_b->load()) return false;
        if (!_tbh_0_1v1->load()) return false;
        if (!_tbh_1_1v1->load()) return false;
        if (!_tb_W->load()) return false;
        if (!_tb_B->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_2v1<PieceID, _BoardSize>::build(char verbose)
    {
        _tb_2v0_w->build(verbose);
        _tb_2v0_b->build(verbose);
        _tbh_0_1v1->build(verbose);
        _tbh_1_1v1->build(verbose);

        assert(_tb_2v0_w->is_build() == true);
        assert(_tb_2v0_b->is_build() == true);
        assert(_tbh_0_1v1->is_build() == true);
        assert(_tbh_1_1v1->is_build() == true);

        std::string str_pieces;
        for (auto& v : _piecesID )str_pieces += _Piece::to_str(v);

        uint64_t n = 0;
        uint64_t m = 0;
        int iter = 0;
        
        if (verbose) { std::cout << "TablebaseHandler_2v1: " << str_pieces << std::endl; }
        if (verbose) { std::cout << "scanning mate in (0/1) ply ..." << std::endl; }
        n = set_mate_score(PieceColor::W, _tb_W); 
        if (verbose) { std::cout << "W (0/1 move) mate positions:" << n << std::endl; }
        n = set_mate_score(PieceColor::B, _tb_B);
        if (verbose) { std::cout << "B (0/1 move) mate positions:" << n << std::endl; }

        do
        {
            iter++;  if (verbose) { std::cout << "Iteration: " << iter << std::endl; }

            _tb_W->clear_marker();
            _tb_B->clear_marker();

            n = set_marker(PieceColor::W, _tb_W, _tb_B);
            if (verbose) { std::cout << "W set_marker positions:" << n << std::endl; }

            n = process_marker(PieceColor::B, _tb_B, _tb_W);
            if (verbose) { std::cout << "B process_marker positions:" << n << std::endl; }

            m = set_marker(PieceColor::B, _tb_B, _tb_W);
            if (verbose) { std::cout << "B set_marker positions:" << m << std::endl; }

            m = process_marker(PieceColor::W, _tb_W, _tb_B);
            if (verbose) { std::cout << "W process_marker positions:" << m << std::endl; }

            if (verbose) _tb_W->print_dtc(20);
            if (verbose) print();
            if ((n+m) == 0) break;

        } while (n+m > 0);

        _tb_W->set_build(true);
        _tb_B->set_build(true);
        _tb_W->set_unknown_to_draw();
        _tb_B->set_unknown_to_draw();
        TablebaseManager<PieceID, _BoardSize>::instance()->add(_tb_W->name(), _tb_W);
        TablebaseManager<PieceID, _BoardSize>::instance()->add(_tb_B->name(), _tb_B);
        return true;
    }

    // find_score_children_tb
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_2v1<PieceID, _BoardSize>::find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const
    {
        uint16_t ret_child_sq0; uint16_t ret_child_sq1;
        Tablebase_1v1<PieceID, _BoardSize>* tb_1v1 = locate_children_1v1(pos, color, ret_child_sq0, ret_child_sq1);
        if (tb_1v1 != nullptr)
        {
            ret_sc = tb_1v1->score(ret_child_sq0, ret_child_sq1);
            return true;
        }
        Tablebase_2v0<PieceID, _BoardSize>* tb_2v0 = locate_children_2v0(pos, color, ret_child_sq0, ret_child_sq1);
        if (tb_2v0 != nullptr)
        {
            ret_sc = tb_2v0->score(ret_child_sq0, ret_child_sq1);
            return true;
        }
        return false;
    }

    // locate_children
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_1v1<PieceID, _BoardSize>* TablebaseHandler_2v1<PieceID, _BoardSize>::locate_children_1v1(const _Board& pos, PieceColor color, uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const
    {
        if ((color == PieceColor::W) && (_tbh_0_1v1->_tb_W->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p0_1v1[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p0_1v1[1])->get_name(), PieceColor::B);
            return _tbh_0_1v1->_tb_W;
        }
        if ((color == PieceColor::B) && (_tbh_0_1v1->_tb_B->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p0_1v1[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p0_1v1[1])->get_name(), PieceColor::B);
            return _tbh_0_1v1->_tb_B;
        }

        if ((color == PieceColor::W) && (_tbh_1_1v1->_tb_W->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p1_1v1[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p1_1v1[1])->get_name(), PieceColor::B);
            return _tbh_1_1v1->_tb_W;
        }
        if ((color == PieceColor::B) && (_tbh_1_1v1->_tb_B->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p1_1v1[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p1_1v1[1])->get_name(), PieceColor::B);
            return _tbh_1_1v1->_tb_B;
        }
        return nullptr;
    }

    // locate_children
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_2v0<PieceID, _BoardSize>* TablebaseHandler_2v1<PieceID, _BoardSize>::locate_children_2v0(const _Board& pos, PieceColor color, uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const
    {
        if ((color == PieceColor::W) && (_tb_2v0_w->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_2v0[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p_2v0[1])->get_name(), PieceColor::W);
            return _tb_2v0_w;
        }
        if ((color == PieceColor::B) && (_tb_2v0_b->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_2v0[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p_2v0[1])->get_name(), PieceColor::W);
            return _tb_2v0_b;
        }
        return nullptr;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseHandler_2v1<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb)
    {      
        uint64_t n_changes = 0;
        ExactScore sc;
        std::vector<Move<PieceID>> m;

        for (uint16_t sq0 = 0; sq0 < tb->_dim1; sq0++)
        for (uint16_t sq1 = 0; sq1 < tb->_dim1; sq1++)
        for (uint16_t sq2 = 0; sq2 < tb->_dim1; sq2++)
        {
            if ((sq0 != sq1) && (sq0 != sq2) && (sq1 != sq2))
            {
                tb->_work_board->clear();
                tb->_work_board->set_color(color_to_play);
                tb->_work_board->set_pieceid_at(tb->_piecesID[0], sq0);
                tb->_work_board->set_pieceid_at(tb->_piecesID[1], sq1);
                tb->_work_board->set_pieceid_at(tb->_piecesID[2], sq2);
                m = tb->_work_board->generate_moves();
                sc = tb->_work_board->final_score(m);
                if (sc != ExactScore::UNKNOWN)
                {
                    //tb->set_dtc(sq0, sq1, sq2, 0); // default is 0
                    tb->set_score(sq0, sq1, sq2, sc);
                    tb->set_marker(sq0, sq1, sq2, false);
                    n_changes++;
                }
                else
                {
                    size_t ret_idx;
                    if (tb->_work_board->can_capture_opposite_king(m, ret_idx))
                    {
                        if (tb->_work_board->get_color() == PieceColor::W)
                        {
                            tb->set_dtc(sq0, sq1, sq2, 1);
                            tb->set_score(sq0, sq1, sq2, ExactScore::WIN);
                            tb->set_marker(sq0, sq1, sq2, false);
                            n_changes++;
                        }
                        else
                        {
                            tb->set_dtc(sq0, sq1, sq2, 1);
                            tb->set_score(sq0, sq1, sq2, ExactScore::LOSS);
                            tb->set_marker(sq0, sq1, sq2, false);
                            n_changes++;
                        }
                    }
                }
            }
        }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseHandler_2v1<PieceID, _BoardSize>::set_marker(PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        Move<PieceID> mv;
        uint16_t sq0, sq1, sq2;
        std::vector<Move<PieceID>> m_child;
        bool one_child_has_score;
        bool one_mark;

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1, sq2);
            if ((sq0 == sq1) || (sq0 == sq2) || (sq1 == sq2)) continue;
            sc = tb->score(sq0, sq1, sq2);
            if (sc != ExactScore::UNKNOWN) continue;

            tb->_work_board->clear();
            tb->_work_board->set_color(color_to_play);
            tb->_work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            tb->_work_board->set_pieceid_at(tb->_piecesID[1], sq1);
            tb->_work_board->set_pieceid_at(tb->_piecesID[2], sq2);

            // if one child has a score, will mark all other child without score
            m_child = tb->_work_board->generate_moves();
            one_child_has_score = false;
            for (size_t k = 0; k < m_child.size(); k++)
            {
                mv = m_child[k];
                tb->_work_board->apply_move(mv);

                if (tb->_work_board->cnt_all_piece() == tb->_NPIECE)
                {
                    uint16_t child_sq0 = tb->_work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                    uint16_t child_sq1 = tb->_work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color());
                    uint16_t child_sq2 = tb->_work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                    sc = tb_oppo->score(child_sq0, child_sq1, child_sq2);
                    if (sc != ExactScore::UNKNOWN)
                    {
                        one_child_has_score = true;
                    }
                }
                else
                {
                    if (find_score_children_tb(*tb->_work_board, tb->_work_board->get_color(), sc))
                    {
                        if (sc != ExactScore::UNKNOWN)
                        {
                            one_child_has_score = true;
                        }
                        else
                        {
                            assert(false);
                        }
                    }
                    else
                    {
                        assert(false);
                    }
                }

                tb->_work_board->undo_move();
                if (one_child_has_score)
                    break;
            }

            one_mark = false;
            if (one_child_has_score)
            {
                for (size_t j = 0; j < m_child.size(); j++)
                {
                    mv = m_child[j];
                    tb->_work_board->apply_move(mv);
                    if (tb->_work_board->cnt_all_piece() == tb->_NPIECE)
                    {
                        uint16_t child_sq0 = tb->_work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                        uint16_t child_sq1 = tb->_work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color());
                        uint16_t child_sq2 = tb->_work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                        sc = tb_oppo->score(child_sq0, child_sq1, child_sq2);
                        if (sc == ExactScore::UNKNOWN)
                        {
                            // mark child
                            if (tb_oppo->marker(child_sq0, child_sq1, child_sq2) == false)
                            {
                                tb_oppo->set_marker(child_sq0, child_sq1, child_sq2, true);
                                //n_changes++;
                            } 
                            n_changes++;
                            one_mark = true;
                        }
                    }
                    else
                    {
                        // child in other tb - should have known score
                    }
                    tb->_work_board->undo_move();
                }
            }

            if (one_mark)
            {
                // mark parent also
                sc = tb->score(sq0, sq1, sq2);
                if (sc == ExactScore::UNKNOWN)
                {
                    if (tb->marker(sq0, sq1, sq2) == false)
                    {
                        tb->set_marker(sq0, sq1, sq2, true);
                        //n_changes++;
                    }
                    n_changes++;
                }
            }
            
            if ((!one_mark) && (one_child_has_score))
            {
                // all known child score
                sc = tb->score(sq0, sq1, sq2);
                if (sc == ExactScore::UNKNOWN)
                {
                    uint8_t ret_dtc; size_t ret_child_index;
                    sc = this->minmax_score(tb->index_item(sq0, sq1, sq2), tb, tb_oppo, ret_dtc, ret_child_index);
                    if (sc != ExactScore::UNKNOWN)
                    {
                        tb->set_dtc(sq0, sq1, sq2, ret_dtc);
                        tb->set_score(sq0, sq1, sq2, sc);
                        tb->set_marker(sq0, sq1, sq2, false);
                        n_changes++;                                // continu since score changes
                    }
                    else
                    {
                       assert(false);
                    }
                }
            }
        }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseHandler_2v1<PieceID, _BoardSize>::process_marker(PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        uint16_t sq0, sq1, sq2;
        std::vector<Move<PieceID>> m;

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1, sq2);
            if ((sq0 == sq1) || (sq0 == sq2) || (sq1 == sq2)) continue;
            if (tb->marker(sq0, sq1, sq2) == false) 
                continue;

            tb->_work_board->clear();
            tb->_work_board->set_color(color_to_play);
            tb->_work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            tb->_work_board->set_pieceid_at(tb->_piecesID[1], sq1);
            tb->_work_board->set_pieceid_at(tb->_piecesID[2], sq2);

            m = tb->_work_board->generate_moves();
            sc = tb->_work_board->final_score(m);
            if (sc != ExactScore::UNKNOWN)
            {
                //tb->set_dtc(sq0, sq1, 1+dtc(...)); // NOT HAPPENING!
                tb->set_score(sq0, sq1, sq2, sc);
                tb->set_marker(sq0, sq1, sq2, false);
                n_changes++;
            }
            else
            {
                uint8_t ret_dtc; size_t ret_child_index;
                sc = this->minmax_score(tb->index_item(sq0, sq1, sq2), tb, tb_oppo, ret_dtc, ret_child_index);

                if (sc != ExactScore::UNKNOWN)
                {
                    tb->set_dtc(sq0, sq1, sq2, ret_dtc);
                    tb->set_score(sq0, sq1, sq2, sc);
                    tb->set_marker(sq0, sq1, sq2, false);
                    n_changes++;
                }
            }
        }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    ExactScore TablebaseHandler_2v1<PieceID, _BoardSize>::minmax_score(const uint64_t& idx_item ,Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo, uint8_t& ret_dtc, size_t& ret_child)
    {
        ExactScore sc = tb->score_at_idx(idx_item);
        if (sc != ExactScore::UNKNOWN)
            return sc;

        bool has_all_child_score = true;
        ExactScore max_score = ExactScore::UNKNOWN;

        uint8_t     best_dtc = -1;
        size_t      best_dtc_child_index = -1;
        ret_dtc     = 0;
        ret_child   = 0;

        Move<PieceID> mv;
        std::vector<Move<PieceID>> m_child = tb->_work_board->generate_moves();
        for (size_t i = 0; i < m_child.size(); i++)
        {
            mv = m_child[i];
            tb->_work_board->apply_move(mv);
            if (tb->_work_board->cnt_all_piece() == tb->_NPIECE)
            {
                uint16_t child_sq0 = tb->_work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                uint16_t child_sq1 = tb->_work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color());
                uint16_t child_sq2 = tb->_work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                sc = tb_oppo->score(child_sq0, child_sq1, child_sq2);
                if (sc == ExactScore::UNKNOWN)
                {
                    has_all_child_score = false;
                }
                else
                {
                    bool is_same; bool is_better;
                    max_score = best_score(tb->_work_board->get_color(), sc, max_score, is_same, is_better);
                    if (is_better)
                    {
                        best_dtc = 1 + tb_oppo->dtc(child_sq0, child_sq1, child_sq2);
                        best_dtc_child_index = i;
                    }
                    else if (is_same)
                    {
                        if (best_dtc == -1)
                        {
                            best_dtc = 1 + tb_oppo->dtc(child_sq0, child_sq1, child_sq2);
                            best_dtc_child_index = i;
                        }
                        else if ( ((tb->_work_board->get_opposite_color() == PieceColor::W) && (max_score == ExactScore::WIN)) ||
                                  ((tb->_work_board->get_opposite_color() == PieceColor::B) && (max_score == ExactScore::LOSS)) )
                        {
                            if (best_dtc > 1 + tb_oppo->dtc(child_sq0, child_sq1, child_sq2))  // seek lower dtc
                            {
                                best_dtc = 1 + tb_oppo->dtc(child_sq0, child_sq1, child_sq2);
                                best_dtc_child_index = i;
                            }
                        }
                        else
                        {
                            if (best_dtc < 1 + tb_oppo->dtc(child_sq0, child_sq1, child_sq2))  // seek higher dtc
                            {
                                best_dtc = 1 + tb_oppo->dtc(child_sq0, child_sq1, child_sq2);
                                best_dtc_child_index = i;
                            }
                        }
                    }
                }
            }
            else
            {
                // children
                if (tb->_work_board->cnt_all_piece() != (tb->_NPIECE - 1))
                {
                    has_all_child_score = false;
                    assert(false);
                    continue;
                }

                if (find_score_children_tb(*tb->_work_board, tb->_work_board->get_color(), sc))
                {
                    if (sc == ExactScore::UNKNOWN)
                    {
                        has_all_child_score = false;
                        assert(false);
                        continue;
                    }
                }
                bool is_same; bool is_better;
                max_score = best_score(tb->_work_board->get_color(), sc, max_score, is_same, is_better);
            }
            tb->_work_board->undo_move();
        }

        ret_dtc = best_dtc;
        ret_child = best_dtc_child_index;
        if (has_all_child_score)
        {
            return max_score;
        }
        if ((tb->_work_board->get_color() == PieceColor::W) && (max_score == ExactScore::WIN))  return ExactScore::WIN;
        if ((tb->_work_board->get_color() == PieceColor::B) && (max_score == ExactScore::LOSS)) return ExactScore::LOSS;
        return ExactScore::UNKNOWN;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    void TablebaseHandler_2v1<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }
};
#endif

