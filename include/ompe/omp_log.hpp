/*
 * OpenMP Log
 *
 * @autor Anibal Fernando Antonelli
 * Catedra: Sistemas De Computación Distribuidos
 * Facultad de Ingeniería
 * Universidad Nacional de Mar del Plata
 */

#pragma once

#include <algorithm>
#include <chrono>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <omp.h>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>

//#define omp_log_test_end
//#define omp_log_test
//#define omp_log_test_logging

namespace omp_log {

static int precision_default = 6; //> time precision to show at stream buffer

class thread;

/**
 * A thread identifier is a series of integer values
 * that indicate its own identifier and the identifiers
 * of its parent threads. It is implemented as a deque
 * to facilitate access to both the first and last values.
 * */
using thread_id = std::deque<int>;

template <typename T> using vec = std::vector<T>;

template <typename T> using sptr = std::shared_ptr<T>;

template <typename T> using opt = std::optional<T>;

/**
 * The dimension of a thread is determined by the quantity
 * of its descendants, represented by the width, and, on
 * the other hand, the maximum depth of the child (deep).
 * Deep is used to set the width of the ID characters,
 * while width is used to provide visual spacing in the
 * execution tree. This functionality will be reflected
 * in the thread_log_dimension class.
 * */
struct thread_dimension {
  int width;
  int deep;

  friend std::ostream &operator<<(std::ostream &os,
                                  const thread_dimension &dim) {
    os << "{w:" << dim.width << " d:" << dim.deep << "}";
    return os;
  }
};

/**
 * */
class thread_team {
  vec<sptr<thread>> threads;
  thread *owner;

public:
  thread_team(sptr<vec<sptr<thread_dimension>>> dimensions,
              thread *owner = nullptr);

  ~thread_team(){
  //  std::cout << "team_deleted" << std::endl; 
  }

  const vec<sptr<thread>> &get_threads() const { return threads; }

  sptr<thread> find(int id);

  sptr<thread> find(thread_id &id) { return find(*this, id); }

  // int get_width() const;
private:
  sptr<thread> find(thread_team &team, thread_id &th_id);
};

std::string vis_id(thread_id id);

class thread {

  int num;
  thread_dimension dimension;
  thread *parent;
  sptr<thread_team> nested;
  std::stack<sptr<thread_team>> stack_nested;


public:
  thread(int num, thread_dimension &dimension, thread *parent)
      : num{num}, dimension{dimension}, parent{parent} {
#ifdef omp_log_test
//        std::cout << "{+" << num << "+}" << std::endl;
#endif
  }

  ~thread() {
 //   std::cout << "{-" << num << "-}" << std::endl; 
#ifdef omp_log_test
     
#endif
  }

  int get_num() const { return num; }

  int get_compelete_width() {
    auto diff = 0;
    if (auto parent = get_parent()) {

      auto parent_threads = parent->get_nested()->get_threads();
      auto last = parent_threads.back();
      if (last.get() == this) {
        auto max_width_parent = parent->get_dimension().width;

        auto actual_width_parent = 0; // parent_threads.size();

        for (auto th_team : parent_threads) {
          actual_width_parent += th_team->get_dimension().width;
        }

        diff = max_width_parent - actual_width_parent;

#ifdef omp_log_test
        std::cerr << max_width_parent << "\t - " << actual_width_parent
                  << "\t = " << diff << "\t {" << vis_id(get_id()) << "}"
                  << std::endl;
#endif
      }
    }
    return dimension.width + diff;
  }

  const thread_dimension &get_dimension() const {

    return dimension;
    /*if(nested)
    {
        return (*nested).get_size();
    }
    return 1;*/
  }

  const thread *get_parent() { return parent; }

  thread_id get_id() {
    thread_id id = {num};
    if (parent) {
      auto id_parent = parent->get_id();
      id.insert(id.end(), id_parent.begin(), id_parent.end());
    }
    return id;
  }

  const sptr<thread_team> &get_nested() const { return nested; }

  void fork(sptr<vec<sptr<thread_dimension>>> dimensions) {
    if( nested != nullptr ) {
      stack_nested.emplace(nested);
    }
    nested = std::make_shared<thread_team>(thread_team{dimensions, this});
  }

  void join() {
    if( stack_nested.empty() ) {
      nested = nullptr;
    } else {
      nested = stack_nested.top();
      stack_nested.pop();
    }
  }

  inline friend bool operator<(const thread &th, const int &num) {
    return th.num < num;
  }

  friend std::ostream &operator<<(std::ostream &os, const thread &th) {
    os << "{num:" << th.num << " dim:" << th.dimension << "}";
    return os;
  }
};

inline thread_team::thread_team(sptr<vec<sptr<thread_dimension>>> dimensions,
                                thread *owner)
    : owner{owner}, threads{dimensions->size()} {
  for (int i = 0; i < dimensions->size(); i++) {
    auto dimension = (*dimensions)[i];
    if (dimension)
      threads[i] = std::make_shared<thread>(i, *(dimension), owner);
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
  find_by_num(const int &num) : num{num} {}
  bool operator()(const sptr<thread> &th) { return th->get_num() == num; }

private:
  int num;
};

inline sptr<thread> thread_team::find(int num) {
  /*auto it = std::find_if( threads.begin(),
                          threads.end(),
                          find_by_num(num));;
  if(it != threads.end())
      return *it;*/
  if (num < threads.size())
    return threads[num];
  return nullptr;
}

inline sptr<thread> thread_team::find(thread_team &team, thread_id &id) {
  if (!id.empty()) {
    auto num = id.back();
    auto th = team.find(num);
    if (th) {
      if (id.size() > 1) {
        if (auto nested_team = th->get_nested()) {
          id.pop_back();
          auto nested_th = find(*nested_team, id);
          id.push_back(num);

          return nested_th;
        }
      } else
        return th;
    }
  }
  return nullptr;
}

inline std::string vis_id(thread_id id) {
  std::ostringstream sparent_id;
  std::ostringstream s_id;

  int num = id.front();
  id.pop_front();

  if (!id.empty()) {

    while (!id.empty()) {
      sparent_id << id.back() << ".";
      id.pop_back();
    }
  }

  s_id << /*"(" <<*/ sparent_id.str() /*<< "{" */ << num /*<< "}" << ")"*/;

  return s_id.str();
}

class thread_id_visual {
  thread_id id;
  int deep;

public:
  thread_id_visual(thread_id &id, int deep) : id{id}, deep{deep} {}

protected:
  std::ostream &vis(std::ostream &os) {
    os /*<< std::left*/ << std::setw(deep * 2 - 1) << vis_id(id);
    return os;
  }

  friend inline std::ostream &operator<<(std::ostream &os,
                                         thread_id_visual &tid) {
    return tid.vis(os);
  }
};

inline void vis_dimensions(sptr<vec<sptr<thread_dimension>>> dimensions,
                           std::ostream &os) {
  os << "dims:{";
  for (int i = 0; i < dimensions->size(); i++) {
    auto dim = (*dimensions)[i];
    if (dim) {
      os << *dim;
    } else
      os << "n";
    os << ", ";
  }
  os << "}";
}

inline std::string vis_space(sptr<thread> th, const std::string &str) {
  std::string v = str;

  for (int i = 0; i < th->get_compelete_width() - 1; i++) {
    v += str + str;
  }
  return v;
}

class thread_team_visual {
  thread_team &team;
  thread_id id;

public:
  thread_team_visual(thread_team &team, thread_id &id) : team{team}, id{id} {}

  std::ostream &vis(std::ostream &os) {
    thread_id actual_id;
    return vis(team, actual_id, id, false, os);
  }

protected:
  std::ostream &vis(const thread_team &team, thread_id &actual_id,
                    thread_id &find_id, bool finded, std::ostream &os) {
    for (auto th : team.get_threads()) {
      auto num = th->get_num();
      actual_id.push_front(num);

      // os << "{" << vis_id( find_id ) <<  "/" << vis_id( actual_id ) << "}";

      if (!finded && actual_id == find_id) {
        os << vis_actual(th);
        finded = true;
      } else {
        if (auto team = th->get_nested())
          vis(*team, actual_id, find_id, finded, os);
        else {
          os << "│"
             << vis_space(th,
                          " "); //! TODO agregar el espachio maximo historico
        }
      }

      actual_id.pop_front();
    }
    return os;
  }

  virtual std::string vis_actual(sptr<thread> &th) = 0;

  friend inline std::ostream &operator<<(std::ostream &os,
                                         thread_team_visual &tv) {
    return tv.vis(os);
  }
};

class thread_team_visual_id : public thread_team_visual {
public:
  thread_team_visual_id(thread_team &team, thread_id &id)
      : thread_team_visual(team, id) {}

protected:
  std::string vis_actual(sptr<thread> &th) override {
    return std::to_string(th->get_num()) + vis_space(th, " "); // vis_space(th);
  }
};

class thread_team_visual_chrono : public thread_team_visual {
  char ch;

public:
  thread_team_visual_chrono(thread_team &team, thread_id &id, char ch)
      : thread_team_visual(team, id), ch{ch} {}

protected:
  std::string vis_actual(sptr<thread> &th) override {
    return ch + vis_space(th, "¨"); // vis_space(th);
  }
};

struct printeable_team {
  std::string point;
  std::string next;
  std::string vertiz;
  std::string last_vertiz;
};

class thread_team_visual_bifurc : public thread_team_visual {
public:
  thread_team_visual_bifurc(thread_team &team, thread_id &id)
      : thread_team_visual(team, id) {}

protected:
  std::string vis_actual(sptr<thread> &th) override {
    auto print = get_printeable();

    std::string v = "";
    if (auto nested = th->get_nested()) {
      std::string actual, next;

      auto nested_begin = (*nested).get_threads().begin();
      auto nested_end = (*nested).get_threads().end();
      for (auto it_th = nested_begin; it_th != nested_end; it_th++) {
        it_th++;
        auto it_th_next = it_th;
        it_th--;
        if (it_th == nested_begin) {
          actual = print.point;
          next = (it_th_next != nested_end) ? print.next : " ";
        } else {

          if (it_th_next != nested_end) {
            actual = print.vertiz;
            next = print.next;
          } else {
            actual = print.last_vertiz;
            next = " ";
          }
        }
        v += actual + vis_space(*it_th, next);
      }
    }
    return v;
  }

  virtual printeable_team get_printeable() = 0;
};

class thread_team_visual_fork : public thread_team_visual_bifurc {
public:
  thread_team_visual_fork(thread_team &team, thread_id &id)
      : thread_team_visual_bifurc(team, id) {}

protected:
  printeable_team get_printeable() override { return {"*", "─", "┬", "┐"}; }
};

class thread_team_visual_join : public thread_team_visual_bifurc {
public:
  thread_team_visual_join(thread_team &team, thread_id &id)
      : thread_team_visual_bifurc(team, id) {}

protected:
  printeable_team get_printeable() override { return {"°", "─", "┴", "┘"}; }
};

class thread_inic_team_log {
  sptr<vec<sptr<thread_dimension>>> dimensions;
  bool finalized = false;

public:
  thread_inic_team_log(int cant)
      : dimensions{std::make_shared<vec<sptr<thread_dimension>>>(cant)} {}

  thread_inic_team_log()
      : dimensions{std::make_shared<vec<sptr<thread_dimension>>>()} {}

  void add_data(int pos, const int &num_threads,
                sptr<thread_dimension> dimension) {
    if (resize_is_needed(num_threads))
      dimensions->resize(num_threads);
    (*dimensions)[pos] = dimension;
  }

  void set_has_finalized() { finalized = true; }

  bool is_finalized() { return finalized; }

  sptr<vec<sptr<thread_dimension>>> get_dimensions() { return dimensions; }

  bool resize_is_needed(const int &num_threds) const {
    return dimensions->size() != num_threds;
  }

  bool complete() const {
    bool is_complete = true;
    for (auto dimension : *dimensions) {
      if (!dimension) {
        is_complete = false;
        break;
      }
    }
    return is_complete;
  }
};

struct chrono_log {
  char id;
  std::chrono::system_clock::time_point time;
};

class duration {
  std::chrono::duration<double> _duration;
  int _precision;

public:
  duration(std::chrono::duration<double> duration =
               std::chrono::duration<double>::zero(),
           int precision = 9)
      : _duration{duration}, _precision{precision} {}

  friend inline ::std::ostream &operator<<(::std::ostream &os,
                                           const duration &d) {
    os << std::fixed << std::setprecision(d._precision) << std::setfill(' ')
       << d._duration.count() << std::scientific
       << std::setprecision(precision_default);
    return os;
  }

  friend inline bool operator<(const duration &da, const duration &db) {
    return da._duration < db._duration;
  }

  friend inline bool operator>(const duration &da, const duration &db) {
    return da._duration > db._duration;
  }

  friend inline duration operator-(const duration &da, const duration &db) {
    return duration(da._duration - db._duration);
  }
};

class chronometer {
  std::unordered_map<char, chrono_log> chronos;

public:
  void regist(const chrono_log &ch) { chronos[ch.id] = ch; }

  duration chronometrate(const chrono_log &ch) {
    auto it_ch_inic = chronos.find(ch.id);
    if (it_ch_inic != chronos.end()) {
      duration d(ch.time - it_ch_inic->second.time);
      chronos.erase(it_ch_inic);
      return d;
    }
    return {};
  }
};

enum class event_t {
  begin,
  message,
  fork,
  join,
  chrono_begin,
  chrono_end,
  end
};
struct event {
  // duration                    time;
  event_t type;
  opt<thread_id> id;
  opt<std::string> msg;
  sptr<thread_inic_team_log> data_team;
  opt<chrono_log> chrono;
};

class thread_logger {
  vec<event> events;

public:
  const vec<event> &get_events() { return events; }

  void begin(const thread_id &id, sptr<thread_inic_team_log> data_team) {
    logging({event_t::begin, {id}, {}, data_team});
  }

  void begin_team(const thread_id &id, sptr<thread_inic_team_log> data_team) {
    logging({event_t::fork, {id}, {}, data_team});
  }

  void end_team(
      const thread_id &id /*, sh_ptr_t<thread_inic_team_log> &data_fork*/) {
    logging({event_t::join, {id}, {}, {} /*data_fork*/});
  }

  void end(const thread_id &id) { logging({event_t::end, {id}, {}, {}}); }

  void message(const thread_id &id, const std::string &msg) {
    logging({event_t::message, {id}, {msg}, {}});
  }

  void begin_chrono(const thread_id &id, const char &ch) {
    auto chr = opt<chrono_log>({ch, std::chrono::system_clock::now()});
    logging({event_t::chrono_begin, {id}, {}, {}, chr});
  }

  void end_chrono(const thread_id &id, const char &ch) {
    auto chr = opt<chrono_log>({ch, std::chrono::system_clock::now()});
    logging({event_t::chrono_end, {id}, {}, {}, chr});
  }

private:
  void logging(const event &data) { events.push_back(data); }
};

inline void show(std::ostream &os, const vec<event> &events) {
  chronometer chrono;
  std::unique_ptr<thread_team> g_team;
  int g_deep;
  std::ostringstream smark;
  std::ostringstream smsg;
  int width_id;

  for (auto event : events) {
    if (event.type == event_t::begin) {
      auto dimensions = event.data_team->get_dimensions();
      g_team = std::make_unique<thread_team>(dimensions);
      g_deep = (*dimensions)[0]->deep;
    }
    auto &id = *event.id;
    if (auto th = g_team->find(id)) {
      thread_id_visual vid(id, g_deep);
      switch (event.type) {
      case event_t::begin: {
        thread_team_visual_fork thv(*g_team, id);
        os << thv;
        smark << "*";
        smsg << "begin";
      } break;
      case event_t::end: {
        thread_team_visual_join thv(*g_team, id);
        os << thv;
        smark << "°";
        smsg << "end";

      } break;
      case event_t::fork: {
        th->fork(event.data_team->get_dimensions());

        thread_team_visual_fork thv(*g_team, id);

        os << thv;
        smark << "*";
        smsg << "fork";
#ifdef omp_log_test_1
        vis_dimensions(event.data_team->get_dimensions(), std::cout);
#endif
      } break;
      case event_t::join: {
        thread_team_visual_join thv(*g_team, id);

        os << thv;
        smark << "°";
        smsg << "join";

        th->join();
      } break;
      case event_t::message: {
        thread_team_visual_id thv(*g_team, id);

        os << thv;
        smark << id.front();
        smsg << "msg: " << *event.msg;
      } break;
      case event_t::chrono_begin: {
        auto chr = *event.chrono;
        thread_team_visual_chrono thv(*g_team, id, chr.id);

        os << thv;
        smark << chr.id;
        smsg << "chrono_begin";

        chrono.regist(chr);
      } break;
      case event_t::chrono_end: {
        auto chr = *event.chrono;
        thread_team_visual_chrono thv(*g_team, id, chr.id);

        os << thv;
        smark << chr.id;
        smsg << "chrono_end: " << chrono.chronometrate(chr) << "s";
      } break;
      default:
        std::cout << "Error!";
        break;
      }
#ifdef omp_log_test
      std::cout << *th;
#endif
      os << smark.str() << "> " << vid << " ~" << smsg.str() << "\n";

      // clear smark
      smark.str("");
      smark.clear();
      // clear smsg
      smsg.str("");
      smsg.clear();
    }
#ifdef omp_log_test
    else
      std::cout << "thread don't found! id: " << vis_id(id) << std::endl;
#endif
  }
}

class thread_log;

class thread_log_dimension {
  sptr<thread_dimension> dimension;
  std::map<int, sptr<thread_dimension>> dimension_childs;

public:
  thread_log_dimension() : dimension{std::make_shared<thread_dimension>()} {}

  sptr<thread_dimension> &get_dimension() { return dimension; }

  void registrate_child_dimension(int pos, sptr<thread_dimension> dimension) {
    dimension_childs[pos] = dimension;
  }

  void calculate() {
    dimension->deep = 1;
    // pensar nowait
    if (dimension_childs.size() > 0) {
      auto new_width = 0;
      int max_deep = 0;
      for (auto dim_ch : dimension_childs) {
        new_width += dim_ch.second->width;
        if (max_deep < dim_ch.second->deep)
          max_deep = dim_ch.second->deep;
      }
      dimension->deep += max_deep;
      dimension->width = std::max(dimension->width, new_width);
    } else {
      dimension->width = 1;
    }
    dimension_childs.clear();
  }
};

class stream_logger_proxy;

struct thread_info {
  int thread_num = omp_get_thread_num();
  int num_threads = omp_get_num_threads();
  stream_logger_proxy *stream_log_pxy = nullptr;

  void finalize();

  ~thread_info() { finalize(); }
};

class stream_logger;

class thread_log {

  /*struct find_by_private_mem {
      bool operator()(const thread_log & th) {
          return th._private_mem == private_mem;
      }
  private:
      void *private_mem;
  };*/

  thread_id id;
  int num;
  int num_threads;

  uintptr_t mem_id;

  opt<thread_logger> u_logger;
  thread_logger &logger;
  stream_logger &s_log;
  thread_log_dimension log_dimension;
  sptr<thread_inic_team_log> data_team;

  thread_log *parent;
  std::map<uintptr_t, thread_log &> childs;

  static opt<thread_logger> get_unique_log(thread_log *parent) {
    if (!parent)
      return thread_logger();
    return std::nullopt;
  }

  static thread_logger &get_log(thread_log *parent,
                                opt<thread_logger> &unique_logger) {
    if (!parent)
      return *unique_logger;
    return parent->get_logger();
  }

  static uintptr_t get_private_mem(const void *private_mem) {
    return (uintptr_t)private_mem;
  }

  void inic() {

    if (!parent) {
      id.push_front(num);
      auto root_team = std::make_shared<thread_inic_team_log>();
      this->vincule_to_team(root_team);
      logger.begin(id, root_team);
#ifdef omp_log_test
      std::cout << "havent parent! @" << this->get_mem_id() << "{"
                << vis_id(this->get_id()) << "}" << std::endl;
#endif
    } else {
      id = parent->get_id();
      id.push_front(num);
      parent->registrate_child(*this);
#ifdef omp_log_test
      std::cout << "parent, registrate child! @" << parent->get_mem_id() << "{"
                << vis_id(parent->get_id()) << "} -> @" << this->get_mem_id()
                << "{" << vis_id(this->get_id()) << "}" << std::endl;
#endif
    }
  }

public:
  /*thread_log(thread_log const&)      = delete;
  void operator=(thread_log const&)  = delete;*/

  /*static thread_log && instance(stream_logger &s_log, thread_log * parent =
  nullptr, void * private_mem = nullptr)
  {
      if( parent != nullptr )
      {
          if( auto opt_child = parent->get_child( (uintptr_t) private_mem) )
              return **opt_child;
          auto ptr = new thread_log(s_log, parent, private_mem);
          return parent->registrate_child( sptr<thread_log>( ptr ) );
      }
      return { s_log, parent, private_mem };
  }*/

  thread_log(const thread_info &info, stream_logger &s_log,
             const void *private_mem, thread_log *parent = nullptr)
      : num{info.thread_num}, num_threads{info.num_threads},
        mem_id{get_private_mem(private_mem)}, u_logger{get_unique_log(parent)},
        logger{get_log(parent, u_logger)}, s_log{s_log},
        data_team{std::make_shared<thread_inic_team_log>()}, parent{parent} {
    inic();
  }

  void finalize_shared_elements() {

    log_dimension.calculate();

    if (!parent) {
      logger.end(id);
      show(std::cout, logger.get_events());
    } else
      parent->remove_child(*this);
  }

  ~thread_log() { }

  thread_id &get_id() { return id; }

  int get_num() { return num; }

  int get_num_threads() { return num_threads; }

  thread_logger &get_logger() { return logger; }

  sptr<thread_dimension> &get_dimension() {
    return log_dimension.get_dimension();
  }

  void vincule_to_team(sptr<thread_inic_team_log> d_team) {
    d_team->add_data(id.front(), num_threads, get_dimension());
  }

  void begin_chrono(const char &ch) { logger.begin_chrono(id, ch); }

  void end_chrono(const char &ch) { logger.end_chrono(id, ch); }

  void message();

  friend bool operator<(thread_log const &th_a, thread_log const &th_b) {
    return th_b.get_mem_id() < th_b.get_mem_id();
  }

  const uintptr_t get_mem_id() const { return mem_id; }

  void registrate_child(
      thread_log &child /*stream_logger &s_log, void * private_mem*/) {
    if (data_team->is_finalized())
      data_team = std::make_shared<thread_inic_team_log>();

    if (data_team->resize_is_needed(child.get_num_threads())) {
      logger.begin_team(id, data_team);
    }

    // auto child = std::make_shared<thread_log>(s_log, this, private_mem);

    child.vincule_to_team(data_team);

    childs.emplace(child.get_mem_id(), child);

    // return *child;
  }

  std::set<uintptr_t> get_mem_id_stream_logger_childs() {
    std::set<uintptr_t> res;

    for (auto child_kv : childs)
      res.insert(child_kv.second.get_mem_id());

    return res;
  }

  stream_logger &get_s_log();

protected:
  /*std::tuple<void *, > get_tuple(thread_log && th_log) {
      return std::tuple{ th_log.private_mem(), th_log };
  }*/

  /*bool child_is_registered(uintptr_t private_mem) {
      return childs.find( private_mem ) !=  childs.end();
  }*/

  /*thread_log & get_child(uintptr_t private_mem)
  {
      auto it = childs.find( private_mem );
      if (it != childs.end() )
          return std::optional{ it->second };
      return std::nullopt;
  }*/

  void remove_child(thread_log &child) {
    auto it = childs.find(child.get_mem_id());
    if (it != childs.end()) {
      log_dimension.registrate_child_dimension(it->second.get_num(),
                                               it->second.get_dimension());
      childs.erase(it);

      if (childs.empty()) {
        if (data_team->complete()) {
          logger.end_team(id);
          data_team->set_has_finalized();
        }
      }
    }
  }
};

class stream_logger {
  thread_log th_log;
  std::ostringstream sstream;
  bool finalized = false;

public:
  stream_logger(const thread_info &info_th, stream_logger &parent)
      : th_log{info_th, *this, &info_th, &(parent.get_th_log())} {
    // parent.get_th_log().registrate_child( th_log );
  }

  stream_logger(const thread_info &info_th) : th_log{info_th, *this, &info_th} {
    /*std::string a = "new steram logger! @"+
            std::to_string(this)+
            "{"+vis_id(this->th_log.get_id())+"} -> @"+
            std::to_string(child.get_mem_id())+
            "{"+vis_id(child.get_id())+ "}\n"  ;

    std::cout << a;*/
  }

  stream_logger(const thread_info &info_th, stream_logger &parent,
                void *private_mem)
      : th_log{info_th, *this, private_mem, &(parent.get_th_log())} {
    // parent.get_th_log().registrate_child( th_log );
  }

  ~stream_logger() {
    finalize();
  }

  void finalize() { 
    th_log.finalize_shared_elements();
    if( !finalized ) {
      flush();
      finalized = true;
    }
  }

  thread_log &get_th_log() { return th_log; }

  const std::string extract() {
    auto str = sstream.str();
    sstream.str("");
    sstream.clear();

    sstream << std::scientific << std::setprecision(precision_default);
    return str;
  }

  void begin_chrono(char c) {
    flush();
    th_log.begin_chrono(c);
  }

  void end_chrono(char c) {
    flush();
    th_log.end_chrono(c);
  }

protected:
  
  void flush() {
    if (!empty())
      th_log.message();
  }

  bool empty() {
    std::streampos pos = sstream.tellp();   // store current location
    sstream.seekp(0, std::ios_base::end);   // go to end
    bool am_empty = (sstream.tellp() == 0); // check size == 0 ?
    sstream.seekp(pos);
    return am_empty;
  }

  template <typename T>
  friend stream_logger &operator<<(stream_logger &sm, const T &t) {
    sm.sstream << t;
#ifdef omp_log_test_logging
    std::cout << "operator<< @" << sm.th_log.get_mem_id() << "{"
              << vis_id(sm.th_log.get_id()) << "}"
              << " << " << t << std::endl;
#endif
    return sm;
  }

  friend stream_logger &operator<<(stream_logger &sm,
                                   std::ostream &(*fun)(std::ostream &)) {
    sm.th_log.message();
#ifdef omp_log_test_logging
    std::cout << "operator<< @" << sm.th_log.get_mem_id() << "{"
              << vis_id(sm.th_log.get_id()) << "}"
              << " << std::endl" << std::endl;
#endif
    return sm;
  }
};

inline void thread_log::message() { logger.message(id, s_log.extract()); }

inline stream_logger &thread_log::get_s_log() { return s_log; }

class stream_logger_proxy;

/**
 * @brief The critical_thread class
 *
 * Tiene la responsabilidad de ...
 *
 */
class critical_thread {
  std::queue<std::function<void()>> operation_queue;
  omp_lock_t queue_lock;
  bool stop_flag;
  std::thread thread;

  stream_logger_proxy *root_stream_logger_proxy;

  critical_thread() : stop_flag(false)
  {  }

public:
  static critical_thread &get_instance() {
    static critical_thread instance;
    return instance;
  }

  ~critical_thread() { 
    thread.join(); 
  }

  void set_root_stream_logger_proxy(stream_logger_proxy *root) {
    root_stream_logger_proxy = root;
  }

  void finish_root_stream_logger();

  void enqueue_operation(const std::function<void()> &operation) {
    omp_set_lock(&queue_lock);
    operation_queue.push(operation);
    omp_unset_lock(&queue_lock);
  }

  void start_operate() {
    thread = std::thread(&critical_thread::run, this);
  }

  void stop_operate() {
    omp_set_lock(&queue_lock);
    stop_flag = true;
    omp_unset_lock(&queue_lock);
  }

private:
  void run() {
    bool stop_loop = false;
    while (!stop_loop) {
      omp_set_lock(&queue_lock);
      if (operation_queue.empty()) {
        if (stop_flag) {
          stop_loop = true;
          finish_root_stream_logger();
        }
      } else {                     // operation_queue not empty
        operation_queue.front()(); // operate
        operation_queue.pop();
      }
      omp_unset_lock(&queue_lock);
    }
  }
};

/**
 * @brief The stream_logger_proxy class
 *
 * Tiene la responsabilidad de encapsular stream_loggers
 * para que pueda realizar las operaciones de forma paralela
 * reciviendo un resolver function<stream,_logger & ()>
 *
 */
class stream_logger_proxy {

  omp_lock_t stream_log_lock;
  stream_logger *stream_log = nullptr;
  std::function<stream_logger *()> resolver;

  stream_logger_proxy *parent;
  std::set<stream_logger_proxy *> childs;

  thread_info &th_info;

public:
  stream_logger_proxy(std::function<stream_logger *()> _resolver,
                      thread_info &th_info,
                      stream_logger_proxy *_parent = nullptr)
      : resolver(_resolver), parent(_parent), th_info{th_info} {

    if (parent != nullptr) {
      parent->add_child(this);
    }
  }

  ~stream_logger_proxy() {
    std::cout << "~stream_logger_proxy @" << this;
  }

  void add_child(stream_logger_proxy *child) { childs.insert(child); }

  /*void finalize() {
      stream_log->finalizer_stream_logger();
      for(auto child : childs) {
          child->finalize();
      }
  }*/

  void make_stream_logger_if_needed() {
    if (stream_log == nullptr) {
      if (parent != nullptr) {
        parent->make_stream_logger_if_needed();
      }
      stream_log = resolver();
    }
  }

  stream_logger *get_stream_logger() {
    this->make_stream_logger_if_needed();
    return stream_log;
  }

  friend stream_logger_proxy &operator<<(stream_logger_proxy &sm,
                                         const std::string &str) {
    critical_thread::get_instance().enqueue_operation(
        [&sm, str] { (*(sm.get_stream_logger())) << str; });
    return sm;
  }

  friend stream_logger_proxy &operator<<(stream_logger_proxy &sm,
                                         const int &i) {
    critical_thread::get_instance().enqueue_operation(
        [&sm, i] { (*(sm.get_stream_logger())) << i; });
    return sm;
  }

  friend stream_logger_proxy &operator<<(stream_logger_proxy &sm,
                                         std::ostream &(*fun)(std::ostream &)) {
    critical_thread::get_instance().enqueue_operation(
        [&sm] { (*(sm.get_stream_logger())) << std::endl; });
    return sm;
  }
};

inline void thread_info::finalize() {
  if (stream_log_pxy != nullptr) {
    auto *s_log_pxy = stream_log_pxy;
    critical_thread::get_instance().enqueue_operation([s_log_pxy] {
      s_log_pxy->get_stream_logger()->finalize();
    });
  }
}

inline void critical_thread::finish_root_stream_logger() {
  auto *s_logger = root_stream_logger_proxy->get_stream_logger();
  delete s_logger;
}

/**
 * @brief The stream_logger_register class
 *
 * Tiene la responsabilidad de crear objetos stream_logger
 * de forma segura y que haya una sola instancia por identificador
 * de memoria (dirección de una variable.
 * Es utilizado solo en los parallel for debido a que estos
 * objetos no tienen elminación automática.
 *
 */
class stream_logger_register {
  static std::unordered_map<uintptr_t, sptr<stream_logger>> sloggers;

public:

  static stream_logger_proxy *
  get_stream_logger(stream_logger_proxy *proxy_parent, thread_info &th_info) {
    auto * proxy = new stream_logger_proxy(
        [th_info, proxy_parent]() -> stream_logger * {
          stream_logger *parent = proxy_parent->get_stream_logger();
          stream_logger *ptr;
          ptr = new stream_logger(th_info, *parent);
          // sloggers[ptr->get_th_log().get_mem_id()] = sptr<stream_logger>(ptr)
          // ;
          return ptr;
        },
        th_info, proxy_parent);
        th_info.stream_log_pxy = proxy;
    return proxy;
  }

  static stream_logger_proxy *get_stream_logger(thread_info &th_info) {
    auto *root = new stream_logger_proxy(
        [th_info]() -> stream_logger * {
          stream_logger *ptr;
          ptr = new stream_logger(th_info);
          // sloggers[ptr->get_th_log().get_mem_id()] = sptr<stream_logger>(ptr)
          // ;
          return ptr;
        },
        th_info);
    th_info.stream_log_pxy = root;
    critical_thread::get_instance().set_root_stream_logger_proxy(root);
    return root;
  }

  static stream_logger_proxy *get_stream_logger(stream_logger_proxy *proxy_parent, void *mem_id) {
    thread_info info_th;
    return new stream_logger_proxy(
        [info_th, proxy_parent, mem_id]() -> stream_logger * {
          stream_logger *parent = proxy_parent->get_stream_logger();
          stream_logger *ptr;
          auto it = sloggers.find((uintptr_t)mem_id);
          if (it != sloggers.end()) {
            ptr = it->second.get();
          } else {
            ptr = new stream_logger(info_th, *parent, mem_id);
            sloggers[ptr->get_th_log().get_mem_id()] = sptr<stream_logger>(ptr);
          }
          return ptr;
        },
        info_th, proxy_parent);
  }

  const static void remove_stream_loggers(stream_logger_proxy *proxy_parent) {
    critical_thread::get_instance().enqueue_operation([proxy_parent] {
      std::set<uintptr_t> mem_ids = proxy_parent->get_stream_logger()
                                        ->get_th_log()
                                        .get_mem_id_stream_logger_childs();
      for (auto mem_id : mem_ids) {
        auto it = sloggers.find(mem_id);
        if (it != sloggers.end()) {
#ifdef omp_log_test
          std::cerr << "delete @" << it->second->get_th_log().get_mem_id()
                    << "{" << vis_id(it->second->get_th_log().get_id()) << "}"
                    << std::endl;
#endif
          sloggers.erase(it);
        }
#ifdef omp_log_test
        else {
          std::cerr << "! delete @" << (uintptr_t)mem_id << std::endl;
        }
#endif
      }
    });
  }
};

//! TODO remove all childs

} // namespace omp_log

#define omp_log_inic_named(name)                                               \
  omp_log::thread_info th_info;                                                \
  auto *name = omp_log::stream_logger_register::get_stream_logger(th_info)

#define omp_log_inic_parented_named_info(name, th_info)                        \
  auto *parent = name;                                                         \
  auto *name =                                                                 \
      omp_log::stream_logger_register::get_stream_logger(parent, th_info)

#define omp_log_inic_parented_named(name)                                      \
  omp_log::thread_info th_info;                                                \
  auto *parent = name;                                                         \
  auto *name =                                                                 \
      omp_log::stream_logger_register::get_stream_logger(parent, th_info)

#define omp_log_inic_parented_mem_id_named(name, ptr)                          \
  auto *parent = name;                                                         \
  auto *name = omp_log::stream_logger_register::get_stream_logger(parent, ptr)

#define omp_log_inic_parented_mem_id_named_data(name, data)                    \
  auto *parent = name;                                                         \
  auto *name =                                                                 \
      omp_log::stream_logger_register::get_stream_logger(parent, data.th_info)

#define omp_log_end_parented_named(name)                                       \
  omp_log::stream_logger_register::remove_stream_loggers(name)

#define omp_log_inic() omp_log_inic_named(omp_log_ptr)

#define omp_log_finalize()                                  \
  omp_log::critical_thread::get_instance().start_operate(); \
  omp_log::critical_thread::get_instance().stop_operate()

#define omp_log_inic_parented() omp_log_inic_parented_named(omp_log_ptr)

#define omp_log_inic_parented_info(th_info)                                    \
  omp_log_inic_parented_named_info(omp_log_ptr, th_info)

#define omp_log_inic_for(ptr)                                                  \
  omp_log_inic_parented_mem_id_named(omp_log_ptr, ptr)

#define omp_log_end_for() omp_log_end_parented_named(omp_log_ptr)

#define omp_logger (*omp_log_ptr)

