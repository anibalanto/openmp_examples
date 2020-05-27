#pragma once

#include <string>
#include <set>
#include <deque>
#include <map>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <optional>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <thread>
#include <mutex>
#include <ctime>
#include <omp.h>

namespace omp_log {

static int precision_default = 6;
    
class thread;

using thread_id = std::deque<int>;

struct thread_dimension
{
    int width;
    int deep;

    friend std::ostream& operator<<(std::ostream& os, const thread_dimension& dim)
    {
        os << "{w:" << dim.width << " d:" << dim.deep << "}";
        return os;
    }
};

class thread_team
{
    std::vector< std::shared_ptr<thread> > threads;
public:
    thread_team(std::shared_ptr<std::vector<std::shared_ptr<thread_dimension> > > dimensions);

    const std::vector< std::shared_ptr<thread> > & get_threads() const
    {
        return threads;
    }

    std::shared_ptr<thread> find(int id);

    std::shared_ptr<thread> find(thread_id &id)
    {
        return find(*this, id);
    }

    //int get_width() const;
private:
    std::shared_ptr<thread> find(thread_team &team, thread_id &th_id);
};

class thread
{
    int num;
    thread_dimension dimension;
    std::optional<thread_team> nested;
public:
    thread(int num, thread_dimension &dimension) :
        num{num},
        dimension{dimension}
    { }

    int get_num() const
    {
        return num;
    }

    const thread_dimension &get_dimension() const
    {
        return dimension;
        /*if(nested)
        {
            return (*nested).get_size();
        }
        return 1;*/
    }

    const std::optional<thread_team> &get_nested() const
    {
        return nested;
    }

    void fork(std::shared_ptr< std::vector< std::shared_ptr< thread_dimension > > >  dimensions)
    {
        if(!nested)
        {
            nested = {thread_team(dimensions)};
        }
    }

    void join()
    {
        if(nested)
        {
            nested = {};
        }
    }

    inline friend bool operator<(const thread& th, const int& num)
    {
        return th.num < num;
    }

    friend std::ostream& operator<<(std::ostream& os, const thread& th)
    {
        os << "{num:" << th.num << " dim:" << th.dimension << "}";
        return os;
    }
};


thread_team::thread_team(std::shared_ptr<std::vector<std::shared_ptr<thread_dimension> > > dimensions) :
    threads { dimensions->size() }
{
    for(int i = 0; i < dimensions->size(); i++)
    {
        auto dimension = (*dimensions)[i];
        if( dimension )
            threads[i] = std::make_shared<thread>( i, *(dimension) ) ;
        else
            std::cout << "null dimension!" << std::endl;
    }
}

/*int thread_team::get_width() const
{
    int sz = 0;
    for(auto th : threads)
    {
        sz += th->get_width();
    }
    return sz;
}*/


struct find_by_num {
    find_by_num(const int & num) : num{num} {}
    bool operator()(const std::shared_ptr<thread>  & th) {
        return th->get_num() == num;
    }
private:
    int num;
};


std::shared_ptr<thread> thread_team::find(int num)
{
    /*auto it = std::find_if( threads.begin(),
                            threads.end(),
                            find_by_num(num));;
    if(it != threads.end())
        return *it;*/
    if( num < threads.size() )
       return threads[num];
    return nullptr;
}


std::shared_ptr<thread> thread_team::find(thread_team &team, thread_id &id)
{
    if( !id.empty() )
    {
        auto num = id.back();
        auto th = team.find(num);
        if( th )
        {
            if(id.size() == 1)
                return th;
            else
            {
                if(auto nested_team = th->get_nested())
                {
                    id.pop_back();
                    auto th = find(*nested_team, id);
                    id.push_back(num);
                    return th;
                }
            }
        }
    }
    return nullptr;
}


std::string vis_id( thread_id id)
{
    std::ostringstream  sparent_id;
    std::ostringstream  s_id;

    int num = id.front();
    id.pop_front();

    if( !id.empty() )
    {

        while( !id.empty() )
        {
            sparent_id << id.back() << ".";
            id.pop_back();
        }

    }

    s_id << /*"(" <<*/ sparent_id.str() /*<< "{" */<< num /*<< "}" << ")"*/;

    return s_id.str();
}


class thread_id_visual
{
    thread_id   id;
    int         deep;
public:
    thread_id_visual(thread_id &id, int deep) :
        id      {id},
        deep    {deep}
    { }

protected:
    std::ostream& vis(std::ostream& os)
    {
        os /*<< std::left*/ << std::setw(deep * 2 - 1) << vis_id(id);
        return os;
    }

    friend inline std::ostream& operator<<(std::ostream& os, thread_id_visual &tid)
    {
        return tid.vis(os);
    }
};

void vis_dimensions(std::shared_ptr<std::vector<std::shared_ptr<thread_dimension> > > dimensions,
                           std::ostream & os)
{
    os << "dims:{";
    for(int i = 0; i < dimensions->size(); i++)
    {
        auto dim = (*dimensions)[i];
        if( dim )
        {
            os << *dim;
        }
        else
            os << "n";
        os << ", ";
    }
    os << "}";
}
std::string vis_space(std::shared_ptr<thread> th, const std::string &str)
{
    std::string v = str;
    for(int i = 0; i < th->get_dimension().width - 1; i++)
    {
        v += str + str;
    }
    return v;
}

class thread_team_visual
{
    thread_team &team;
    thread_id id;
public:
    thread_team_visual(thread_team &team, thread_id &id) :
        team{team},
        id {id}
    { }

    std::ostream& vis(std::ostream& os)
    {
        thread_id actual_id;
        return vis(team, actual_id, id, false, os);
    }

protected:

    std::ostream& vis(  const thread_team &     team,
                                thread_id &     actual_id,
                                thread_id &     find_id,
                                bool            finded,
                                std::ostream &  os)
    {
        for(auto th : team.get_threads())
        {
            auto num = th->get_num();
            actual_id.push_front(num);

            //os << "{" << vis_id( find_id ) <<  "/" << vis_id( actual_id ) << "}";

            if(!finded && actual_id == find_id)
            {
                os << vis_actual(th);
                finded = true;
            }
            else
            {
                if(auto team = th->get_nested())
                    vis(*team, actual_id, find_id, finded, os);
                else
                {
                    os << "│" << vis_space(th, " ");
                }
            }

            actual_id.pop_front();
        }
        return os;
    }

    virtual std::string vis_actual( std::shared_ptr<thread> & th) = 0;

    friend inline std::ostream& operator<<(std::ostream& os, thread_team_visual &tv)
    {
        return tv.vis(os);
    }
};

class thread_team_visual_id : public thread_team_visual
{
public:
    thread_team_visual_id(thread_team &team, thread_id &id) :
        thread_team_visual(team, id)
    { }

protected:

    std::string vis_actual( std::shared_ptr<thread> &   th ) override
    {
        return std::to_string( th->get_num() ) + vis_space(th, " ");//vis_space(th);
    }
};

class thread_team_visual_chrono : public thread_team_visual
{
    char ch;
public:
    thread_team_visual_chrono(thread_team &team, thread_id &id, char ch) :
        thread_team_visual(team, id),
        ch { ch }
    { }

protected:

    std::string vis_actual( std::shared_ptr<thread> &   th ) override
    {
        return ch + vis_space(th, " ");//vis_space(th);
    }
};

struct printeable_team
{
    std::string point;
    std::string next;
    std::string vertiz;
    std::string last_vertiz;
};



class thread_team_visual_bifurc : public thread_team_visual
{
public:
    thread_team_visual_bifurc(thread_team &team, thread_id &id) :
        thread_team_visual(team, id)
    { }

protected:

    std::string vis_actual( std::shared_ptr<thread> & th ) override
    {
        auto print = get_printeable();

        std::string v = "";
        if(auto  nested = th->get_nested() )
        {
            std::string actual, next;

            auto nested_begin   = (*nested).get_threads().begin();
            auto nested_end     = (*nested).get_threads().end();
            for (   auto it_th   =  nested_begin;
                    it_th       !=  nested_end;
                    it_th++)
            {
                it_th++;
                auto it_th_next = it_th;
                it_th--;
                if(it_th == nested_begin)
                {
                    actual  = print.point;
                    next    = ( it_th_next != nested_end ) ? print.next : " ";
                }
                else
                {
                   
                    if ( it_th_next != nested_end )
                    {
                        actual  = print.vertiz;
                        next    = print.next;
                    }
                    else
                    {
                        actual  = print.last_vertiz;
                        next    = " ";
                    }
                }
                v += actual  + vis_space( *it_th, next );
            }
        }
        return v;
    }

    virtual printeable_team get_printeable() = 0;
};



class thread_team_visual_fork : public thread_team_visual_bifurc
{
public:
    thread_team_visual_fork(thread_team &team, thread_id &id) :
        thread_team_visual_bifurc(team, id)
    { }
protected:


    printeable_team get_printeable() override
    {
        return { "*", "─", "┬", "┐" };
    }
};

class thread_team_visual_join : public thread_team_visual_bifurc
{
public:
    thread_team_visual_join(thread_team &team, thread_id &id) :
        thread_team_visual_bifurc(team, id)
    { }
protected:

    printeable_team get_printeable() override
    {
        return { "°", "─", "┴", "┘" };
    }
};


class thread_inic_team_log
{
    std::shared_ptr< std::vector< std::shared_ptr< thread_dimension > > > dimensions;

public:
    thread_inic_team_log(int cant) :
        dimensions { std::make_shared<std::vector< std::shared_ptr< thread_dimension > > >(cant) }
        { }

    thread_inic_team_log() :
        dimensions { std::make_shared<std::vector< std::shared_ptr< thread_dimension > > >() }
        { }

    void add_data(int pos, std::shared_ptr< thread_dimension > dimension)
    {
        if( !initialized() )
            dimensions->resize(omp_get_num_threads());
        (*dimensions)[pos] = dimension;
    }

    std::shared_ptr< std::vector< std::shared_ptr< thread_dimension > > >  get_dimensions()
    {
        return dimensions;
    }

    bool initialized() const
    {
        return dimensions->size() > 0;
    }

    bool complete() const
    {
        bool is_complete = true;
        for(auto dimension : *dimensions)
        {
            if(!dimension)
            {
                is_complete = false;
                break;
            }
        }
        return is_complete;
    }
};


struct chrono_log
{
    char id;
    std::chrono::system_clock::time_point time;
};

class duration
{
    std::chrono::duration<double>   _duration;
    int                             _precision;
public:
    duration(   std::chrono::duration<double>   duration    =
                    std::chrono::duration<double>::zero(),
                int                             precision   =   9) :
        _duration{duration},
        _precision{precision}
    { }

    friend inline ::std::ostream& operator<< (::std::ostream & os, const duration &d)
    {
        os << std::fixed
           << std::setprecision(d._precision ) << std::setfill( ' ' )
           << d._duration.count()
           << std::scientific
           << std::setprecision( precision_default ); 
        return os;
    }

    friend inline bool operator< (const duration &da, const duration &db)
    {
        return da._duration < db._duration;
    }

    friend inline bool operator> (const duration &da, const duration &db)
    {
        return da._duration > db._duration;
    }

    friend inline duration operator- (const duration &da, const duration &db)
    {
        return duration(da._duration - db._duration);
    }
};


class chronometer
{
    std::unordered_map<char, chrono_log> chronos;
public:
    void regist(const chrono_log & ch)
    {
        chronos[ch.id] = ch;
    }
    
    duration chronometrate(const chrono_log & ch)
    {
        auto it_ch_inic = chronos.find(ch.id);
        if(it_ch_inic != chronos.end())
        {
            duration d(ch.time - it_ch_inic->second.time);
            chronos.erase(it_ch_inic);
            return d;
        }
        return { };
    }
};


enum class event_t {begin, message, fork, join, chrono_begin, chrono_end, end};
struct event
{
    //duration                    time;
    event_t                                 type;
    std::optional< thread_id >              id;
    std::optional< std::string >            msg;
    std::shared_ptr<thread_inic_team_log>   data_team;
    std::optional< chrono_log >             chrono;
};

class thread_logger
{
    std::vector<event>  events;

public:
    const std::vector<event> & get_events()
    {
        return events;
    }

    void begin( const thread_id &id, std::shared_ptr<thread_inic_team_log> data_team )
    {
        logging({   event_t::begin,
                    {id}, {}, data_team });
    }

    void begin_team(const thread_id &id, std::shared_ptr<thread_inic_team_log> data_team)
    {
        logging({   event_t::fork,
                    {id}, {}, data_team });
    }

    void end_team(const thread_id &id/*, std::shared_ptr<thread_inic_team_log> &data_fork*/)
    {
        logging({   event_t::join,
                    {id}, {}, {}/*data_fork*/ });
    }

    void end(const thread_id &id)
    {
        logging({   event_t::end,
                    {id}, {}, {} });
    }

    void message(const thread_id &id, const std::string &msg)
    {
        logging({   event_t::message,
                    {id}, {msg}, {} });
    }
    
    void begin_chrono(const thread_id &id, const char & ch)
    {
        auto chr = std::optional<chrono_log>( { ch, std::chrono::system_clock::now() } );
        logging({   event_t::chrono_begin,
                    {id}, {}, {}, chr } );
    }
    
        void end_chrono(const thread_id &id, const char & ch)
    {
        auto chr = std::optional<chrono_log>( { ch, std::chrono::system_clock::now() } );
        logging({   event_t::chrono_end,
                    {id}, {}, {}, chr } );
    }

private:

    void logging(const event &data)
    {
        events.push_back(data);
    }
};


void show(std::ostream & os, const std::vector<event> &events)
{
    chronometer                         chrono;
    std::unique_ptr<thread_team>        g_team;
    int                                 g_deep;
    std::ostringstream                  smark;
    std::ostringstream                  smsg;
    int                                 width_id;
    
    for( auto event : events )
    {
        if(event.type == event_t::begin)
        {
            auto dimensions = event.data_team->get_dimensions();
            g_team      = std::make_unique<thread_team>(dimensions);
            g_deep = (*dimensions)[0]->deep;
        }
        auto &id = *event.id;
        if(auto th = g_team->find(id))
        {
            thread_id_visual vid(id, g_deep);
            switch (event.type)
            {
                case event_t::begin :
                {
                    thread_team_visual_fork thv( *g_team, id );
                    os      << thv;
                    smark   << "*";
                    smsg    << "begin";
                }
                    break;
                case event_t::end :
                {
                    thread_team_visual_join thv( *g_team, id );
                    os      << thv;
                    smark   << "°";
                    smsg    << "end";

                    g_team = nullptr;
                }
                    break;
                case event_t::fork :
                {
                    th->fork( event.data_team->get_dimensions() );

                    thread_team_visual_fork thv( *g_team, id );
                    
                    os      << thv;
                    smark   << "*";
                    smsg    << "fork";
                    #ifdef omp_log_test
                    vis_dimensions(dimensions, std::cout);
                    #endif
                }
                    break;
                case event_t::join :
                {
                    thread_team_visual_join thv( *g_team, id );
                        
                    os      << thv;
                    smark   << "°";
                    smsg    << "join";

                    th->join();
                }
                    break;
                case event_t::message :
                {
                    thread_team_visual_id thv( *g_team, id );
                        
                    os      << thv;
                    smark   << id.front();
                    smsg    << "msg: " << *event.msg;
                }
                    break;
                case event_t::chrono_begin:
                {
                    auto chr = *event.chrono;
                    thread_team_visual_chrono thv( *g_team, id, chr.id );
                        
                    os      << thv;
                    smark   << chr.id;
                    smsg    << "chrono_begin";
                    
                    chrono.regist(chr);
                }
                    break;
                case event_t::chrono_end:
                {
                    auto chr = *event.chrono;
                    thread_team_visual_chrono thv( *g_team, id, chr.id );
                        
                    os      << thv;
                    smark   << chr.id;
                    smsg    << "chrono_end: " << chrono.chronometrate(chr) << "s";
                }
                    break;
                default:
                    std::cout << "Error!";
                    break;
            }
            #ifdef omp_log_test
            std::cout << *th;
            #endif
            os << smark.str() << "> " << vid << " ~" << smsg.str() << "\n";

            //clear smark
            smark.str("");
            smark.clear();
            //clear smsg
            smsg.str("");
            smsg.clear();
        }
        else
            std::cout << "thread don't found!";

    }
}

class thread_log;


class thread_log_dimension
{
    std::shared_ptr<thread_dimension> dimension;
    std::map<int, std::shared_ptr<thread_dimension> >   dimension_childs;

public:
    thread_log_dimension() :
        dimension { std::make_shared<thread_dimension>() }
    { }

    std::shared_ptr<thread_dimension> &get_dimension()
    {
        return dimension;
    }

    void registrate_child_dimension(int pos, std::shared_ptr<thread_dimension> dimension)
    {
        dimension_childs[pos] = dimension;
    }

    void calculate()
    {
        dimension->deep  = 1;
        //pensar nowait
        if( dimension_childs.size() > 0 )
        {
            dimension->width = 0;
            int max_deep = 0;
            for( auto dim_ch : dimension_childs )
            {
                dimension->width += dim_ch.second->width;
                if( max_deep < dim_ch.second->deep )
                    max_deep = dim_ch.second->deep;
            }
            dimension->deep += max_deep;
        }
        else
        {
            dimension->width = 1;
        }
        dimension_childs.clear();
    }
};


class stream_logger;

class thread_log{
    
    thread_id id;
    int num;
    
    std::optional<thread_logger>            u_logger;
    thread_logger&                          logger;
    stream_logger &                         s_log;
    thread_log_dimension                    log_dimension;
    std::shared_ptr<thread_inic_team_log>   data_team;

    thread_log*                             parent;
    std::vector< thread_log* >              childs;

    static std::optional<thread_logger> get_unique_log(thread_log *parent)
    {
        if(!parent)
            return thread_logger();
        return std::nullopt;
    }

    static thread_logger& get_log(thread_log *parent, std::optional<thread_logger> &unique_logger)
    {
        if(!parent)
            return *unique_logger;
        return parent->get_logger();
    }

public:
    thread_log(stream_logger &s_log, thread_log *parent = nullptr) :
        num         { omp_get_thread_num() },
        u_logger    { get_unique_log(parent) },
        logger      { get_log(parent, u_logger) },
        s_log       { s_log },
        data_team   { std::make_shared<thread_inic_team_log>() },
        parent      { parent }
    {
        #pragma omp critical
        {
            if( !parent )
            {
                id.push_front( num );
                auto root_team = std::make_shared<thread_inic_team_log>();
                this->vincule_to_team(root_team);
                logger.begin(id, root_team);
            }
            else
            {
                id = parent->get_id();
                id.push_front( num );
                parent->add_child( this );
            }
        }
    }

    ~thread_log()
    {
        #pragma omp critical
        {
            log_dimension.calculate();

            if(!parent)
            {
                logger.end(id);
                show(std::cout, logger.get_events());
            }
            else
                parent->remove_child( this );
        }
    }

    thread_id & get_id()
    {
        return id;
    }
    
    int get_num()
    {
        return num;
    }

    thread_logger &get_logger()
    {
        return logger;
    }

    std::shared_ptr<thread_dimension> & get_dimension()
    {
        return log_dimension.get_dimension();
    }

    void vincule_to_team(std::shared_ptr<thread_inic_team_log> d_team)
    {
        d_team->add_data(id.front(), get_dimension());
    }

    void begin_chrono(const char & ch)
    {
        logger.begin_chrono(id, ch);
    }
    
    void end_chrono(const char & ch)
    {
        logger.end_chrono(id, ch);
    }
    
    void message();

protected:

    void add_child(thread_log *ch)
    {
         
        
        if( !data_team->initialized() )
        {
            logger.begin_team(id, data_team);
        }
        
        
        ch->vincule_to_team(data_team);
        
        childs.push_back(ch);

        



    }

    void remove_child(thread_log *child)
    {
        auto it = std::find(childs.begin(), childs.end(), child);
        if (it != childs.end())
        {
            log_dimension.registrate_child_dimension( (*it)->get_num(), (*it)->get_dimension() );
            childs.erase(it);

            if(childs.empty())
            {
               if(data_team->complete())
               {
                    logger.end_team(id);
               }
            }
        }
    }
};


class stream_logger
{
    thread_log          th_log;
    std::ostringstream  sstream;
    

public:
    stream_logger() :
        th_log { *this }
    { }

    stream_logger( stream_logger & parent ) :
        th_log{ *this, &(parent.get_th_log()) }
    { }
    
    ~stream_logger()
    {
        flush();
    }

    thread_log &get_th_log()
    {
        return th_log;
    }

    const std::string extract()
    {
        auto str = sstream.str();
        sstream.str("");
        sstream.clear();
        
        sstream << std::scientific << std::setprecision(precision_default);
        return str;
    }
    
    void begin_chrono(char c)
    {
        flush();
        th_log.begin_chrono(c);
    }

    void end_chrono(char c)
    {
        flush();
        th_log.end_chrono(c);
    }
    
protected:
    void flush()
    {
        if ( ! empty() )
            th_log.message();
    }
    bool empty()
    {
        std::streampos pos = sstream.tellp();   // store current location
        sstream.seekp(0, std::ios_base::end);        // go to end
        bool am_empty = (sstream.tellp() == 0);    // check size == 0 ?
        sstream.seekp(pos); 
        return am_empty;
    }
    template <typename T>
    friend stream_logger& operator<<(stream_logger& sm, const T& t)
    {
        sm.sstream << t;
        return sm;
    }
    
    friend stream_logger& operator<<(stream_logger& sm, std::ostream& (*fun)(std::ostream&) )
    {
        sm.th_log.message();
        return sm;
    }
};


void thread_log::message()
{
    #pragma omp critical
    {
        logger.message(    id,
                        s_log.extract());
    }
}

}


#define omp_log_inic(name)              \
omp_log::stream_logger name             \

#define omp_log_inic_parented(name)             \
auto &parent = name;            \
omp_log::stream_logger name( parent )   \

