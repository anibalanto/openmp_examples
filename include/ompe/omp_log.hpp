#include <string>
#include <set>
#include <deque>
#include <map>
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <optional>
#include <algorithm>
#include <chrono>
#include <thread>
#include <ctime>
#include <omp.h>

//stumb omp
/*void omp_set_nested(int)    { }
void omp_set_dynamic(int)   { }
int omp_get_num_threads()   { return 1; }
int omp_get_thread_num()    { return 0; }*/

namespace omp_log {

class thread;

using thread_id = std::deque<int>;

class thread_team
{
    std::vector< std::shared_ptr<thread> > threads;
public:
    thread_team(std::shared_ptr< std::vector< std::shared_ptr< int > > > deeps);

    const std::vector< std::shared_ptr<thread> > & get_threads() const
    {
        return threads;
    }

    std::shared_ptr<thread> find(int id);

    std::shared_ptr<thread> find(thread_id &id)
    {
        return find(*this, id);
    }

    int get_deep() const;
private:
    std::shared_ptr<thread> find(thread_team &team, thread_id &th_id);
};

class thread
{
    int num;
    int deep;
    std::optional<thread_team> nested;
public:
    thread(int num, int deep) :
        num{num},
        deep{deep}
    { }

    int get_num() const
    {
        return num;
    }

    int get_deep() const
    {
        return deep;
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

    void fork(std::shared_ptr< std::vector< std::shared_ptr< int > > >  deeps)
    {
        if(!nested)
        {
            nested = {thread_team(deeps)};
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
        os << "{n:" << th.num << " d:" << th.deep << "}";
        return os;
    }
};


thread_team::thread_team(std::shared_ptr<std::vector<std::shared_ptr<int> > > deeps) :
    threads { deeps->size() }
{
    for(int i = 0; i < deeps->size(); i++)
    {
        auto deep = (*deeps)[i];
        if( deep )
            threads[i] = std::make_shared<thread>( i, *(deep) ) ;
        else
            threads[i] = std::make_shared<thread>(i, -10 );
    }
}

int thread_team::get_deep() const
{
    int sz = 0;
    for(auto th : threads)
    {
        sz += th->get_deep();
    }
    return sz;
}


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


std::string vis_id(  thread_id id )
{
    std::ostringstream  ss;
    while( !id.empty() )
    {
        ss << id.back() << ":";
        id.pop_back();
    }
    return "[" + ss.str() + "]";
}

std::string vis_deeps(std::shared_ptr<std::vector<std::shared_ptr<int> > > deeps)
{
    std::string vd = "dp:{";
    for(int i = 0; i < deeps->size(); i++)
    {
        auto deep = (*deeps)[i];
        if( deep )
            vd += std::to_string( *(deep) );
        else
            vd += "n";
        vd += ", ";
    }
    vd += "}";
    return vd;
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
        vis(team, actual_id, id, false, os);
        os << vis_id( id ) << " >";
        return os;
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
                    os << "│ " << vis(th->get_deep() - 1, " ");
                }
            }

            actual_id.pop_front();
        }
        return os;
    }

    std::string vis(int rep, const std::string &str)
    {
        std::string v = "";
        for(int i = 0; i < rep; i++)
        {
            v += str + str;
        }
        return v;
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
        return std::to_string( th->get_num() ) + " " + vis(th->get_deep() - 1, " ");//vis_space(th);
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
                v += actual  + next + vis( (*it_th)->get_deep() - 1, next );
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


class duration
{
    std::chrono::duration<double> _duration;
    int _precision;
public:
    duration(std::chrono::duration<double> duration = std::chrono::duration<double>::zero(), int precision = 9) :
        _duration{duration},
        _precision{precision}
    { }

    friend inline ::std::ostream& operator<< (::std::ostream & os, const duration &d)
    {
        os << std::fixed
           << std::setprecision(d._precision ) << std::setfill( ' ' )
           << d._duration.count();
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


class thread_inic_team_log
{
    std::shared_ptr< std::vector< std::shared_ptr< int > > >  deeps;

public:
    thread_inic_team_log(int cant) :
        deeps { std::make_shared<std::vector< std::shared_ptr< int > > >(cant) }
        { }

    thread_inic_team_log() :
        deeps { std::make_shared<std::vector< std::shared_ptr< int > > >() }
        { }

    void add_data(int pos, std::shared_ptr< int > max_deep)
    {
        if( !initialized() )
            deeps->resize(omp_get_num_threads());
        (*deeps)[pos] = max_deep;
    }

    std::shared_ptr< std::vector< std::shared_ptr< int > > > get_deeps()
    {
        return deeps;
    }

    bool initialized() const
    {
        return deeps->size() > 0;
    }

    bool complete() const
    {
        bool is_complete = true;
        for(auto deep : *deeps)
        {
            if(!deep)
            {
                is_complete = false;
                break;
            }
        }
        return is_complete;
    }
};


enum class event_t {begin, message, fork, join, end};
struct event
{
    //duration                    time;
    event_t                                 type;
    std::optional< thread_id >              id;
    std::optional< std::string >            msg;
    std::shared_ptr<thread_inic_team_log>   data_team;
};

class thread_logger //! instancia unica de logger (singleton)
{
    std::vector<event>    events;


public:
    const std::vector<event> & get_events()
    {
        return events;
    }

    void begin( const thread_id &id, std::shared_ptr<thread_inic_team_log> data_team )
    {
        //std::cout << "begin " << vis_id(id) << " " << data_team.use_count() << "\n";
        logging({   //chrono(),
                    event_t::begin,
                    {id}, {}, data_team });
    }

    void begin_team(const thread_id &id, std::shared_ptr<thread_inic_team_log> data_team)
    {
        //std::cout << "beginteam " << vis_id(id) << " " << data_team.use_count() << "\n";
        logging({   //chrono(),
                    event_t::fork,
                    {id}, {}, data_team });
    }

    void end_team(const thread_id &id/*, std::shared_ptr<thread_inic_team_log> &data_fork*/)
    {
        //std::cout << "endteam " << vis_id(id) << "\n";
        logging({   //chrono(),
                    event_t::join,
                    {id}, {}, {}/*data_fork*/ });
    }

    void end(const thread_id &id)
    {
        //std::cout << "end " << vis_id(id) << "\n";
        logging({   //chrono(),
                    event_t::end,
                    {id}, {}, {} });
    }

    void message(const thread_id &id, const std::string &msg)
    {
        //std::cout << "msg " << vis_id(id) << " " <<msg << "\n";
        logging({   //chrono(),
                    event_t::message,
                    {id}, {msg}, nullptr });
    }

private:

    void logging(const event &data)
    {
        events.push_back(data);
    }

    /*duration chrono()
    {
        return std::chrono::system_clock::now() - start;
    }*/
};


void show(const std::vector<event> &events)
{

    std::shared_ptr<thread_team> g_team;
    for( auto event : events )
    {
        switch (event.type)
        {
            case event_t::begin :
                {
                    //std::cout << "begin " << vis_id(*event.id) << " " << event.data_team.use_count() << "\n";
                    g_team = std::make_shared<thread_team>(event.data_team->get_deeps());
                    if(auto th = g_team->find(*event.id))
                    {
                        thread_team_visual_fork vfk(*g_team,
                                                    *event.id);
                        std::cout << vfk << /**th << */" @begin" << "\n";
                    }
                }

                break;
            case event_t::end :
                {
                    //std::cout << "end " << vis_id(*event.id) << "\n";
                    if(auto th = g_team->find(*event.id))
                    {
                        thread_team_visual_join vjn(*g_team,
                                                    *event.id);
                        std::cout << vjn << /**th << */" @end" << "\n";
                        g_team = nullptr;
                    }
                }
                break;
            case event_t::fork :
                {
                    //std::cout << "fork " << vis_id(*event.id) << "\n";
                    if(auto th = g_team->find(*event.id))
                    {
                        auto deeps = event.data_team->get_deeps();
                        th->fork( deeps );
                        thread_team_visual_fork vfk(*g_team,
                                                    *event.id);
                        std::cout << vfk << /**th << vis_deeps(deeps) << */" @fork" << "\n";
                    }
                }
                break;
            case event_t::join :
                {
                    //std::cout << "join " << vis_id(*event.id) << "\n";
                    if(auto th = g_team->find(*event.id))
                    {
                        thread_team_visual_join vjn(*g_team,
                                                    *event.id);
                        std::cout << vjn << /**th << */" @join" << "\n";
                        th->join();
                    }
                }
                break;
            case event_t::message :
                {
                    //std::cout << "message find th" << vis_id(*event.id) ;
                    if(auto th = g_team->find(*event.id))
                    {
                        //std::cout << "found ";

                    }
                    else
                    {
                        //std::cout << "dont found";

                    }
                    //std::cout << " " << *event.msg << "\n";
                    thread_team_visual_id vid(*g_team, *event.id);
                    std::cout << vid << " " << *event.msg  << "\n";
                }
                break;
            default:
                std::cout << "Error!";
                break;
        }

        //root_team.find();
    }
}

class thread_log;

struct thread_deep_log //puede ser un width
{
    std::shared_ptr< int >  max;
    std::map<int, int>      max_deep_childs;

public:
    thread_deep_log() :
        max     { std::make_shared<int>(1) }
    { }

    void add_max_deep_child(int pos, int max)
    {
        max_deep_childs[pos] = max;
    }

    void calculate_max_deep()
    {
        *max = 0;
        for( auto deep_ch : max_deep_childs )
        {
            *max += deep_ch.second;
        }
        max_deep_childs.clear();
    }

    
};


class stream_logger;


/*
 * Es necesario alguien que sepa cuando hay
 * fork, join, begin y end
 * de aqui se puede sacar el id
 * no es necesario darle ess responsabilidad
 * a stream_logger
 */


class thread_log{
    thread_id id;
    int num;
    
    std::optional<thread_logger>            u_logger;
    thread_logger&                          logger;
    stream_logger &                         s_log;
    thread_deep_log                         deep;
    std::shared_ptr<thread_inic_team_log>   data_team;

    thread_log*                 parent;
    std::vector< thread_log* >  childs;

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
            if(!parent)
            {
                logger.end(id);
                show(logger.get_events());
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

    thread_deep_log & get_deep()
    {
        return deep;
    }

    void vincule_to_team(std::shared_ptr<thread_inic_team_log> d_team)
    {
        d_team->add_data(id.front(), deep.max);
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
        
        //else
        childs.push_back(ch);

        //deep.update(childs.size());
        //deep.inc();//no va a funcionar para nowait

        



    }

    void remove_child(thread_log *child)
    {
        //future: vec.erase(std::remove(vec.begin(), vec.end(), 8), vec.end());
        auto it = std::find(childs.begin(), childs.end(), child);
        if (it != childs.end())
        {
            deep.add_max_deep_child( (*it)->get_num(), *((*it)->get_deep().max) );
            childs.erase(it);
            //deep.update(childs.size());
            if(childs.empty())
            {
               if(data_team->complete())
               {
                    deep.calculate_max_deep();
                    logger.end_team(id);
               }
            }
        }
    }
};


/*void thread_deep_log::inc()
{
    actual++;

    if(*(max) < actual)
    {
        *(max) = actual;
        if(parent)
            parent->get_deep().inc();
    }
}

void thread_deep_log::dec()
{
    actual--;

    if(parent)
        parent->get_deep().dec();
}*/

enum class msg { end };

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

    thread_log &get_th_log()
    {
        return th_log;
    }

    const std::string extract()
    {
        auto str = sstream.str();
        sstream.str("");
        sstream.clear();
        return str;
    }

    template <typename T>
    friend stream_logger& operator<<(stream_logger& sm, const T& t)
    {
        sm.sstream << t;
        return sm;
    }

    friend stream_logger& operator<<(stream_logger& sm, const msg& em)
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


int main()
{

    omp_set_nested(0);
    omp_set_dynamic(0);


    omp_log_inic(log);
    log << "first 1" << omp_log::msg::end;
    #pragma omp parallel num_threads(2)
    {
        omp_log_inic_parented(log);
        log << "first 2" << omp_log::msg::end;
        #pragma omp parallel num_threads(4)
        {
            omp_log_inic_parented(log);
            log << "first 3" << omp_log::msg::end;
            #pragma omp parallel num_threads(4)
            {
                omp_log_inic_parented(log);
                log << "first 4" << omp_log::msg::end;
                for(int i = 0; i < 30; i++)
                    log << "for 4:" << i << omp_log::msg::end;
                log << "last 4" << omp_log::msg::end;
            }
            log << "last 3" << omp_log::msg::end;
        }
        log << "last 2" << omp_log::msg::end;
    }
    log << "last 1" << omp_log::msg::end;

    return 0;
}
