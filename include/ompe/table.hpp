#pragma once
#include <vector>
#include <iostream>

class Field
{
    bool _valid;
    int _w = 0;

public:
    Field() :
        _valid{false}
    { }
    
    Field(int w) :
        _valid{true},
        _w{w}
    { }

    bool isValid() const
    {
        return _valid;
    }

    int w() const
    {
        return _w;
    }

    friend inline ::std::ostream& operator<< (::std::ostream & os, const Field &f)
    {
        if(f.isValid())
        {
            os << std::setw(f.w());
        }
        return os;
    }

};

class Header : public Field
{
    bool _first;

public:
    Header() { } //no valid Field

    Header(int w, bool first) :
        Field(w),
        _first{first}
    { }

    friend inline ::std::ostream& operator<< (::std::ostream & os, const Header &h)
    {
        if(h.isValid())
        {
            if (h._first)
            {
                os << std::left;
            }
            os << std::setw(h.w());
        }
        return os;
    }
};

class Table
{
    std::vector<int> _w_cols;
public:
    Table(const std::vector<int> &w_cols) :
        _w_cols{w_cols}
    { }
    
    Header header(int col)
    {
        if( col < _w_cols.size())
        {
            return { _w_cols[col], col == 0 };
        }
        return { };
    }

    Field column(int col)
    {
        if( col < _w_cols.size())
        {
            return { _w_cols[col] };
        }
        return { };
    }
};