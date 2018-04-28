#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//  ChessGeneticAlgorithm<T, PARAM_NBIT>
//
//
#ifndef _AL_CHESS_CHESSGA_CHESSGENALGO_HPP
#define _AL_CHESS_CHESSGA_CHESSGENALGO_HPP

namespace chess
{
    namespace ga
    {
        using namespace galgo;

        template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT, int WEIGHT_BOUND>
        class ChessGeneticAlgorithm : public GeneticAlgorithm<TYPE_PARAM, PARAM_NBIT>
        {
            using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
            using _BaseGame = BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
            using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
            friend class ChessCoEvolveGA<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT, WEIGHT_BOUND>;

        public:
            ChessGeneticAlgorithm(  bool evolve_white, bool is_single_pop, 
                                    _DomainPlayer* player, _DomainPlayer* player_opposite, BaseGame_Config cfg,
                                    int popsize, int nbgen, int _tournament_n_player, int _tournament_n_game, char verbose = 0);

            void run(bool reentry) override;

            ~ChessGeneticAlgorithm()
            {
                delete _game;
            }

        protected:
            void setup_params();

            void set_player_term_nodes(std::vector<_ConditionValuationNode*>& nodes, std::vector<TYPE_PARAM>& param) const
            {
                size_t next = 0;
                size_t i;
                std::vector<TYPE_PARAM> w;
                for (auto& v : nodes)
                {
                    //......................
                    //w = v->get_weights();
                    i = 0;
                    for (auto& vw : w)
                    {
                        vw = param[next++];
                        w[i] = vw;
                        i++;
                    }
                    //.................
                    //v->set_weights(w);
                }
            }

        public:
            // tournament (as the fitness function)
            std::vector<TYPE_PARAM> tournament(bool is_at_creation, const std::vector<TYPE_PARAM>& param) const override
            {
                std::vector<TYPE_PARAM> param_candidate = param;
                std::vector<TYPE_PARAM> param_opponent;
                std::vector<TYPE_PARAM> v;
                TYPE_PARAM total_fit = 0;  
                TYPE_PARAM score_fit = 0;
                ExactScore sc;

                set_player_term_nodes(_player_terminal_nodes, param_candidate);

                for (size_t i = 0; i < _tournament_n_player; i++)
                {
                    for (size_t j = 0; j < _tournament_n_game; j++)
                    {
                       if (_is_single_pop)
                       {
                           size_t rnd_opponent = rand() % popsize;
                           const std::shared_ptr<Chromosome<TYPE_PARAM, PARAM_NBIT>>& curpop_player = pop.get_cur(rnd_opponent);
                           std::vector<TYPE_PARAM> param_opponent = curpop_player->decode_param();
                           set_player_term_nodes(_player_opposite_terminal_nodes, param_opponent);
                       }

                       if (_evolve_white)   _game->set_board(_player->domain()->get_random_position(true));
                       else                 _game->set_board(_player_opposite->domain()->get_random_position(true));

                       sc = _game->play(false);
                       if (_evolve_white)
                       {
                           if (sc == chess::ExactScore::WIN)        score_fit = 1.0;
                           else if (sc == chess::ExactScore::LOSS)  score_fit = 0.0;
                           else if (sc == chess::ExactScore::DRAW)  score_fit = 0.5;
                           else score_fit = 0.0;
                       }
                       else
                       {
                           if (sc == chess::ExactScore::WIN)        score_fit = 0.0;
                           else if (sc == chess::ExactScore::LOSS)  score_fit = 1.0;
                           else if (sc == chess::ExactScore::DRAW)  score_fit = 0.5;
                           else score_fit = 0.0;
                       }
                       total_fit += score_fit;
                    }
                }
                v.push_back( total_fit / ((TYPE_PARAM)(_tournament_n_player * _tournament_n_game)) );
                return v;
            }

        private:
            bool _evolve_white;
            bool _is_single_pop;
            _DomainPlayer*                          _player;
            _DomainPlayer*                          _player_opposite;
            mutable std::vector<_ConditionValuationNode*>   _player_terminal_nodes;
            mutable std::vector<_ConditionValuationNode*>   _player_opposite_terminal_nodes;
            _BaseGame*                              _game;
            BaseGame_Config                         _cfg;
            int                                     _tournament_n_player;
            int                                     _tournament_n_game;
            char _verbose;
        };


        // ChessGeneticAlgorithm()
        template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT, int WEIGHT_BOUND>
        ChessGeneticAlgorithm<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT, WEIGHT_BOUND>::ChessGeneticAlgorithm(
                bool evolve_white, bool is_single_pop, _DomainPlayer* player, _DomainPlayer* player_opposite, BaseGame_Config cfg,
                int popsize, int nbgen, int tournament_n_player, int tournament_n_game, char verbose)
            : GeneticAlgorithm<TYPE_PARAM, PARAM_NBIT>(popsize, nbgen)
        {
            _evolve_white   = evolve_white;
            _is_single_pop  = is_single_pop;
            _cfg            = cfg;

            if (_evolve_white)
            {
                _player = player;
                _player_opposite = player_opposite;
            }
            else
            {
                _player = player_opposite;
                _player_opposite = player;
            }

            _player->get_root()->get_term_nodes(_player_terminal_nodes);
            _player_opposite->get_root()->get_term_nodes(_player_opposite_terminal_nodes);

            if (_evolve_white)  _game = new BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(*_player, *_player_opposite);
            else                _game = new BaseGame<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(*_player_opposite, *_player);

            _game->set_constraints(_cfg);
            _tournament_n_player    = tournament_n_player;
            _tournament_n_game      = tournament_n_game;
            _verbose                = verbose;

            setup_params();
        }

        // setup_params
        template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT, int WEIGHT_BOUND>
        void ChessGeneticAlgorithm<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT, WEIGHT_BOUND>::setup_params()
        {
            param.clear();
            idx.clear();
            nogen = 0;

            std::vector<TYPE_PARAM> _lowerBound;
            std::vector<TYPE_PARAM> _upperBound;
            std::vector<TYPE_PARAM> _initialSet;

            std::vector<TYPE_PARAM> w;
            for (int i = 0; i < _player_terminal_nodes.size(); i++)
            {
                //...
                //w = _player_terminal_nodes[i]->get_weights();
                for (int j = 0; j < w.size(); j++)
                {
                    _lowerBound.push_back(-WEIGHT_BOUND);
                    _upperBound.push_back(+WEIGHT_BOUND);
                    _initialSet.push_back(w[j]);
                }
            }

            for (int i = 0; i<_lowerBound.size(); i++)
            {
                std::vector<TYPE_PARAM> w;
                w.push_back(_lowerBound[i]); w.push_back(_upperBound[i]); w.push_back(_initialSet[i]);
                Parameter<double, PARAM_NBIT> p(w);
                param.emplace_back(new decltype(p)(p));

                if (i == 0) idx.push_back(0);
                else idx.push_back(idx[i - 1] + PARAM_NBIT);
            }
            lowerBound = _lowerBound;
            upperBound = _upperBound;
            initialSet = _initialSet;

            // this->Objective = ...
            this->nbbit = (int)_lowerBound.size()*PARAM_NBIT;
            this->nbparam = (int)_lowerBound.size();
        }

        // run()
        template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT, int WEIGHT_BOUND>
        void ChessGeneticAlgorithm<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT, WEIGHT_BOUND>::run(bool reentry)
        {
            _player->attachToDomains();
            _player_opposite->attachToDomains();

            this->check();
            if (!reentry)
            {
                pop = Population<TYPE_PARAM, PARAM_NBIT>(*this);
                pop.creation(false, true);                          // setup all chromo then evaluate() them
            }
            else
            {
                pop(0)->evaluate();
            }
            TYPE_PARAM bestResult = pop(0)->getTotal();

            for (nogen = 1; nogen <= nbgen; ++nogen)
            {
                pop.evolution();                                    // evaluate() called in recombination, completion newpop[i]->evaluate()
                bestResult = pop(0)->getTotal();
            }

            const std::shared_ptr<Chromosome<TYPE_PARAM, PARAM_NBIT>>& best_player = pop.get_cur(0);
            std::vector<TYPE_PARAM> param_best = best_player->decode_param();
            set_player_term_nodes(_player_terminal_nodes, param_best);

            _player->set_ga_instance(0);
            _player->set_ga_fitness(bestResult);

            _player->detachFromDomains();
            _player_opposite->detachFromDomains();

            _player->save();
            _player_opposite->save();
        }

    };
};
#endif
