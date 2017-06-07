#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Partiton<...>    : A partition of the chess space into hiearchical domains
//
//
#ifndef _AL_CHESS_PARTITION_H
#define _AL_CHESS_PARTITION_H

#include <map>
#include <string>
#include <memory>

namespace chess
{
    // forward
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class Domain;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class Partition;

    // Partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Partition
    {
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        Partition() = default;
        ~Partition() = default;

        Partition(std::string aname) : _name(aname) { }
        const std::string name()   const { return _name; }

        // Persistence
        bool save() const;
        bool load();

        bool add_domain(_Domain* p)
        {
            auto iter = _domains.find(p->domain_key()); 
            if (iter == _domains.end())
            {
                _domains.insert({ p->domain_key(), p });
                return true;
            }
            return false;
        }

        _Domain* find_domain(const std::string& domain_key) const
        {
            auto iter = _domains.find(domain_key);
            if (iter != _domains.end())
            {
                _Domain* p = iter->second;
                return p;
            }
            return nullptr;
        }
        _Domain* find_domain(const std::string& classname_key, const std::string& instance_key) const
        {
            return find_domain(_Domain::domain_key(classname_key, instance_key));
        }

    private:
        std::string                      _name;
        std::map<std::string, _Domain*>  _domains;  // on heap - owner (TODO std::unique_ptr...)
    };

    // save()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save() const
    {
        std::string f = PersistManager::instance()->get_stream_name("partition", _name);
        std::ofstream   filestream;
        filestream.open(f.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (filestream.good())
        {
            filestream << _name; filestream << std::endl;
            filestream << _domains.size(); filestream << std::endl;
            for (auto& v : _domains)
            {
                filestream << v.second->classname_key(); filestream << std::endl;
                filestream << v.second->instance_key(); filestream << std::endl;
            }
        }
        else
        {
            filestream.close();
            return false;
        }
        filestream.close();
        return true;
    }

    // load()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load()
    {
        std::string f = PersistManager::instance()->get_stream_name("partition", _name);
        std::ifstream   filestream;
        filestream.open(f.c_str(), std::ofstream::in);
        if (filestream.good())
        {
            size_t n_child;
            std::string partition_key;
            std::string classname_key;
            std::string instance_key;

            filestream >> partition_key;
            assert(partition_key == _name); // check
            filestream >> n_child;

            // reloading or creating
            _domains.clear();

            bool ok = true;
            for (size_t i = 0; i < n_child; i++)
            {
                ok = false;
                filestream >> classname_key;
                filestream >> instance_key;
                {
                    _Domain* ptr_dom = _Domain::make(partition_key, classname_key, instance_key);
                    if (ptr_dom != nullptr)
                    {
                        if (add_domain(ptr_dom))
                        {                          
                            ok = true;
                        }
                    }
                }
                if (!ok)
                {
                    filestream.close();
                    return false;
                }
            }

            // children/data of domains
            _Domain* p;
            for (auto& v : _domains)
            {
                p = v.second;
                if (p != nullptr)
                {
                    if (p->load() == false)
                    {
                        filestream.close();
                        return false;
                    }
                }
            }

            filestream.close();
            return true;
        }
        else
        {
            filestream.close();
            return false;
        }
    }
};

#endif