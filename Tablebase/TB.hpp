#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase (TB)
//
//
#ifndef _AL_CHESS_TABLEBASE_TB_HPP
#define _AL_CHESS_TABLEBASE_TB_HPP

namespace chess
{
    constexpr uint64_t  powN(uint64_t v, uint64_t n) { return (n > 0) ? v * powN(v, n - 1) : 1; }
    constexpr uint8_t   TB_size_item() { return 3; }    // score = 2 bits + marker = 1 bit
    constexpr uint64_t  TB_size_dim(uint64_t boardsize) { return boardsize*boardsize; }
    constexpr uint64_t  TB_FULL_SIZE(uint64_t boardsize, uint64_t npiece) { return powN(boardsize*boardsize, npiece); }

    // NEXT: TB type can come from pieceset or _piecesID =>nw/nb ...
    enum class TB_TYPE {
        tb_unknown,
        tb_0vX, tb_Xv0,
        tb_1v1,
        tb_2v1, tb_2v1_sym,
        tb_3v1, tb_2v2, tb_3v1_sym,
        tb_4v1, tb_3v2, tb_4v1_sym, tb_3v2_sym,
        tb_5v1, tb_4v2, tb_3v3, tb_5v1_sym, tb_4v2_sym,
        tb_6v1, tb_5v2, tb_4v3, tb_6v1_sym, tb_5v2_sym, tb_4v3_sym,

        tb_7v1, tb_6v2, tb_5v3, tb_4v4, //...
        tb_8v1, tb_7v2, tb_6v3, tb_5v4,
        tb_9v1, tb_8v2, tb_7v3, tb_6v4, tb_5v5
    };

    enum class TBH_OPTION { none, try_load_on_build, force_rebuild, memory_map_on_build };
    enum class TBH_IO_MODE { tb_only, tb_and_child, tb_hiearchy };

    TB_TYPE sym_tb_type(TB_TYPE t)
    {
        if      (t == TB_TYPE::tb_2v1) return TB_TYPE::tb_2v1_sym;
        else if (t == TB_TYPE::tb_3v1) return TB_TYPE::tb_3v1_sym;
        else if (t == TB_TYPE::tb_4v1) return TB_TYPE::tb_4v1_sym;
        else if (t == TB_TYPE::tb_3v2) return TB_TYPE::tb_3v2_sym;
        else if (t == TB_TYPE::tb_5v1) return TB_TYPE::tb_5v1_sym;
        else if (t == TB_TYPE::tb_4v2) return TB_TYPE::tb_4v2_sym;
        else if (t == TB_TYPE::tb_6v1) return TB_TYPE::tb_6v1_sym;
        else if (t == TB_TYPE::tb_5v2) return TB_TYPE::tb_5v2_sym;
        else if (t == TB_TYPE::tb_4v3) return TB_TYPE::tb_4v3_sym;
        return TB_TYPE::tb_unknown;
    }

    std::string TB_TYPE_to_string(TB_TYPE t)
    {
        if (t == TB_TYPE::tb_0vX)           return "tb_0vX";
        else if (t == TB_TYPE::tb_Xv0)      return "tb_Xv0";
        else if (t == TB_TYPE::tb_1v1)      return "tb_1v1";
        else if (t == TB_TYPE::tb_2v1)      return "tb_2v1";
        else if (t == TB_TYPE::tb_2v2)      return "tb_2v2";
        else if (t == TB_TYPE::tb_2v1_sym)  return "tb_2v1_sym";
        else if (t == TB_TYPE::tb_3v1)      return "tb_3v1";
        else if (t == TB_TYPE::tb_3v1_sym)  return "tb_3v1_sym";
        else if (t == TB_TYPE::tb_4v1)      return "tb_4v1";
        else if (t == TB_TYPE::tb_3v2)      return "tb_3v2";
        else if (t == TB_TYPE::tb_4v1_sym)  return "tb_4v1_sym";
        else if (t == TB_TYPE::tb_3v2_sym)  return "tb_3v2_sym";
        else if (t == TB_TYPE::tb_5v1)      return "tb_5v1";
        else if (t == TB_TYPE::tb_4v2)      return "tb_4v2";
        else if (t == TB_TYPE::tb_3v3)      return "tb_3v3";
        else if (t == TB_TYPE::tb_5v1_sym)  return "tb_5v1_sym";
        else if (t == TB_TYPE::tb_4v2_sym)  return "tb_4v2_sym";
        else if (t == TB_TYPE::tb_6v1)      return "tb_6v1";
        else if (t == TB_TYPE::tb_5v2)      return "tb_5v2";
        else if (t == TB_TYPE::tb_4v3)      return "tb_4v3";
        else if (t == TB_TYPE::tb_6v1_sym)  return "tb_6v1_sym";
        else if (t == TB_TYPE::tb_5v2_sym)  return "tb_5v2_sym";
        else if (t == TB_TYPE::tb_4v3_sym)  return "tb_4v3_sym";
        return "tb_unknown";
    }

     TB_TYPE tb_type_NM(uint16_t nw, uint16_t nb)
    {
         if ((nw == 1) && (nb == 0)) return TB_TYPE::tb_Xv0;
         else if ((nw == 0) && (nb == 1)) return TB_TYPE::tb_0vX;

         else if ((nw == 1) && (nb == 1)) return TB_TYPE::tb_1v1;
         else if ((nw == 2) && (nb == 1)) return TB_TYPE::tb_2v1;
         else if ((nw == 1) && (nb == 2)) return TB_TYPE::tb_2v1_sym;

         else if ((nw == 3) && (nb == 1)) return TB_TYPE::tb_3v1;
         else if ((nw == 2) && (nb == 2)) return TB_TYPE::tb_2v2;
         else if ((nw == 1) && (nb == 3)) return TB_TYPE::tb_3v1_sym;

         else if ((nw == 4) && (nb == 1)) return TB_TYPE::tb_4v1;
         else if ((nw == 3) && (nb == 2)) return TB_TYPE::tb_3v2;
         else if ((nw == 1) && (nb == 4)) return TB_TYPE::tb_4v1_sym;
         else if ((nw == 2) && (nb == 3)) return TB_TYPE::tb_3v2_sym;

        else if ((nw == 5) && (nb == 1)) return TB_TYPE::tb_5v1;
        else if ((nw == 4) && (nb == 2)) return TB_TYPE::tb_4v2;
        else if ((nw == 3) && (nb == 3)) return TB_TYPE::tb_3v3;
        else if ((nw == 1) && (nb == 5)) return TB_TYPE::tb_5v1_sym;
        else if ((nw == 2) && (nb == 4)) return TB_TYPE::tb_4v2_sym;

        else if ((nw == 6) && (nb == 1)) return TB_TYPE::tb_6v1;
        else if ((nw == 5) && (nb == 2)) return TB_TYPE::tb_5v2;
        else if ((nw == 4) && (nb == 3)) return TB_TYPE::tb_4v3;
        else if ((nw == 1) && (nb == 6)) return TB_TYPE::tb_6v1_sym;
        else if ((nw == 2) && (nb == 5)) return TB_TYPE::tb_5v2_sym;
        else if ((nw == 3) && (nb == 4)) return TB_TYPE::tb_4v3_sym;

        return TB_TYPE::tb_unknown;
    }

    // TablebaseBase
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseBase
    {
    public:
        TablebaseBase() {}
        virtual ~TablebaseBase() {}

        virtual bool        is_full_type() const = 0;               // FULL TB or Partial TB
        virtual bool        is_symmetry_TB() const = 0;

        virtual bool        legal(const std::vector<uint16_t>& sq) const = 0;
        virtual ExactScore  score_v(const std::vector<uint16_t>& sq) const = 0;
        virtual uint8_t     dtc_v(const std::vector<uint16_t>& sq) const = 0;

        virtual bool        is_build_and_loaded() const = 0;
        virtual bool        load() = 0;
        virtual bool        save() const = 0;
        virtual void        print() const = 0;
        virtual uint8_t     num_piece() const = 0;

        virtual uint64_t    size_tb() const = 0;
        virtual uint64_t    size_full_tb() const = 0;

        virtual bool valid_index(uint64_t index, Board<PieceID, _BoardSize>& _work_board, std::vector<uint16_t>& ret_sq) const = 0;
        static void order_sq_v(std::vector<uint16_t>& sq, const std::vector<PieceID>& piecesID);
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    struct STRUCT_PIECE_RANK
    {
        PieceID     ret_id;
        uint16_t    ret_count;
        uint16_t    rank_ret_id;
        uint16_t    ret_instance;
    };

    // Tablebase
    // TB is either FULL or Partial (subset fitting in RAM) - this would provide initial train/test data set for making predictor
    // Partial TB will contains unknown positions that maybe completed later by more exhaustive minmax search
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    class Tablebase : public TablebaseBase<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

        friend class TBH_Symmetry<PieceID, _BoardSize, 1>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 2>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 3>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 4>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 5>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 6>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 7>;

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        uint64_t friend set_mate_score_v(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, size_t from, size_t to);

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        uint64_t friend setup_marker_v(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo, size_t from, size_t to);

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        uint64_t friend process_marker_v(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo, size_t from, size_t to);

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        bool friend build_base_vv(TBH<PieceID, _BoardSize>* tbh, TablebaseBase<PieceID, _BoardSize>* tb_W, TablebaseBase<PieceID, _BoardSize>* tb_B, char verbose);

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        ExactScore friend generate_child_info(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play,
                            Tablebase<PieceID, _BoardSize, NPIECE>* tb, 
                            Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo,
                            Board<PieceID, _BoardSize>* _work_board,
                            std::vector<ExactScore>&    child_sc,
                            std::vector<uint8_t>&       child_dtc,
                            std::vector<bool>&          child_is_promo,
                            std::vector<bool>&          child_is_capture,
                            std::vector<bool>&          child_is_pawn,
                            std::vector<uint16_t>&      child_sq,
                            bool&                       exist_child_score,
                            uint8_t&                    ret_dtc,
                            size_t&                     ret_idx, bool early_exit);

    protected:
        const bool                      _do_x_symmetry = true;
        const uint8_t                   _size_item = TB_size_item();

        uint64_t                        _size_tb = TB_Manager<PieceID, _BoardSize>::instance()->new_TB_setup_size(_BoardSize, NPIECE);
        const uint64_t                  _size_full_tb = powN(_BoardSize*_BoardSize, NPIECE);
        bool                            _is_full_type = (_size_tb == powN(_BoardSize*_BoardSize, NPIECE)) ? true : false;

        const PieceColor                _color;             // Side to play
        const uint8_t                   _NPIECE;            // KQvK is 3 pieces
        std::vector<PieceID>            _piecesID;
        std::vector<const _Piece*>      _pieces;
        bool                            _is_build_and_loaded;
        std::vector<bool>*              _vbits;             // 2 bit score + 1 bit marker (true/false)
        uint8_t*                        _vdtc;              // distance to conversion
        std::map<uint64_t, uint32_t>    _vkeys;             // partial TB: list of <position indexes , index in _vbits/_vdtc>
        std::vector<uint32_t>           _vkeys_dtc_count;   // partial TB: keep free slots for various dtc
        std::map<uint64_t, uint32_t>    _vmarkers;          // partial TB: list of <marker positions indexes, index in _vbits>

        mutable std::recursive_mutex    _mutex;

    public:
        Tablebase(std::vector<PieceID>& v, PieceColor c);
        virtual ~Tablebase() { delete _vbits; delete[]_vdtc; }

        bool do_x_symmetry() const { return  _do_x_symmetry; }
        bool legal(const std::vector<uint16_t>& sq) const override;

        bool load() override
        {
            if (!load_tb()) return false;;
            return true;
        }
        bool save() const override
        {
            return save_tb();
        }

        bool        is_symmetry_TB() const override { return false; } // symmetry is number white pieces n_w < n_b
        bool        is_build_and_loaded() const override { return _is_build_and_loaded; }

        bool        is_full_type()      const override { return _is_full_type; }
        PieceColor  color()             const { return _color; }
        uint8_t     num_piece()         const override { return _NPIECE; }
        std::vector<PieceID> piecesID() const { return _piecesID; }
        uint64_t    size_tb()           const override  { return _size_tb; }
        uint64_t    size_full_tb()      const override  { return _size_full_tb; }
        uint64_t    size_keys()         const { if (!_is_full_type) return _vkeys.size(); else return 0; }

        void        print_score(int n)  const;
        void        print_dtc(int n)    const;
        uint8_t     checksum_dtc()      const;

        // score_v - read score
        ExactScore score_v(const std::vector<uint16_t>& sq)  const
        {
            if (!_is_full_type)
            {
                // LOCK
                std::lock_guard<std::recursive_mutex> lock(_mutex);

                uint64_t m = index_v(sq);
                auto search = _vkeys.find(m);
                if (search != _vkeys.end())
                {
                    uint32_t idx = search->second;
                    return score_at_idx(idx * _size_item);
                }
                else
                {
                    return ExactScore::UNKNOWN;
                }
            }
            uint64_t n = index_item_v(sq);
            return score_at_idx(n);
        }

        // set_dtc_v
        void set_dtc_v(std::vector<uint16_t>& sq, uint8_t dtc)
        {
            if (!_is_full_type)
            {
                // LOCK
                std::lock_guard<std::recursive_mutex> lock(_mutex);

                uint64_t m = index_v(sq);
                auto search = _vkeys.find(m);
                if (search != _vkeys.end())
                {
                    uint32_t idx = search->second;
                    set_dtc_at_idx(idx, dtc);
                    _vkeys_dtc_count[dtc] = _vkeys_dtc_count[dtc]+1;
                    return;
                }
                return; // set_score before setting dtc, to have a key
            }
            uint64_t n = index_v(sq);
            set_dtc_at_idx(n, dtc);
        }

        // has_space_at_dtc
        bool has_space_at_dtc(uint8_t dtc)
        {
            if (_is_full_type) return true;
            
            // LOCK
            std::lock_guard<std::recursive_mutex> lock(_mutex);

            // Maintain free space per dtc - Find typical distribution of dtc...
            // dtc[0]   20%
            // dtc[1]   20%
            // dtc[2...255] 60%/254
            if (dtc == 0) return _vkeys_dtc_count[0] < (0.20 * _size_tb);
            else if (dtc == 1) return _vkeys_dtc_count[1] < (0.20 * _size_tb);
            double n = (double)TB_Manager<PieceID, _BoardSize>::instance()->_TB_MAX_DTC;
            if (n < 3) n = 3;
            return _vkeys_dtc_count[dtc] < ((0.60/(n-2)) * _size_tb);          
        }

        // has_space_marker
        bool has_space_marker()
        {
            if (_is_full_type) return false; // only for partial TB

            // LOCK
            std::lock_guard<std::recursive_mutex> lock(_mutex);

            return _vmarkers.size() < _size_tb;
        }

        // set_score_v
        void set_score_v(std::vector<uint16_t>& sq, ExactScore sc)
        {
            if (!_is_full_type)
            {
                // LOCK
                std::lock_guard<std::recursive_mutex> lock(_mutex);

                uint64_t m = index_v(sq);
                auto search = _vkeys.find(m);
                if (search != _vkeys.end())
                {
                    uint32_t idx = search->second;
                    set_score_at_idx(idx * _size_item, sc);
                    return;
                }
                else
                {
                    // create an entry
                    uint32_t size_map = (uint32_t)_vkeys.size(); // 0...size_map-1
                    if (size_map < _size_tb)
                    {
                        _vkeys.insert(std::pair<uint64_t, uint32_t>({ m, size_map }));
                        uint32_t idx = size_map;
                        set_score_at_idx(idx * _size_item, sc);
                        return;
                    }
                    else
                    {
                        //...
                        std::stringstream ss_detail;
                        ss_detail << "TB score limit reached." << "\n";
                        std::cerr << ss_detail.str();
                        return;
                    }
                }
            }
            uint64_t n = index_item_v(sq);
            set_score_at_idx(n, sc);
        }

        // set_marker_v
        void set_marker_v(const std::vector<uint16_t>& sq, bool v)
        {
            if (!_is_full_type)
            {
                // LOCK
                std::lock_guard<std::recursive_mutex> lock(_mutex);

                uint64_t m = index_v(sq);
                auto search = _vmarkers.find(m);
                if (search != _vmarkers.end())
                {
                    uint32_t idx = search->second;
                    set_bit(idx * _size_item, 2, v);
                    return;
                }
                else
                {
                    if (v)  // true
                    {
                        // create a new entry 
                        uint32_t size_map = (uint32_t)_vmarkers.size(); // 0-size_map-1
                        if (size_map < _size_tb)
                        {
                            _vmarkers.insert(std::pair<uint64_t, uint32_t>({ m, size_map }));

                            uint32_t idx = size_map;
                            set_bit(idx * _size_item, 2, v);
                            return;
                        }
                        else
                        {
                            //...may see this msg one time per thread
                            std::stringstream ss_detail;
                            ss_detail << "TB markers limit reached." << "\n";
                            std::cerr << ss_detail.str();
                            return;
                        }
                    }
                }
                return;
            }
            uint64_t n = index_item_v(sq);
            set_bit(n, 2, v);
        }

        // square_at_index_v - raw board square
        void square_at_index_v(uint64_t idx, std::vector<uint16_t>& sq) const
        {
            assert(_NPIECE == sq.size());
            square_at_index_vv(0, idx, sq);
        }

        // marker_v
        bool marker_v(std::vector<uint16_t>& sq)
        {
            if (!_is_full_type)
            {
                // LOCK
                std::lock_guard<std::recursive_mutex> lock(_mutex);

                uint64_t m = index_v(sq);
                auto search = _vmarkers.find(m);
                if (search != _vmarkers.end())
                {
                    uint32_t idx = search->second;
                    return bit(idx * _size_item, 2);
                }
                return false;
            }
            uint64_t n = index_item_v(sq);
            return bit(n, 2);
        }

        // dtc_v
        uint8_t dtc_v(const std::vector<uint16_t>& sq)  const 
        {
            if (!_is_full_type)
            {
                // LOCK
                std::lock_guard<std::recursive_mutex> lock(_mutex);

                uint64_t m = index_v(sq);
                auto search = _vkeys.find(m);
                if (search != _vkeys.end())
                {
                    uint32_t idx = search->second;
                    return dtc_at_idx(idx);
                }
                return 0;
            }
            uint64_t n = index_v(sq);
            return dtc_at_idx(n);
        }

        // has_key
        bool has_key(const std::vector<uint16_t>& sq)
        {
            if (!_is_full_type)
            {
                // LOCK
                std::lock_guard<std::recursive_mutex> lock(_mutex);

                uint64_t m = index_v(sq);
                auto search = _vkeys.find(m);
                if (search != _vkeys.end())
                {
                    return true;
                }
                return false;
            }
            return false; // not for full TB
        }

        uint32_t keys_size() 
        {
            if (!_is_full_type)
            {
                // LOCK
                std::lock_guard<std::recursive_mutex> lock(_mutex);

                return (uint32_t)_vkeys.size();
            }
            return 0; // not for full TB
        }

        // name()
        std::string name() const    // Unique name to identify and persist in file stream
        {
            return TB_Manager<PieceID, _BoardSize>::instance()->name_pieces(_piecesID, _color);
        }

        // translate_x
        void translate_x(std::vector<uint16_t>& sq)  const
        {
            uint16_t x;
            uint16_t y;
            for (size_t i = 0; i < sq.size(); i++)
            {
                x = (uint16_t)(sq[i] % _BoardSize);
                y = (uint16_t)(sq[i] / _BoardSize);
                sq[i] = (y * _BoardSize) + ((_BoardSize-1) - x); // 6: 0<-->5
            }
        }

        // can_translate_x
        bool can_translate_x(const std::vector<uint16_t>& sq)  const
        {
            PieceID WKid = _Piece::get_id(PieceName::K, PieceColor::W);
            for (size_t i = 0; i < sq.size(); i++)
            {
                if (_piecesID[i] == WKid)
                {
                    double x = sq[i] % _BoardSize;
                    double b = _BoardSize;
                    if (x >= (b / 2.0)) // 6: >= 3, 5: >=2.5
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        bool valid_index(uint64_t index, Board<PieceID, _BoardSize>& _work_board, std::vector<uint16_t>& ret_sq) const override;

    protected:
        uint64_t dim(size_t n) const { return (n > 0) ? TB_size_dim(_BoardSize) * dim(n - 1) : 1; }

        PieceID pieceID(uint8_t idx) { return _piecesID[idx]; }
        std::vector<uint64_t> get_index_dtc(uint8_t value_dtc, ExactScore value_sc) const;

        void square_at_index_vv(size_t n, uint64_t idx, std::vector<uint16_t>& sq)  const
        {
            if (n >= sq.size()) return;
            sq[n] = (uint16_t)(idx / dim(sq.size() - (n+1)));
            uint64_t t = idx - (sq[n] * dim(sq.size() - (n + 1)));
            square_at_index_vv(n+1, t, sq);
            if (n==0) order_sq_v(sq);
        }

        void order_sq_v(std::vector<uint16_t>& sq)  const
        {
            TablebaseBase<PieceID, _BoardSize>::order_sq_v(sq, _piecesID);
        }

    protected:
        bool save_tb() const;
        bool save_tb_dtc() const;
        bool save_tb_keys() const;
        bool read_tb();
        bool read_tb_dtc();
        bool read_tb_keys();
        bool load_tb();
 
        uint64_t index_item_v(const std::vector<uint16_t>& sq)  const
        {
            return index_v(sq) * _size_item;
        }

        uint64_t index_v(const std::vector<uint16_t>& sq)  const
        {
            if (_do_x_symmetry)
            {
                if (can_translate_x(sq))
                {
                    std::vector<uint16_t> sq_copy = sq;
                    translate_x(sq_copy);

                    uint64_t n = 0;
                    for (size_t i = 0; i < sq_copy.size(); i++) n += (dim((sq_copy.size() - (i + 1))) * sq_copy[i]);
                    return n;
                }
            }

            uint64_t n = 0;
            for (size_t i = 0; i < sq.size(); i++) n += (dim((sq.size() - (i + 1))) * sq[i]);
            return n;
        }

        bool bit(const uint64_t& idx_item, const uint8_t& idx_bit)  const
        {
            try
            {
                return (_vbits->at(idx_item + idx_bit) == true);
            }
            catch (std::exception& re)
            {
                std::stringstream ss_detail;
                ss_detail << re.what() << "\n";
                ss_detail << " bit " << (long long)(idx_item + idx_bit) << " size: " << (long long)_vbits->size() << "\n";
                std::cerr << ss_detail.str();
                throw;
            }
        }

        void set_bit(const uint64_t& idx_item, const uint8_t& idx_bit, bool v)
        {
            try
            {
                _vbits->at(idx_item + idx_bit) = v;
            }
            catch (std::exception& re)
            {
                std::stringstream ss_detail;
                ss_detail << re.what() << "\n";
                ss_detail << " set_bit " << (long long)(idx_item + idx_bit) << " size: " << (long long)_vbits->size() << "\n";
                std::cerr << ss_detail.str();
                throw;
            }
        }

        void print() const override;
        bool check_unknown() const;
        void set_unknown_to_draw();
        void clear_marker();
        void set_build_and_loaded(bool v) { _is_build_and_loaded = v; }

        ExactScore score_at_idx(const uint64_t& idx_item)  const
        {
            bool bit0 = bit(idx_item, 0);
            bool bit1 = bit(idx_item, 1);
            if (bit0 && bit1)    return ExactScore::WIN;
            if (bit0 && (!bit1)) return ExactScore::DRAW;
            if ((!bit0) && bit1) return ExactScore::LOSS;
            return ExactScore::UNKNOWN;
        }
        void set_score_at_idx(const uint64_t& idx_item, ExactScore sc)
        {
            if (sc == ExactScore::WIN) { set_bit(idx_item, 0, 1); set_bit(idx_item, 1, 1); }
            else if (sc == ExactScore::DRAW) { set_bit(idx_item, 0, 1); set_bit(idx_item, 1, 0); }
            else if (sc == ExactScore::LOSS) { set_bit(idx_item, 0, 0); set_bit(idx_item, 1, 1); }
            else { set_bit(idx_item, 0, 0); set_bit(idx_item, 1, 0); }
        }
        uint8_t dtc_at_idx(const uint64_t& idx)  const
        {
            return _vdtc[idx];
        }

        void set_marker_at_idx(const uint64_t& idx_item, bool v)
        {
            if (v == true) set_bit(idx_item, 2, 1);
            else  set_bit(idx_item, 2, 0);
        }
        void set_dtc_at_idx(const uint64_t& idx, uint8_t v)
        {
            _vdtc[idx] = v;
        }

        void writeBits(std::ostream& os) const;
        void readBits(std::istream& is);
    };

    // order_sq_v
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TablebaseBase<PieceID, _BoardSize>::order_sq_v(std::vector<uint16_t>& sq, const std::vector<PieceID>& piecesID)
    {
        bool not_done = true;
        while (not_done)
        {
            not_done = false;
            for (size_t i = 0; i < sq.size() - 1; i++)
            {
                // sorting
                if ((piecesID[i] == piecesID[i + 1]) && (sq[i] > sq[i + 1]))
                {
                    // swap
                    uint16_t t = sq[i];  sq[i] = sq[i + 1]; sq[i + 1] = t;
                    not_done = true;
                    break;
                }
            }
        }
    }
    
    // legal
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::legal(const std::vector<uint16_t>& sq) const
    {
        if (sq.size() != NPIECE) return false;
        for (size_t z = 0; z < NPIECE; z++)
        {
            if (std::count(sq.begin(), sq.end(), sq[z]) > 1) 
                return false;
        }
        Board<PieceID, _BoardSize> work_board;
        work_board.clear();
        work_board.set_color(this->color());
        for (size_t z = 0; z < NPIECE; z++)  work_board.set_pieceid_at(this->_piecesID[z], sq[z]);
        if (!work_board.legal_pos())
            return false;

        return true;
    }

    // valid_index
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::valid_index(uint64_t index, Board<PieceID, _BoardSize>& _work_board, std::vector<uint16_t>& sq) const
    {
        if (sq.size() != NPIECE) return false;
        if (!_is_full_type)
        {
            // allow scanning of the full TB space to decide what to insert in the partial TB !
        }

        this->square_at_index_v(index, sq);
        for (size_t z = 0; z < NPIECE; z++)
        {
            if (std::count(sq.begin(), sq.end(), sq[z]) > 1)
                return false;
        }
        if (this->do_x_symmetry() && (this->can_translate_x(sq)))
            return false;

        _work_board.clear();
        _work_board.set_color(this->color());
        for (size_t z = 0; z < NPIECE; z++)  _work_board.set_pieceid_at(this->_piecesID[z], sq[z]);
        if (!_work_board.legal_pos())
            return false;

        return true;
    }

    // print
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::print() const
    {
        uint64_t n = 0;
        uint64_t n_win = 0;
        uint64_t n_loss = 0;
        uint64_t n_draw = 0;
        ExactScore sc;

        if (this->color() == PieceColor::W) std::cout << "TB Color: White" << std::endl;
        else std::cout << "TB Color: Black" << std::endl;
        double d = ((double)_size_tb) / ((double)TB_FULL_SIZE(_BoardSize, NPIECE));
        std::cout << "TB size: " << this->_size_tb << " / " << TB_FULL_SIZE(_BoardSize, NPIECE) <<  " (" << d << ")" << std::endl;
        std::cout << "TB Number pieces: " << (int)this->_NPIECE << std::endl;
        for (auto& v : _piecesID)
        {
            std::cout << _Piece::to_str(v);
        }
        std::cout << std::endl;
        {
            n = 0; n_win = 0; n_loss = 0; n_draw = 0;
            for (uint64_t i = 0; i < this->_size_tb; i++)
            {
                sc = this->score_at_idx(i * this->_size_item);
                if (sc != ExactScore::UNKNOWN) n++;
                if (sc == ExactScore::WIN) n_win++;
                if (sc == ExactScore::LOSS) n_loss++;
                if (sc == ExactScore::DRAW)
                    n_draw++;
            }
            std::cout << "score positions: " << n << std::endl;
            std::cout << "win  positions:  " << n_win << std::endl;
            std::cout << "loss positions:  " << n_loss << std::endl;
            std::cout << "draw positions:  " << n_draw << std::endl;
            std::cout << std::endl;
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::check_unknown() const
    {
        ExactScore sc;
        {
            for (uint64_t i = 0; i < this->_size_tb; i++)
            {
                sc = this->score_at_idx(i * this->_size_item);
                if (sc == ExactScore::UNKNOWN) 
                    return true;
            }
        }
        return false;
    }

    // set_unknown_to_draw
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::set_unknown_to_draw()
    {
        if (!_is_full_type) return;

        ExactScore sc;
        {
            for (uint64_t i = 0; i < this->_size_tb; i++)
            {
                sc = this->score_at_idx(i * this->_size_item);
                if (sc == ExactScore::UNKNOWN)                          
                {
                    // ILLEGAL position exist in the TB _bits, check legal_pos()...
                    set_score_at_idx(i * _size_item, ExactScore::DRAW); // dtc...
                }
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::clear_marker()
    {
        if (!_is_full_type)
        {
            _vmarkers.clear();
        }

        for (uint64_t i = 0; i < this->_size_tb; i++)
        {
            set_marker_at_idx(i * _size_item, false);
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::save_tb() const
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("tablebase", name());

        // skip if exist
        {
            std::ifstream is;
            is.open(f.c_str(), std::fstream::in);
            if (is.good())
            {
                is.close();
                return true;
            }
            is.close();
        }

        std::ofstream os;
        os.open(f.c_str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
        if (os.good())
        {
            os << (size_t)_size_tb;
            if (os.bad())
            {
                //...
                os.close();
                return false;
            }

            writeBits(os);
            if (!os.bad())
            {
                os.close();
                if (save_tb_dtc())
                {
                    if (!_is_full_type)
                    {
                        if (save_tb_keys())
                        {
                            os.close();
                            return true;
                        }
                        else
                        {
                            os.close();
                            return false;
                        }
                    }
                }
                else
                {
                    os.close();
                    return false;
                }
            }
        }
        os.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::save_tb_dtc() const
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("tablebase", name() + ".dtc");

        // skip if exist
        {
            std::ifstream is;
            is.open(f.c_str(), std::fstream::in);
            if (is.good())
            {
                is.close();
                return true;
            }
            is.close();
        }

        std::ofstream os;
        os.open(f.c_str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
        if (os.good())
        {
            os << (size_t)_size_tb;
            for (uint64_t i = 0; i < _size_tb; i++)
            {
                os << (unsigned char)_vdtc[i];
            }
            if (!os.bad())
            {
                os.close();
                return true;
            }
        }
        os.close();
        return false;
    }

    // save_tb_keys
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::save_tb_keys() const
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("tablebase", name() + ".keys");

        // skip if exist
        {
            std::ifstream is;
            is.open(f.c_str(), std::fstream::in);
            if (is.good())
            {
                is.close();
                return true;
            }
            is.close();
        }

        std::ofstream os;
        os.open(f.c_str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
        if (os.good())
        {
            uint64_t ns = (uint32_t)_vkeys.size();
            os.write((char*) (&ns), sizeof(ns));

            std::vector<uint64_t> vidx;
            vidx.assign(_vkeys.size(), 0);
            uint64_t n;
            for (auto &v : _vkeys)
            {
                n = (uint64_t)v.first;
                vidx[v.second] = n;
            }
            for(auto &vi : vidx)
            {
                n = vi;
                os.write((char*)(&n), sizeof(n));
            }
            if (!os.bad())
            {
                os.close();
                return true;
            }
        }
        os.close();
        return false;
    }

    // writeBits
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::writeBits(std::ostream& os) const
    {
        for (std::size_t bit_pos = 0; bit_pos < _vbits->size(); )
        {
            unsigned char currentByte = 0;
            for (int currentBit = 0; currentBit < 8 * sizeof(unsigned char) && bit_pos < _vbits->size(); ++currentBit, ++bit_pos)
            {
                currentByte |= _vbits->at(bit_pos) ? (0x80 >> currentBit) : 0;
            }
            os << currentByte;
        }
        //0x80 >> 0 = 0b10000000
        //0x80 >> 1 = 0b01000000
        //0x80 >> 2 = 0b00100000
        //0x80 >> 3 = 0b00010000
        //0x80 >> 4 = 0b00001000
        //0x80 >> 5 = 0b00000100
        //0x80 >> 6 = 0b00000010
        //0x80 >> 7 = 0b00000001
    }

    // load_tb
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::load_tb()
    {
        if (_is_build_and_loaded)
        {
            if (is_symmetry_TB()) // futur...
                TB_Manager<PieceID, _BoardSize>::instance()->add_sym(name(), this);
            else
                TB_Manager<PieceID, _BoardSize>::instance()->add_N(name(), this);
        }
        else
        {
            if (!is_symmetry_TB())
            {
                _is_build_and_loaded = read_tb();
                if (_is_build_and_loaded)
                {
                    TB_Manager<PieceID, _BoardSize>::instance()->add_N(name(), this);
                }
            }
        }
        return _is_build_and_loaded;
    }

    // read_tb
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::read_tb()
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("tablebase", name());
        std::ifstream is;
        is.open(f.c_str(), std::ofstream::in | std::ofstream::binary);
        if (is.good())
        {
            size_t n; is >> n;
            _size_tb = n;
            uint64_t m = TB_size_item();
            delete _vbits;
            _vbits = new std::vector<bool>(_size_tb * m); // real size: ((_Count + _VBITS - 1) / _VBITS); _VBITS is 32   

            _is_full_type = (_size_tb == powN(_BoardSize*_BoardSize, NPIECE)) ? true : false;

            readBits(is);
            if (!is.bad())
            {
                is.close();
                if (read_tb_dtc())
                {
                    if (!_is_full_type)
                    {
                        if (read_tb_keys())
                        {
                            is.close();
                            return true;
                        }
                        else
                        {
                            std::stringstream ss_detail;
                            ss_detail << "Failure reading keys of TB file: " << f.c_str() << "\n";
                            std::cerr << ss_detail.str();
                        }
                    }
                }
                else
                {
                    std::stringstream ss_detail;
                    ss_detail << "Failure reading dtc of TB file: " << f.c_str() << "\n";
                    std::cerr << ss_detail.str();
                }
            }
            else
            {
                std::stringstream ss_detail;
                ss_detail << "Failure reading score of TB file: " << f.c_str() << "\n";
                std::cerr << ss_detail.str();
            }
        }
        else
        {
            std::stringstream ss_detail;
            ss_detail << "Failure opening TB score file for reading: " << f.c_str() << "\n";
            std::cerr << ss_detail.str();
        }
        is.close();
        return false;
    }

    // read_tb_dtc
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::read_tb_dtc()
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("tablebase", name() + ".dtc");
        std::ifstream is;
        is.open(f.c_str(), std::ofstream::in | std::ofstream::binary);
        if (is.good())
        {
            size_t n; is >> n;
            assert(_size_tb == n);

            delete []_vdtc;
            _vdtc = _vdtc = new uint8_t[_size_tb];
            memset(_vdtc, 0, _size_tb);

            char* buffer = new char[n];
            is.read(buffer, n);
            if (is.bad())
            {            
                std::stringstream ss_detail;
                ss_detail << "Failure reading content of TB dtc file: " << f.c_str() << "\n";
                std::cerr << ss_detail.str();
            
                delete[]buffer;
                is.close();
                return false;
            }
            for (uint64_t i = 0; i < n; i++) _vdtc[i] = buffer[i];
            delete[]buffer;

            if (!is.bad())
            {
                is.close();
                return true;
            }
        }
        else
        {
            std::stringstream ss_detail;
            ss_detail << "Failure opening TB dtc file for reading: " << f.c_str() << "\n";
            std::cerr << ss_detail.str();
        }        
        is.close();
        return false;
    }

    // read_tb_keys
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline bool Tablebase<PieceID, _BoardSize, NPIECE>::read_tb_keys()
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("tablebase", name() + ".keys");
        std::ifstream is;
        is.open(f.c_str(), std::ifstream::in | std::ifstream::binary);
        if (is.good())
        {
            uint32_t n; 
            is.read((char*) (&n), sizeof(n));
            assert(_size_tb >= n);
            _vkeys.clear();

            char* buffer = new char[n * sizeof(uint64_t)];
            is.read(buffer, n*sizeof(uint64_t));
            if (is.bad())
            {
                std::stringstream ss_detail;
                ss_detail << "Failure reading content of TB keys file: " << f.c_str() << "\n";
                std::cerr << ss_detail.str();

                delete[]buffer;
                is.close();
                return false;
            }
            for (uint32_t i = 0; i < n; i++)
            {
                _vkeys.insert(std::pair<uint64_t, uint32_t>({ (uint64_t)buffer[i * sizeof(uint64_t)], i }));
            }
            delete[]buffer;

            if (!is.bad())
            {
                is.close();
                return true;
            }
        }
        else
        {
            std::stringstream ss_detail;
            ss_detail << "Failure opening TB keys file for reading: " << f.c_str() << "\n";
            std::cerr << ss_detail.str();
        }
        is.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline uint8_t Tablebase<PieceID, _BoardSize, NPIECE>::checksum_dtc() const
    {
        uint16_t sum1 = 0;
        uint16_t sum2 = 0;
        for (uint64_t i = 0; i < _size_tb; i++)
        {
            sum1 = (sum1 + _vdtc[i]) % 255;
            sum2 = (sum2 + sum1) % 255;
        }
        return (sum2 << 8) | sum1;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::readBits(std::istream& is)
    {
        std::size_t num_bits = _vbits->size();
        //_vbits->reset();...
        std::size_t total_bytes = num_bits / (8 * sizeof(unsigned char)) + 1;
        std::size_t bit_pos = 0;
        for (std::size_t i = 0;
            is &&
            i < total_bytes &&
            bit_pos < num_bits;
            ++i)
        {
            unsigned char currentByte = 0;
            if (is >> currentByte)
            {
                for (int currentBit = 0;
                    currentBit < (8 * sizeof(unsigned char)) &&
                    bit_pos < num_bits;
                    ++currentBit, ++bit_pos)
                {
                    _vbits->at(bit_pos) = (currentByte &  (0x80 >> currentBit)) > 0;
                }
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::print_score(int n) const
    {
        ExactScore sc;
        for (uint64_t i = 0; i < this->_size_tb; i++)
        {
            if (i < n)
            {
                sc = this->score_at_idx(i * this->_size_item);
                std::cout << i << " : " << ExactScore_to_string(sc) << std::endl;
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline void Tablebase<PieceID, _BoardSize, NPIECE>::print_dtc(int n) const
    {
        std::vector<long> v_win;
        std::vector<long> v_loss;
        std::vector<long> v_draw;
        for (uint64_t i = 0; i < 256; i++) { v_win.push_back(0); v_loss.push_back(0); v_draw.push_back(0); }

        ExactScore sc;
        for (uint64_t i = 0; i < this->_size_tb; i++)
        {
            sc = this->score_at_idx(i * this->_size_item);
            if (sc == ExactScore::WIN)
            {
                int k = _vdtc[i];
                v_win[k] = v_win[k] + 1;
            }
            if (sc == ExactScore::LOSS)
            {
                int k = _vdtc[i];
                v_loss[k] = v_loss[k] + 1;
            }
            if (sc == ExactScore::DRAW)
            {
                int k = _vdtc[i];
                v_draw[k] = v_draw[k] + 1;
            }
        }
        for (uint64_t i = 0; i < n; i++)
            std::cout << i << " : " << v_win[i] << " " << v_loss[i] << " " << v_draw[i] << std::endl;
    }


    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    inline std::vector<uint64_t> Tablebase<PieceID, _BoardSize, NPIECE>::get_index_dtc(uint8_t value_dtc, ExactScore value_sc) const
    {
        std::vector<uint64_t> v;
        for (uint64_t i = 0; i < this->_size_tb; i++)
        {
            if ((this->score_at_idx(i * this->_size_item) == value_sc) && (_vdtc[i] == value_dtc))
                v.push_back(i);
        }
        return v;
    }

    // Tablebase()
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    Tablebase<PieceID, _BoardSize, NPIECE>::Tablebase(std::vector<PieceID>& v, PieceColor c)
        : TablebaseBase<PieceID, _BoardSize>(), _color(c), _NPIECE(NPIECE), _is_build_and_loaded(false), _vbits(nullptr), _vdtc(nullptr)
    {
        // May not have enough RAM
        uint64_t MAX = TB_Manager<PieceID, _BoardSize>::instance()->new_TB_setup_size(_BoardSize, NPIECE);
        try
        {
            uint64_t n = TB_size_item();
            _vbits = new std::vector<bool>(MAX * n); // real size: ((_Count + _VBITS - 1) / _VBITS); _VBITS is 32   
            assert(_vbits->size() == MAX * n);
        }
        catch (std::exception& re)
        {
            std::stringstream ss_detail;
            ss_detail << re.what() << "\n";
            std::cerr << ss_detail.str();

            _vbits = nullptr;
            throw;
        }

        try
        {
            _vdtc = new uint8_t[MAX];
        }
        catch (std::exception& re)
        {
            std::stringstream ss_detail;
            ss_detail << re.what() << "\n";
            std::cerr << ss_detail.str();

            delete _vbits; 
            _vbits = nullptr;
            _vdtc = nullptr;
            throw;
        }

        if (!_is_full_type)
        {
            try
            {
                _vkeys_dtc_count.assign(TB_Manager<PieceID, _BoardSize>::instance()->_TB_MAX_DTC,0);
            }
            catch (std::exception& re)
            {
                std::stringstream ss_detail;
                ss_detail << re.what() << "\n";
                std::cerr << ss_detail.str();

                delete _vbits;
                delete _vdtc;
                _vbits = nullptr;
                _vdtc  = nullptr;
                throw;
            }
        }

        for (auto& vv : v)
        {
            _piecesID.push_back(vv);
            _pieces.push_back(_Piece::get(vv));
        }

        memset(_vdtc, 0, MAX);
        for (uint64_t i = 0; i < this->_size_tb; i++) set_score_at_idx(i* this->_size_item, ExactScore::UNKNOWN);
        clear_marker();
    }

};
#endif
