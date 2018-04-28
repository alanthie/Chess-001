#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// FeatureValuAlgo_rvm_trainer<...>     :
//
//
#ifndef _AL_CHESS_FEATURE_FeatureValuAlgo_rvm_trainer_HPP
#define _AL_CHESS_FEATURE_FeatureValuAlgo_rvm_trainer_HPP

#include <ExternLib/dlib/svm.h>

namespace chess
{
    // FeatureValuAlgo_rvm_trainer
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class FeatureValuAlgo_rvm_trainer : public FeatureValuAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board    = Board<PieceID, _BoardSize>;
        using _Move     = Move<PieceID>;
        using _FeatureManager   = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgo          = FeatureAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureValuAlgo      = FeatureValuAlgo<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureAlgoConfig    = FeatureAlgoConfig<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer         = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
    
    private:
        typedef dlib::matrix<double, 0, 1>                          sample_type;
        typedef dlib::radial_basis_kernel<sample_type>              kernel_type;
        typedef dlib::linear_kernel<sample_type>                    linear_kernel_type;
        typedef dlib::decision_function<kernel_type>                dec_funct_type;
        typedef dlib::normalized_function<dec_funct_type>           funct_type; 
        typedef dlib::probabilistic_decision_function<kernel_type>  probabilistic_funct_type;
        typedef dlib::normalized_function<probabilistic_funct_type> pfunct_type;
        struct STRUCT_DATASET
        {
            std::vector<sample_type>    _samples;
            std::vector<double>	        _labels;
            void clear()
            {
                _samples.clear();
                _labels.clear();
            }
        };

        ScoreBinaryClassifier                   _score_binary_class = ScoreBinaryClassifier::WIN_OTHER; //...
        std::vector<_ValuationFeature*>         _valuations;
        funct_type                              _learned_function;
        pfunct_type                             _learned_pfunct;  
        dlib::rvm_trainer<kernel_type>          _trainer;
        dlib::vector_normalizer<sample_type>    _normalizer_training;
        dlib::vector_normalizer<sample_type>    _normalizer_testing;

        // work area
        STRUCT_DATASET                          _training_dataset;
        STRUCT_DATASET                          _testing_dataset;

    public:
        FeatureValuAlgo_rvm_trainer(const _FeatureAlgoConfig& cfg) : _FeatureValuAlgo(cfg)
        {
        }
        ~FeatureValuAlgo_rvm_trainer() {}

        TYPE_PARAM get_valuations_value(const _Board& position, const std::vector<_Move>& m, char verbose, std::stringstream& verbose_stream) const override
        { 
            sample_type samp;
            samp.set_size(_valuations.size());
            for (size_t i = 0; i < _valuations.size(); i++)
            {
                samp(i) = _valuations[i]->compute(position, m);
            }
            //return (TYPE_PARAM)learned_function(samp);  // sigmoid(c);
            return (TYPE_PARAM)_learned_pfunct(samp);
        }

        bool load() override
        {
            std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("ConditionValuationNode", cfg()._persist_key);
            std::ifstream is;
            is.open(f.c_str(), std::fstream::in);
            if (!load_cfg(is))
            {
                is.close();
                return true;
            }
            if (!load_detail(is))
            {
                dlib::deserialize(f) >> _learned_pfunct;
                is.close();
                return true;
            }
            is.close();
            return false;
        }

        bool load_cfg(std::ifstream& is) override
        {
            if (is.good())
            {
                cfg().load_detail(is);
                return true;
            }
            return false;
        }

        bool load_detail(std::ifstream& is) override
        {
            for (size_t i = 0; i < _valuations.size(); i++) _valuations[i] = nullptr; // not owner
            _valuations.clear(); 
            size_t n;
            is >> n;
            for (size_t i = 0; i < n; i++)
            {
                _ValuationFeature* feature = read_valu(is);
                if (feature == nullptr) return false;
                _valuations.push_back(feature);
            }
            return true;
        }

        bool save() const override
        {
            if (cfg()._persist_key.size() == 0)
                cfg()._persist_key = PersistManager<PieceID, _BoardSize>::instance()->create_persist_key();
            std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("ConditionValuationNode", cfg()._persist_key);
            std::ofstream os;
            os.open(f.c_str(), std::fstream::out | std::fstream::trunc);
            if (save_detail(os))
            {
                dlib::serialize(f) << _learned_pfunct;

                os.close();
                return true;
            }
            os.close();
            return false;
        }

        bool save_detail(std::ofstream& os) const override
        {
            if (os.good())
            {  
                cfg().save_detail(os);
                os << _valuations.size();   os << " ";
                for (auto& v : _valuations) v->save(os);
                return true;
            }
            return false;
        }

        // prepare
        bool prepare(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override
        {
            _valuations.clear();
            _training_dataset.clear();  // futur re-entry....
            _testing_dataset.clear();

            _ValuationFeature* f;
            for (size_t i = 0; i < cfg()._valu_primitive_features.size(); i++)
            {
                f = _FeatureManager::instance()->get_valu_feature_by_fullname(cfg()._valu_primitive_features[i]);
                if (f != nullptr)
                {
                    _valuations.push_back(f);
                }
                else
                {
                    // ...
                }
            }

            std::vector<_ConditionValuationNode*> v_nodes = node->get_path(); // in reverse order 
            _Board* b = player.domain()->first_position( player.color_player() );
            if (b == nullptr)
            {
                //...
                return false;
            }

            bool pos_in_domain_and_node;
            ExactScore sc;
            std::vector<_Move> m;
            size_t ret_mv_idx;

            while ((_training_dataset._samples.size() < max_size_dataset) && (_testing_dataset._samples.size() < max_size_dataset))
            {
                pos_in_domain_and_node = true;
                if (player.domain()->isInDomain(*b))
                {
                    // does position match node path Fcond[] implicit sub_domain
                    bool cond = true;
                    m = b->generate_moves();
                    for (size_t i = v_nodes.size() - 1; i >= 0; i--)
                    {
                        if (i != v_nodes.size() - 1)
                        {
                            cond = v_nodes[i]->get_condition_value(*b, m);
                            if ((cond && v_nodes[i]->is_positive_node()) || (!cond && !v_nodes[i]->is_positive_node()))
                            {
                                // ok
                            }
                            else
                            {
                                pos_in_domain_and_node = false;
                                break;
                            }
                        }                       
                    }
                }
                else
                {
                    pos_in_domain_and_node = false;
                }

                if (pos_in_domain_and_node)
                {
                    sample_type samp;
                    samp.set_size(_valuations.size());
                    for (size_t i = 0; i < _valuations.size(); i++)
                    {
                        samp(i) = _valuations[i]->compute(*b, m);
                    }
                    sc = player.domain()->get_known_score_move(*b, m, ret_mv_idx); // TB read

                    if (sc != ExactScore::UNKNOWN)
                    {
                        // WIN_OTHER is keeping all data, WIN_LOSS is skipping DRAW score, ...
                        if (_score_binary_class == ScoreBinaryClassifier::WIN_OTHER)
                        {
                            if (_training_dataset._samples.size() <= _testing_dataset._samples.size())
                            {
                                _training_dataset._samples.push_back(samp);
                                {
                                    if (sc == ExactScore::WIN) _training_dataset._labels.push_back(+1.0);
                                    else _training_dataset._labels.push_back(-1.0);
                                }
                            }
                            else
                            {
                                _testing_dataset._samples.push_back(samp);
                                {
                                    if (sc == ExactScore::WIN) _testing_dataset._labels.push_back(+1.0);
                                    else _testing_dataset._labels.push_back(-1.0);
                                }
                            }
                        }
                        else
                        {
                            //...
                        }
                    }
                }
                b = player.domain()->next_position();
                if (b == nullptr)
                    break;
            }

            // normalize data sets
            _normalizer_training.train(_training_dataset._samples);
            for (unsigned long i = 0; i < _training_dataset._samples.size(); ++i)
                _training_dataset._samples[i] = _normalizer_training(_training_dataset._samples[i]);

            _normalizer_testing.train(_testing_dataset._samples);
            for (unsigned long i = 0; i < _testing_dataset._samples.size(); ++i)
                _testing_dataset._samples[i] = _normalizer_testing(_testing_dataset._samples[i]);

            //...
            return true;
        }

        bool train(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset, char verbose) override
        {
            randomize_samples(_training_dataset._samples, _training_dataset._labels);
            _trainer.set_epsilon(0.001);
            _trainer.set_max_iterations(2000);

            if (verbose) std::cout << "doing cross validation" << std::endl;
            for (double gamma = 0.000001; gamma <= 1; gamma *= 5)
            {
                _trainer.set_kernel(kernel_type(gamma));
                if (verbose) if (verbose) std::cout << "gamma: " << gamma;
                // Print out the cross validation accuracy for 3-fold cross validation using the current gamma.  
                // cross_validate_trainer() returns a row vector.  The first element of the vector is the fraction
                // of +1 training examples correctly classified and the second number is the fraction of -1 training 
                // examples correctly classified.
                if (verbose) std::cout << "     cross validation accuracy: " << cross_validate_trainer(_trainer, _training_dataset._samples, _training_dataset._labels, 3);
                //matrix<double> result = cross_validate_trainer(trainer, samples, labels, 3);
            }
            _trainer.set_kernel(kernel_type(0.08)); // lookup best gamma...

            _learned_function.normalizer    = _normalizer_training;
            _learned_function.function      = _trainer.train(_training_dataset._samples, _training_dataset._labels);

            _learned_pfunct.normalizer      = _normalizer_training;
            _learned_pfunct.function        = dlib::train_probabilistic_decision_function(_trainer, _training_dataset._samples, _training_dataset._labels, 3);
            return true;
        }

        bool test(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override
        {
            double result_prob;
            double score_class; // -1, 0, +1
            double delta_err;
            double sum_err_training = 0;
            double sum_err_testing = 0;

            for (unsigned long i = 0; i < _training_dataset._samples.size(); ++i)
            {
                result_prob = _learned_pfunct(_training_dataset._samples[i]); // 0-1 probability
                if (result_prob <= _FeatureManager::instance()->LOSS_THRESHOLD_IN_PROB_01()) score_class = -1.0;
                else if (result_prob >= _FeatureManager::instance()->WIN_THRESHOLD_IN_PROB_01()) score_class = +1.0;
                else score_class = 0.0;
                delta_err = std::pow(std::abs(_training_dataset._labels[i] - score_class), 2);
                sum_err_training += delta_err;
            }
            sum_err_training = sum_err_training / (1 + _training_dataset._samples.size());  // 1+n (in case n==0)
            sum_err_training = std::pow(sum_err_training, 0.5); // root-mean-square error (RMSE)

            for (unsigned long i = 0; i < _testing_dataset._samples.size(); ++i)
            {
                result_prob = _learned_pfunct(_testing_dataset._samples[i]); // 0-1
                if (result_prob <= _FeatureManager::instance()->LOSS_THRESHOLD_IN_PROB_01()) score_class = -1.0;
                else if (result_prob >= _FeatureManager::instance()->WIN_THRESHOLD_IN_PROB_01()) score_class = +1.0;
                else score_class = 0.0;
                delta_err = std::pow(std::abs(_testing_dataset._labels[i] - score_class), 2);
                sum_err_testing += delta_err;
            }
            sum_err_testing = sum_err_testing / (1 + _testing_dataset._samples.size());
            sum_err_testing = std::pow(sum_err_testing, 0.5);

            this->cfg()._train_dataset_average_error = sum_err_training;
            this->cfg()._test_dataset_average_error  = sum_err_testing;

            // already linked by set_test_valu_algo(_FeatureValuAlgo* algo)
            // node->test_valu_algo()->cfg()._test_dataset_average_error = sum_err_testing;

            return true;
        }

        bool compare(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override
        {
            if (node->test_valu_algo()->cfg()._test_dataset_average_error <= node->current_valu_algo()->cfg()._test_dataset_average_error)
                return true; // can change algo on node (since better performance)
            return false;
        }

        void cleanup(_DomainPlayer& player, _ConditionValuationNode* node, size_t max_size_dataset) override
        {
            //...
            _training_dataset.clear();
            _testing_dataset.clear();
        }

    };

};

#endif
