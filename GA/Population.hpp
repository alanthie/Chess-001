#pragma once
//=================================================================================================
//          Modified work is Copyright (C) 2017 Alain Lanthier - All Rights Reserved  
//                      Original source is GALGO 2.0 (see github)         
//=================================================================================================
//
// class Population<T, PARAM_NBIT>
//
//
#ifndef _AL_CHESS_GA_POPULATION_HPP
#define _AL_CHESS_GA_POPULATION_HPP

namespace galgo 
{
    template <typename T, int PARAM_NBIT>
    class Population
    {
       static_assert(std::is_same<float,T>::value || std::is_same<double,T>::value, "variable type can only be float or double, please amend.");

    public: 
       Population() {}
       Population(const GeneticAlgorithm<T, PARAM_NBIT>& ga);

       void creation(bool init_all_first = false, bool eval_on_creation = true);   // create a population of chromosomes
       void evolution();                            // evolve population, get next generation

       // access element in current population at position pos
       const CHR<T, PARAM_NBIT>& operator()(int pos) const;
       // access element in mating population at position pos
       const CHR<T, PARAM_NBIT>& operator[](int pos) const;  
       // return iterator to current population beginning
       typename std::vector<CHR<T, PARAM_NBIT>>::iterator begin();
       // return const iterator to current population beginning
       typename std::vector<CHR<T, PARAM_NBIT>>::const_iterator cbegin() const;
       // return iterator to current population ending  
       typename std::vector<CHR<T, PARAM_NBIT>>::iterator end();  
       // return const iterator to current population ending  
       typename std::vector<CHR<T, PARAM_NBIT>>::const_iterator cend() const;  
       // select element at position pos in current population and copy it into mating population
       void select(int pos);
       // set all fitness to positive values 
       void adjustFitness();
       // compute fitness sum of current population
       T getSumFitness() const;
       // get worst objective function total result from current population
       T getWorstTotal() const;
       // return population size
       int popsize() const;
       // return mating population size
       int matsize() const;
       // return tournament size
       int tntsize() const;
       // return numero of generation
       int nogen() const;
       // return number of generations
       int nbgen() const;
       // return selection pressure
       T SP() const; 

       const CHR<T, PARAM_NBIT>& get_cur(size_t idx) const { return curpop[idx]; }

    private:
       std::vector<CHR<T, PARAM_NBIT>> curpop;              // current population
       std::vector<CHR<T, PARAM_NBIT>> matpop;              // mating population
       std::vector<CHR<T, PARAM_NBIT>> newpop;              // new population

       const GeneticAlgorithm<T, PARAM_NBIT>* ptr = nullptr; // pointer to genetic algorithm              
       int nbrcrov;                              // number of cross-over
       int matidx;                               // mating population index

       void elitism();                          // elitism => saving best chromosomes in new population
       void recombination();                    // create new population from recombination of the old one
       void completion();                       // complete new population randomly
       void updating();                         // update population (adapting, sorting)
    };

    // constructor
    template <typename T, int PARAM_NBIT>
    Population<T, PARAM_NBIT>::Population(const GeneticAlgorithm<T, PARAM_NBIT>& ga)
    {
       ptr = &ga;   
       nbrcrov = (int)floor(ga.covrate * (ga.popsize - ga.elitpop));
       // adjusting nbrcrov (must be an even number)
       if (nbrcrov % 2 != 0) nbrcrov -= 1;
       // for convenience, we add elitpop to nbrcrov
       nbrcrov += ga.elitpop;
       // allocating memory
       curpop.resize(ga.popsize);
       matpop.resize(ga.matsize);
    }

    // create a population of chromosomes
    template <typename T, int PARAM_NBIT>
    void Population<T, PARAM_NBIT>::creation(bool init_all_first, bool eval_on_creation)
    {
       int start = 0;
       if (init_all_first == false)
       {
           // initializing first chromosome
           if (!ptr->initialSet.empty()) 
           {
               curpop[0] = std::make_shared<Chromosome<T, PARAM_NBIT>>(*ptr);
               curpop[0]->initialize();
               if (eval_on_creation) curpop[0]->evaluate(true);
               start++;
           }
           for (int i = start; i < ptr->popsize; ++i)
           {
               curpop[i] = std::make_shared<Chromosome<T, PARAM_NBIT>>(*ptr);
               curpop[i]->create();
               if (eval_on_creation) curpop[i]->evaluate(true);
           }
       }
       else
       {
           if (!ptr->initialSet.empty()) 
           {
               for (int i = 0; i < ptr->popsize; ++i)
               {
                   curpop[i] = std::make_shared<Chromosome<T, PARAM_NBIT>>(*ptr);
                   curpop[i]->initialize();
                   if (eval_on_creation) curpop[i]->evaluate(true);
               }
           }
           else
           {
               for (int i = 0; i < ptr->popsize; ++i)
               {
                   curpop[i] = std::make_shared<Chromosome<T, PARAM_NBIT>>(*ptr);
                   curpop[i]->create();
                   if (eval_on_creation) curpop[i]->evaluate(true);
               }
           }
       }

       if (eval_on_creation == false)
       {
           std::cout << "INIT evaluate() start " << std::endl;
           for (int i = 0; i < ptr->popsize; ++i)
           {
               curpop[i]->evaluate(true);
           }
           std::cout << "INIT evaluate() done " << std::endl;
       }

       // updating population
       this->updating();
    }

    // population evolution (selection, recombination, completion, mutation), get next generation
    template <typename T, int PARAM_NBIT>
    void Population<T, PARAM_NBIT>::evolution()
    {   
       // initializing mating population index
       matidx = 0;
       // selecting mating population
       ptr->Selection(*this);
       // applying elitism if required
       this->elitism(); 
       // crossing-over mating population
       this->recombination();
       // completing new population
       this->completion();
       // moving new population into current population for next generation
       curpop = std::move(newpop);
       // updating population
       this->updating(); 
    }

    // elitism => saving best chromosomes in new population, making a copy of each elit chromosome
    template <typename T, int PARAM_NBIT>
    void Population<T, PARAM_NBIT>::elitism()
    {
       // (re)allocating new population
       newpop.resize(ptr->popsize);

       if (ptr->elitpop > 0) {
          // copying elit chromosomes into new population
          std::transform(curpop.cbegin(), curpop.cend(), newpop.begin(), [](const CHR<T, PARAM_NBIT>& chr)->CHR<T, PARAM_NBIT>{return std::make_shared<Chromosome<T, PARAM_NBIT>>(*chr);});
       }
    }

    // create new population from recombination of the old one
    template <typename T, int PARAM_NBIT>
    void Population<T, PARAM_NBIT>::recombination()
    {
       // creating a new population by cross-over
       #ifdef _OPENMP 
       #pragma omp parallel for num_threads(MAX_THREADS)
       #endif
       for (int i = ptr->elitpop; i < nbrcrov; i = i + 2) {      
          // initializing 2 new chromosome
          newpop[i] = std::make_shared<Chromosome<T, PARAM_NBIT>>(*ptr);
          newpop[i+1] = std::make_shared<Chromosome<T, PARAM_NBIT>>(*ptr);
          // crossing-over mating population to create 2 new chromosomes
          ptr->CrossOver(*this, newpop[i], newpop[i+1]);
          // mutating new chromosomes
          ptr->Mutation(newpop[i]);   
          ptr->Mutation(newpop[i+1]);   
          // evaluating new chromosomes
          newpop[i]->evaluate(false);
          newpop[i + 1]->evaluate(false);
       } 
    }

    // complete new population
    template <typename T, int PARAM_NBIT>
    void Population<T, PARAM_NBIT>::completion()
    {
       #ifdef _OPENMP 
       #pragma omp parallel for num_threads(MAX_THREADS)
       #endif
       for (int i = nbrcrov; i < ptr->popsize; ++i) {
          // selecting chromosome randomly from mating population
          newpop[i] = std::make_shared<Chromosome<T, PARAM_NBIT>>(*matpop[uniform<int>(0, ptr->matsize)]);
          // mutating chromosome
          ptr->Mutation(newpop[i]);
          // evaluating chromosome
          newpop[i]->evaluate(false);
       }
    }

    // update population (adapting, sorting)
    template <typename T, int PARAM_NBIT>
    void Population<T, PARAM_NBIT>::updating()
    {
       // adapting population to constraints
       if (ptr->Constraint != nullptr) {
          ptr->Adaptation(*this); 
       }
       // sorting chromosomes from best to worst fitness
       std::sort(curpop.begin(),curpop.end(),[](const CHR<T, PARAM_NBIT>& chr1,const CHR<T, PARAM_NBIT>& chr2)->bool{return chr1->fitness > chr2->fitness;});
    }

    // access element in current population at position pos
    template <typename T, int PARAM_NBIT>
    const CHR<T, PARAM_NBIT>& Population<T, PARAM_NBIT>::operator()(int pos) const
    {
       #ifndef NDEBUG
       if (pos > ptr->popsize - 1) {
          throw std::invalid_argument("Error: in galgo::Population<T, PARAM_NBIT>::operator()(int), exceeding current population memory.");
       }
       #endif

       return curpop[pos];
    }

    // access element in mating population at position pos
    template <typename T, int PARAM_NBIT>
    const CHR<T, PARAM_NBIT>& Population<T, PARAM_NBIT>::operator[](int pos) const
    {
       #ifndef NDEBUG
       if (pos > ptr->matsize - 1) {
          throw std::invalid_argument("Error: in galgo::Population<T, PARAM_NBIT>::operator[](int), exceeding mating population memory.");
       }
       #endif

       return matpop[pos];
    }

    // return iterator to current population beginning 
    template <typename T, int PARAM_NBIT>
    inline typename std::vector<CHR<T, PARAM_NBIT>>::iterator Population<T, PARAM_NBIT>::begin()
    {
       return curpop.begin(); 
    }

    // return const iterator to current population beginning 
    template <typename T, int PARAM_NBIT>
    inline typename std::vector<CHR<T, PARAM_NBIT>>::const_iterator Population<T, PARAM_NBIT>::cbegin() const
    {
       return curpop.cbegin(); 
    }

    // return iterator to current population ending
    template <typename T, int PARAM_NBIT>
    inline typename std::vector<CHR<T, PARAM_NBIT>>::iterator Population<T, PARAM_NBIT>::end()
    { 
       return curpop.end();
    }

    // return const iterator to current population ending
    template <typename T, int PARAM_NBIT>
    inline typename std::vector<CHR<T, PARAM_NBIT>>::const_iterator Population<T, PARAM_NBIT>::cend() const
    { 
       return curpop.cend();
    }

    // select element at position pos in current population and copy it into mating population
    template <typename T, int PARAM_NBIT>
    inline void Population<T, PARAM_NBIT>::select(int pos)
    {
       #ifndef NDEBUG
       if (pos > ptr->popsize - 1) {
          throw std::invalid_argument("Error: in galgo::Population<T, PARAM_NBIT>::select(int), exceeding current population memory.");
       }
       if (matidx == ptr->matsize) {
          throw std::invalid_argument("Error: in galgo::Population<T, PARAM_NBIT>::select(int), exceeding mating population memory.");
       }
       #endif

       matpop[matidx] = curpop[pos];
       matidx++;
    }

    // set all fitness to positive values (used in RWS and SUS selection methods)
    template <typename T, int PARAM_NBIT>
    void Population<T, PARAM_NBIT>::adjustFitness()
    {
       // getting worst population fitness
       T worstFitness = curpop.back()->fitness;

       if (worstFitness < 0) {
          // getting best fitness
          T bestFitness = curpop.front()->fitness;
          // case where all fitness are equal and negative
          if (worstFitness == bestFitness) {
             std::for_each(curpop.begin(), curpop.end(), [](CHR<T, PARAM_NBIT>& chr)->void{chr->fitness *= -1;});
           } else {
             std::for_each(curpop.begin(), curpop.end(), [worstFitness](CHR<T, PARAM_NBIT>& chr)->void{chr->fitness -= worstFitness;});
          }
       }
    }

    // compute population fitness sum (used in TRS, RWS and SUS selection methods)
    template <typename T, int PARAM_NBIT>
    inline T Population<T, PARAM_NBIT>::getSumFitness() const
    {
       return std::accumulate(curpop.cbegin(), curpop.cend(), 0.0, [](T sum, const CHR<T, PARAM_NBIT>& chr)->T{return sum + T(chr->fitness);});
    }

    // get worst objective function total result from current population (used in constraint(s) adaptation)
    template <typename T, int PARAM_NBIT>
    inline T Population<T, PARAM_NBIT>::getWorstTotal() const
    {
       auto it = std::min_element(curpop.begin(), curpop.end(), [](const CHR<T, PARAM_NBIT>& chr1, const CHR<T, PARAM_NBIT>& chr2)->bool{return chr1->getTotal() < chr2->getTotal();});

       return (*it)->getTotal();
    }

    // return population size
    template <typename T, int PARAM_NBIT>
    inline int Population<T, PARAM_NBIT>::popsize() const
    {
       return ptr->popsize;
    }

    // return mating population size
    template <typename T, int PARAM_NBIT>
    inline int Population<T, PARAM_NBIT>::matsize() const
    {
       return ptr->matsize;
    }

    // return tournament size
    template <typename T, int PARAM_NBIT>
    inline int Population<T, PARAM_NBIT>::tntsize() const
    {
       return ptr->tntsize;
    }

    // return numero of generation
    template <typename T, int PARAM_NBIT>
    inline int Population<T, PARAM_NBIT>::nogen() const
    {
       return ptr->nogen;
    }

    // return number of generations
    template <typename T, int PARAM_NBIT>
    inline int Population<T, PARAM_NBIT>::nbgen() const
    {
       return ptr->nbgen;
    }

    // return selection pressure
    template <typename T, int PARAM_NBIT>
    inline T Population<T, PARAM_NBIT>::SP() const
    {
       return ptr->SP;
    }
}

#endif


