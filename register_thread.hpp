#pragma once

#include <iostream>
#include <iomanip>
#include <omp.h>

#include <vector>
#include <queue>

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
    
    bool setCol(int col, int size)
    {
        if( col < _w_cols.size())
        {
            _col = col;
            _size = size;
            return true;
        }
        return false;
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
        /*if(t._col == 0)
        {
            os << std::fixed;
        }*/
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
    
    /*operator int() const
    {
        return _tid._cant;
    }*/
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
        std::string id = std::to_string(ntid._th_n) + ":" + std::to_string(ntid._n_ths);
        os << id;
        return os;
    }

};

#include <chrono>
#include <ctime>


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

    operator int () const  
    { 
        //precision mas "0." y " s"
        return _precision + 4;
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
    std::string _name;
    int _reg_count;
    Table _table;
    std::chrono::system_clock::time_point _start;
    
    Duration _first, _last;
    
    struct Register
    {
        int n_ths;
        int th_n;
        Duration diff;
        int reg_id;
        std::string msg;
        
    };
    std::queue<Register> _regs;
    
public:
    RegisterThread(const std::string & name) :
    _name {name},
    _table {{6, 8, 6, 16, 27}}
    {
        inic();
    }
    
    ~RegisterThread()
    {
        #pragma omp critical
        {
            std::cout << "begin {" << _name << "}"
                      << std::endl
                      << std::left
                      << column(0) << "#"
                      << column(1) << "vt"
                      << column(2) << "tid"
                      << column(3) << "time"
                      << column(4) << "message"
                      << std::endl;
              
             while (!_regs.empty())
             {
                auto r = _regs.front();
                VisualThreadId vtid(r.n_ths, r.th_n);
                ThreadId tid(r.n_ths, r.th_n);
                std::cout << column(0) << r.reg_id
                          << column(1) << vtid
                          << column(2) << tid
                          << column(3) << r.diff
                          << column(4) << r.msg
                          << std::endl;
                _regs.pop();
            }
        
            int w = 12;
            Duration t(std::chrono::system_clock::now() - _start);
            std::cout << "end {" << _name << "} in "<< t
                      << std::endl
                      << std::setw(w)
                      << "first "  << _first
                      << std::endl
                      << std::setw(w)
                      << "last " << _last
                      << std::endl
                      << std::setw(w)
                      << "diff " << _last - _first
                      << std::endl
                      << "-----------------------------------------------------"
                      << std::endl;
        }
    }

    void inic()
    {
        _reg_count = 0;
        _start = std::chrono::system_clock::now();
        _first = Duration(std::chrono::duration<double>::max());
        _last = Duration(std::chrono::duration<double>::min());
    }

    void registrate(const std::string &msg = "")
    {
        Duration diff(std::chrono::system_clock::now() - _start);
        #pragma omp critical
        {
        
            _regs.push({
                omp_get_num_threads(),
                omp_get_thread_num(),
                diff,
                _reg_count++,
                msg
            });
            
            if(_first > diff) _first = diff;
            if(_last  < diff) _last  = diff;
        }
    }

private:

    const Table & column(int c, const std::string &str)
    {
         _table.setCol(c, str.size());
         return _table;
    }
    const Table & column(int c, int s)
    {
         _table.setCol(c, s);
         return _table;
    }
    const Table & column(int c)
    {
         _table.setCol(c, 0);
         return _table;
    }
};
