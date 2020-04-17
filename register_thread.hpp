#pragma once

#include <iostream>
#include <iomanip>
#include <omp.h>

#include <vector>
#include <queue>

#include <chrono>
#include <ctime>

class Table
{
    int _col;
    int _size;
    std::vector<int> _w_cols;
public:
    Table(const std::vector<int> &w_cols) :
    _col{0},
    _w_cols{w_cols}
    { }
    
    void setCol(int col)
    {
        if( col < _w_cols.size())
        {
            _col = col;
        }
    }

    int wCol() const
    {
        if(_w_cols.size() > 0)
        {
            return _w_cols[_col];
        }
        return 0;
    }

    friend inline ::std::ostream& operator<< (::std::ostream & os, const Table &t)
    {
        os << std::setw(t.wCol());
        return os;
    }
};


class VisualThreadId
{
    int _n_ths;
    int _th_n;
public:
    VisualThreadId(int n_ths, int th_n) :
        _n_ths{n_ths},
        _th_n{th_n}
    { }
    
    friend inline std::ostream& operator<<(std::ostream& os, VisualThreadId &vtid)
    { 
        std::string svid = "";
        for (int i = 0; i < vtid._n_ths; i++)
        {
            //if i is equal to thread number writes "*" else will write "|"
            svid += (i == vtid._th_n)? "*" : "|";
        }
        os << std::left << svid;
        return os;
    }
};

class ThreadId
{
    int _n_ths;
    int _th_n;
public:
    ThreadId(int n_ths, int th_n) :
        _n_ths{n_ths},
        _th_n{th_n}
    { }
    
    friend inline std::ostream& operator<<(std::ostream& os, ThreadId &ntid)  
    { 
        std::string id = std::to_string(ntid._th_n)
                         + ":" +
                         std::to_string(ntid._n_ths);
        os << id;
        return os;
    }

};

class Duration
{
    std::chrono::duration<double> _duration;
    int _precision;
public:
    Duration(std::chrono::duration<double> duration = std::chrono::duration<double>::zero(), int precision = 9) :
        _duration{duration},
        _precision{precision}
    { }

    friend inline ::std::ostream& operator<< (::std::ostream & os, const Duration &d)
    {
        os << std::fixed
           << std::setprecision(d._precision ) << std::setfill( ' ' )
           << d._duration.count();
        return os;
    }

    friend inline bool operator< (const Duration &da, const Duration &db)
    {
        return da._duration < db._duration;
    }

    friend inline bool operator> (const Duration &da, const Duration &db)
    {
        return da._duration > db._duration;
    }

    friend inline Duration operator- (const Duration &da, const Duration &db)
    {
        return Duration(da._duration - db._duration);
    }
};




class RegisterThread
{

    struct DataRegister
        {
            int n_ths;
            int th_n;
            Duration diff;
            //int reg_id;
            std::string msg;
        };

    std::queue<DataRegister> _regs;

    std::string _name;
    int _reg_count = 0;
    Table _table;
    std::chrono::system_clock::time_point _start, _end;
    
    Duration _first = Duration(std::chrono::duration<double>::max()),
             _last  = Duration(std::chrono::duration<double>::min());

    bool _initialized, _finalized = false;
    
public:
    RegisterThread(const std::string & name) :
    _name {name},
    _start {std::chrono::system_clock::now()},
    _table {{8, 6, 16, 27}}
    { }
    
    ~RegisterThread()
    {
        #pragma omp critical
        {
            std::cout << "begin {" << _name << "}"
                      << std::endl
                      << std::left
                      //<< column(0) << "#"
                      << column(0) << "vt"
                      << column(1) << "tid"
                      << column(2) << "time"
                      << column(3) << "message"
                      << std::endl;
              
             while (!_regs.empty())
             {
                auto r = _regs.front();
                VisualThreadId vtid(r.n_ths, r.th_n);
                ThreadId tid(r.n_ths, r.th_n);
                //std::cout << column(0) << r.reg_id
                std::cout << column(0) << vtid
                          << column(1) << tid
                          << column(2) << r.diff
                          << column(3) << r.msg
                          << std::endl;
                _regs.pop();
            }
        
            int w = 12;
            std::cout << "end {" << _name << "} in " << chronometrate()
                      << std::endl
                      << std::setw(w)
                      << "first_reg "  << _first
                      << std::endl
                      << std::setw(w)
                      << "last_reg " << _last
                      << std::endl
                      << std::setw(w)
                      << "diff " << _last - _first
                      << std::endl
                      << "-----------------------------------------------------"
                      << std::endl;
        }
    }

    void registrate(const std::string &msg = "")
    {
        #pragma omp critical
        {
            auto time = chronometrate();
            _regs.push({
                omp_get_num_threads(),
                omp_get_thread_num(),
                time,
                //_reg_count++,
                msg
            });
            
            if(_first > time) _first = time;
            if(_last  < time) _last  = time;
        }
    }

private:

    Duration chronometrate()
    {
        return { std::chrono::system_clock::now() - _start };
    }

    const Table & column(int c)
    {
         _table.setCol(c);
         return _table;
    }
};
