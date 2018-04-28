#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//
#ifndef _AL_CHESS_PLAYER_PLAYERBASE_HPP
#define _AL_CHESS_PLAYER_PLAYERBASE_HPP

namespace chess
{
    // BasePlayer interface - GA capable
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class BasePlayer
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _BasePlayer = BasePlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    protected:
        std::string     _playername;
        uint32_t        _ga_instance;   // position in a GA population - 0 default
        TYPE_PARAM      _ga_fitness;    // last fitness given by GA
        size_t          _PARAM_NBIT;
        uint16_t        _elo = 0;       // todo...

    public:
        BasePlayer(const std::string& playername, uint32_t ga_instance = 0) :
            _playername(playername) , _ga_instance(ga_instance), _ga_fitness(0), _PARAM_NBIT(PARAM_NBIT)
        {
        }
        virtual ~BasePlayer() {}

        // = 0
        virtual size_t      select_move_algo(    _Board& board, std::vector<_Move>& m, size_t max_num_position_per_move, size_t max_num_position, uint16_t max_depth_per_move, uint16_t max_game_ply, size_t& num_pos_eval, char verbose, std::stringstream& verbose_stream) = 0;
        virtual TYPE_PARAM  eval_position_algo(  _Board& board, std::vector<_Move>& m, char verbose, std::stringstream& verbose_stream) = 0;
        virtual bool save() const = 0;
        virtual bool load() = 0;

        const std::string   playername()    const   { return _playername; }
        uint32_t            ga_instance()   const   { return _ga_instance; }
        TYPE_PARAM          ga_fitness()    const   { return _ga_fitness; }
        const size_t        param_NBIT()    const   { return _PARAM_NBIT; }
        const size_t        elo()           const   { return _elo; }

        void set_ga_instance(uint32_t v)    { _ga_instance = v; }
        void set_ga_fitness(TYPE_PARAM v)   { _ga_fitness = v; }
    };
};

#endif
