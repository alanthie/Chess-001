#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Partiton<...>    : A partition of the chess space into hiearchical domains
//
//
#ifndef _AL_CHESS_DOMAIN_PARTITION_HPP
#define _AL_CHESS_DOMAIN_PARTITION_HPP

namespace chess
{
    enum class PartitionType { lookup_eDomainName, lookup_PieceSet};

    int PartitionType_to_int(PartitionType c)
    {
        if (c == PartitionType::lookup_eDomainName) return 1;
        else return 0;
    }
    PartitionType int_to_PartitionType(int t)
    {
        if (t == 1) return PartitionType::lookup_eDomainName;
        return PartitionType::lookup_PieceSet;
    }

    // Partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Partition
    {
        using _Partition    = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Domain       = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PartitionManager = PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    private:
        std::string                         _name;
        PartitionType                       _partition_type;
        std::map<std::string, _Domain*>     _domains;  // on heap - owner (TODO std::unique_ptr...)

    public:
        Partition() = default;
        ~Partition() = default;

        Partition(std::string aname, PartitionType pt = PartitionType::lookup_eDomainName) : _name(aname), _partition_type(pt)
        { 
        }
        std::string name()              const { return _name; }
        PartitionType partition_type()  const { return _partition_type; }       

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

        _Domain* find_domain(const std::string& domainname_key, const std::string& instance_key) const
        {
            return find_domain(_Domain::domain_key(domainname_key, instance_key));
        }
    };

    // save()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save() const
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("partition", _name);
        std::ofstream   is;
        is.open(f.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (is.good())
        {
            int c = PartitionType_to_int(_partition_type);

            is << _name;            is << " ";
            is << c;                is << " ";
            is << _domains.size();  is << " ";
            for (auto& v : _domains)
            {
                is << v.second->domainname_key();   is << " ";
                is << v.second->instance_key();     is << " ";
            }
        }
        else
        {
            is.close();
            return false;
        }
        is.close();
        return true;
    }

    // load()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load()
    {
        std::string f = PersistManager<PieceID, _BoardSize>::instance()->get_stream_name("partition", _name);
        std::ifstream   is;
        is.open(f.c_str(), std::fstream::in);
        if (is.good())
        {
            int c;
            size_t n_child;
            std::string partition_key;
            std::string domainname_key;
            std::string instance_key;

            is >> partition_key;
            assert(partition_key == _name); // check
            is >> c;
            _partition_type = int_to_PartitionType(c);

            is >> n_child;

            // reloading or creating
            _domains.clear();

            bool ok = true;
            for (size_t i = 0; i < n_child; i++)
            {
                ok = false;
                is >> domainname_key;
                is >> instance_key;
                {
                    _Domain* ptr_dom = _Domain::make(partition_key, domainname_key, instance_key);
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
                    is.close();
                    return false;
                }
            }

            // Load children and data of domains
            _Domain* p;
            for (auto& v : _domains)
            {
                p = v.second;
                if (p != nullptr)
                {
                    if (p->load() == false)
                    {
                        is.close();
                        return false;
                    }
                }
            }

            is.close();
            return true;
        }
        else
        {
            is.close();
            return false;
        }
    }
};

#endif
